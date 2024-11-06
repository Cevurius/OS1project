//
// Created by os on 5/30/24.
//

#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_H
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_H
#include "../lib/hw.h"

class _memory_allocator {
public:
    struct Node{
        size_t size;
        Node* next;
    };

    static _memory_allocator* _get_instance();
    static void* _mem_alloc(size_t size);
    static int _mem_free(void* ptr);

private:
    static Node* head_free, *head_busy;
    static _memory_allocator* allocator;
};


#endif //PROJECT_BASE_V1_1_MEMORYALLOCATOR_H
