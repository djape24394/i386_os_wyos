#ifndef MYOS_MEMORY_MANAGEMENT_H
#define MYOS_MEMORY_MANAGEMENT_H

#include <common/types.h>

namespace myos
{
    struct MemoryChunk
    {
        MemoryChunk *next;
        MemoryChunk *prev;
        bool allocated;
        common::size_t size;
    };

    class MemoryManager
    {
    protected:
        MemoryChunk *first; // should this be a chunk?
    public:
        static MemoryManager* active_memory_manager;
        MemoryManager(common::size_t start, common::size_t size);
        ~MemoryManager();

        void* malloc(common::size_t size);
        void free(void* ptr);  
    };
}

void* operator new(myos::common::size_t size);
void* operator new[](myos::common::size_t size);

// placement new
void* operator new(myos::common::size_t size, void* ptr);
void* operator new[](myos::common::size_t size, void* ptr);

void operator delete(void *ptr);
void operator delete[](void *ptr);

#endif
