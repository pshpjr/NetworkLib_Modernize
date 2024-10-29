#include "../include/Allocator.h"

#include <iostream>
#include <memory>

#ifdef _WIN32

#else
#include <malloc.h>
#endif



namespace psh::memory
{

	size_t MemSize(void* ptr)
	{
#ifdef _WIN32
		return _msize(ptr);
#else
		return malloc_usable_size(ptr);
#endif
	}


	void* DefaultAllocator::Alloc(size_t size)
	{
		return std::malloc(size);
	}

	void DefaultAllocator::Free(void* ptr)
	{
		return std::free(ptr);
	}

    void* DebugAllocator::Alloc(size_t size)
    {
		void* ptr = std::malloc(size + 2 * sizeof(uint32_t));

		if (ptr == nullptr) {
			return nullptr;
		}

		uint32_t* marker1 = static_cast<uint32_t*>(ptr) + size / sizeof(uint32_t);
		uint32_t* marker2 = marker1 + 1;
		*marker1 = 0xdeadbeaf;
		*marker2 = 0xdeadbeaf;

		return ptr;
    }

	void DebugAllocator::Free(void* ptr)
	{
		if (ptr == nullptr) {
			return;
		}

		size_t size = MemSize(ptr) - 2 * sizeof(uint32_t);
		uint32_t* marker1 = static_cast<uint32_t*>(ptr) + size / sizeof(uint32_t);
		uint32_t* marker2 = marker1 + 1;

		if (*marker1 != 0xdeadbeaf || *marker2 != 0xdeadbeaf) {
			std::cerr << "Memory corruption detected!\n";
			std::abort();
		}

		std::free(ptr);
	}

	void SetDefaultAllocator(std::unique_ptr<IAllocator> allocator)
	{
		detail::g_Allocator = std::move(allocator);
	}


}

