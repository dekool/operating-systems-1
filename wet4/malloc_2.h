
#ifndef WET4_MALLOC_2_H
#define WET4_MALLOC_2_H

#include <cstdlib>
#include <unistd.h>

#define MAX_MALLOC_SIZE 100000000
struct meta_data {
    size_t block_size;
    bool is_free;
};
class Node{
public:
    Node(): data(NULL), next_node(NULL){

    };

    void setNext(Node* next){
        this->next_node = next;
    }

    void setData(meta_data* data){
        this->data = data;
    }

    meta_data* getData(){
        return data;
    }

    Node* next(){
        return this->next_node;
    }

    //allocates new node at the end of the heap, and puts it at the end
    //of the linked list after last
    static Node* allocateNode(Node* last){
        intptr_t node_size = sizeof(Node);
        void* ptr = sbrk(node_size);
        if(*(int*)ptr == -1){
            return NULL;
        }
        Node* node_ptr = (Node*) ptr;
        *node_ptr = Node();
        last->setNext(node_ptr);
        return node_ptr;
    }

private:
    meta_data* data;
    Node* next_node;
};
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
