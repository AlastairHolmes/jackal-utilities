/*!
	@file		test_allocator.h
	@author		Alastair Holmes
	@date		26/10/2018
 */

#ifndef JKUTIL_TEST_ALLOCATOR_H
#define JKUTIL_TEST_ALLOCATOR_H

#include <jkutil\allocator.h>
#include <type_traits>
#include <list>
#include <utility>
#include <stdexcept>

namespace jkutil
{

	class test_allocator
	{
	public:

		using propagate_on_container_copy_assignment = std::false_type;
		using propagate_on_container_move_assignment = std::false_type;
		using propagate_on_container_swap = std::false_type;
		using is_always_equal = std::false_type;

		test_allocator() = default;

		~test_allocator() noexcept(false);

		test_allocator(const test_allocator& p_instance);
		
		test_allocator(test_allocator&& p_instance);

		test_allocator& operator=(const test_allocator& p_rhs);
		test_allocator& operator=(test_allocator&& p_rhs);

		void* allocate(std::size_t p_size, std::size_t p_alignment);
		void deallocate(void* p_ptr, std::size_t p_size);

		bool operator==(const allocator& p_instance) const;
		bool operator!=(const allocator& p_instance) const;

	private:

		jkutil::allocator m_allocator;
		std::list<std::pair<void*, std::size_t>> m_allocation;

	};

}

#endif