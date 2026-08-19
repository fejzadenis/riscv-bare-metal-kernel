#include "../inc/_thread.hpp"
#include "../inc/riscv.hpp"
#include "../inc/MemoryAllocator.hpp"
#include "../inc/syscall_c.hpp"
#include "../lib/hw.h"
#include "../lib/console.h"

extern void userMain();


int main() {

    // Inicijalizacija trap handler-
    Riscv::w_stvec((uint64)&Riscv::supervisorTrap);

    // Inicijalizacija memorije
    MemoryAllocator::initialisation();

    // Kreiranje glavne niti
    _thread* mainThread = _thread::createMainThread();
    _thread::running = mainThread;


    // Kreiranje korisnicke niti
    thread_t userThread = nullptr;
    thread_create(&userThread, (void(*)(void*))userMain, nullptr);

    // Omogucavanje prekida
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
    Riscv::ms_sie(Riscv::SIE_SEIE | Riscv::SIE_SSIE);

    // cekaj zavrssetak korisničke niti
    while (!userThread->isFinished()) {
        thread_dispatch();
    }

    return 0;
}