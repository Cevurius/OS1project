//
// Created by os on 5/30/24.
//
#include "../h/_scheduler.h"
#include "../h/_memory_allocator.h"

_scheduler::Elem* _scheduler::head = nullptr;
_scheduler::Elem* _scheduler::tail = nullptr;

_thread *_scheduler::_get() {
    if(!head) return nullptr;
    Elem* elem = head;
    head = head->next;
    if(!head) tail = 0;
    _thread* ret = elem->thr;
    _memory_allocator::_get_instance()->_mem_free(elem);
    return ret;
}

void _scheduler::_put(_thread *thr) {
    Elem* elem = (Elem*) _memory_allocator::_get_instance()->_mem_alloc((sizeof(Elem) + MEM_BLOCK_SIZE - 1) /  MEM_BLOCK_SIZE);
    elem->thr = thr;
    elem->next = nullptr;
    if(tail){
        tail->next = elem;
        tail = elem;
    }
    else{
        head = tail = elem;
    }
}