
#include <unistd.h>
#include <iostream>
#include <assert.h>
#include <time.h>
#include "malloc_1.h"


#define TEST_NUM 10
void testMalloc();

int main (int argc, char *argv[]) {
    testMalloc();
    return 0;
}

void testMalloc(){
    srand(time(NULL));
    assert(_malloc(0) == NULL);
    assert(_malloc(MAX_MALLOC_SIZE + 1) == NULL);
    int sum = 0;
    void* start_addr = sbrk(0);
    for(int i = 0; i < TEST_NUM ;  i++ ){
        int size = rand() % 1000;
        std::cout << "Malloc size : " << size << std::endl;
        std::cout << "Current program break : " << sbrk(0) << std::endl;
        void* p = _malloc(size);
        std::cout << "Current program break : " << sbrk(0) << std::endl;
        std::cout << "Address given by malloc is : " << p << std::endl;
        sum += size;
    }
    std::cout << "Start program break : " << start_addr << std::endl;
    std::cout << "Final program break : " << sbrk(0) << std::endl;

    std::cout << "Final sum : " << sum << std::endl;


}
