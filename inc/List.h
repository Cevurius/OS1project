//
// Created by os on 5/30/24.
//

#ifndef PROJECT_BASE_V1_1_LIST_H
#define PROJECT_BASE_V1_1_LIST_H


#include "_memory_allocator.h"
//#include "../test/printing.hpp"

template<typename T>
class List{
private:
    struct Elem{
        T *data;
        Elem *next;
    };

    Elem *head, *tail;

public:
    List() : head(0), tail(0) {}
    List(const List<T> &) = delete;
    List<T> &operator=(const List<T> &) = delete;

    static List* list_create() {
        List<T>* list = (List<T>*)_memory_allocator::_get_instance()->_mem_alloc((sizeof(List<T>) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE);
        if (!list) return nullptr;
        list->head = nullptr;
        list->tail = nullptr;

        return list;
    }

    void addFirst(T *data){
        Elem* elem = (Elem*) _memory_allocator::_get_instance()->_mem_alloc((sizeof(Elem)+MEM_BLOCK_SIZE-1)/MEM_BLOCK_SIZE);
        elem->data = data;
        elem->next = head;
        head = elem;
        if (!tail) tail = head;
    }

    void addLast(T *data) {
        //printString("uslo");
        Elem *elem =(Elem*)_memory_allocator::_get_instance()->_mem_alloc((sizeof(Elem)+MEM_BLOCK_SIZE-1)/MEM_BLOCK_SIZE);
        elem->data = data;
        elem->next = nullptr;
        if (tail){
            tail->next = elem;
            tail = elem;
        } else{
            head = tail = elem;
        }
    }

    T *removeFirst() {
        if (!head) return 0;

        Elem *elem = head;
        head = head->next;
        if (!head) tail = 0;

        T *ret = elem->data;
        _memory_allocator::_get_instance()->_mem_free(elem);
        return ret;
    }

    T *peekFirst() {
        if (!head) return 0;
        return head->data;
    }

    T *removeLast() {
        if (!head) return 0;

        Elem *prev = 0;
        for (Elem *curr = head; curr && curr != tail; curr = curr->next){
            prev = curr;
        }

        Elem *elem = tail;
        if (prev) prev->next = 0;
        else head = 0;
        tail = prev;

        T *ret = elem->data;
        _memory_allocator::_get_instance()->_mem_free(elem);
        return ret;
    }

    T *peekLast() {
        if (!tail) return 0;
        return tail->data;
    }

};

#endif //PROJECT_BASE_V1_1_LIST_H
