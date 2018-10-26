/*!
	@file		allocator_pointer.h
	@author		Alastair Holmes
	@date		26/10/2018
 */

#ifndef JKUTIL_ALLOCATOR_POINTER_H
#define JKUTIL_ALLOCATOR_POINTER_H

#include <stdexcept>
#include <type_traits>
#include <jkutil\assert.h>

namespace jkutil
{

	/*!
		
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

		bool operator==(const allocator_pointer& p_rhs);
		bool operator!=(const allocator_pointer& p_rhs);

		void* allocate(size_t p_size, size_t p_alignment);
		void deallocate(void* p_ptr, size_t p_size);

		allocatorType* get_internal_allocator() const;

	private:

		allocatorType* m_allocator;

	};

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
	inline bool allocator_pointer<allocatorType, propagatePointer>::operator==(const allocator_pointer& p_rhs)
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
	inline bool allocator_pointer<allocatorType, propagatePointer>::operator!=(const allocator_pointer& p_rhs)
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
	inline void * allocator_pointer<allocatorType, propagatePointer>::allocate(size_t p_size, size_t p_alignment)
	{
		if (!m_allocator)
		{
			throw std::bad_alloc();
		}
		return m_allocator->allocate(p_size, p_alignment);
	}

	template<class allocatorType, bool propagatePointer>
	inline void allocator_pointer<allocatorType, propagatePointer>::deallocate(void * p_ptr, size_t p_size)
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