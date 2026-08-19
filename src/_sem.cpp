#include "../inc/_sem.hpp"
#include "../inc/syscall_c.hpp"
#include "../inc/MemoryAllocator.hpp"

// Kernel alokacija ide direktno preko MemoryAllocator-a
void* _sem::operator new(size_t size) {
    return MemoryAllocator::memoryAllocate(size);
}
void _sem::operator delete(void* ptr) {
    MemoryAllocator::memoryDeallocate(ptr);
}
void* _sem::operator new[](size_t size) {
    return MemoryAllocator::memoryAllocate(size);
}
void _sem::operator delete[](void* ptr) {
    MemoryAllocator::memoryDeallocate(ptr);
}

// Fabricke metode za kreiranje i unistavanje semafora
_sem* _sem::createSemaphore(unsigned init) {
    return new _sem(init);
}
void _sem::destroySemaphore(_sem* sem) {
    delete sem;
}

// Jednostavni wrapperi
int _sem::wait()   { return wait_n(1); }
int _sem::signal() { return signal_n(1); }

// Zauzima n resursa ili blokira nit
int _sem::wait_n(unsigned n) {
    if (closed) return -1;
    if (n == 0)  return 0;

    if ((int)value >= (int)n) {
        value -= (int)n;
        return 0;
    }

    WaitEntry* entry = (WaitEntry*)MemoryAllocator::memoryAllocate(sizeof(WaitEntry));
    if (!entry) return -1;
    entry->thread = _thread::running;
    entry->needed = n;
    blocked.addLast(entry);

    _thread::running->setBlocked(true);
    thread_dispatch();

    if (closed) return -1;
    return 0;
}

// Dodaje resurse i budi niti po FIFO principu
int _sem::signal_n(unsigned n) {
    if (closed) return -1;
    value += (int)n;

    while (true) {
        WaitEntry* entry = blocked.peekFirst();
        if (!entry) break;

        if ((int)entry->needed <= value) {
            blocked.removeFirst();
            value -= (int)entry->needed;
            _thread* t = entry->thread;
            MemoryAllocator::memoryDeallocate(entry);
            t->setBlocked(false);
            Scheduler::put(t);
        } else {
            break;
        }
    }
    return 0;
}

// Zatvara semafor i budi sve blokirane niti
int _sem::close() {
    closed = true;
    signalAll();
    return 0;
}

// Interno budjenje svih niti
int _sem::signalAll() {
    while (blocked.peekFirst() != nullptr) {
        WaitEntry* entry = blocked.removeFirst();
        _thread* t = entry->thread;
        MemoryAllocator::memoryDeallocate(entry);
        t->setBlocked(false);
        Scheduler::put(t);
    }
    return 0;
}