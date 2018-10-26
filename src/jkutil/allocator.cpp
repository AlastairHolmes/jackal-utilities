#include <jkutil\allocator.h>
#include <jkutil\memory.h>

#include <stdexcept>

namespace jkutil
{

	void* allocator::allocate(size_t p_size, size_t p_alignment)
	{
		void* memory = jkutil::aligned_malloc(p_size,p_alignment);

		if (!memory)
		{
			throw std::bad_alloc();
		}

		return memory;
	}

	void allocator::deallocate(void* p_ptr, size_t p_size)
	{
		jkutil::aligned_free(p_ptr, p_size);
	}

	bool allocator::operator==(const allocator & p_instance) const
	{
		return true;
	}

	bool allocator::operator!=(const allocator & p_instance) const
	{
		return false;
	}

}