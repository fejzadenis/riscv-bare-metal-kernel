#include "../inc/syscall_cpp.hpp"
#include "../inc/syscall_c.hpp"
#include "../inc/scheduler.hpp"
#include "../inc/_thread.hpp"

// C++ API kao omotač oko C API-ja

// Globalni new/delete koriste kernel alokator
void* operator new(size_t size)    { return mem_alloc(size); }
void  operator delete(void* ptr)   { mem_free(ptr); }
void* operator new[](size_t size)  { return mem_alloc(size); }
void  operator delete[](void* ptr) { mem_free(ptr); }

// wrapper poziva body() ili virtuelnu run(), u zavisnosti od načina kreiranja niti
void Thread::wrapper(void* p) {
    if (p) {
        Thread* t = (Thread*)p;
        if (t->body) t->body(t->arg);
        else         t->run();
    }
}

// Konstruktor niti sa funkcijom
Thread::Thread(void (*b)(void*), void* a)
    : myHandle(nullptr), body(b), arg(a)
{
    thread_create_no_start(&myHandle, b, a);
}

// Konstruktor za nasleđivanje i run()
Thread::Thread()
    : myHandle(nullptr), body(nullptr), arg(nullptr)
{
    thread_create_no_start(&myHandle, wrapper, this);
}

Thread::~Thread() {
    myHandle = nullptr;
}

// Pokretanje niti
int Thread::start() {
    if (!myHandle) return -1;
    Scheduler::put((_thread*)myHandle);
    return 0;
}

void Thread::dispatch()      { thread_dispatch(); }
int Thread::sleep(time_t t)  { return time_sleep(t); }

// Semafor
Semaphore::Semaphore(unsigned init) : myHandle(nullptr) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    if (myHandle) {
        sem_close(myHandle);
        myHandle = nullptr;
    }
}

int Semaphore::wait()   { return sem_wait(myHandle); }
int Semaphore::signal() { return sem_signal(myHandle); }

// Periodična nit
PeriodicThread::PeriodicThread(time_t p) : Thread(), period(p) {}

void PeriodicThread::terminate() {
    thread_t h = getHandle();
    if (h) ((_thread*)h)->setFinished(true);
}

// Konzola
char Console::getc()       { return ::getc(); }
void Console::putc(char c) { ::putc(c); }