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
		// void deallocate(void* p_ptr, size_t p_size);
			//Deallocates storage pointed to p_ptr, which must be a value returned by a
			//previous call to allocate that has not been invalidated by an intervening
			//call to deallocate. p_size must match the value previously passed to allocate.
			//Does not throw exceptions.

	/*!
		@class virtual_allocator

		@brief The interface used to pass allocators in an abstract way.

		@attention A custom allocator DOES NOT need to inherit this interface.
	*/
	class virtual_allocator
	{
	public:

		/*!
			@fn virtual void* allocate(size_t p_size, size_t p_alignment)
			
			@brief Allocates a continous block of \p p_size bytes and aligned to atleast \p p_alignment.

			@details Implementations of this function should never return \c nullptr, but
			use std::bad_alloc exceptions to indicate errors.

			@param p_size The size in bytes of the requested memory block.
			@param p_alignment The alignment of the requested memory block.
			@return A void pointer to the memory block of the reuested size and alignment.
		*/
		virtual void* allocate(size_t p_size, size_t p_alignment) = 0;

		/*!
			@fn virtual void deallocate(void* p_ptr, size_t p_size)

			@brief Deallocates the memory pointed by \p p_ptr.

			@details This function only needs to except values of \p p_ptr, that where previously returned from
			the corresponding \c allocate function; the \p p_size parameter will always match the \p p_size used during
			the corresponding call to \c allocate. \c deallocate will only be called once for each call to allocate, as
			in all jkutil structures guarentee they will not attempt to deallocate the same piece of memory more than once,
			unless it has been reallocated (by another call to allocate) between the deallocations. Implementations
			should avoid throwing exceptions from \c deallocate, but instead use asserts if reasonable. 

			@param p_ptr A void pointer to the memory to deallocate.
			@param p_psize The size in bytes of the memory to deallocate.
		*/
		virtual void deallocate(void* p_ptr, size_t p_size) = 0;

	};

	/*!
		@class virtual_allocator_adapter

		@brief A type erasure wrapper for JKAllocator's, so an allocator can be passed abstractly as a \c virtual_allocator&.

		@details Stores an instance of \c allocatorType internally and uses it to implement the virtual_allocator interface,
		by passing the calls to \c allocate and \c deallocate to it. \c allocatorType must meet the named requirement 'JKAllocator'.

		@tparam allocatorType The type of the allocator to be adapted.

		@see virtual_allocator
	*/
	template <class allocatorType>
	class virtual_allocator_adapter : public virtual_allocator
	{
	public:

		/*!
			@fn virtual_allocator_adapter(const allocatorType& p_allocator)

			@param p_allocator Copied by the internal instance of \c allocatorType.

			@brief Uses \c allocatorType's copy constructor to construct an internal instance of
			\c allocatorType.
		*/
		virtual_allocator_adapter(const allocatorType& p_allocator)
			: m_allocator(p_allocator)
		{

		}

		/*!
			@fn virtual_allocator_adapter(allocatorType&& p_allocator)

			@param p_allocator Moved into the internal instance of \c allocatorType.

			@brief Uses \c allocatorType's move constructor to construct an internal instance of
			\c allocatorType.
		*/
		virtual_allocator_adapter(allocatorType&& p_allocator)
			: m_allocator(std::move(p_allocator))
		{

		}

		/*!
			@fn void* allocate(size_t p_size, size_t p_alignment)

			@brief Passes allocation requests directly to the internal instance of \c allocatorType.

			@param p_size The size in bytes of the requested memory block.
			@param p_alignment The alignment of the requested memory block.
			@return A void pointer to the memory block of the reuested size and alignment.

			@see virtual_allocator::allocate
		*/
		virtual void* allocate(size_t p_size, size_t p_alignment) override final
		{
			return m_allocator.allocate(p_size, p_alignment);
		}

		/*!
			@fn void deallocate(void* p_ptr, size_t p_size)

			@brief Passes deallocation requests directly to the internal instance of \c allocatorType.

			@param p_size The size in bytes of the requested memory block.
			@param p_alignment The alignment of the requested memory block.
			@return A void pointer to the memory block of the reuested size and alignment.

			@see virtual_allocator::deallocate
		*/
		virtual void deallocate(void* p_ptr, size_t p_size) override final
		{
			m_allocator.deallocate(p_ptr, p_size);
		}

	private:

		allocatorType m_allocator;

	};

	// JKStorableAllocator
		// JKAllocator
		// bool operator==()
			//If two allocators A and B are equal it must be valid to use B to deallocate memory allocated using A and vice versa. 
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
		@class allocator
		
		@brief A simple 'JKStorableAllocator' that uses \c jkutil::aligned_malloc and \c jkutil::aligned_free to implement \c allocate and \c deallocate.

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

			@param p_size The size of the block in bytes.
			@param p_alignment The alignment of the block, must be a power of two.

			@see jkutil::aligned_malloc
		*/
		void* allocate(size_t p_size, size_t p_alignment);

		/*!
			@fn void deallocate(void* p_ptr, size_t p_size);

			@brief Deallocates the memory pointed by \p p_ptr, using jkutil::aligned_free

			@details Deallocates storage pointed to p_ptr, which must be a value returned by a
			previous call to allocate that has not been invalidated by an intervening
			call to deallocate.

			@note Memory allocator using this class can be deallocated using any other instance of
			this class.

			@param p_ptr The memory to be deallocated.
			@param p_size Must match the value previously passed to allocate.

			@see jkutil::aligned_free
		*/
		void deallocate(void* p_ptr, size_t p_size);

		/*!
			@fn bool operator==(const allocator& p_instance) const;

			@brief Returns if two instances of \c allocator are equal.
			
			@param p_instance The other instance to compare.
			@return Always returns true.
		*/
		bool operator==(const allocator& p_instance) const;

		/*!
			@fn bool operator!=(const allocator& p_instance) const;

			@brief Returns if two instances of \c allocator are not equal.

			@param p_instance The other instance to compare.
			@return Always returns false.
		*/
		bool operator!=(const allocator& p_instance) const;

	};

	/*! 
		@class allocator_stl_adapter

		@brief Adapters a 'JKStorableAllocator' to the C++17 standard's named requirement 'Allocator'.
	
		@details The standard's documentation of the named requirement 'Allocator'.

		@tparam elementType The type of elements that will be allocated.
		@tparam storableAllocatorType The type of the allocator to be adapted.
	*/
	template <class elementType, class storableAllocatorType>
	class allocator_stl_adapter
	{
	public:

		/*!@brief STL Allocators require a value_type and only allocate their particular value_type.*/
		using value_type = elementType;
		/*!@brief Explicitiy setting size_type to the default.*/
		using size_type = size_t;
		/*!@brief pointer is a simple pointer as JKStorableAllocators always use simple pointers.*/
		using pointer = elementType*;

		/*!@brief Inherits the propagation of the adapted \c storableAllocatorType.*/
		using propagate_on_container_copy_assignment = typename storableAllocatorType::propagate_on_container_copy_assignment;
		/*!@brief Inherits the propagation of the adapted \c storableAllocatorType.*/
		using propagate_on_container_move_assignment = typename storableAllocatorType::propagate_on_container_move_assignment;
		/*!@brief Inherits the propagation of the adapted \c storableAllocatorType.*/
		using propagate_on_container_swap = typename storableAllocatorType::propagate_on_container_swap;
		/*!@brief Inherits the equality of the adapted \c storableAllocatorType.*/
		using is_always_equal = typename storableAllocatorType::is_always_equal;

	public:

		/*!@brief Copy constructs the adapted \c storableAllocatorType.*/
		allocator_stl_adapter(const storableAllocatorType& p_allocator)
			: m_allocator(p_allocator)
		{}

		/*!@brief Move constructs the adapted \c storableAllocatorType.*/
		allocator_stl_adapter(storableAllocatorType&& p_allocator)
			: m_allocator(std::move(p_allocator))
		{}

		/*!@brief Default copy constructor.*/
		allocator_stl_adapter(const allocator_stl_adapter&) = default;
		/*!@brief Default move constructor.*/
		allocator_stl_adapter(allocator_stl_adapter&&) = default;

		/*!@brief Default copy assignment.*/
		allocator_stl_adapter& operator=(const allocator_stl_adapter&) = default;
		/*!@brief Default move assignment.*/
		allocator_stl_adapter& operator=(allocator_stl_adapter&&) = default;

		/*!@brief Passes through allocation requests to the internal \c storableAllocatorType instance.*/
		pointer allocate(size_type p_size)
		{
			return static_cast<pointer>(m_allocator.allocate(sizeof(value_type) * p_size, alignof(value_type)));
		}

		/*!@brief Passes through deallocation requests to the internal \c storableAllocatorType instance.*/
		void deallocate(pointer p_ptr, size_type p_size)
		{
			m_allocator.deallocate(p_ptr, sizeof(value_type) * p_size);
		}

		/*!@brief Passes through equality to the underlying storableAllocatorType.*/
		bool operator==(const allocator_stl_adapter& p_instance) const
		{
			return m_allocator == p_instance.m_allocator;
		}

		/*!@brief Passes through equality to the underlying storableAllocatorType.*/
		bool operator!=(const allocator_stl_adapter& p_instance) const
		{
			return m_allocator != p_instance.m_allocator;
		}

	private:

		storableAllocatorType m_allocator;

	};



}

#endif