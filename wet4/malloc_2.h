
#ifndef WET4_MALLOC_2_H
#define WET4_MALLOC_2_H

#include <cstdlib>
#include <unistd.h>

#define MAX_MALLOC_SIZE 100000000

void* _malloc(size_t size);
void* _calloc(size_t size);
void _free(void* p);
void* _realloc(void* oldp, size_t size);
size_t _num_free_blocks();
size_t _num_free_bytes();
size_t _num_allocated_blocks();
size_t _num_allocated_bytes();
size_t _num_meta_data_bytes();
size_t _size_meta_data();

#endif //WET4_MALLOC_2_H
