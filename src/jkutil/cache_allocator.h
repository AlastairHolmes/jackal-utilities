/*!
	@file		cache_allocator.h
	@author		Alastair Holmes
	@date		20/11/2018
 */

#ifndef JKUTIL_CACHE_ALLOCATOR_H
#define JKUTIL_CACHE_ALLOCATOR_H

#include <jkutil\assert.h>
#include <jkutil\memory.h>
#include <jkutil\allocator.h>
#include <cstddef>
#include <limits>
#include <utility>
#include <memory>

namespace jkutil
{

	template <class allocatorType = jkutil::allocator, bool propagateAllocator = false>
	class safe_cache_allocator //JKStorableAllocator
	{
	private:

		//The purpose of this allocator is that it is always valid to deallocate anything allocated
		//through this allocator through its internal underlying allocator and vice versa.

		using propagate_on_container_copy_assignment = std::bool_constant<propagateAllocator>;
		using propagate_on_container_move_assignment = std::bool_constant<propagateAllocator>;
		using propagate_on_container_swap = std::bool_constant<propagateAllocator>;
		using is_always_equal = typename allocatorType::is_always_equal;

		using cache_element_t = void*;

	public:

		safe_cache_allocator(std::size_t p_element_size, std::size_t p_element_alignment, std::size_t p_maximum_pool_size = 0, const allocatorType& p_allocator = allocatorType())
			: m_element_size(p_element_size >= sizeof(cache_element_t) ? p_element_size : sizeof(cache_element_t)),
			m_element_alignment(p_element_alignment >= alignof(cache_element_t) ? p_element_alignment : alignof(cache_element_t)),
			m_maximum_cache_size(p_maximum_pool_size == 0 ? std::numeric_limits<std::size_t>::max() : p_maximum_pool_size),
			m_allocator(p_allocator),
			m_cache(nullptr),
			m_current_cache_size(0)
		{
			JKUTIL_ASSERT(p_element_size > sizeof(cache_element_t));
		}

		~safe_cache_allocator()
		{
			reset();
		}

		safe_cache_allocator(const safe_cache_allocator& p_instance)
			: m_element_size(p_instance.m_element_size),
			m_element_alignment(p_instance.m_element_alignment),
			m_maximum_cache_size(p_instance.m_maximum_cache_size),
			m_allocator(p_instance.m_allocator),
			m_cache(nullptr),
			m_current_cache_size(0)
		{

		}

		safe_cache_allocator(safe_cache_allocator&& p_instance)
			: m_element_size(p_instance.m_element_size),
			m_element_alignment(p_instance.m_element_alignment),
			m_maximum_cache_size(p_instance.m_maximum_cache_size),
			m_allocator(std::move(p_instance.m_allocator)),
			m_cache(p_instance.m_cache),
			m_current_cache_size(p_instance.m_current_cache_size)
		{
			p_instance.m_cache = nullptr;
			p_instance.m_current_cache_size = 0;
		}

		safe_cache_allocator& operator=(const safe_cache_allocator& p_rhs)
		{
			reset();

			m_element_size = p_rhs.m_element_size;
			m_element_alignment = p_rhs.m_element_alignment;
			m_maximum_cache_size = p_rhs.m_maximum_cache_size;

			m_allocator = p_rhs.m_allocator;

			m_cache = nullptr;
			m_current_cache_size = 0;

			return *this;
		}

		safe_cache_allocator& operator=(safe_cache_allocator&& p_rhs)
		{
			reset();

			m_element_size = p_rhs.m_element_size;
			m_element_alignment = p_rhs.m_element_alignment;
			m_maximum_cache_size = p_rhs.m_maximum_cache_size;

			m_allocator = std::move(p_rhs.m_allocator);

			m_cache = p_rhs.m_cache;
			m_current_cache_size = p_rhs.m_current_cache_size;

			p_rhs.m_cache = nullptr;
			p_rhs.m_current_cache_size = 0;

			return  *this;
		}

		bool operator==(const safe_cache_allocator& p_rhs) const
		{
			return m_allocator == p_rhs.m_allocator;
		}

		bool operator!=(const safe_cache_allocator& p_rhs) const
		{
			return m_allocator != p_rhs.m_allocator;
		}

		void* allocate(std::size_t p_size, std::size_t p_alignment)
		{
			if (m_element_size == p_size && p_alignment < m_element_alignment)
			{
				p_alignment = m_element_alignment;
			}

			if (m_cache != nullptr && m_element_size == p_size && p_alignment <= m_element_alignment)
			{
				cache_element_t temp = m_cache;
				m_cache = *m_cache;
				return temp;
			}
			else
			{
				jkutil::memory_allocate(m_allocator, p_size, p_alignment);
			}
		}

		void deallocate(void* p_ptr, std::size_t p_size)
		{
			void* temp = p_ptr;
			std::size_t temp_size = 1;
			if (m_current_cache_size < m_maximum_cache_size && m_element_size == p_size && std::align(m_element_alignment,1,temp,temp_size) == p_ptr)
			{
				cache_element_t* temp = reinterpret_cast<cache_element_t*>(p_ptr);
				*temp = m_cache;
				m_cache = temp;
				++m_current_cache_size;
			}
			else
			{
				jkutil::memory_deallocate(m_allocator, p_ptr, p_size);
			}

		}

		void reset()
		{
			m_current_cache_size = 0;
			while (m_cache != nullptr)
			{
				cache_element_t* temp = *reinterpret_cast<cache_element_t*>(m_cache);
				jkutil::memory_deallocate(m_allocator, m_cache, m_element_size);
				m_cache = temp;
			}
		}

		const allocatorType& get_allocator() const
		{
			return m_allocator;
		}

	private:

		std::size_t m_element_size;
		std::size_t m_element_alignment;
		std::size_t m_maximum_cache_size;
		allocatorType m_allocator;
		cache_element_t m_cache;
		std::size_t m_current_cache_size;

	};

	template <class allocatorType = jkutil::allocator, bool propagateAllocator = false>
	class cache_allocator //JKStorableAllocator
	{
	private:

		using cache_element_t = void*;

	public:

		using propagate_on_container_copy_assignment = std::bool_constant<propagateAllocator>;
		using propagate_on_container_move_assignment = std::bool_constant<propagateAllocator>;
		using propagate_on_container_swap = std::bool_constant<propagateAllocator>;
		using is_always_equal = std::false_type;

		cache_allocator(std::size_t p_element_size, std::size_t p_element_alignment, std::size_t p_maximum_pool_size = 0, const allocatorType& p_allocator = allocatorType())
			: m_element_size(p_element_size >= sizeof(cache_element_t) ? p_element_size : sizeof(cache_element_t)),
			m_element_alignment(p_element_alignment >= alignof(cache_element_t) ? p_element_alignment : alignof(cache_element_t)),
			m_maximum_cache_size(p_maximum_pool_size == 0 ? std::numeric_limits<std::size_t>::max() : p_maximum_pool_size),
			m_allocator(p_allocator),
			m_cache(nullptr),
			m_current_cache_size(0)
		{}

		~cache_allocator()
		{
			reset();
		}

		cache_allocator(const cache_allocator& p_instance)
			: m_element_size(p_instance.m_element_size),
			m_element_alignment(p_instance.m_element_alignment),
			m_maximum_cache_size(p_instance.m_maximum_cache_size),
			m_allocator(p_instance.m_allocator),
			m_cache(nullptr),
			m_current_cache_size(0)
		{

		}

		cache_allocator(cache_allocator&& p_instance)
			: m_element_size(p_instance.m_element_size),
			m_element_alignment(p_instance.m_element_alignment),
			m_maximum_cache_size(p_instance.m_maximum_cache_size),
			m_allocator(std::move(p_instance.m_allocator)),
			m_cache(p_instance.m_cache),
			m_current_cache_size(p_instance.m_current_cache_size)
		{
			p_instance.m_cache = nullptr;
			p_instance.m_current_cache_size = 0;
		}

		cache_allocator& operator=(const cache_allocator& p_rhs)
		{
			reset();

			m_element_size = p_rhs.m_element_size;
			m_element_alignment = p_rhs.m_element_alignment;
			m_maximum_cache_size = p_rhs.m_maximum_cache_size;

			m_allocator = p_rhs.m_allocator;

			return *this;
		}

		cache_allocator& operator=(cache_allocator&& p_rhs)
		{
			reset();

			m_element_size = p_rhs.m_element_size;
			m_element_alignment = p_rhs.m_element_alignment;
			m_maximum_cache_size = p_rhs.m_maximum_cache_size;

			m_allocator = std::move(p_rhs.m_allocator);

			m_cache = p_rhs.m_cache;
			m_current_cache_size = p_rhs.m_current_cache_size;
			p_rhs.m_cache = nullptr;
			p_rhs.m_current_cache_size = 0;

			return *this;

		}

		bool operator==(const cache_allocator& p_rhs) const
		{
			return m_allocator == p_rhs.m_allocator && (m_element_size == p_rhs.m_element_size) && (m_element_alignment == p_rhs.m_element_alignment);
		}

		bool operator!=(const cache_allocator& p_rhs) const
		{
			return m_allocator != p_rhs.m_allocator || (m_element_size != p_rhs.m_element_size) || (m_element_alignment != p_rhs.m_element_alignment);
		}

		void* allocate(std::size_t p_size, std::size_t p_alignment)
		{
			JKUTIL_ASSERT(p_size <= m_element_size && p_alignment <= m_element_alignment);

			if (m_cache != nullptr)
			{
				cache_element_t temp = m_cache;
				m_cache = *m_cache;
				return temp;
			}
			else
			{
				jkutil::memory_allocate(m_allocator, m_element_size, m_element_alignment);
			}
		}

		void deallocate(void* p_ptr, std::size_t p_size)
		{
			JKUTIL_ASSERT(p_size <= m_element_size);

			if (m_current_cache_size < m_maximum_cache_size)
			{
				cache_element_t* temp = reinterpret_cast<cache_element_t*>(p_ptr);
				*temp = m_cache;
				m_cache = temp;
				++m_current_cache_size;
			}
			else
			{
				jkutil::memory_deallocate(m_allocator, p_ptr, m_element_size);
			}

		}

		void reset()
		{
			m_current_cache_size = 0;
			while (m_cache != nullptr)
			{
				cache_element_t* temp = *reinterpret_cast<cache_element_t*>(m_cache);
				jkutil::memory_deallocate(m_allocator, m_cache, m_element_size);
				m_cache = temp;
			}
		}

		const allocatorType& get_allocator() const
		{
			return m_allocator;
		}

	private:

		std::size_t m_element_size;
		std::size_t m_element_alignment;
		std::size_t m_maximum_cache_size;
		allocatorType m_allocator;
		cache_element_t m_cache;
		std::size_t m_current_cache_size;

	};

}

#endif