//
// Created by os on 5/30/24.
//

#ifndef PROJECT_BASE_V1_1__THREAD_H
#define PROJECT_BASE_V1_1__THREAD_H

#include "../lib/hw.h"
#include "_scheduler.h"
#include "Riscv.h"
#include "_sem.h"
#include "List.h"
//#include "../test/printing.hpp"

class _thread;
typedef _thread* thread_t;

class _thread {
public:
    friend class _sem;
    using Body = void (*)(void*);
private:
    struct Context{
        uint64 ra;
        uint64 sp;
    };
    Context context;
    Body body;
    uint64* stack;
    bool finished;
    bool blocked;
    void* arg;
    int myID;
    List<_thread>* joinedThreads;


    static void _thread_wrapper();
    static uint64 constexpr STACK_SIZE = DEFAULT_STACK_SIZE;
    static uint64 constexpr TIME_SLICE = DEFAULT_TIME_SLICE;

public:


    static int ID;
    static int _get_my_ID();
    static thread_t _thread_create(Body body, void* arg);
    static void _dispatch();
    static void _context_switch(Context* oldContext, Context* newContext);
    static void _set_thread();
    static void _thread_exit();
    static void _thread_join(thread_t thread);



    bool _is_finished() const {return finished;}
    void _set_finished(bool val) {finished = val;}

    bool _is_blocked() const {return blocked;}
    void _set_blocked(bool val) {blocked = val;}

    void _start();

    static thread_t running;
};


#endif //PROJECT_BASE_V1_1__THREAD_H
