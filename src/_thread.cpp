//
// Created by os on 5/30/24.
//

#include "../h/_thread.h"
#include "../h/_scheduler.h"
#include "../h/_memory_allocator.h"
#include "../test/printing.hpp"
#include "../h/syscall_cpp.hpp"

thread_t _thread::running = nullptr;
int _thread::ID = 0;


int _thread::_get_my_ID() {
    return running->myID;
}

void _thread::_dispatch() {
    thread_t old = running;
    if(!running->_is_blocked() && !running->_is_finished()){
        //printString("scheduler::put\n");
        _scheduler::_put(old);}
    running = _scheduler::_get();
    _context_switch(&old->context, &running->context);
}

void _thread::_thread_wrapper() {
    _set_thread();

    running->body(running->arg);
    running->_set_finished(true);
    //running->release();
    while(running->joinedThreads->peekFirst()){
        //printString("oslobadja se\n");
        thread_t thr = running->joinedThreads->removeFirst();
        thr->_set_blocked(running);
        _scheduler::_put(thr);
    }
    ::thread_exit();

}

void _thread::_set_thread() {
    uint64 sstatus;
    __asm__ volatile("csrr %0, sstatus": "=r"(sstatus));
    sstatus &= ~(1ULL << 8);
    sstatus |= (1ULL << 5);
    __asm__ volatile("csrw sstatus, %0": : "r"(sstatus));
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

thread_t _thread::_thread_create(_thread::Body body, void *arg) {
    //alociramo prostor za novu nit
    thread_t newThread = (_thread*)_memory_allocator::_get_instance()->_mem_alloc((sizeof(_thread) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
    if(!newThread) return nullptr;

    //postavimo joj ostale atribute
    //alociramo stek
    newThread->stack = body ? (uint64*)_memory_allocator::_get_instance()->_mem_alloc((DEFAULT_STACK_SIZE + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE) : 0;

    newThread->body = body;
    newThread->blocked = false, newThread->finished = false;
    newThread->arg = arg;
    /*for(int i = 0; i < 34; i++) newThread->context.x[i] = 0;*/
    newThread->context.ra = 0;
    newThread->context.sp = 0;
    newThread->context.ra = (uint64)&_thread_wrapper;
    newThread->context.sp = newThread->stack != nullptr ? (uint64) &(newThread->stack[STACK_SIZE]) : 0;
    newThread->myID = ID++;
    newThread->joinedThreads = List<_thread>::list_create();
    if(!newThread->joinedThreads) return nullptr;
    return newThread;
}

void _thread::_thread_exit() {
    if (!_thread::running ) return;
    _thread::running->_set_finished(true);
    //printString("usao u exit\n");
    thread_t old =_thread::running;
    _memory_allocator::_get_instance()->_mem_free(old->stack);
    _thread::_dispatch();

    //_sem::semClose(running->threadsToActivate);

    return;
}

void _thread::_start() {
    if (!running) running = this;
    if (this->body) {
        _scheduler::_put(this);
    }
}

void _thread::_thread_join(thread_t thread) {
    if(!thread->_is_finished()){
        _thread::running->_set_blocked(true);
        thread->joinedThreads->addLast(_thread::running);
        _thread::_dispatch();
    }
    /*while(!thread->_is_finished()){
        _thread::_dispatch();
    }*/
}




/*
int _thread::_thread_sleep(time_t t) {
    if(t == 0) return 0;
    SleepThread* newThread = (SleepThread*)_memory_allocator::_get_instance()->_mem_alloc((sizeof(SleepThread) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE );
    newThread->next = nullptr;
    newThread->time = t + sleep_timer;
    newThread->thr = _thread::running;

    //printInt(newThread->time);
    //printString("\n");


    if (!sleepThreads || sleepThreads->time > newThread->time) {
        //printString("Umetanje na prvo mesto! \n");
        newThread->next = sleepThreads;
        sleepThreads = newThread;
        _thread::running->_set_blocked(true);
    } else {
        //printString("Umetanje na neko mesto unutra! \n");
        SleepThread* current = sleepThreads;
        while (current->next && current->next->time <= newThread->time) {
            current = current->next;
        }
        newThread->next = current->next;
        current->next = newThread;
        _thread::running->_set_blocked(true);
    }

    _thread::_dispatch();
    return 0;
}
void _thread::_thread_awake() {

    while (sleepThreads && sleepThreads->time == sleep_timer) {
        SleepThread* temp = sleepThreads;
        sleepThreads = sleepThreads->next;
        temp->thr->_set_blocked(false);
        _scheduler::_put(temp->thr);
        _memory_allocator::_get_instance()->_mem_free(temp);
    }

}*/