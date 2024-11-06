//
// Created by os on 5/31/24.
//
#include "../h/syscall_cpp.hpp"
#include "../h/_thread.h"
#include "../lib/hw.h"
#include "../h/syscall_c.h"





void wrapperRun(void* p){
    if(!p) return;
    ((Thread*)p)->run();
}

void* operator new(size_t size){
    return mem_alloc(size);

}

void operator delete(void * p){
    mem_free(p);
}



Thread::Thread(void (*body)(void *), void *arg):body(body), arg(arg){
    thread_create(&myHandle, body, arg);
}

Thread::~Thread() {
    //thread_exit();
    mem_free(this);

}


int Thread::start() {
    thread_start(this->myHandle);
    return 0;
}

void Thread::dispatch() {
    thread_dispatch();
}
void Thread::join() {
    thread_join(myHandle);

}

int Thread::getID(){
    return get_thread_id();
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

Thread::Thread(): body(nullptr), arg(nullptr){

    thread_create_only(&myHandle,wrapperRun,this);
}



int Semaphore::tryWait() {
    if(myHandle){
        return sem_trywait(myHandle);
    }
    return -1;
}
Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    if (myHandle) {
        sem_close(myHandle);
    }
}

int Semaphore::wait() {
    if (myHandle) {
        return sem_wait(myHandle);
    }
    return -1;
}

int Semaphore::signal() {
    if (myHandle) {
        return sem_signal(myHandle);
    }
    return -1;
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}

void PeriodicThread::terminate() {

}


void PeriodicThread::periodicActivation() {

}

void PeriodicThread::run() {

}