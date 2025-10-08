#include <vector>
#include <cstdlib>
#include <iostream>
#include <iomanip>

#ifndef POOLALLOC
#define POOLALLOC

const size_t POOL_SIZE = 134217728;

struct Pool {
	uint8_t* pool = nullptr;
	size_t instance_count = 0;
	uint8_t* free_mem = nullptr;
};

extern std::vector<Pool> memory;

template<typename T>
struct PoolAllocator {
	typedef T value_type;
	PoolAllocator() noexcept {};
	template<class U> PoolAllocator(const PoolAllocator<U>&) noexcept {}
	template<class U> bool operator==(const PoolAllocator<U>&) const noexcept
	{
		return true;
	}
	template<class U> bool operator!=(const PoolAllocator<U>&) const noexcept
	{
		return false;
	}
	T* allocate(const size_t n) const;
	void deallocate(T* const p, size_t) const noexcept;
};

template<typename T>
T* PoolAllocator<T>::allocate(const size_t n) const
{
	if (memory.size() > 64)
		throw std::runtime_error("No memory to allocate");

	if (n == 0)
		return nullptr;
	if (n > size_t(-1) / sizeof(T))
		throw std::bad_array_new_length();

	// Searching for avaliable chunk of data
	size_t allocation_size = n * sizeof(T);
	if (allocation_size > POOL_SIZE)
		throw std::bad_alloc();

	Pool* pool = nullptr;
	{
		for (auto& p : memory) {
			if (p.free_mem - p.pool + allocation_size < POOL_SIZE) {
				pool = &p;
				break;
			}
		}
	}

	if (pool == nullptr)
	{
		void* mr = (uint8_t*)malloc(POOL_SIZE);
		if (mr == nullptr)
			throw std::bad_alloc();
		memory.push_back({ (uint8_t*)mr, 0, (uint8_t*)mr });
		pool = &(memory.back());
	}
	void* ptr = pool->free_mem;
	pool->free_mem += allocation_size;
	++(pool->instance_count);
	return (T*)ptr;
}

template<typename T>
void PoolAllocator<T>::deallocate(T* const p, size_t) const noexcept
{
	auto begin = memory.begin();
	for (auto it = memory.begin(); it != memory.end(); ++it) {
		if ((void*)p >= it->pool && (void*)p <= it->free_mem) {
			--(it->instance_count);
			if (it->instance_count == 0) {
				free(it->pool);
				memory.erase(it);
				return;
			}
		}
	}
}

#endif
