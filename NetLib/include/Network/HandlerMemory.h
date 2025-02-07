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

class HandlerMemory
{
public:
    HandlerMemory() : inUse_(false) {}

    ~HandlerMemory();

    HandlerMemory(const HandlerMemory&) = delete;
    HandlerMemory& operator=(const HandlerMemory&) = delete;

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

using boost::asio::ip::tcp;

// The allocator to be associated with the handler objects. This allocator only
// needs to satisfy the C++11 minimal allocator requirements.
template <typename T>
class HandlerAllocator
{
public:
    using value_type = T;

    explicit HandlerAllocator(HandlerMemory& mem)
        : memory_(mem) {}

    template <typename U>
    HandlerAllocator(const HandlerAllocator<U>& other) noexcept
        : memory_(other.memory_) {}

    bool operator==(const HandlerAllocator& other) const noexcept
    {
        return &memory_ == &other.memory_;
    }

    bool operator!=(const HandlerAllocator& other) const noexcept
    {
        return &memory_ != &other.memory_;
    }

    T* allocate(std::size_t n) const
    {
        return static_cast<T*>(memory_.allocate(sizeof(T) * n));
    }

    void deallocate(T* p, std::size_t /*n*/) const
    {
        return memory_.deallocate(p);
    }

private:
    template <typename>
    friend class HandlerAllocator;

    // The underlying memory.
    HandlerMemory& memory_;
};


