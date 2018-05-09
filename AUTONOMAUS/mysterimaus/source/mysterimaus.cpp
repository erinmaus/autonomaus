// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <mutex>
#include <dlfcn.h>
#include <boost/stacktrace.hpp>
#include "mysterimaus/mysterimaus.hpp"

static char re_malloc_temp_buffer[1024*1024];
static std::size_t re_malloc_temp_buffer_offset = 0;

static void* (* re_original_malloc)(std::size_t) = nullptr;
static void* (* re_original_calloc)(std::size_t, std::size_t) = nullptr;
static void* (* re_original_realloc)(void*, std::size_t) = nullptr;
static void* (* re_original_memalign)(std::size_t, std::size_t) = nullptr;
static void (* re_original_free)(void*) = nullptr;

static void re_malloc_init()
{
	re_original_malloc = (void*(*)(std::size_t))dlsym(RTLD_NEXT, "malloc");
	re_original_calloc = (void* (*)(std::size_t, std::size_t))dlsym(RTLD_NEXT, "calloc");
	re_original_realloc = (void* (*)(void*, std::size_t))dlsym(RTLD_NEXT, "realloc");
	re_original_memalign = (void* (*)(std::size_t, std::size_t))dlsym(RTLD_NEXT, "memalign");
	re_original_free = (void(*)(void*))dlsym(RTLD_NEXT, "free");
}

static std::size_t re_malloc_allocations_count = 0;

static std::recursive_mutex re_malloc_lock;
static int re_malloc_lock_count = 0;

std::size_t mysterimaus::get_num_allocations()
{
	return re_malloc_allocations_count;
}

static mysterimaus::Allocation* re_malloc_allocations = nullptr;
static mysterimaus::Allocation* re_malloc_allocations_tail = nullptr;

mysterimaus::Allocation* mysterimaus::get_head_allocation()
{
	return re_malloc_allocations;
}

mysterimaus::Allocation* mysterimaus::get_tail_allocation()
{
	return re_malloc_allocations_tail;
}

mysterimaus::Allocation* mysterimaus::get_allocation(void* pointer)
{
	return ((mysterimaus::Allocation*)pointer) - 1;
}

static void re_link_allocation(mysterimaus::Allocation* allocation)
{
	allocation->ref_count = 1;
	allocation->next = nullptr;
	allocation->previous = re_malloc_allocations_tail;

	std::memset(allocation->backtrace, 0, sizeof(void*) * mysterimaus::MAX_BACKTRACE);

	if (re_malloc_allocations_tail == nullptr)
	{
		re_malloc_allocations = allocation;
		re_malloc_allocations_tail = allocation;
	}
	else
	{
		re_malloc_allocations_tail->next = allocation;
		re_malloc_allocations_tail = allocation;
	}

	++re_malloc_allocations_count;
}

static void re_unlink_allocation(mysterimaus::Allocation* allocation)
{
	if (allocation->previous != nullptr)
	{
		allocation->previous->next = allocation->next;
	}

	if (allocation->next != nullptr)
	{
		allocation->next->previous = allocation->previous;
	}

	if (re_malloc_allocations == allocation)
	{
		re_malloc_allocations = allocation->next;
	}

	if (re_malloc_allocations_tail == allocation)
	{
		re_malloc_allocations_tail = allocation->previous;
	}

	--re_malloc_allocations_count;
}

static void original_free(void* pointer)
{
	if (pointer == nullptr)
	{
		return;
	}

	mysterimaus::Allocation* allocation = ((mysterimaus::Allocation*)pointer) - 1;
	--allocation->ref_count;

	if (allocation->ref_count <= 0)
	{
		re_unlink_allocation(allocation);
		re_original_free(allocation->allocation);
	}
}

static mysterimaus::Allocation* original_malloc(std::size_t size)
{
	auto adjusted_size = size + sizeof(mysterimaus::Allocation);

	mysterimaus::Allocation* allocation = (mysterimaus::Allocation*)re_original_malloc(adjusted_size);
	if (allocation == nullptr)
	{
		return nullptr;
	}

	allocation->allocation = allocation;
	allocation->pointer = allocation + 1;
	allocation->size = size;

	re_link_allocation(allocation);

	return allocation;
}

static mysterimaus::Allocation* original_memalign(std::size_t alignment, std::size_t size)
{
	auto offset = sizeof(mysterimaus::Allocation) + alignment - 1;
	auto adjusted_size = size + offset;

	void* p = re_original_memalign(alignment, adjusted_size);
	if (p == nullptr)
	{
		return nullptr;
	}

	void* aligned_p = (void**)(((std::size_t)p + offset) & ~(alignment - 1));
	mysterimaus::Allocation* allocation = (mysterimaus::Allocation*)aligned_p - 1;
	allocation->pointer = aligned_p;
	allocation->allocation = p;
	allocation->size = size;

	re_link_allocation(allocation);

	return allocation;
}

static mysterimaus::Allocation* original_calloc(std::size_t count, std::size_t size)
{
	auto adjusted_size = (count * size) + sizeof(mysterimaus::Allocation);

	mysterimaus::Allocation* allocation = (mysterimaus::Allocation*)re_original_malloc(adjusted_size);
	if (allocation == nullptr)
	{
		return nullptr;
	}

	allocation->allocation = allocation;
	allocation->pointer = allocation + 1;
	allocation->size = count * size;

	std::memset(allocation->pointer, 0, allocation->size);

	re_link_allocation(allocation);

	return allocation;
}

static mysterimaus::Allocation* original_realloc(void* pointer, std::size_t size)
{
	if (pointer == nullptr)
	{
		return original_malloc(size);
	}

	mysterimaus::Allocation* previous = ((mysterimaus::Allocation*)pointer) - 1;
	mysterimaus::Allocation* allocation = original_malloc(size);
	if (allocation == nullptr)
	{
		return nullptr;
	}

	std::memcpy(allocation->pointer, previous->pointer, previous->size > size ? size : previous->size);
	original_free(previous->pointer);

	return allocation;
}

#include <execinfo.h>

static void re_build_stack_trace(mysterimaus::Allocation* allocation)
{
	void* b[mysterimaus::MAX_BACKTRACE + 2];
	int count = backtrace(b, mysterimaus::MAX_BACKTRACE + 2);

	int index = 0;
	for (int i = 2; i < count; ++i)
	{
		allocation->backtrace[index++] = b[i];
	}
}

extern "C"
void* malloc(std::size_t count)
{
	static bool bootstrapping = false;
	if (re_original_malloc == nullptr)
	{
		if (!bootstrapping)
		{
			bootstrapping = true;
			re_malloc_init();
			bootstrapping = false;
		}
		else
		{
			if (re_malloc_temp_buffer_offset + count < sizeof(re_malloc_temp_buffer))
			{
				void* result = re_malloc_temp_buffer + re_malloc_temp_buffer_offset;
				re_malloc_temp_buffer_offset += count;

				return result;
			}
			else
			{
				std::fprintf(stderr, "(Autonomaus) Failed to allocate buffer %lu during boostrap.\n", count);
				std::abort();
			}
		}
	}

	std::lock_guard<std::recursive_mutex> lock(re_malloc_lock);
	++re_malloc_lock_count;
	auto allocation = original_malloc(count);
	if (allocation != nullptr)
	{
		if (re_malloc_lock_count == 1)
		{
			re_build_stack_trace(allocation);
		}

		--re_malloc_lock_count;
		return allocation->pointer;
	}

	--re_malloc_lock_count;
	return nullptr;
}

extern "C"
void* calloc(std::size_t count, std::size_t size)
{
	if (re_original_calloc == nullptr)
	{
		auto result = malloc(count * size);
		if (result != nullptr)
		{
			std::memset(result, 0, count * size);
		}

		return result;
	}

	std::lock_guard<std::recursive_mutex> lock(re_malloc_lock);
	auto allocation = original_calloc(count, size);
	if (allocation != nullptr)
	{
		return allocation->pointer;
	}

	return nullptr;
}

extern "C"
void* realloc(void* pointer, std::size_t size)
{
	if (re_original_realloc == nullptr)
	{
		auto result = malloc(size);
		if (result != nullptr && pointer != nullptr)
		{
			// XXX: ACK! This is bad! It should be fine since we're in the temporary
			// heap....
			std::memcpy(result, pointer, size);
			free(pointer);
		}

		return result;
	}

	std::lock_guard<std::recursive_mutex> lock(re_malloc_lock);
	auto allocation = original_realloc(pointer, size);
	if (allocation != nullptr)
	{
		return allocation->pointer;
	}

	return nullptr;
}

extern "C"
void free(void* pointer)
{
	if (pointer >= re_malloc_temp_buffer && pointer <= re_malloc_temp_buffer + re_malloc_temp_buffer_offset)
	{
		// Nothing.
		return;
	}

	if (pointer == nullptr)
	{
		return;
	}

	std::lock_guard<std::recursive_mutex> lock(re_malloc_lock);
	original_free(pointer);
}

extern "C"
std::size_t malloc_usable_size(void* pointer)
{
	auto allocation = (mysterimaus::Allocation*)pointer - 1;
	return allocation->size;
}

extern "C"
void* memalign(std::size_t alignment, std::size_t size)
{
	if (re_original_memalign == nullptr)
	{
		auto result = malloc(size);
		return result;
	}

	std::lock_guard<std::recursive_mutex> lock(re_malloc_lock);
	auto allocation = original_memalign(alignment, size);
	if (allocation != nullptr)
	{
		return allocation->pointer;
	}

	return nullptr;
}
