#ifndef JKUTIL_ALLOCATOR_REFERENCE_H
#define JKUTIL_ALLOCATOR_REFERENCE_H

#include <stdexcept>
#include <type_traits>

namespace jkutil
{

	template <class allocatorType, bool propagateReference = false>
	class allocator_reference
	{
	public:

		using propagate_on_container_copy_assignment = std::bool_constant<propagateReference>;
		using propagate_on_container_move_assignment = std::bool_constant<propagateReference>;
		using propagate_on_container_swap = std::bool_constant<propagateReference>;
		using is_always_equal = std::false_type;

		allocator_reference(allocatorType& p_allocator);

		allocator_reference(const allocator_reference&) = default;
		allocator_reference(allocator_reference&&) = default;

	public:

		allocator_reference& operator=(const allocator_reference&) = default;
		allocator_reference& operator=(allocator_reference&&) = default;

		bool operator==(const allocator_reference& p_rhs);
		bool operator!=(const allocator_reference& p_rhs);

		void* allocate(size_t p_size, size_t p_alignment);
		void deallocate(void* p_ptr, size_t p_size);

		allocatorType& get_internal_allocator() const;

	private:

		allocatorType* m_allocator;

	};

	template<class allocatorType, bool propagateReference>
	inline allocator_reference<allocatorType, propagateReference>::allocator_reference(allocatorType& p_allocator)
		: m_allocator(&p_allocator)
	{
	}

	template<class allocatorType, bool propagateReference>
	inline bool allocator_reference<allocatorType, propagateReference>::operator==(const allocator_reference & p_rhs)
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

	template<class allocatorType, bool propagateReference>
	inline bool allocator_reference<allocatorType, propagateReference>::operator!=(const allocator_reference & p_rhs)
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

	template<class allocatorType, bool propagateReference>
	inline void* allocator_reference<allocatorType, propagateReference>::allocate(size_t p_size, size_t p_alignment)
	{
		return m_allocator->allocate(p_size, p_alignment);
	}

	template<class allocatorType, bool propagateReference>
	inline void allocator_reference<allocatorType, propagateReference>::deallocate(void * p_ptr, size_t p_size)
	{
		m_allocator->deallocate(p_ptr, p_size);
	}

	template<class allocatorType, bool propagateReference>
	inline allocatorType & allocator_reference<allocatorType, propagateReference>::get_internal_allocator() const
	{
		return *m_allocator;
	}

}

#endif