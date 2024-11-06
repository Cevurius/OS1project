//
// Created by os on 5/30/24.
//

#include "../lib/hw.h"
#include "../lib/console.h"
#include "../h/_memory_allocator.h"
#include "../h/syscall_c.h"
#include "../h/Riscv.h"
#include "../h/_thread.h"

#include "../test/printing.hpp"

extern "C" void trap();
void userMain();

void wrapperUserMain(void *){
    userMain();
}

int main(){

    _thread* threads[2];

    threads[0] = _thread::_thread_create(nullptr, nullptr);
    threads[1] = _thread::_thread_create(&wrapperUserMain, nullptr);
    threads[1]->_start();

    Riscv::w_stvec((uint64) &trap);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    _thread::running = threads[0];

    disable_interrupt();

    //_thread::_thread_join(threads[0]);
    thread_join(threads[1]);
    //::thread_join(threads[0]);

    /*while (!(threads[1]->_is_finished()))
     {
         _thread::_dispatch();
     }*/

    return 0;
}

