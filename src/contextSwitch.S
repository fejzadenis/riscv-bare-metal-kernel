# Promena konteksta između dve niti
# a0 = stari Context, a1 = novi Context

.global _ZN7_thread13contextSwitchEPNS_7ContextES1_

_ZN7_thread13contextSwitchEPNS_7ContextES1_:

    # Sacuvaj kontekst stare niti
    sd ra,  0(a0)
    sd sp,  8(a0)

    # Ucitaj kontekst nove niti
    ld ra,  0(a1)
    ld sp,  8(a1)

    # Nastavi izvrsavanje nove niti
    ret