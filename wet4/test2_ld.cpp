#include <unistd.h>
#include <iostream>
#include <assert.h>
#include <time.h>
#include "malloc_2.h"


#define TEST_NUM 10
void testMalloc();

// add these function because it can't overload them
// ###############################################################
size_t _num_free_blocks();
size_t _num_free_bytes();
size_t _num_allocated_blocks();
size_t _num_allocated_bytes();
size_t _num_meta_data_bytes();
size_t _size_meta_data();

Node root = Node();

size_t _num_free_blocks(){
    size_t num = 0;
    Node* curr = root.next();
    while(curr != NULL){
        if(curr->getData()->is_free){
            num++;
        }
        curr = curr->next();
    }
    return num;
}

size_t _num_free_bytes(){
    size_t num = 0;
    Node* curr = root.next();
    while(curr != NULL){
        if(curr->getData()->is_free){
            num += curr->getData()->block_size;
        }
        curr = curr->next();
    }
    return num;
}

size_t _num_allocated_blocks(){
    size_t num = 0;
    Node* curr = root.next();
    while(curr != NULL){
        num++;
        curr = curr->next();
    }
    return num;
}
size_t _num_allocated_bytes(){
    size_t num = 0;
    Node* curr = root.next();
    while(curr != NULL){
        num += curr->getData()->block_size;
        curr = curr->next();
    }
    return num;
}

//TODO check if this needs to be only meta or node as well
size_t _size_meta_data(){
    return sizeof(meta_data) + sizeof(Node);
}

size_t _num_meta_data_bytes(){
    size_t allocated_blocks = _num_allocated_blocks();
    size_t meta_size = _size_meta_data();
    return allocated_blocks * meta_size;
}
// ##################################################################

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
