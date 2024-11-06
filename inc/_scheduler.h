//
// Created by os on 5/30/24.
//

#ifndef PROJECT_BASE_V1_1_SCHEDULER_H
#define PROJECT_BASE_V1_1_SCHEDULER_H

class _thread;

class _scheduler {
public:
    static _thread* _get();
    static void _put(_thread* thr);

    struct Elem{
        _thread* thr;
        Elem* next;
    };
    static Elem* head;
    static Elem* tail;
};

#endif //PROJECT_BASE_V1_1_SCHEDULER_H
