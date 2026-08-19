//
// Created by os on 6/18/26.
//

#ifndef PROJECT_BASE_V1_1_1__THREAD_HPP
#define PROJECT_BASE_V1_1_1__THREAD_HPP


#include "../lib/hw.h"
#include "scheduler.hpp"
#include "MemoryAllocator.hpp"

class _thread {
public:
    using Body = void (*)(void*);

    struct Context {
        uint64 ra;
        uint64 sp;
    };


    // Fabricke metode
    static _thread* createThread(Body body, void* arg, void* stackTop);
    static _thread* createThreadNoStart(Body body, void* arg, void* stackTop);  // kreira, ne stavlja
    static _thread* createMainThread();                         // main nit (bez steka)

    ~_thread() {
        if (stack) {
            MemoryAllocator::memoryDeallocate(
                (void*)((char*)stack - DEFAULT_STACK_SIZE)
            );
        }
    }

    bool isFinished() const { return finished; }
    void setFinished(bool v) { finished = v; }
    bool isBlocked()  const { return blocked; }
    void setBlocked(bool v)  { blocked = v; }
    bool isMain()     const  { return main; }

    static void dispatch();
    static void contextSwitch(Context* old, Context* newCtx);

    static _thread* running;

    Context context;

private:
    _thread(Body body, void* arg, void* stackTop);          // regularni ctor (stavi u scheduler)
    _thread(Body body, void* arg, void* stackTop, int);     // no-start ctor
    _thread();                              // main ctor (nema steka)

    static void threadWrapper();


    void* operator new(size_t size);
    void  operator delete(void* ptr);

    Body    body;
    void*   arg;
    uint64* stack    = nullptr;
    bool    finished = false;
    bool    blocked  = false;
    bool    main     = false;

    friend class _sem;
};
#endif //PROJECT_BASE_V1_1_1__THREAD_HPP
