//
// Created by os on 5/31/24.
//

#ifndef PROJECT_BASE_V1_1_SYSCALL_CPP_H
#define PROJECT_BASE_V1_1_SYSCALL_CPP_H

#include "../lib/hw.h"
#include "../h/_thread.h"
#include "../h/_sem.h"


void* operator new (size_t);
void operator delete (void*);



class Thread{
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread();

    int start();

    void join();

    static void dispatch();
    static int sleep(time_t);




    friend void wrapperRun(void* p);
    thread_t getMyHandle() {return myHandle;}
    static int getID();


protected:
    Thread();
    virtual void run(){};


private:
    thread_t myHandle;
    void (*body)(void*); void* arg;


};

class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
    int tryWait();
private:
    sem_t myHandle;
};

class Console{
public:
    static char getc();
    static void putc(char);
};

class PeriodicThread : public Thread {
public:
    void terminate ();
protected:
    //PeriodicThread (time_t period);
    virtual void periodicActivation ();
private:
    void run() override;

    time_t period;
    bool flag;
};

#endif //PROJECT_BASE_V1_1_SYSCALL_CPP_H
