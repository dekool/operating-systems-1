#include <unistd.h>
#include <stdlib.h>

#define MAX_MALLOC_SIZE 100000000

void* malloc(std::size_t size){
    if(size == 0 || size > MAX_MALLOC_SIZE){
        return NULL;
    }
    intptr_t increment = size;
    void* ptr = sbrk(increment);
    if(*(int*)ptr == -1){
        return NULL;
    }
    return ptr;
}
