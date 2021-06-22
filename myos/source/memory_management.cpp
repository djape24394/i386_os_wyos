#include <memory_management.h>

using namespace myos;
using namespace myos::common;

struct MemoryChunk
{
    MemoryChunk *next;
    MemoryChunk *prev;
    bool allocated;
    common::size_t size;
};


MemoryManager* MemoryManager::active_memory_manager = nullptr;

MemoryManager::MemoryManager(common::size_t start, common::size_t size)
{
    active_memory_manager = this;
    if(size < sizeof(MemoryChunk))
    {
        first = nullptr;
    }
    else
    {
        first = (MemoryChunk*) start;
        first->allocated = false;
        first->next = nullptr;
        first->prev = nullptr;
        first->size = size - sizeof(MemoryChunk);       
    }
}

MemoryManager::~MemoryManager()
{
    if(active_memory_manager != nullptr) active_memory_manager = nullptr;
}

void* MemoryManager::malloc(common::size_t size)
{
    MemoryChunk *result{nullptr};
    for(MemoryChunk* chunk = first; chunk != nullptr && result == nullptr; chunk = chunk->next)
    {
        if(chunk->size > size && !chunk->allocated)
        {
            result = chunk;
        }
    }
    if(result == nullptr) return nullptr;
    if(result->size >= size + sizeof(MemoryChunk) + 1)
    {
        MemoryChunk *temp = (MemoryChunk*)((size_t)result + sizeof(MemoryChunk) + size);
        
        temp->allocated = false;
        temp->prev = result;
        temp->size = result->size - size - sizeof(MemoryChunk);
        temp->next = result->next;
        if(temp->next != nullptr)
        {
            temp->next->prev = temp;
        }

        result->size = size;
        result->next = temp;
        result->allocated = true;
    }
    result->allocated = true;

    return (void *)((size_t)result + sizeof(MemoryChunk));
}

void MemoryManager::free(void* ptr)
{
    MemoryChunk *chunk = (MemoryChunk*)((size_t)ptr - sizeof(MemoryChunk));
    chunk->allocated = false;
    if(chunk->prev != nullptr && chunk->prev->allocated == false)
    {
        chunk->prev->size += chunk->size + sizeof(MemoryChunk);
        chunk->prev->next = chunk->next;
        if(chunk->next != nullptr) chunk->next->prev = chunk->prev;
        chunk = chunk->prev;
    }

    if(chunk->next != nullptr && chunk->next->allocated == false)
    {
        chunk->size += chunk->next->size + sizeof(MemoryChunk);
        chunk->next = chunk->next->next;
        if(chunk->next != nullptr) chunk->next->prev = chunk;
    }
}

void* operator new(size_t size)
{
    if(myos::MemoryManager::active_memory_manager == nullptr) return nullptr;
    return myos::MemoryManager::active_memory_manager->malloc(size);
}

void* operator new[](size_t size)
{
    if(myos::MemoryManager::active_memory_manager == nullptr) return nullptr;
    return myos::MemoryManager::active_memory_manager->malloc(size);
}

void* operator new(size_t size, void* ptr)
{
    return ptr;
}

void* operator new[](size_t size, void* ptr)
{
    return ptr;
}

void operator delete(void *ptr)
{
    if(myos::MemoryManager::active_memory_manager != nullptr) 
        myos::MemoryManager::active_memory_manager->free(ptr);
}

void operator delete[](void *ptr)
{
    if(myos::MemoryManager::active_memory_manager != nullptr) 
        myos::MemoryManager::active_memory_manager->free(ptr);
}
