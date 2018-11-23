/*!
	@file		allocator_pointer.h
	@author		Alastair Holmes
	@date		26/10/2018
 */

#ifndef JKUTIL_ALLOCATOR_POINTER_H
#define JKUTIL_ALLOCATOR_POINTER_H

#include <jkutil\assert.h>
#include <stdexcept>
#include <type_traits>
#include <cstddef>

namespace jkutil
{

	/*!
		@class allocator_pointer

		@brief An allocator that is a pointer to another allocator.

		@details This is intended to act like a pointer in the sense it can be changed to refer to other instances
		and can be invalid/not refer to any instance (nullptr). It uses an external instance of another allocator
		which it points/refers to to perform allocate and deallocate operations. Its purpose is to allow multiple
		containers to use the same instance of a stateful allocator type. If the allocator refers to no allocator (nullptr),
		allocation, and deallocation are invalid. You can use a allocator_reference if you want the guarentee
		that the allocator is always refering to something (Not nullptr).

		@attention This class uses deep equality tests, and not simple pointer equality.

		@tparam allocatorType The underlying type of allocator this pointers to. This type meet the requirements
		of 'JKAllocator'.

		@tparam propagatePointer This controls whether the pointer is propagated when a container that uses this type of
		allocator is swapped or assigned (move or copy).

		@see jkutil::allocator_reference
	*/
	template <class allocatorType, bool propagatePointer = false>
	class allocator_pointer
	{
	public:

		using propagate_on_container_copy_assignment = std::bool_constant<propagatePointer>;
		using propagate_on_container_move_assignment = std::bool_constant<propagatePointer>;
		using propagate_on_container_swap = std::bool_constant<propagatePointer>;
		using is_always_equal = std::false_type;

		allocator_pointer(std::nullptr_t);
		allocator_pointer(allocatorType* p_allocator);

		allocator_pointer(const allocator_pointer&) = default;
		allocator_pointer(allocator_pointer&&) = default;

	public:

		allocator_pointer& operator=(std::nullptr_t);
		allocator_pointer& operator=(allocatorType* p_allocator);

		allocator_pointer& operator=(const allocator_pointer&) = default;
		allocator_pointer& operator=(allocator_pointer&&) = default;

		/*!
			@brief Deep compares two allocator_pointer

			@details This equality operation uses a deep equality test, not simple pointer equality, meaning it is use \c allocatorType's
			equality operator.
		*/
		bool operator==(const allocator_pointer& p_rhs) const;

		/*!
			@brief Deep compares two allocator_pointer

			@details This equality operation uses a deep equality test, not simple pointer equality, meaning it is uses \c allocatorType's
			equality operator.
		*/
		bool operator!=(const allocator_pointer& p_rhs) const;

		/*!
			@brief Allocates memory using the referred to allocator.

			@details If no allocator instance is referred to a call to allocate() will throw a std::bad_alloc.
		*/
		void* allocate(std::size_t p_size, std::size_t p_alignment);

		/*!
			@brief Deallocates memory using the referred to allocator.

			@details If no allocator instance is referred to a call to deallocate() will assert (In debug mode). If the underlying allocator cam throw this can also throw.
		*/
		void deallocate(void* p_ptr, std::size_t p_size);

		/*! @brief Get a pointer to the referred to allocator. Can return nullptr. */
		allocatorType* get_internal_allocator() const;

	private:

		allocatorType* m_allocator;

	};

	template<bool propagatePointer = false, class allocatorType>
	allocator_pointer<allocatorType, propagatePointer> make_allocator_pointer(allocatorType* p_allocator)
	{
		return allocator_pointer<allocatorType, propagatePointer>(p_allocator);
	}

	template<class allocatorType, bool propagatePointer>
	inline allocator_pointer<allocatorType, propagatePointer>::allocator_pointer(std::nullptr_t)
		: m_allocator(nullptr)
	{
	}

	template<class allocatorType, bool propagatePointer>
	inline allocator_pointer<allocatorType, propagatePointer>::allocator_pointer(allocatorType * p_allocator)
		: m_allocator(p_allocator)
	{
	}

	template<class allocatorType, bool propagatePointer>
	inline auto allocator_pointer<allocatorType, propagatePointer>::operator=(std::nullptr_t) -> allocator_pointer&
	{
		m_allocator = nullptr;
		return *this;
	}

	template<class allocatorType, bool propagatePointer>
	inline auto allocator_pointer<allocatorType, propagatePointer>::operator=(allocatorType * p_allocator) -> allocator_pointer&
	{
		m_allocator = p_allocator;
		return *this;
	}

	template<class allocatorType, bool propagatePointer>
	inline bool allocator_pointer<allocatorType, propagatePointer>::operator==(const allocator_pointer& p_rhs) const
	{
		if (m_allocator && p_rhs.m_allocator)
		{
			return *m_allocator == *p_rhs.m_allocator;
		}
		else
		{
			return p_rhs.m_allocator == m_allocator;
		}
	}

	template<class allocatorType, bool propagatePointer>
	inline bool allocator_pointer<allocatorType, propagatePointer>::operator!=(const allocator_pointer& p_rhs) const
	{
		if (m_allocator && p_rhs.m_allocator)
		{
			return *m_allocator != *p_rhs.m_allocator;
		}
		else
		{
			return p_rhs.m_allocator != m_allocator;
		}
	}

	template<class allocatorType, bool propagatePointer>
	inline void* allocator_pointer<allocatorType, propagatePointer>::allocate(std::size_t p_size, std::size_t p_alignment)
	{
		if (!m_allocator)
		{
			throw std::bad_alloc();
		}
		return m_allocator->allocate(p_size, p_alignment);
	}

	template<class allocatorType, bool propagatePointer>
	inline void allocator_pointer<allocatorType, propagatePointer>::deallocate(void * p_ptr, std::size_t p_size)
	{
		JKUTIL_ASSERT(m_allocator);
		if (m_allocator)
		{
			m_allocator->deallocate(p_ptr, p_size);
		}
	}

	template<class allocatorType, bool propagatePointer>
	inline allocatorType* allocator_pointer<allocatorType, propagatePointer>::get_internal_allocator() const
	{
		return m_allocator;
	}

}

#endif