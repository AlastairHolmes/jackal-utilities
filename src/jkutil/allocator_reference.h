/*!
	@file		allocator_reference.h
	@author		Alastair Holmes
	@date		26/10/2018
 */

#ifndef JKUTIL_ALLOCATOR_REFERENCE_H
#define JKUTIL_ALLOCATOR_REFERENCE_H

#include <stdexcept>
#include <type_traits>

namespace jkutil
{

	/*!
		@class allocator_reference

		@brief An allocator that acts as reference to another allocator.

		@details This class is not entirely like a reference, as it is not const and its
		assignment operators will change what it references (Unlike a real reference).

		@tparam storableAllocatorType The type of the referenced allocator.
		@tparam propagateReference Whether the reference is propagated on container swap and assignment.
	*/
	template <class storableAllocatorType, bool propagateReference = false>
	class allocator_reference
	{
	public:

		using propagate_on_container_copy_assignment = std::bool_constant<propagateReference>;
		using propagate_on_container_move_assignment = std::bool_constant<propagateReference>;
		using propagate_on_container_swap = std::bool_constant<propagateReference>;
		using is_always_equal = std::false_type;

		allocator_reference(storableAllocatorType& p_allocator);

		allocator_reference(const allocator_reference&) = default;
		allocator_reference(allocator_reference&&) = default;

	public:

		allocator_reference& operator=(const allocator_reference& p_rhs) = default;
		allocator_reference& operator=(allocator_reference&& p_rhs) = default;

		/*! @brief Deep equality comparison.*/
		bool operator==(const allocator_reference& p_rhs);

		/*! @brief Deep inequality comparison.*/
		bool operator!=(const allocator_reference& p_rhs);

		/*!
			@brief Allocates memory using the referenced allocator.
			@details Simply pass through any allocation requests to the referenced allocator.
		*/
		void* allocate(size_t p_size, size_t p_alignment);

		/*!
			@brief Dealocates memory using the referenced allocator.
			@details Simply pass through any deallocation requests to the referenced allocator.
		*/
		void deallocate(void* p_ptr, size_t p_size);

		/*! @brief Gets the current referenced allocator. */
		storableAllocatorType& get_internal_allocator() const;

	private:

		storableAllocatorType* m_allocator;

	};

	template<class storableAllocatorType, bool propagateReference>
	inline allocator_reference<storableAllocatorType, propagateReference>::allocator_reference(storableAllocatorType& p_allocator)
		: m_allocator(&p_allocator)
	{
	}

	template<class storableAllocatorType, bool propagateReference>
	inline bool allocator_reference<storableAllocatorType, propagateReference>::operator==(const allocator_reference & p_rhs)
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

	template<class storableAllocatorType, bool propagateReference>
	inline bool allocator_reference<storableAllocatorType, propagateReference>::operator!=(const allocator_reference & p_rhs)
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

	template<class storableAllocatorType, bool propagateReference>
	inline void* allocator_reference<storableAllocatorType, propagateReference>::allocate(size_t p_size, size_t p_alignment)
	{
		return m_allocator->allocate(p_size, p_alignment);
	}

	template<class storableAllocatorType, bool propagateReference>
	inline void allocator_reference<storableAllocatorType, propagateReference>::deallocate(void * p_ptr, size_t p_size)
	{
		m_allocator->deallocate(p_ptr, p_size);
	}

	template<class storableAllocatorType, bool propagateReference>
	inline storableAllocatorType & allocator_reference<storableAllocatorType, propagateReference>::get_internal_allocator() const
	{
		return *m_allocator;
	}

}

#endif