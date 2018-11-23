/*!
	@file		allocator_reference.h
	@author		Alastair Holmes
	@date		26/10/2018
 */

#ifndef JKUTIL_ALLOCATOR_REFERENCE_H
#define JKUTIL_ALLOCATOR_REFERENCE_H

#include <jkutil\assert.h>
#include <stdexcept>
#include <type_traits>
#include <cstddef>

namespace jkutil
{

	/*!
		@class allocator_reference

		@brief An allocator that acts as reference to another allocator.

		@details This class is not entirely like a reference, as it is not const and its
		assignment operators will change what it references (Unlike a real reference).

		@tparam allocatorType The type of the referenced allocator. This type meet the requirements
		of 'JKAllocator'.
		@tparam propagateReference Whether the reference is propagated on container swap and assignment.
	*/
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

		/*! @brief Deep equality comparison.*/
		bool operator==(const allocator_reference& p_rhs) const;

		/*! @brief Deep inequality comparison.*/
		bool operator!=(const allocator_reference& p_rhs) const;

		/*!
			@brief Allocates memory using the referenced allocator.
			@details Simply pass through any allocation requests to the referenced allocator.
		*/
		void* allocate(std::size_t p_size, std::size_t p_alignment);

		/*!
			@brief Dealocates memory using the referenced allocator.
			@details Simply pass through any deallocation requests to the referenced allocator.
		*/
		void deallocate(void* p_ptr, std::size_t p_size);

		/*! @brief Gets the current referenced allocator. */
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
	inline bool allocator_reference<allocatorType, propagateReference>::operator==(const allocator_reference& p_rhs) const
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
	inline bool allocator_reference<allocatorType, propagateReference>::operator!=(const allocator_reference& p_rhs) const
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
	inline void* allocator_reference<allocatorType, propagateReference>::allocate(std::size_t p_size, std::size_t p_alignment)
	{
		return m_allocator->allocate(p_size, p_alignment);
	}

	template<class allocatorType, bool propagateReference>
	inline void allocator_reference<allocatorType, propagateReference>::deallocate(void * p_ptr, std::size_t p_size)
	{
		m_allocator->deallocate(p_ptr, p_size);
	}

	template<class allocatorType, bool propagateReference>
	inline allocatorType& allocator_reference<allocatorType, propagateReference>::get_internal_allocator() const
	{
		return *m_allocator;
	}

	/*!
		@class allocator_immutable_reference

		@brief An allocator that acts as reference to another allocator.

		@tparam allocatorType The type of the referenced allocator. This type meet the requirements
		of 'JKAllocator'.
	*/
	template <class allocatorType>
	class allocator_immutable_reference
	{
	public:

		using propagate_on_container_copy_assignment = std::false_type;
		using propagate_on_container_move_assignment = std::false_type;
		using propagate_on_container_swap = std::false_type;
		using is_always_equal = std::false_type;

		allocator_immutable_reference(allocatorType& p_allocator);

		allocator_immutable_reference(const allocator_immutable_reference&) = default;
		allocator_immutable_reference(allocator_immutable_reference&&) = default;

	public:

		allocator_immutable_reference& operator=(const allocator_immutable_reference&) = delete;
		allocator_immutable_reference& operator=(allocator_immutable_reference&&) = delete;

		/*! @brief Deep equality comparison.*/
		bool operator==(const allocator_immutable_reference& p_rhs) const;

		/*! @brief Deep inequality comparison.*/
		bool operator!=(const allocator_immutable_reference& p_rhs) const;

		/*!
			@brief Allocates memory using the referenced allocator.
			@details Simply pass through any allocation requests to the referenced allocator.
		*/
		void* allocate(std::size_t p_size, std::size_t p_alignment);

		/*!
			@brief Dealocates memory using the referenced allocator.
			@details Simply pass through any deallocation requests to the referenced allocator.
		*/
		void deallocate(void* p_ptr, std::size_t p_size);

		/*! @brief Gets the current referenced allocator. */
		allocatorType& get_internal_allocator() const;

	private:

		allocatorType& m_allocator;

	};

	template<class allocatorType>
	inline allocator_immutable_reference<allocatorType>::allocator_immutable_reference(allocatorType& p_allocator)
		: m_allocator(p_allocator)
	{
	}

	template<class allocatorType>
	inline bool allocator_immutable_reference<allocatorType>::operator==(const allocator_immutable_reference & p_rhs) const
	{
		return m_allocator == p_rhs.m_allocator;
	}

	template<class allocatorType>
	inline bool allocator_immutable_reference<allocatorType>::operator!=(const allocator_immutable_reference & p_rhs) const
	{
		return m_allocator != p_rhs.m_allocator;
	}

	template<class allocatorType>
	inline void* allocator_immutable_reference<allocatorType>::allocate(std::size_t p_size, std::size_t p_alignment)
	{
		return m_allocator.allocate(p_size, p_alignment);
	}

	template<class allocatorType>
	inline void allocator_immutable_reference<allocatorType>::deallocate(void* p_ptr, std::size_t p_size)
	{
		m_allocator.deallocate(p_ptr, p_size);
	}

	template<class allocatorType>
	inline allocatorType& allocator_immutable_reference<allocatorType>::get_internal_allocator() const
	{
		return m_allocator;
	}

	/*!
		@class opaque_allocator_reference

		@brief An allocator that acts as reference to another allocator.

		@details This class is not entirely like a reference, as it is not const and its
		assignment operators will change what it references (Unlike a real reference).

		@tparam allocatorType The type of the referenced allocator. This type meet the requirements
		of 'JKAllocator'.
		@tparam propagateReference Whether the reference is propagated on container swap and assignment.
	*/
	template <class allocatorType, bool propagateReference = false>
	class opaque_allocator_reference
	{
	public:

		using propagate_on_container_copy_assignment = std::bool_constant<propagateReference>;
		using propagate_on_container_move_assignment = std::bool_constant<propagateReference>;
		using propagate_on_container_swap = std::bool_constant<propagateReference>;
		using is_always_equal = std::false_type;

		opaque_allocator_reference(allocatorType& p_allocator);

		opaque_allocator_reference(const opaque_allocator_reference&) = default;
		opaque_allocator_reference(opaque_allocator_reference&&) = default;

	public:

		opaque_allocator_reference& operator=(const opaque_allocator_reference&) = default;
		opaque_allocator_reference& operator=(opaque_allocator_reference&&) = default;

		/*! @brief Deep equality comparison.*/
		bool operator==(const opaque_allocator_reference& p_rhs) const;

		/*! @brief Deep inequality comparison.*/
		bool operator!=(const opaque_allocator_reference& p_rhs) const;

		/*!
			@brief Allocates memory using the referenced allocator.
			@details Simply pass through any allocation requests to the referenced allocator.
		*/
		void* allocate(std::size_t p_size, std::size_t p_alignment);

		/*!
			@brief Dealocates memory using the referenced allocator.
			@details Simply pass through any deallocation requests to the referenced allocator.
		*/
		void deallocate(void* p_ptr, std::size_t p_size);

	private:

		allocatorType* m_allocator;

	};

	template<class allocatorType, bool propagateReference>
	inline opaque_allocator_reference<allocatorType, propagateReference>::opaque_allocator_reference(allocatorType& p_allocator)
		: m_allocator(&p_allocator)
	{
	}

	template<class allocatorType, bool propagateReference>
	inline bool opaque_allocator_reference<allocatorType, propagateReference>::operator==(const opaque_allocator_reference& p_rhs) const
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
	inline bool opaque_allocator_reference<allocatorType, propagateReference>::operator!=(const opaque_allocator_reference& p_rhs) const
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
	inline void* opaque_allocator_reference<allocatorType, propagateReference>::allocate(std::size_t p_size, std::size_t p_alignment)
	{
		return m_allocator->allocate(p_size, p_alignment);
	}

	template<class allocatorType, bool propagateReference>
	inline void opaque_allocator_reference<allocatorType, propagateReference>::deallocate(void * p_ptr, std::size_t p_size)
	{
		m_allocator->deallocate(p_ptr, p_size);
	}

	/*!
		@class opaque_allocator_immutable_reference

		@brief An allocator that acts as reference to another allocator.

		@tparam allocatorType The type of the referenced allocator. This type meet the requirements
		of 'JKAllocator'.
	*/
	template <class allocatorType>
	class opaque_allocator_immutable_reference
	{
	public:

		using propagate_on_container_copy_assignment = std::false_type;
		using propagate_on_container_move_assignment = std::false_type;
		using propagate_on_container_swap = std::false_type;
		using is_always_equal = std::false_type;

		opaque_allocator_immutable_reference(allocatorType& p_allocator);

		opaque_allocator_immutable_reference(const opaque_allocator_immutable_reference&) = default;
		opaque_allocator_immutable_reference(opaque_allocator_immutable_reference&&) = default;

	public:

		opaque_allocator_immutable_reference& operator=(const opaque_allocator_immutable_reference&) = delete;
		opaque_allocator_immutable_reference& operator=(opaque_allocator_immutable_reference&&) = delete;

		/*! @brief Deep equality comparison.*/
		bool operator==(const opaque_allocator_immutable_reference& p_rhs) const;

		/*! @brief Deep inequality comparison.*/
		bool operator!=(const opaque_allocator_immutable_reference& p_rhs) const;

		/*!
			@brief Allocates memory using the referenced allocator.
			@details Simply pass through any allocation requests to the referenced allocator.
		*/
		void* allocate(std::size_t p_size, std::size_t p_alignment);

		/*!
			@brief Dealocates memory using the referenced allocator.
			@details Simply pass through any deallocation requests to the referenced allocator.
		*/
		void deallocate(void* p_ptr, std::size_t p_size);

	private:

		allocatorType& m_allocator;

	};

	template<class allocatorType>
	inline opaque_allocator_immutable_reference<allocatorType>::opaque_allocator_immutable_reference(allocatorType& p_allocator)
		: m_allocator(p_allocator)
	{
	}

	template<class allocatorType>
	inline bool opaque_allocator_immutable_reference<allocatorType>::operator==(const opaque_allocator_immutable_reference & p_rhs) const
	{
		return m_allocator == p_rhs.m_allocator;
	}

	template<class allocatorType>
	inline bool opaque_allocator_immutable_reference<allocatorType>::operator!=(const opaque_allocator_immutable_reference & p_rhs) const
	{
		return m_allocator != p_rhs.m_allocator;
	}

	template<class allocatorType>
	inline void* opaque_allocator_immutable_reference<allocatorType>::allocate(std::size_t p_size, std::size_t p_alignment)
	{
		return m_allocator.allocate(p_size, p_alignment);
	}

	template<class allocatorType>
	inline void opaque_allocator_immutable_reference<allocatorType>::deallocate(void* p_ptr, std::size_t p_size)
	{
		m_allocator.deallocate(p_ptr, p_size);
	}

}

#endif