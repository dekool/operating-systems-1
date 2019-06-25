
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory>

#define MAX_MALLOC_SIZE 100000000
#define MIN_FOR_SPLIT 128

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

int align_root();
void split(Node* curr, size_t size);
Node* getMetaNode(meta_data* meta);
Node* getPrevNode(Node* node);
void merge(Node* node);
void tryToMerge(meta_data* meta);


int aligned_root = align_root();
Node root = Node();

int align_root() {
    void* start_addr = sbrk(0);
    intptr_t start_addr_int = reinterpret_cast<intptr_t>(start_addr);
    // Makes start_addr be multiplication of 4
    int addr_to_add= (start_addr_int % 4 == 0) ? 0 : 4 - (start_addr_int % 4);
    void* new_start_addr = sbrk(addr_to_add);
    if(*(int*)new_start_addr == -1){
        return -1;
    }
    return 0;
}


//Split curr node into 2 nodes first one size of var size and other the remaining size
void split(Node* curr, size_t size){
    assert(curr != NULL);
    //allocating new node for the remain part of the memory
    //and adding it after the pre splitted node in the list
    Node *next_node = curr->next();
    // allocating node before new meta data
    Node *remain = (Node *) ((char *) (curr->getData() + 1) + size);
    curr->setNext(remain);
    remain->setNext(next_node);

    //define the new meta data for the remain part after the Node space
    meta_data* remain_meta = (meta_data *) (remain + 1); // +1 will add node size to the pointer
    remain_meta->is_free = true;
    //remain block size will be the remain size from the block size minus metaDara and node sizes
    remain_meta->block_size = curr->getData()->block_size - size - sizeof(meta_data) -
            sizeof(Node);
    remain->setData(remain_meta);
    curr->getData()->block_size = size;
    //we try to merge the remain meta
    tryToMerge(remain_meta);
}

//Returns the node which points to the given meta data
Node* getMetaNode(meta_data* meta){
    assert(meta != NULL);
    Node* curr = root.next();
    while(curr != NULL) {
        if(curr->getData() == meta){
            return curr;
        }
        curr = curr->next();
    }
    return NULL;
}

//Returns previous node to the node given
Node* getPrevNode(Node* node){
    assert(node != NULL);
    Node* curr = root.next();
    Node* last = &root;
    while(curr != NULL) {
        if(curr == node){
            return last;
        }
        last = curr;
        curr = curr->next();
    }
    return NULL;
}

// Will merge given node with it`s next node
void merge(Node* node){
    assert(node != NULL && node->next() != NULL);
    meta_data* curr_meta = node->getData();
    meta_data* next_meta = node->next()->getData();
    // Between two nodes we have node and meta space
    // which we don`t need after merging
    size_t new_size = curr_meta->block_size + next_meta->block_size + sizeof(meta_data) +
            sizeof(Node);
    curr_meta->block_size =  new_size;
    //removing next node from the list
    node->setNext(node->next()->next());
}

// Will try to merge given meta to it`s adjacent metas (nodes)
void tryToMerge(meta_data* meta){
    assert(meta != NULL);
    Node* curr_node = getMetaNode(meta);
    assert(curr_node != NULL);
    Node* prev_node = getPrevNode(curr_node);
    assert(prev_node != NULL);
    Node* next_node = curr_node->next();
    //try to merge curr with next nodes
    if(next_node != NULL){
        if(next_node->getData()->is_free){
            merge(curr_node);
        }
    }
    //try to merge curr with prev nodes
    //Will not merge root
    if(prev_node != &root){
        if(prev_node->getData()->is_free){
            merge(prev_node);
        }
    }
}



void* malloc(size_t size){
    if(size == 0 || size > MAX_MALLOC_SIZE){
        return NULL;
    }
    size += (size % 4 == 0) ? 0 : 4 - (size % 4); // Makes size be multiplication of 4
    Node* curr = root.next();
    Node* last = &root;
    while(curr != NULL){
        if(curr->getData()->is_free){
            if(curr->getData()->block_size >= size){
                curr->getData()->is_free = false;
                //check if the block we found is big enough for splitting it
                if(curr->getData()->block_size >= size + sizeof(Node) + sizeof(meta_data) + MIN_FOR_SPLIT) {
                    split(curr, size);
                }
                return curr->getData() + 1; //returns the start of the block after meta_data
            }
        }
        last = curr;
        curr = curr->next();
    }

    // if we reached this, we need to allocate space at the end of the heap
    // first we check if wilderness is free, var last will hold last node of the list
    if(last->getData() != NULL && last->getData()->is_free){
        //allocating space at the end of the heap for the remaining part
        void* last_space = sbrk(size - last->getData()->block_size);
        if(*(int*)last_space == -1){
            return NULL;
        }
        last->getData()->is_free = false;
        last->getData()->block_size = size;
        return last->getData() + 1;
    }
    //allocating new node at the end of the heap
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

void* calloc(size_t num, size_t size){
    //malloc will check that num* size is smaller than max size and not 0
    void* ptr = malloc(size*num);
    if(ptr == NULL){
        return NULL;
    }
    // Zeroing all allocated memory
    memset(ptr, 0, size*num);
    return ptr;
}

void free(void* p){
    if(p == NULL){
        return;
    }
    //pointer to the meta data of p
    meta_data* meta = (meta_data*)p - 1;
    meta->is_free = true;
    //check if any adjacent block is also free and merge in that case
    tryToMerge(meta);
}

void* realloc(void* oldp, size_t size){
    if(oldp == NULL){
        return malloc(size);
    }
    if(size == 0 || size > MAX_MALLOC_SIZE){
        return NULL;
    }
    size += (size % 4 == 0) ? 0 : 4 - (size % 4); // Makes size be multiplication of 4
    meta_data* meta = (meta_data*)oldp - 1;
    //If current block is bigger try to split
    if(meta->block_size >= size){
        if(meta->block_size >= size + sizeof(Node) + sizeof(meta_data) + MIN_FOR_SPLIT) {
            Node* curr = getMetaNode(meta);
            assert(curr != NULL);
            split(curr, size);
        }
        return oldp;
    }
    else {
        // check if we can merge with next node before trying to malloc
        Node* curr_node = getMetaNode(meta);
        Node* next_node = curr_node->next();
        //try to merge curr with next nodes
        if(next_node != NULL){
            if(next_node->getData()->is_free && (meta->block_size + next_node->getData()->block_size +
                                                        sizeof(meta_data) + sizeof(Node) )>= size){
                merge(curr_node);
                // check if we need to split after the merge
                if(meta->block_size >= size + sizeof(Node) + sizeof(meta_data) + MIN_FOR_SPLIT) {
                    Node* curr = getMetaNode(meta);
                    assert(curr != NULL);
                    split(curr, size);
                }
                return oldp;
            }
        }
        else {
            // if this is the last node (next_node == NULL) - we need to check if there is an free block big enough
            // if there is, call malloc. if there isn't - expand the wilderness
            Node* curr = root.next();
            Node* last = &root;
            while(curr != NULL) {
                if(curr->getData()->is_free){
                    if(curr->getData()->block_size >= size){
                        //check if the block we found is big enough for splitting it
                        if(curr->getData()->block_size >= size + sizeof(Node) + sizeof(meta_data) + MIN_FOR_SPLIT) {
                            split(curr, size);
                        }
                        // we found new block for us :)
                        curr->getData()->is_free = false;
                        void* new_ptr = curr->getData() + 1;
                        //copies old block data to new block (even if the real user data is not at this size)
                        memcpy(new_ptr, oldp, meta->block_size);
                        meta->is_free = true; //free previous block
                        //check if any adjacent block is also free and merge in that case
                        tryToMerge(meta);
                        return new_ptr;
                    }
                }
                last = curr;
                curr = curr->next();
            }
            // if we reached here - expand the last block to avoid mem-copying
            void* last_space = sbrk(size - last->getData()->block_size);
            if(*(int*)last_space == -1){
                return NULL;
            }
            last->getData()->block_size = size;
            return last->getData() + 1;
        }
    }
    void* new_ptr = malloc(size);
    if(new_ptr == NULL){
        return NULL;
    }
    //copies old block data to new block (even if the real user data is not at this size)
    memcpy(new_ptr, oldp, meta->block_size);
    meta->is_free = true; //free previous block
    //check if any adjacent block is also free and merge in that case
    tryToMerge(meta);
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

size_t _size_meta_data(){
    return sizeof(meta_data)+sizeof(Node);
}

size_t _num_meta_data_bytes(){
    size_t allocated_blocks = _num_allocated_blocks();
    size_t meta_size = _size_meta_data();
    return allocated_blocks * meta_size;
}
