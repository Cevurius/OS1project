//
// Created by os on 5/31/24.
//

#include "../lib/hw.h"
#include "../h/_memory_allocator.h"
#include "../h/syscall_c.h"
#include "../h/_sem.h"
#include "../h/Riscv.h"
#include "../h/_thread.h"
#include "../lib/console.h"


enum sys_calls{
    MEM_ALLOC = 0x01,
    MEM_FREE = 0x02,
    THREAD_CREATE = 0x11,
    THREAD_EXIT = 0x12,
    THREAD_DISPATCH = 0x13,
    THREAD_CREATE_ONLY = 0x14,
    THREAD_START = 0x15,
    THREAD_JOIN = 0x16,
    GET_THREAD_ID = 0x17,
    SEM_OPEN = 0x21,
    SEM_CLOSE = 0x22,
    SEM_WAIT = 0x23,
    SEM_SIGNAL = 0x24,
    SEM_TRYWAIT = 0x26,
    TIME_SLEEP = 0x31,
    GETC = 0x41,
    PUTC = 0x42,
    DISABLE_INTERRUPT = 0x60,
};

void call_ecall(int serviceId, ... ){
    __asm__ volatile("ecall");
}

extern "C" void trap();

extern "C" void trapHandler() {

    uint64 scause = Riscv::r_scause();
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {
        uint64 volatile sepc = 0;
        __asm__ volatile ("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
        sepc +=4;

        uint64 volatile status = Riscv::r_sstatus();

        size_t syscall;
        __asm__ volatile("mv %0, a0": "=r" (syscall));

        if (syscall == MEM_ALLOC){
            size_t size;
            __asm__ volatile("mv %0, a1": "=r"(size));

            void* ret = _memory_allocator::_get_instance()->_mem_alloc(size);
            __asm__ volatile("sd a0, 10*8(fp)");

            __asm__ volatile("mv a0, %0" : : "r"(ret));
        }
        else if(syscall == MEM_FREE)
        {
            void *p;
            __asm__ volatile("mv %0, a1": "=r"(p));
            _memory_allocator::_get_instance()->_mem_free(p);
        }
        else if(syscall == THREAD_CREATE)
        {

            thread_t* handle = nullptr;
            __asm__ volatile("mv %0, s1": "=r"(handle));
            _thread::Body body;
            __asm__ volatile("mv %0, s2": "=r"(body));
            void* arg;
            __asm__ volatile("mv %0, s3": "=r"(arg));


            *handle = _thread::_thread_create(body,arg);
            (*handle)->_start();

            int ret = 0;
            if (!handle) { ret = -1;}

            __asm__ volatile("mv a0, %0" : :"r"(ret));

        }
        else if(syscall == THREAD_EXIT){
            _thread::_thread_exit();
        }
        else if(syscall== THREAD_DISPATCH)
        {
            _thread::_dispatch();

        }
        else if(syscall == THREAD_JOIN){
            thread_t handle = nullptr;
            __asm__ volatile("mv %0, a1": "=r"(handle));

            _thread::_thread_join(handle);
        }
        else if(syscall == THREAD_CREATE_ONLY)
        {

            thread_t* handle = nullptr;
            __asm__ volatile("mv %0, s1": "=r"(handle));
            _thread::Body body;
            __asm__ volatile("mv %0, s2": "=r"(body));
            void* arg;
            __asm__ volatile("mv %0, s3": "=r"(arg));

            *handle = _thread::_thread_create(body,arg);

            int ret = 0;
            if (!handle) {ret = -1;}
            __asm__ volatile("mv a0, %0" : :"r"(ret));
        }
        else if(syscall == THREAD_START)
        {
            thread_t handle = nullptr;
            __asm__ volatile("mv %0, a1": "=r"(handle));

            handle->_start();
        }
        else if(syscall==SEM_OPEN){
            sem_t* handle = nullptr;
            __asm__ volatile("mv %0, a1": "=r"(handle));
            unsigned init = 0;
            __asm__ volatile("mv %0, a2": "=r"(init));

            *handle = _sem::_sem_open(init);
            __asm__ volatile("mv a0, %0" : :"r"(handle));
        }
        else if(syscall==SEM_CLOSE){
            sem_t handle = nullptr;
            __asm__ volatile("mv %0, a1": "=r"(handle));

            int ret = _sem::_sem_close(handle);
            __asm__ volatile("mv a0, %0" : :"r"(ret));

        }
        else if(syscall==SEM_WAIT){
            sem_t id = nullptr;
            __asm__ volatile("mv %0, a1": "=r"(id));

            int ret = _sem::_sem_wait(id);
            __asm__ volatile("mv a0, %0" : :"r"(ret));
        }
        else if(syscall==SEM_SIGNAL){
            sem_t id = nullptr;
            __asm__ volatile("mv %0, a1": "=r"(id));

            int ret = _sem::_sem_signal(id);
            __asm__ volatile("mv a0, %0" : :"r"(ret));
        }
        else if(syscall==SEM_TRYWAIT){
            sem_t id = nullptr;
            __asm__ volatile("mv %0, a1": "=r"(id));

            int ret = _sem::_sem_trywait(id);
            __asm__ volatile("mv a0, %0" : :"r"(ret));
        }
        else if(syscall==PUTC){
            char c;
            __asm__ volatile("mv %0, a1": "=r"(c));
            __putc(c);

        }
        else if(syscall == GETC){
            char c = __getc();
            __asm__ volatile("sd a0, 10*8(fp)");
            __asm__ volatile("mv a0, %0" : : "r"(c));

        }
        else if(syscall == GET_THREAD_ID){
            int ret;
            ret = _thread::_get_my_ID();
            __asm__ volatile("sd a0, 10*8(fp)");
            __asm__ volatile ("mv a0, %0" : : "r"(ret));
        }
        else if(syscall == TIME_SLEEP){
            time_t t = 0;
            __asm__ volatile("mv %0, a1": "=r"(t));

            //int ret = _thread::_thread_sleep(t);
            int ret = 0;
            __asm__ volatile("mv a0, %0" : :"r"(ret));
        }


        Riscv::w_sepc(sepc);
        Riscv::w_sstatus(status);
    } else if (scause == 0x8000000000000001UL) {
        /*_thread::time_slice_counter++;
        _thread::sleep_timer++;
        _thread::_thread_awake();*/
        Riscv::mc_sip(Riscv::SIP_SSIP);


/*
        if (_thread::time_slice_counter >= _thread::running->_get_time_slice()) {
            _thread::time_slice_counter = 0;
            _thread::_dispatch();
        }*/

    } else if (scause == 0x8000000000000009UL) {

        console_handler();

    } else if (scause == 2){
        __putc('x');
        __putc('\n');


    }
    else {
    }
}




void* mem_alloc(size_t size){
    size_t numOfBlocks = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;

    call_ecall(MEM_ALLOC, numOfBlocks);

    void* p = nullptr;
    __asm__ volatile("mv a0, %0": "=r"(p));
    return p;

}

int mem_free(void *p){
    call_ecall(MEM_FREE, p);

    uint64 ret;
    __asm__ volatile("mv a0, %0": "=r"(ret));
    return ret;
}

int thread_exit(){
    call_ecall(THREAD_EXIT);

    uint64 ret;
    __asm__ volatile("mv a0, %0": "=r"(ret));
    return ret;
}

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg){
    void * stack = _memory_allocator::_get_instance()->_mem_alloc((DEFAULT_STACK_SIZE+MEM_BLOCK_SIZE-1)/MEM_BLOCK_SIZE);
    if (!stack) return -1;

    call_ecall(THREAD_CREATE, handle, start_routine, arg);

    uint64 ret;
    __asm__ volatile("mv a0, %0": "=r"(ret));
    return ret;
}

void thread_dispatch(){
    call_ecall(THREAD_DISPATCH);
}

void thread_create_only(thread_t* handle,
                        void(*start_routine)(void*),
                        void* arg){
    void * stack = _memory_allocator::_get_instance()->_mem_alloc((DEFAULT_STACK_SIZE+MEM_BLOCK_SIZE-1)/MEM_BLOCK_SIZE);
    if (!stack) return ;

    call_ecall(THREAD_CREATE_ONLY, handle, start_routine, arg);

    uint64 ret;
    __asm__ volatile("mv a0, %0": "=r"(ret));
}

void thread_start(thread_t handle){
    call_ecall(THREAD_START, handle);
}

void thread_join(thread_t handle)
{
    call_ecall(THREAD_JOIN, handle);
}

int sem_open(sem_t* handle, unsigned init){
    call_ecall(SEM_OPEN, handle, init);

    int ret;
    __asm__ volatile("mv a0, %0": "=r"(ret));
    return ret;
}

int sem_close(sem_t handle){
    call_ecall(SEM_CLOSE, handle);

    int ret;
    __asm__ volatile ("mv a0, %0": "=r"(ret));
    return ret;
}

int sem_wait(sem_t id){
    call_ecall(SEM_WAIT, id);

    int ret;
    __asm__ volatile ("mv a0, %0": "=r"(ret));
    return ret;
}

int sem_signal(sem_t id){
    call_ecall(SEM_SIGNAL, id);

    int ret;
    __asm__ volatile ("mv a0, %0": "=r"(ret));
    return ret;
}

int sem_trywait(sem_t id){
    call_ecall(SEM_TRYWAIT, id);

    int ret;
    __asm__ volatile ("mv a0, %0": "=r"(ret));
    return ret;
}

int time_sleep (time_t t){
    call_ecall(TIME_SLEEP, t);

    int ret;
    __asm__ volatile ("mv a0, %0": "=r"(ret));
    return ret;
}

void putc(char c){
    call_ecall(PUTC, c);
}

char getc(){
    call_ecall(GETC);
    char ret;
    __asm__ volatile ("mv %0, a0": "=r"(ret));
    return ret;
}

void disable_interrupt()
{
    call_ecall(DISABLE_INTERRUPT);
}

int get_thread_id() {
    call_ecall(GET_THREAD_ID);

    uint64 ret;
    __asm__ volatile("mv %0, a0" : "=r"(ret));
    return (int)ret;
}