#include "../inc/MemoryAllocator.hpp"
#include "../lib/hw.h"

// Dvostruko ulančane liste slobodnih i zauzetih blokova
Block* MemoryAllocator::freePtr  = nullptr;
Block* MemoryAllocator::allocPtr = nullptr;

// Inicijalizacija heap-a kao jednog slobodnog bloka
void MemoryAllocator::initialisation() {
    freePtr  = (Block*)HEAP_START_ADDR;
    allocPtr = nullptr;

    freePtr->size = ((size_t)HEAP_END_ADDR - (size_t)HEAP_START_ADDR - sizeof(Block))
                    / MEM_BLOCK_SIZE * MEM_BLOCK_SIZE;
    freePtr->next = nullptr;
    freePtr->prev = nullptr;
}

// Spajanje susednih slobodnih blokova
void MemoryAllocator::tryToJoin(Block* blk) {
    if (!blk || !blk->next) return;

    if ((char*)blk->next == (char*)blk + sizeof(Block) + blk->size) {
        blk->size += blk->next->size + sizeof(Block);
        blk->next  = blk->next->next;
        if (blk->next) blk->next->prev = blk;
    }
}

// First-fit alokacija memorije
void* MemoryAllocator::memoryAllocate(size_t size) {
    if (!freePtr || size == 0) return nullptr;

    size_t newSize = ((size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE) * MEM_BLOCK_SIZE;

    Block* cur = freePtr;
    while (cur && cur->size < newSize) cur = cur->next;
    if (!cur) return nullptr;

    if (cur->size - newSize > sizeof(Block) + MEM_BLOCK_SIZE) {
        // Podela slobodnog bloka
        Block* newFree = (Block*)((char*)cur + sizeof(Block) + newSize);
        newFree->size  = cur->size - newSize - sizeof(Block);
        cur->size      = newSize;

        newFree->next = cur->next;
        newFree->prev = cur->prev;
        if (cur->prev) cur->prev->next = newFree;
        else           freePtr         = newFree;
        if (cur->next) cur->next->prev = newFree;
    } else {
        // Uzimanje celog bloka
        if (cur->prev) {
            cur->prev->next = cur->next;
            if (cur->next) cur->next->prev = cur->prev;
        } else {
            freePtr = cur->next;
            if (freePtr) freePtr->prev = nullptr;
        }
    }

    cur->prev = nullptr;
    cur->next = nullptr;

    // Dodavanje u listu zauzetih blokova
    if (!allocPtr || (char*)cur < (char*)allocPtr) {
        cur->next = allocPtr;
        if (allocPtr) allocPtr->prev = cur;
        allocPtr = cur;
    } else {
        Block* p = allocPtr;
        while (p->next && (char*)p->next < (char*)cur) p = p->next;
        cur->next = p->next;
        cur->prev = p;
        p->next   = cur;
        if (cur->next) cur->next->prev = cur;
    }

    return (char*)cur + sizeof(Block);
}

// Oslobađanje memorije i spajanje slobodnih blokova
int MemoryAllocator::memoryDeallocate(void* ptr) {
    if (!ptr) return -1;

    if ((char*)ptr < (char*)HEAP_START_ADDR + sizeof(Block) ||
        (char*)ptr >= (char*)HEAP_END_ADDR)
        return -2;

    Block* blk = (Block*)((char*)ptr - sizeof(Block));

    // Uklanjanje iz liste zauzetih blokova
    if (blk->prev) blk->prev->next = blk->next;
    else           allocPtr        = blk->next;
    if (blk->next) blk->next->prev = blk->prev;

    blk->prev = nullptr;
    blk->next = nullptr;

    // Ubacivanje u listu slobodnih blokova
    if (!freePtr || (char*)blk < (char*)freePtr) {
        blk->next = freePtr;
        if (freePtr) freePtr->prev = blk;
        freePtr = blk;
    } else {
        Block* p = freePtr;
        while (p->next && (char*)p->next < (char*)blk) p = p->next;
        blk->next = p->next;
        blk->prev = p;
        p->next   = blk;
        if (blk->next) blk->next->prev = blk;
    }

    // Spajanje susednih blokova
    tryToJoin(blk);
    if (blk->prev) tryToJoin(blk->prev);

    return 0;
}