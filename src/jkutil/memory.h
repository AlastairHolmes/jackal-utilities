/*!
	@file		memory.h
	@author		Alastair Holmes
	@date		26/10/2018
 */

#ifndef JKUTIL_MEMORY_H
#define JKUTIL_MEMORY_H

#include <jkutil\assert.h>

namespace jkutil
{

	/*!
		@brief Allocates memory using the system's malloc function.
		@details Memory allocated with this function must be deallocated using \c jkutil::aligned_free().
		@note Memory returned from this function can not be deallocated using a system's free() function,
		but only by using \c jkutil::aligned_free().
	*/
	void* aligned_malloc(size_t p_size, size_t p_alignment);

	/*!
		@brief Deallocates memory using the system's free function.
		@details Deallocates memory pointed by \p p_ptr which must be a value returned by a previous call to \c jkutil::aligned_malloc()
		that has not been invalidated by an intervening call to deallocate. \p p_size must match the value preiouvsly passed to
		\c jkutil::aligned_malloc().
	*/
	void aligned_free(void* p_ptr, size_t p_size);

	/*!
		@brief Does a memory copy using memcpy().
		@details The purpose of this function is to be safer than a direct \c memcpy.
		memcpy is undefined for nullptrs even if the size is zero. This function is valid
		is all reasonable cases, and will assert if not valid.
	*/
	void memory_copy(void* p_destination, const void* p_source, size_t p_size)
	{
		JKUTIL_ASSERT(p_size == 0 || (p_source != nullptr && p_destination != nullptr));

		if (p_size != 0 && p_destination != nullptr && p_source != nullptr)
		{
			memcpy(p_destination, p_source, p_size);
		}
	}

	/*!
		@brief Allocates memory using \p p_allocator of \p p_size bytes and alignment \p p_alignment.
		@return A ptr to the allocated memory.
	*/
	template <class allocatorType>
	inline void* memory_allocate(allocatorType& p_allocator, size_t p_size, size_t p_alignment)
	{
		return p_allocator.allocate(p_size, p_alignment);
	}

	/*! @brief Deallocates \p p_memory using \p p_allocator of \p p_size bytes. */
	template <class allocatorType>
	inline void memory_deallocate(allocatorType& p_allocator, void* p_memory, size_t p_size)
	{
		p_allocator.deallocate(p_memory, p_size);
	}

	/*! @brief Runs a placement new. */
	template <class constructType, class... argumentTypes>
	inline constructType* construct(void* p_memory, argumentTypes&&... p_arguments)
	{
		return new (p_memory) constructType(std::forward<argumentTypes>(p_arguments)...);
	}

	/*! @brief Calls destructor. */
	template <class deconstructType>
	inline void destruct(deconstructType* p_object)
	{
		p_object->~deconstructType();
	}

	/*!
		@class deallocate_guard
		@brief Ensures a piece of memory is deallocated exactly once even in the case of an exception.
		@details Whether the deallocation is done is dependent on if the guard is enabled or disable.
		@note Consider the example below:
			@code
				auto x = ...; //lambda
				//new scope S1
				{
					auto A = make_deallocate_guard(x);
					//new scope S2
					{
						auto B = std::move(A);
						//x is run here on the exiting of scope S2.
					}
					//x is not run here on the exiting of scope S1.
				}
			@endcode
			Here when \c A is moved to \c B, \c B takes over responsibility for running \c x, so x will be run when B is destructed and not when A is.
			A becomes inactive.
	*/
	template <class allocatorType>
	class deallocate_guard
	{
	public:

		deallocate_guard(allocatorType& p_allocator, void* p_memory, size_t p_size, bool p_enabled = true);

		deallocate_guard(const deallocate_guard&) = delete;
		deallocate_guard(deallocate_guard&& p_instance);

		deallocate_guard& operator=(const deallocate_guard&) = delete;
		deallocate_guard& operator=(deallocate_guard&&) = delete;

		~deallocate_guard();

		void disable();
		void enable();

	private:

		void run();

		bool m_run;
		bool m_enabled;
		allocatorType& m_allocator;

	protected:

		allocatorType& get_allocator();

		void* m_memory;
		size_t m_size;
	};

	template<class allocatorType>
	inline deallocate_guard<allocatorType>::deallocate_guard(allocatorType& p_allocator, void* p_memory, size_t p_size, bool p_enabled)
		: m_run(false),
		m_enabled(p_enabled),
		m_memory(p_memory),
		m_size(p_size),
		m_allocator(p_allocator)
	{
	}

	template<class allocatorType>
	inline deallocate_guard<allocatorType>::deallocate_guard(deallocate_guard&& p_instance)
		: m_run(p_instance.m_run),
		m_enabled(p_instance.m_enabled),
		m_memory(p_instance.m_memory),
		m_size(p_instance.m_size),
		m_allocator(p_instance.m_allocator)
	{
		p_instance.m_run = true;
		p_instance.m_active = false;
		p_instance.m_memory = nullptr;
		p_instance.m_size = 0;
	}

	template<class allocatorType>
	inline deallocate_guard<allocatorType>::~deallocate_guard()
	{
		run();
	}

	template<class allocatorType>
	inline void deallocate_guard<allocatorType>::disable()
	{
		m_enabled = false;
	}

	template<class allocatorType>
	inline void deallocate_guard<allocatorType>::enable()
	{
		m_enabled = true;
	}

	template<class allocatorType>
	inline allocatorType& deallocate_guard<allocatorType>::get_allocator()
	{
		return m_allocator;
	}

	template<class allocatorType>
	inline void deallocate_guard<allocatorType>::run()
	{
		if (m_enabled && !m_run)
		{
			memory_deallocate(m_allocator, m_memory, m_size);
			m_enabled = false;
		}
		m_run = true;
	}

	template <class allocatorType>
	class allocate_guard : public deallocate_guard<allocatorType>
	{
	public:

		allocate_guard(allocatorType& p_allocator, size_t p_size, size_t p_alignment, bool p_enabled = true);

		void* data() const;
		size_t size() const;

	};

	template<class allocatorType>
	inline allocate_guard<allocatorType>::allocate_guard(allocatorType& p_allocator, size_t p_size, size_t p_alignment, bool p_enabled)
		: deallocate_guard<allocatorType>(p_allocator, nullptr, p_size, false)
	{
		m_memory = memory_allocate(p_allocator, p_size, p_alignment);
		if (p_enabled)
		{
			enable();
		}
	}

	template<class allocatorType>
	inline void* allocate_guard<allocatorType>::data() const
	{
		return m_memory;
	}

	template<class allocatorType>
	inline size_t allocate_guard<allocatorType>::size() const
	{
		return m_size;
	}

	template <class allocatorType>
	deallocate_guard<allocatorType> make_deallocate_guard(allocatorType& p_allocator, void* p_memory, size_t p_size, bool p_enabled = true)
	{
		return deallocate_guard<allocatorType>(p_allocator, p_memory, p_size, p_enabled);
	}

	template <class allocatorType>
	allocate_guard<allocatorType> make_allocate_guard(allocatorType& p_allocator, size_t p_size, size_t p_alignment, bool p_enabled = true)
	{
		return allocate_guard<allocatorType>(p_allocator, p_size, p_alignment, p_enabled);
	}

	/*!
		@brief Allocates memory for and constructs an object.
		@details This will NOT leak memory if the constructor throws.
	*/
	template <class constructType, class allocatorType, class... argumentTypes>
	inline constructType* create(allocatorType& p_allocator, argumentTypes&&... p_arguments)
	{
		allocate_guard<allocatorType> memory = make_allocate_guard(p_allocator, sizeof(constructType), alignof(constructType));
		jkutil::construct<constructType>(memory.data(), std::forward<argumentTypes>(p_arguments)...);
		memory.disable();
		return static_cast<constructType*>(memory.data());
	}

	/*!
		@brief Destructs an object and deallocates its memory.
		@details This will NOT leak memory if the destructor throws.
	*/
	template <class deconstructType, class allocatorType>
	inline void destroy(allocatorType& p_allocator, deconstructType* p_element)
	{
		auto guard = make_deallocate_guard(p_allocator, p_element, sizeof(deconstructType));
		jkutil::destruct<deconstructType>(p_element);
	}

}

#endif