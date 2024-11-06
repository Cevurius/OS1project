//
// Created by os on 6/2/24.
//

#ifndef PROJECT_BASE_V1_1__SEM_H
#define PROJECT_BASE_V1_1__SEM_H

#include "List.h"
#include "_thread.h"

class _sem;
typedef _sem* sem_t;

class _sem {
public:
    friend class _thread;

    static sem_t _sem_open(unsigned init);

    static int _sem_close(sem_t handle);

    static int _sem_wait(sem_t id);

    static int _sem_signal(sem_t id);

    static int _sem_trywait(sem_t id);
private:
    int n;
    List<_thread>* blocked;

};


#endif //PROJECT_BASE_V1_1__SEM_H
