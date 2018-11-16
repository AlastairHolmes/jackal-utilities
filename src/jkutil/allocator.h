/*!
	@file		allocator.h
	@author		Alastair Holmes
	@date		26/10/2018
 */

#ifndef JKUTIL_ALLOCATOR_H
#define JKUTIL_ALLOCATOR_H

#include <jkutil\type_traits.h>

namespace jkutil
{

	// Named Requirment: JKAllocator
		// void* allocate(size_t p_size, size_t p_alignment);
			//Allocates a block of memory of size \c p_size and alignment \c p_alignment. May throw exceptions.
			//Returns a pointer to the allocated memory.
		// void deallocate(void* p_ptr, size_t p_size);
			//Deallocates storage pointed to p_ptr, which must be a value returned by a
			//previous call to allocate that has not been invalidated by an intervening
			//call to deallocate. p_size must match the value previously passed to allocate.
			//Does not/Must not throw exceptions.

	// Named Requirment: JKStorableAllocator
		// JKAllocator
		// bool operator==()
			//If two allocators A and B are equal it must be valid to use B to deallocate memory allocated using A and vice versa.
			//Therefore for any allocators where is_always_equal = std::true_type, deallocating an allocation using different instances
			//must always be valid.
		// bool operator!=(): A != B must be equivalent to A == B.
		// copy constructor (optional)
		// move constructor (optional)
			//If A is move constructed using B, it must be valid to deallocate memory allocated with B before the move, with A. 
		// copy assignment (optional)
		// move assignment (optional)
			//If A is move assigned with B, it must be valid to deallocate memory allocated with B before the move, with A. 
		// propagate_on_container_copy_assignment: std::true_type or std::false_type
			//Defines if a container should propagate allocator values when the container is copy assigned.
		// propagate_on_container_move_assignment: std::true_type or std::false_type
			//Defines if a container should propagate allocator values when the container is move assigned.
		// propagate_on_container_swap: std::true_type or std::false_type
			//Defines if a container should propagate allocator values when the container is swapped.
				//If equal to std::true_type the swap is done using move assignment and construction.
					/*
						auto temp = std::move(A);
						A = std::move(B);
						B = std::move(A);
					*/
		// is_always_equal: std::true_type or std::false_type
			//std::true_type if and only if operator==() always returns true.


	/*!
		@class virtual_allocator

		@brief The interface used to pass allocators in an abstract way.

		@details This interface allows any type of allocator to be passed into a function by wrapping it in
		a virtual_allocator_adapter, this is particular useful for the implementation of any_derived. Any
		class that implements this interface must meet the requirement JKAllocator.

		@attention A custom allocator DOES NOT need to inherit this interface.
	*/
	class virtual_allocator
	{
	public:

		virtual void* allocate(size_t p_size, size_t p_alignment) = 0;
		virtual void deallocate(void* p_ptr, size_t p_size) = 0;

	};

	/*!
		@class virtual_allocator_adapter

		@brief A type erasure wrapper for JKAllocator's, so an allocator can be passed abstractly as a \c virtual_allocator&.

		@details Stores an instance of \c allocatorType internally and uses it to implement the virtual_allocator interface,
		by passing the calls to \c allocate and \c deallocate to it. \c allocatorType must meet the named requirement 'JKAllocator'.

		@tparam allocatorType The type of the allocator to be adapted. Must meet the named requirement 'JKAllocator'.

		@see virtual_allocator
	*/
	template <class allocatorType>
	class virtual_allocator_adapter : public virtual_allocator
	{
	public:

		virtual_allocator_adapter(const allocatorType& p_allocator)
			: m_allocator(p_allocator)
		{
		}

		virtual_allocator_adapter(allocatorType&& p_allocator)
			: m_allocator(std::move(p_allocator))
		{
		}

		/*!
			@fn void* allocate(size_t p_size, size_t p_alignment)
			@brief Passes allocation requests directly to the internal instance of \c allocatorType.
			@see virtual_allocator::allocate
		*/
		virtual void* allocate(size_t p_size, size_t p_alignment) override final
		{
			return m_allocator.allocate(p_size, p_alignment);
		}

		/*!
			@fn void deallocate(void* p_ptr, size_t p_size)
			@brief Passes deallocation requests directly to the internal instance of \c allocatorType.
			@see virtual_allocator::deallocate
		*/
		virtual void deallocate(void* p_ptr, size_t p_size) override final
		{
			m_allocator.deallocate(p_ptr, p_size);
		}

	private:

		allocatorType m_allocator;

	};

	/*!
		@class allocator	
		@brief A simple 'JKStorableAllocator' that uses \c jkutil::aligned_malloc and \c jkutil::aligned_free to implement \c allocate and \c deallocate. This allocator is thread-safe if and only if the system's malloc and free are thread-safe.
		@details This class also serves as an example of a 'JKStorableAllocator'.
		@see jkutil::aligned_malloc jkutil::aligned_free
	*/
	class allocator
	{
	public:

		using propagate_on_container_copy_assignment = std::false_type;
		using propagate_on_container_move_assignment = std::false_type;
		using propagate_on_container_swap = std::false_type;
		using is_always_equal = std::true_type;

		/*!
			@fn void* allocate(size_t p_size, size_t p_alignment);
			@brief Allocates and returns a block of memory, using jkutil::aligned_malloc
			@see jkutil::aligned_malloc
		*/
		void* allocate(size_t p_size, size_t p_alignment);

		/*!
			@fn void deallocate(void* p_ptr, size_t p_size);
			@brief Deallocates the memory pointed by \p p_ptr, using jkutil::aligned_free
			@details Deallocates storage pointed to p_ptr, which must be a value returned by a
			previous call to allocate that has not been invalidated by an intervening
			call to deallocate.
			@see jkutil::aligned_free
		*/
		void deallocate(void* p_ptr, size_t p_size);

		bool operator==(const allocator& p_instance) const;
		bool operator!=(const allocator& p_instance) const;

	};

	/*! 
		@class allocator_stl_adapter

		@brief Adapters a 'JKStorableAllocator' to the C++17 standard's named requirement 'Allocator'. See the standard's named requirement 'Allocator' for more details.

		@tparam elementType The type of elements that will be allocated.
		@tparam storableAllocatorType The type of the allocator to be adapted. Must meet the named requirement JKStorableAllocator.
	*/
	template <class elementType, class storableAllocatorType>
	class allocator_stl_adapter
	{
	private:

		template <class otherElementType, class otherStorableAllocatorType>
		friend class allocator_stl_adapter;

	public:

		using value_type = elementType;
		using size_type = size_t;
		using pointer = elementType*;

		using propagate_on_container_copy_assignment = typename storableAllocatorType::propagate_on_container_copy_assignment;
		using propagate_on_container_move_assignment = typename storableAllocatorType::propagate_on_container_move_assignment;
		using propagate_on_container_swap = typename storableAllocatorType::propagate_on_container_swap;
		using is_always_equal = typename storableAllocatorType::is_always_equal;

	public:

		allocator_stl_adapter(const storableAllocatorType& p_allocator)
			: m_allocator(p_allocator)
		{}

		allocator_stl_adapter(storableAllocatorType&& p_allocator)
			: m_allocator(std::move(p_allocator))
		{}

		allocator_stl_adapter(const allocator_stl_adapter&) = default;
		allocator_stl_adapter(allocator_stl_adapter&&) = default;

		template <class otherElementType>
		allocator_stl_adapter(const allocator_stl_adapter<otherElementType, storableAllocatorType>& p_instance)
			: m_allocator(p_instance.m_allocator)
		{

		}

		template <class otherElementType>
		allocator_stl_adapter(allocator_stl_adapter<otherElementType, storableAllocatorType>&& p_instance)
			: m_allocator(std::move(p_instance.m_allocator))
		{

		}

		allocator_stl_adapter& operator=(const allocator_stl_adapter&) = default;
		allocator_stl_adapter& operator=(allocator_stl_adapter&&) = default;

		template <class otherElementType>
		allocator_stl_adapter& operator=(const allocator_stl_adapter<otherElementType, storableAllocatorType>& p_instance)
		{
			m_allocator = p_instance.m_allocator;
			return *this;
		}

		template <class otherElementType>
		allocator_stl_adapter& operator=(allocator_stl_adapter<otherElementType, storableAllocatorType>&& p_instance)
		{
			m_allocator = std::move(p_instance.m_allocator);
			return *this;
		}

		pointer allocate(size_type p_size)
		{
			return static_cast<pointer>(m_allocator.allocate(sizeof(value_type) * p_size, alignof(value_type)));
		}

		void deallocate(pointer p_ptr, size_type p_size)
		{
			m_allocator.deallocate(p_ptr, sizeof(value_type) * p_size);
		}

		bool operator==(const allocator_stl_adapter& p_instance) const
		{
			return m_allocator == p_instance.m_allocator;
		}

		bool operator!=(const allocator_stl_adapter& p_instance) const
		{
			return m_allocator != p_instance.m_allocator;
		}

		const storableAllocatorType& get_allocator() const
		{
			return m_allocator;
		}

	private:

		storableAllocatorType m_allocator;

	};

	template <class storableAllocatorType>
	class allocator_store
	{
	public:

		allocator_store(const storableAllocatorType& p_allocator)
			: m_allocator(p_allocator)
		{

		}

		allocator_store(const allocator_store& p_instance)
			: m_allocator(p_instance.m_allocator)
		{
		}

		allocator_store(allocator_store&& p_instance)
			: m_allocator(p_instance.m_allocator)
		{
		}

		allocator_store& operator=(const allocator_store& p_instance)
		{
			if constexpr (storableAllocatorType::propagate_on_container_copy_assignment::value)
			{
				m_allocator = p_instance.m_allocator;
			}
			return *this;
		}

		allocator_store& operator=(allocator_store&& p_instance)
		{
			if constexpr (storableAllocatorType::propagate_on_container_move_assignment::value)
			{
				m_allocator = std::move(p_instance.m_allocator);
			}
			return *this;
		}

		void swap(allocator_store& p_instance)
		{
			if constexpr (storableAllocatorType::propagate_on_container_swap::value)
			{
				std::swap(m_allocator, p_instance.m_allocator);
			}
		}

		storableAllocatorType& get_allocator()
		{
			return m_allocator;
		}

		const storableAllocatorType& get_allocator() const
		{
			return m_allocator;
		}

	private:

		storableAllocatorType m_allocator;

	};

}

#endif