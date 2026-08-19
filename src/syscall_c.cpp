#include "../inc/syscall_c.hpp"
#include "../lib/hw.h"

// C API wrapperi oko ecall instrukcije

// ── Memorija ───────────────────────────────────────────────────

// Alokacija memorije
void* mem_alloc(size_t size) {
    size_t blocks = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    uint64 ret;
    __asm__ volatile(
        "mv a1, %[blk]\n"
        "li a0, 0x01\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : [blk] "r"(blocks)
        : "a0", "a1", "memory"
    );
    return (void*)ret;
}

// Oslobadjaanje memorije
int mem_free(void* ptr) {
    uint64 ret;
    __asm__ volatile(
        "mv a1, %[p]\n"
        "li a0, 0x02\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : [p] "r"(ptr)
        : "a0", "a1", "memory"
    );
    return (int)ret;
}

// ── Niti ───────────────────────────────────────────────────────

// Kreiranje niti
int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg) {
    void* stackBase = mem_alloc(DEFAULT_STACK_SIZE);
    if (!stackBase) return -1;
    void* stackTop = (char*)stackBase + DEFAULT_STACK_SIZE;
    uint64 ret;
    __asm__ volatile(
        "mv a1, %[h]\n"
        "mv a2, %[fn]\n"
        "mv a3, %[st]\n"
        "mv a7, %[a]\n"
        "li a0, 0x11\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : [h] "r"(handle), [fn] "r"(start_routine), [st] "r"(stackTop), [a] "r"(arg)
        : "a0", "a1", "a2", "a3", "a7", "memory"
    );
    return (int)ret;
}

int thread_exit() {
    uint64 ret;
    __asm__ volatile(
        "li a0, 0x12\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        :
        : "a0", "memory"
    );
    return (int)ret;
}

void thread_dispatch() {
    __asm__ volatile(
        "li a0, 0x13\n"
        "ecall"
        ::: "a0", "memory"
    );
}

int thread_create_no_start(thread_t* handle, void (*start_routine)(void*), void* arg) {
    void* stackBase = mem_alloc(DEFAULT_STACK_SIZE);
    if (!stackBase) return -1;
    void* stackTop = (char*)stackBase + DEFAULT_STACK_SIZE;
    uint64 ret;
    __asm__ volatile(
        "mv a1, %[h]\n"
        "mv a2, %[fn]\n"
        "mv a3, %[st]\n"
        "mv a7, %[a]\n"
        "li a0, 0x14\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : [h] "r"(handle), [fn] "r"(start_routine), [st] "r"(stackTop), [a] "r"(arg)
        : "a0", "a1", "a2", "a3", "a7", "memory"
    );
    return (int)ret;
}

// ── Semafori ───────────────────────────────────────────────────

int sem_open(sem_t* handle, unsigned init) {
    uint64 ret;
    __asm__ volatile(
        "mv a1, %[h]\n"
        "mv a2, %[i]\n"
        "li a0, 0x21\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : [h] "r"(handle), [i] "r"(init)
        : "a0", "a1", "a2", "memory"
    );
    return (int)ret;
}

int sem_close(sem_t handle) {
    uint64 ret;
    __asm__ volatile(
        "mv a1, %[h]\n"
        "li a0, 0x22\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : [h] "r"(handle)
        : "a0", "a1", "memory"
    );
    return (int)ret;
}

int sem_wait(sem_t id) {
    uint64 ret;
    __asm__ volatile(
        "mv a1, %[id]\n"
        "li a0, 0x23\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : [id] "r"(id)
        : "a0", "a1", "memory"
    );
    return (int)ret;
}

int sem_signal(sem_t id) {
    uint64 ret;
    __asm__ volatile(
        "mv a1, %[id]\n"
        "li a0, 0x24\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : [id] "r"(id)
        : "a0", "a1", "memory"
    );
    return (int)ret;
}

int sem_wait_n(sem_t id, unsigned n) {
    uint64 ret;
    __asm__ volatile(
        "mv a1, %[id]\n"
        "mv a2, %[n]\n"
        "li a0, 0x25\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : [id] "r"(id), [n] "r"(n)
        : "a0", "a1", "a2", "memory"
    );
    return (int)ret;
}

int sem_signal_n(sem_t id, unsigned n) {
    uint64 ret;
    __asm__ volatile(
        "mv a1, %[id]\n"
        "mv a2, %[n]\n"
        "li a0, 0x26\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : [id] "r"(id), [n] "r"(n)
        : "a0", "a1", "a2", "memory"
    );
    return (int)ret;
}

// ── Vreme ──────────────────────────────────────────────────────

int time_sleep(time_t t) {
    uint64 ret;
    __asm__ volatile(
        "mv a1, %[t]\n"
        "li a0, 0x31\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        : [t] "r"(t)
        : "a0", "a1", "memory"
    );
    return (int)ret;
}

// ── Konzola ────────────────────────────────────────────────────

char getc() {
    uint64 ret;
    __asm__ volatile(
        "li a0, 0x41\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r"(ret)
        :
        : "a0", "memory"
    );
    return (char)ret;
}

void putc(char c) {
    __asm__ volatile(
        "mv a1, %[c]\n"
        "li a0, 0x42\n"
        "ecall"
        :
        : [c] "r"(c)
        : "a0", "a1", "memory"
    );
}