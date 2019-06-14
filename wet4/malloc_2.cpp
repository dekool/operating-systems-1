
#include <memory.h>
#include <iostream>
#include "malloc_2.h"
/* TODO add this here instead of h file
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
};*/

Node root = Node();

void* _malloc(size_t size){
    if(size == 0 || size > MAX_MALLOC_SIZE){
        return NULL;
    }

    Node* curr = root.next();
    Node* last = &root;
    while(curr != NULL){
        if(curr->getData()->is_free){
            if(curr->getData()->block_size >= size){
                curr->getData()->is_free = false;
                // Can change block size to the requested block size and create
                // new node that points to the rest of the size
                // but we are not suppose to optimize in this section
                return curr->getData() + 1; //returns the start of the block after meta_data
            }
        }
        last = curr;
        curr = curr->next();
    }

    // if we reached this, we need to allocate space at the end of the heap
    Node* new_node = Node::allocateNode(last);
    void* ptr = sbrk(size + sizeof(meta_data)); //allocating space at the end of the heap
    if(*(int*)ptr == -1){
        return NULL;
    }
    meta_data* meta = (meta_data*) ptr;
    meta->is_free = false;
    meta->block_size = size;
    new_node->setData(meta);
    //returns the address of the start of the block after meta data
    return meta + 1;
}

void* _calloc(size_t num, size_t size){
    void* ptr = _malloc(size*num);
    if(ptr == NULL){
        return NULL;
    }
    // Zeroing all allocated memory
    memset(ptr, 0, size*num);
    return ptr;
}

void _free(void* p){
    if(p == NULL){
        return;
    }
    //pointer to the meta data of p
    meta_data* meta = (meta_data*)p - 1;
    meta->is_free = true;
}

void* _realloc(void* oldp, size_t size){
    if(oldp == NULL){
        return _malloc(size);
    }
    meta_data* meta = (meta_data*)oldp - 1;
    if(meta->block_size > size){
        return oldp;
    }
    void* new_ptr = _malloc(size);
    if(new_ptr == NULL){
        return NULL;
    }
    //copies old block data to new block (even if the real user data is not at this size)
    memcpy(new_ptr, oldp, meta->block_size);
    meta->is_free = true; //free previous block
    return new_ptr;
}

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

size_t _num_meta_data_bytes(){
    size_t allocated_blocks = _num_allocated_blocks();
    size_t meta_size = sizeof(meta_data);
    return allocated_blocks * meta_size;
}

size_t _size_meta_data(){
    return sizeof(meta_data);
}
