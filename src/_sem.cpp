//
// Created by os on 6/2/24.
//

#include "../h/_sem.h"
#include "../h/_memory_allocator.h"
#include "../h/_thread.h"
#include "../h/_scheduler.h"

sem_t _sem::_sem_open(unsigned int init) {
    sem_t newSem = (sem_t)_memory_allocator::_get_instance()->_mem_alloc((sizeof(_sem) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
    if(!newSem) return nullptr;
    newSem->n = init;
    newSem->blocked = List<_thread>::list_create();
    if(!newSem->blocked) return nullptr;
    return newSem;
}

int _sem::_sem_wait(sem_t id) {
    if(!id) return -1;
    id->n--;
    if(id->n < 0) {
        thread_t old = _thread::running;
        _thread::running->_set_blocked(true);
        id->blocked->addLast(_thread::running);
        _thread::running = _scheduler::_get();
        _thread::_context_switch(&old->context, &_thread::running->context);
    }
    return 0;
}

int _sem::_sem_signal(sem_t id) {
    if(!id) return -1;
    id->n++;
    if(id->n <= 0) {
        thread_t t = id->blocked->peekFirst();
        id->blocked->removeFirst();
        t->_set_blocked(false);
        _scheduler::_put(t);
    }
    return 0;
}

int _sem::_sem_trywait(sem_t id) {
    if(!id) return -1;
    id->n--;
    if(id->n < 0) {
        return 0;
    }
    return 1;
}

int _sem::_sem_close(sem_t handle) {
    if(!handle) return -1;
    while(handle->blocked->peekFirst() != nullptr){
        thread_t t = handle->blocked->peekFirst();
        handle->blocked->removeFirst();
        t->_set_blocked(false);
        _scheduler::_put(t);
    }
    _memory_allocator::_get_instance()->_mem_free(handle);
    return 0;
}

