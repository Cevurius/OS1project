//
// Created by os on 5/30/24.
//

#include "../h/_memory_allocator.h"

_memory_allocator* _memory_allocator::allocator = nullptr;
_memory_allocator::Node* _memory_allocator::head_free = nullptr;
_memory_allocator::Node* _memory_allocator::head_busy = nullptr;

_memory_allocator *_memory_allocator::_get_instance() {
    if(!allocator){
        head_free = (Node*)((size_t)HEAP_START_ADDR + sizeof(_memory_allocator));
        head_free->size = (size_t)HEAP_END_ADDR - (size_t)HEAP_START_ADDR - sizeof(Node) - sizeof(_memory_allocator);
        head_free->next = nullptr;
        allocator = (_memory_allocator*)HEAP_START_ADDR;
    }
    return allocator;
}

void *_memory_allocator::_mem_alloc(size_t size) {
    if(size == 0) return nullptr;
    size_t var = size * MEM_BLOCK_SIZE + sizeof(Node);
    Node* cur = head_free, *prev = nullptr;
    while(cur){
        if (cur->size < var - sizeof(Node)){
            prev = cur; cur = cur->next;
        }
        else{
            //ako u cur ima prostora za jos jedan blok, njega treba dodati u listu slobodnih
            if(cur->size - MEM_BLOCK_SIZE >= var){
                Node* block = (Node*)((size_t)cur + var);
                block->next = cur->next;
                block->size = cur->size - var;
                if(!prev) head_free = block;
                else prev->next = block;
                cur->size = var - sizeof(Node);
            } //ako nema, uzimamo ceo cur
            else{
                if(!prev) head_free = cur->next;
                else prev->next = cur->next;
            }
            //dodajem u listu zauzetih
            cur->next = head_busy;
            head_busy = cur;
            return (void*)((size_t)cur + sizeof(Node));
        }
    }
    head_free = nullptr;
    return nullptr;
}

int _memory_allocator::_mem_free(void *ptr) {
    if(!ptr) return 0;
    if(!head_busy) return -1;

    Node* p = head_busy;
    Node* q = nullptr;
    uint flag = 0;
    Node* block = (Node*)(((size_t)ptr) - sizeof(Node));
    for(; p ; q = p, p=p->next){
        if(p == block){
            if(q) q->next = p->next;
            else head_busy = p->next;
            flag = 1;
            break;
        }
    }
    //ako nije pronasao blok, vrati neg. vrednost
    if(!flag) return -2;

    //ako je lista slobodnih prazna
    if(!head_free){
        head_free = block;
        head_free->next = nullptr;
        return 0;
    }

    //ako se blok nalazi na memorijski nizoj lokaciji od headFree
    if(block < head_free){
        if(((size_t)block) + block->size + sizeof(Node) != (size_t)head_free){
            block->next = head_free;
            head_free = block;
        } //ako je blok ipak tacne velicine da moze da se spoji sa headFree, spojimo
        else{
            block->size = block->size + sizeof(Node) + head_free->size;
            block->next = head_free->next;
            head_free = block;
        }
        return 0;
    }

    //nalazimo poziciju nakon koje sledi mesto za block
    for(p = head_free; p != nullptr; p = p->next){
        if(!p->next) break;
        if(p->next && block < p->next) break;
    }

    if(!p) return -1;

    if((p + sizeof(Node) + p->size) != block){
        block->next = p->next;
        p->next = block;
    } //ako je blok ipak tacne velicine da moze da se spoji sa p, spojimo
    else{
        p->size = p->size + sizeof(Node) + block->size;
        block = p;
    }
    //ako mozemo block da spojimo sa sledecim u listi, spojimo ga
    if(block->next){
        if( (block + sizeof(Node) + block->size) == block->next){
            block->size = block->size + sizeof(Node) + block->next->size;
            block->next = block->next->next;
        }
    }
    return 0;
}