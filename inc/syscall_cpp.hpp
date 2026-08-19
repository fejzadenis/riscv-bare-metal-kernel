//
// Created by os on 6/18/26.
//

#ifndef PROJECT_BASE_V1_1_1_SYSCALL_CPP_HPP
#define PROJECT_BASE_V1_1_1_SYSCALL_CPP_HPP

#include "syscall_c.hpp"

void* operator new(size_t);
void  operator delete(void*);

class Thread {
public:
    Thread(void (*body)(void*), void* arg);
    virtual ~Thread();
    int start();
    static void dispatch();
    static int  sleep(time_t);


protected:
    Thread();
    virtual void run() {}
    thread_t getHandle() const { return myHandle; }

private:
    thread_t myHandle;
    void   (*body)(void*);
    void*    arg;

    static void wrapper(void* p);
};

class Semaphore {
public:
    explicit Semaphore(unsigned init = 1);
    virtual ~Semaphore();
    int wait();
    int signal();

private:
    sem_t myHandle;
};

class PeriodicThread : public Thread {
public:
    void terminate();

protected:
    explicit PeriodicThread(time_t period);
    virtual void periodicActivation() {}

private:
    time_t period;
};

class Console {
public:
    static char getc();
    static void putc(char);
};


#endif //PROJECT_BASE_V1_1_1_SYSCALL_CPP_HPP
