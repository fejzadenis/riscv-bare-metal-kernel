//
// Created by os on 6/18/26.
//

#ifndef PROJECT_BASE_V1_1_1_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_V1_1_1_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

struct Block {
    size_t size;   // velicina korisnog dela (bez headera) u bajtovima
    Block* next;
    Block* prev;
};

class MemoryAllocator {
public:
    static void initialisation();
    static void* memoryAllocate(size_t size);
    static int memoryDeallocate(void* ptr);

private:
    static Block* freePtr;
    static Block* allocPtr;

    static void tryToJoin(Block* blk);
};




#endif //PROJECT_BASE_V1_1_1_MEMORYALLOCATOR_HPP
