#include <unistd.h>
#include <iostream>
#include <assert.h>
#include <time.h>
#include "malloc_2.cpp"


#define TEST_NUM 10
void testMalloc();

int main (int argc, char *argv[]) {
    testMalloc();
    return 0;
}

void testMalloc(){
    srand(time(NULL));
    assert(malloc(0) == NULL);
    assert(malloc(MAX_MALLOC_SIZE + 1) == NULL);
    void* pointers[TEST_NUM];
    int sum = 0;
    void* start_addr = sbrk(0);
    for(int i = 0; i < TEST_NUM ;  i++ ){
        int size = rand() % 1000;
        char* start = (char*)sbrk(0);
        void* p = malloc(size);
        pointers[i] = p;
        char* end = (char*)sbrk(0);
        assert((char*)p + size == end);
        assert((char*)p - start == sizeof(meta_data) + sizeof(Node));//size of meta data and node
        sum += size;
    }
    size_t meta_size = TEST_NUM * sizeof(meta_data);
    size_t node_size = TEST_NUM * sizeof(Node);
    void* end_addr = sbrk(0);
    assert((char*)end_addr - (char*)start_addr == sum + meta_size + node_size);
    assert(_num_allocated_blocks() == TEST_NUM);
    assert(_num_allocated_bytes() == sum);
    free(pointers[1]);
    assert(_num_free_blocks() == 1);

    void* ptr = calloc(1,_num_free_bytes());
    assert(ptr != NULL);
    assert(ptr == pointers[1]);
    assert(_num_free_bytes() == 0);
    assert(_num_allocated_blocks() == TEST_NUM);


    free(ptr);
    assert(_num_free_blocks() == 1);
    ptr = calloc(1,_num_free_bytes() - 1);
    assert(ptr != NULL);
    assert(ptr == pointers[1]);
    assert(_num_free_bytes() == 0);
    assert(_num_allocated_blocks() == TEST_NUM);

    free(ptr);
    assert(_num_free_blocks() == 1);
    ptr = calloc(1,_num_free_bytes() + 1);
    assert(ptr != NULL);
    assert(ptr != pointers[1]);
    assert(_num_free_blocks() == 1);
    assert(_num_allocated_blocks() == TEST_NUM + 1);

    void* rAlloc = realloc(ptr, 1);
    assert (rAlloc != NULL);
    assert(rAlloc == ptr);
    assert(_num_free_blocks() == 1);
    assert(_num_allocated_blocks() == TEST_NUM + 1);

    rAlloc = realloc(rAlloc, _num_free_bytes() + 10);
    assert(rAlloc != NULL);
    assert(rAlloc != ptr);
    assert(_num_free_blocks() == 2);
    assert(_num_allocated_blocks() == TEST_NUM +2);
}
