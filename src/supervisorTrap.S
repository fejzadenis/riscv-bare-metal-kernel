# Ulazna tačka za sve trap-ove (syscall, prekidi i izuzeci).

.global _ZN5Riscv14supervisorTrapEv

_ZN5Riscv14supervisorTrapEv:

    # Sacvuvaj registre
    addi sp, sp, -256

    # x10 (a0) se ne čuva kako bi ostala povratna vrednost syscall
    .irp reg, 1,5,6,7,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\reg, \reg * 8(sp)
    .endr

    # Obrada trap
    call _ZN5Riscv20handleSupervisorTrapEv

    # Vrati registre
    .irp reg, 1,5,6,7,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\reg, \reg * 8(sp)
    .endr

    addi sp, sp, 256

    # Povratak iz trap
    sret