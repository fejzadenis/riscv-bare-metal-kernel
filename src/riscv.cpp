#include "../inc/riscv.hpp"
#include "../inc/_thread.hpp"
#include "../inc/_sem.hpp"
#include "../inc/syscall_c.hpp"
#include "../inc/MemoryAllocator.hpp"
#include "../lib/console.h"
#include "../lib/hw.h"

// Prelazak iz S-mode u U-mode pri prvom pokretanju niti
void Riscv::popSppSpie() {
    __asm__ volatile("csrw sepc, ra");
    mc_sstatus(SSTATUS_SPP);
    ms_sstatus(SSTATUS_SPIE);
    __asm__ volatile("sret");
}

// Obrada svih trap-ova: syscall, prekidi i izuzeci
void Riscv::handleSupervisorTrap() {

    // Sačuvaj registre sa argumentima syscall-
    uint64 a0val, a1val, a2val, a3val, a7val;
    __asm__ volatile("mv %0, a0" : "=r"(a0val));
    __asm__ volatile("mv %0, a1" : "=r"(a1val));
    __asm__ volatile("mv %0, a2" : "=r"(a2val));
    __asm__ volatile("mv %0, a3" : "=r"(a3val));
    __asm__ volatile("mv %0, a7" : "=r"(a7val));

    uint64 scause = r_scause();

    // Obrada sistemskih poziva
    if (scause == 0x0000000000000008UL ||
        scause == 0x0000000000000009UL)
    {
        uint64 volatile sepc    = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        switch (a0val) {

            // Memorijski pozivi
            case 0x01: {
                size_t bytes = (size_t)a1val * MEM_BLOCK_SIZE;
                w_a0((uint64)MemoryAllocator::memoryAllocate(bytes));
                break;
            }
            case 0x02: {
                w_a0((uint64)MemoryAllocator::memoryDeallocate((void*)a1val));
                break;
            }

            // Operacije nad nitima
            case 0x11: {
                _thread** handle = (_thread**)a1val;
                *handle = _thread::createThread(
                    (_thread::Body)a2val, (void*)a7val, (void*)a3val);
                w_a0(*handle ? 0UL : (uint64)-1UL);
                break;
            }
            case 0x12: {
                _thread::running->setFinished(true);
                _thread::dispatch();
                w_a0(0);
                break;
            }
            case 0x13: {
                _thread::dispatch();
                break;
            }
            case 0x14: {
                _thread** handle = (_thread**)a1val;
                *handle = _thread::createThreadNoStart(
                    (_thread::Body)a2val, (void*)a7val, (void*)a3val);
                w_a0(*handle ? 0UL : (uint64)-1UL);
                break;
            }

            // Operacije nad semaforima
            case 0x21: {
                sem_t* handle = (sem_t*)a1val;
                *handle = _sem::createSemaphore((unsigned)a2val);
                w_a0(*handle ? 0UL : (uint64)-1UL);
                break;
            }
        case 0x22: {
                    sem_t handle = (sem_t)a1val;
                    if (!handle) { w_a0((uint64)-1UL); break; }
                    int ret = handle->close();   // samo zatvori i probudi, NE briši
                    w_a0((uint64)ret);
                    break;
        }
            case 0x23: {
                sem_t id = (sem_t)a1val;
                w_a0(id ? (uint64)id->wait() : (uint64)-1UL);
                break;
            }
            case 0x24: {
                sem_t id = (sem_t)a1val;
                w_a0(id ? (uint64)id->signal() : (uint64)-1UL);
                break;
            }
            case 0x25: {
                sem_t id = (sem_t)a1val;
                w_a0(id ? (uint64)id->wait_n((unsigned)a2val) : (uint64)-1UL);
                break;
            }
            case 0x26: {
                sem_t id = (sem_t)a1val;
                w_a0(id ? (uint64)id->signal_n((unsigned)a2val) : (uint64)-1UL);
                break;
            }

            // Sleep (nisam ruadio)
            case 0x31: {
                w_a0(0);
                break;
            }

            // Konzola
            case 0x41: {
                w_a0((uint64)__getc());
                break;
            }
            case 0x42: {
                __putc((char)a1val);
                break;
            }

            default:
                break;
        }

        w_sstatus(sstatus);
        w_sepc(sepc);
    }

    // Hardverski prekid konzole
    else if (scause == 0x8000000000000009UL) {
        console_handler();
    }

    // Tajmerski prekid
    else if (scause == 0x8000000000000001UL) {
        mc_sip(SIP_SSIP);
    }
}