#include <unistd.h>
#include <iostream>
#include <assert.h>
#include <time.h>
#include "malloc_3.h"


#define TEST_NUM 10
void testMalloc();

int main (int argc, char *argv[]) {
    testMalloc();
    return 0;
}

void testMalloc() {
    assert(_malloc(0) == NULL);
    assert(_malloc(MAX_MALLOC_SIZE + 1) == NULL);
    void *pointers[TEST_NUM];
    void *start_addr = sbrk(0);
    int sum = 0;
    //porinter = { 100, 200, 300, 400, 500 ,600 ,700, 800 ,900, 1000}
    for (int i = 1; i <= TEST_NUM; i++) {
        int size = i * 100;
        void *p = _malloc(size);
        pointers[i - 1] = p;
        sum += size;
    }
    size_t meta_size = TEST_NUM * sizeof(meta_data);
    size_t node_size = TEST_NUM * sizeof(Node);
    void *end_addr = sbrk(0);
    assert((char *) end_addr - (char *) start_addr ==
           sum + meta_size + node_size);
    assert(_num_allocated_blocks() == TEST_NUM);
    assert(_num_allocated_bytes() == sum);

    _free(pointers[1]); //free 200 bytes
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == 200);

    void *p1 = _malloc(100);//shouldn`t split the 200 block
    assert(p1 == pointers[1]);
    assert(_num_free_blocks() == 0);
    assert(_num_free_bytes() == 0);

    _free(pointers[2]); //free 300 bytes
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == 300);

    p1 = _malloc(100); //should split the 300 block
    assert(p1 == pointers[2]);
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == 200 - _size_meta_data() - sizeof(Node));

    _free(p1); //should merge with next block
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == 300);

    p1 = _realloc(pointers[8], 800); //shouldn`t split
    assert(p1 == pointers[8]);
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == 300);

    _free(pointers[9]);
    _free(pointers[7]);
    assert(_num_free_blocks() == 3);
    assert(_num_free_bytes() == 2100);

    _free(p1); // should merge 3 blocks to 1
    assert(_num_free_blocks() == 2);
    assert(_num_free_bytes() == 3000 + 2 * (sizeof(Node) + sizeof(meta_data)));
    //now last block has 2732 bytes
    //lets ask for 3000 bytes and see if it uses the last block
    p1 = _malloc(3000); //should use last block
    assert(p1 == pointers[7]);
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == 300);

    p1 = _malloc(300);
    assert(p1 == pointers[2]);
    assert(_num_free_blocks() == 0);
    assert(_num_free_bytes() == 000);
    assert(_num_allocated_blocks() == 8);
    //all curr blocks are assigned

    //lets check if we round all sizes to be multiplication of 4
    //curr array = {100, 200 ,300 ,400 ,500 ,600 ,700 ,3000};
    p1 = _malloc(5); // should allocate 8 bytes instead of 5
    assert(_num_allocated_blocks() == 9);
    _free(p1);
    assert(_num_free_bytes() == 8);
    pointers[8] = _malloc(8);
    assert(_num_allocated_blocks() == 9);
    assert(_num_free_blocks() == 0);
    p1 = _malloc(4);
    _free(p1);
    assert(_num_free_blocks() == 1);
    assert(_num_free_bytes() == 4);
    _malloc(4);
    p1 = _malloc(401);// should allocate 404
    p1 = _realloc(p1, 201); //should split 404 to 204 and (16 + 184)
    assert(_num_free_bytes() == 184);
}
