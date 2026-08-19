//
// Created by os on 6/18/26.
//

#ifndef PROJECT_BASE_V1_1_1__SEM_HPP
#define PROJECT_BASE_V1_1_1__SEM_HPP
#include "_thread.hpp"
#include "list.hpp"

class _sem {
public:
    explicit _sem(unsigned init = 1) : value((int)init), closed(false) {}
    ~_sem() { if (!closed) close(); }

    int wait();
    int signal();

    // Novi sistemski pozivi 2026
    int wait_n(unsigned n);
    int signal_n(unsigned n);

    int  close();
    bool isClosed() const { return closed; }
    int  getValue()  const { return value; }

    static _sem* createSemaphore(unsigned init = 1);
    static void  destroySemaphore(_sem* sem);

private:
    int  value;
    bool closed;

    // Blokirana nit + broj jedinica koje čeka
    struct WaitEntry {
        _thread* thread;
        unsigned needed;
    };
    List<WaitEntry> blocked;


    int  signalAll();
    void setClosed(bool v) { closed = v; }

    // Interni new/delete koji koriste MemoryAllocator
    void* operator new(size_t size);
    void  operator delete(void* ptr);
    void* operator new[](size_t size);
    void  operator delete[](void* ptr);
};


#endif //PROJECT_BASE_V1_1_1__SEM_HPP
