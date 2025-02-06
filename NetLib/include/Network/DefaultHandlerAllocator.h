/* 
DefaultAllocHandler
Created by: pshpjr
Date: 2025-02-05

Description: 
asio 핸들러 관련 메모리 처리를 위한 allocator
*/

#pragma once
#include <type_traits>

//STL 헤더

//서드파티 헤더

//프로젝트 헤더

class DefaultHandlerAllocator
{
public:
    DefaultHandlerAllocator() : inUse_(false)
    {
    }

    ~DefaultHandlerAllocator();

    DefaultHandlerAllocator(const DefaultHandlerAllocator&) = delete;
    DefaultHandlerAllocator& operator=(const DefaultHandlerAllocator&) = delete;

    void* allocate(size_t size)
    {
        if (!inUse_ && size < sizeof(storage_))
        {
            inUse_ = true;
            return &storage_;
        }
        else
        {
            return ::operator new(size);
        }
    }

    void deallocate(void* pointer)
    {
        if (pointer == &storage_)
        {
            inUse_ = false;
        }
        else
        {
            ::operator delete(pointer);
        }
    }

private:
    // Storage space used for handler-based custom memory allocation.
    std::aligned_storage_t<512> storage_;

    // Whether the handler-based custom allocation storage has been used.
    bool inUse_;
};

