// Allocator
// Created by: pshpjr
// Date: 2024-10-28
//
// Description: 
//

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

//STL 헤더
#include <memory>

//서드파티 헤더

//프로젝트 헤더



namespace psh::memory
{
	class IAllocator
	{
	public:
		virtual ~IAllocator() = default;

		virtual void* Alloc(size_t size) = 0;
		virtual void Free(void* ptr) = 0;
	};

	class DefaultAllocator : public IAllocator
	{
	public:
		void* Alloc(size_t size) override;
		void Free(void* ptr) override;
	};

	class DebugAllocator : public IAllocator
	{
	public:
		void* Alloc(size_t size) override;
		void Free(void* ptr) override;

	};


	namespace detail {
		inline std::unique_ptr<IAllocator> g_Allocator = std::make_unique<DefaultAllocator>();
	}

	void SetDefaultAllocator(std::unique_ptr<IAllocator> allocator);
}


template<typename T, typename... Args>
T* New(psh::memory::IAllocator& allocator, Args&&... args)
{
	return new (allocator.Alloc(sizeof(T))) T(std::forward<Args>(args)...);
}

template<typename T>
void Delete(psh::memory::IAllocator& allocator, T* ptr)
{
	ptr->~T();
	allocator.Free(ptr);
}

template<typename T, typename... Args>
T* New(Args&&... args)
{
	return New<T>(*psh::memory::detail::g_Allocator, std::forward<Args>(args)...);
}

template<typename T>
void Delete(T* ptr)
{
	Delete(*psh::memory::detail::g_Allocator, ptr);
}


#endif // ALLOCATOR_H