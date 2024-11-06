//
// Created by os on 5/31/24.
//

#ifndef PROJECT_BASE_V1_1_SYSCALL_C_H
#define PROJECT_BASE_V1_1_SYSCALL_C_H

#include "../lib/hw.h"
#include "../h/_thread.h"
#include "../h/_sem.h"


void* mem_alloc(size_t size);

int mem_free(void *p);

int thread_create( thread_t* handle, void(*start_routine)(void*),void* arg);

int thread_exit();

void thread_dispatch();

void thread_create_only(thread_t* handle, void(*start_routine)(void*), void* arg);

void thread_start(thread_t t);

void thread_join(thread_t handle);

int get_thread_id();

int sem_open(sem_t* handle, unsigned init);

int sem_close(sem_t handle);

int sem_wait(sem_t id);

int sem_signal(sem_t id);

int sem_trywait(sem_t id);

typedef unsigned long time_t;
int time_sleep (time_t);

const int EOF = -1;
void putc(char);

char getc();

void disable_interrupt();

#endif //PROJECT_BASE_V1_1_SYSCALL_C_H
