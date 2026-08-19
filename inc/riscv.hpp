//
// Created by os on 6/18/26.
//

#ifndef PROJECT_BASE_V1_1_1_RISCV_HPP
#define PROJECT_BASE_V1_1_1_RISCV_HPP

#include "../lib/hw.h"

class Riscv {
public:
    static void popSppSpie();

    static uint64 r_scause();
    static void   w_scause(uint64 scause);
    static uint64 r_sepc();
    static void   w_sepc(uint64 sepc);
    static uint64 r_stvec();
    static void   w_stvec(uint64 stvec);
    static uint64 r_stval();
    static void   w_stval(uint64 stval);

    enum BitMaskSip {
        SIP_SSIP = (1 << 1),
        SIP_STIP = (1 << 5),
        SIP_SEIP = (1 << 9),
    };

    enum BitMaskSie {
    SIE_SSIE = (1 << 1),
    SIE_SEIE = (1 << 9),
};
    static void   ms_sip(uint64 mask);
    static void   mc_sip(uint64 mask);
    static uint64 r_sip();
    static void   w_sip(uint64 sip);

    static void   ms_sie(uint64 mask);
    static void   mc_sie(uint64 mask);
    static uint64 r_sie();
    static void   w_sie(uint64 sie);

    enum BitMaskSstatus {
        SSTATUS_SIE  = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP  = (1 << 8),
    };
    static void   ms_sstatus(uint64 mask);
    static void   mc_sstatus(uint64 mask);
    static uint64 r_sstatus();
    static void   w_sstatus(uint64 sstatus);

    static void supervisorTrap();

    static uint64 r_a0();
    static void   w_a0(uint64 v);
    static uint64 r_a1();
    static void   w_a1(uint64 v);
    static uint64 r_a2();
    static void   w_a2(uint64 v);
    static uint64 r_a7();
    static void   w_a7(uint64 v);

private:
    static void handleSupervisorTrap();

};

// ---- inline implementacije ----

inline uint64 Riscv::r_scause() {
    uint64 volatile v;
    __asm__ volatile("csrr %[v], scause" : [v]"=r"(v));
    return v;
}
inline void Riscv::w_scause(uint64 v) {
    __asm__ volatile("csrw scause, %[v]" : : [v]"r"(v));
}
inline uint64 Riscv::r_sepc() {
    uint64 volatile v;
    __asm__ volatile("csrr %[v], sepc" : [v]"=r"(v));
    return v;
}
inline void Riscv::w_sepc(uint64 v) {
    __asm__ volatile("csrw sepc, %[v]" : : [v]"r"(v));
}
inline uint64 Riscv::r_stvec() {
    uint64 volatile v;
    __asm__ volatile("csrr %[v], stvec" : [v]"=r"(v));
    return v;
}
inline void Riscv::w_stvec(uint64 v) {
    __asm__ volatile("csrw stvec, %[v]" : : [v]"r"(v));
}
inline uint64 Riscv::r_stval() {
    uint64 volatile v;
    __asm__ volatile("csrr %[v], stval" : [v]"=r"(v));
    return v;
}
inline void Riscv::w_stval(uint64 v) {
    __asm__ volatile("csrw stval, %[v]" : : [v]"r"(v));
}
inline void Riscv::ms_sip(uint64 mask) {
    __asm__ volatile("csrs sip, %[mask]" : : [mask]"r"(mask));
}
inline void Riscv::mc_sip(uint64 mask) {
    __asm__ volatile("csrc sip, %[mask]" : : [mask]"r"(mask));
}
inline uint64 Riscv::r_sip() {
    uint64 volatile v;
    __asm__ volatile("csrr %[v], sip" : [v]"=r"(v));
    return v;
}
inline void Riscv::w_sip(uint64 v) {
    __asm__ volatile("csrw sip, %[v]" : : [v]"r"(v));
}
inline void Riscv::ms_sstatus(uint64 mask) {
    __asm__ volatile("csrs sstatus, %[mask]" : : [mask]"r"(mask));
}
inline void Riscv::mc_sstatus(uint64 mask) {
    __asm__ volatile("csrc sstatus, %[mask]" : : [mask]"r"(mask));
}

inline void Riscv::ms_sie(uint64 mask) {
    __asm__ volatile("csrs sie, %[mask]" : : [mask]"r"(mask));
}
inline void Riscv::mc_sie(uint64 mask) {
    __asm__ volatile("csrc sie, %[mask]" : : [mask]"r"(mask));
}
inline uint64 Riscv::r_sie() {
    uint64 volatile v;
    __asm__ volatile("csrr %[v], sie" : [v]"=r"(v));
    return v;
}
inline void Riscv::w_sie(uint64 v) {
    __asm__ volatile("csrw sie, %[v]" : : [v]"r"(v));
}
inline uint64 Riscv::r_sstatus() {
    uint64 volatile v;
    __asm__ volatile("csrr %[v], sstatus" : [v]"=r"(v));
    return v;
}
inline void Riscv::w_sstatus(uint64 v) {
    __asm__ volatile("csrw sstatus, %[v]" : : [v]"r"(v));
}
inline uint64 Riscv::r_a0() {
    uint64 volatile v;
    __asm__ volatile("mv %0, a0" : "=r"(v));
    return v;
}
inline void Riscv::w_a0(uint64 v) {
    __asm__ volatile("mv a0, %0" : : "r"(v));
}
inline uint64 Riscv::r_a1() {
    uint64 volatile v;
    __asm__ volatile("mv %0, a1" : "=r"(v));
    return v;
}
inline void Riscv::w_a1(uint64 v) {
    __asm__ volatile("mv a1, %0" : : "r"(v));
}
inline uint64 Riscv::r_a2() {
    uint64 volatile v;
    __asm__ volatile("mv %0, a2" : "=r"(v));
    return v;
}
inline void Riscv::w_a2(uint64 v) {
    __asm__ volatile("mv a2, %0" : : "r"(v));
}
inline uint64 Riscv::r_a7() {
    uint64 volatile v;
    __asm__ volatile("mv %0, a7" : "=r"(v));
    return v;
}
inline void Riscv::w_a7(uint64 v) {
    __asm__ volatile("mv a7, %0" : : "r"(v));
}

#endif //PROJECT_BASE_V1_1_1_RISCV_HPP
