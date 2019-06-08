
#include "malloc_2.h"

struct meta_data {
    size_t block_size;
    bool is_free;
};

class Node{
public:
    Node(): data(NULL), next_node(NULL){};

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

Node root;

void* _malloc(size_t size){
    if(size == 0 || size > MAX_MALLOC_SIZE){
        return NULL;
    }
    Node* curr = &root;
    Node* last = curr;
    while(curr != NULL){
        if(curr->getData()->is_free){
            if(curr->getData()->block_size >= size){
                curr->getData()->is_free = false;
                // Can change block size to the requested block size and create
                // new node that points to the rest of the size
                // but we are not suppose to optimize in this section
                return curr->getData() + sizeof(meta_data); //returns the start of the block after meta_data
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
    return meta + sizeof(meta_data);
}

void* _calloc(size_t size);
void _free(void* p);
void* _realloc(void* oldp, size_t size);
size_t _num_free_blocks();
size_t _num_free_bytes();
size_t _num_allocated_blocks();
size_t _num_allocated_bytes();
size_t _num_meta_data_bytes();
size_t _size_meta_data();
