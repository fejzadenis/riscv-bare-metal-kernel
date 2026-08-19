#include "../inc/_thread.hpp"
#include "../inc/riscv.hpp"
#include "../inc/syscall_c.hpp"
#include "../lib/hw.h"

// Trenutno izvrsavana nit
_thread* _thread::running = nullptr;



// Kernel alokacija preko MemoryAllocator-a
void* _thread::operator new(size_t size) {
    return MemoryAllocator::memoryAllocate(size);
}
void _thread::operator delete(void* ptr) {
    MemoryAllocator::memoryDeallocate(ptr);
}

// Konstruktor za niti koje odmah ulaze u Scheduler
_thread::_thread(Body b, void* a, void* stackTop)
    : body(b), arg(a), finished(false), blocked(false), main(b == nullptr)
{
    uint64* sp = (uint64*)stackTop;
    sp--;
    *sp = (uint64)&_thread::threadWrapper;
    context.ra = (uint64)&_thread::threadWrapper;
    context.sp = (uint64)sp;
    stack = (uint64*)stackTop;
    Scheduler::put(this);
}


// Konstruktor za niti koje cekaju start()
_thread::_thread(Body b, void* a, void* stackTop, int /*tag*/)
    : body(b), arg(a), finished(false), blocked(false), main(false)
{
    uint64* sp = (uint64*)stackTop;
    sp--;
    *sp = (uint64)&_thread::threadWrapper;
    context.ra = (uint64)&_thread::threadWrapper;
    context.sp = (uint64)sp;
    stack = (uint64*)stackTop;
}

// Konstruktor glavne niti
_thread::_thread()
    : body(nullptr), arg(nullptr), finished(false), blocked(false), main(true)
{
    context.ra = 0;
    context.sp = 0;
    stack = nullptr;
}

// Fabricke metode
_thread* _thread::createThread(Body b, void* a, void* stackTop) {
    return new _thread(b, a, stackTop);
}
_thread* _thread::createThreadNoStart(Body b, void* a, void* stackTop) {
    return new _thread(b, a, stackTop, 0);
}
_thread* _thread::createMainThread() {
    return new _thread();
}

// Promena konteksta izmedju niti
void _thread::dispatch() {
    _thread* old = running;

    if (old && !old->isFinished() && !old->isBlocked()) {
        Scheduler::put(old);
    }

    running = Scheduler::get();


    // Main ostaje u S-mode, ostale niti prelaze u U-mode
    if (running->isMain()) {
        Riscv::ms_sstatus(Riscv::SSTATUS_SPP);
    } else {
        Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    }

    _thread::contextSwitch(&old->context, &running->context);
}

// Pocetna funkcija svake nove niti
void _thread::threadWrapper() {
    Riscv::popSppSpie();
    running->body(running->arg);

    if (!running->isFinished()) {
        running->setFinished(true);
        thread_dispatch();
    }
}