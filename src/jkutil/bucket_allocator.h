/*!
	@file		bucket_allocator.h
	@author		Alastair Holmes
	@date		21/11/2018
 */

#ifndef JKUTIL_BUCKET_ALLOCATOR_H
#define JKUTIL_BUCKET_ALLOCATOR_H

#include <jkutil\vector.h>
#include <jkutil\bitwise.h>

namespace jkutil
{

	template <class allocatorType, class fallbackAllocator = jkutil::allocator, bool propagateAllocator = false>
	class bucket_allocator
	{
	public:

		using propagate_on_container_copy_assignment = std::bool_constant<propagateAllocator>;
		using propagate_on_container_move_assignment = std::bool_constant<propagateAllocator>;
		using propagate_on_container_swap = std::bool_constant<propagateAllocator>;
		using is_always_equal = typename allocatorType::is_always_equal;

		template <class bucketAllocatorMapConstructorCallable>
		bucket_allocator(std::size_t p_buckets, std::size_t p_smallest_bucket, std::size_t p_bucket_power_step, bucketAllocatorMapConstructorCallable&& p_allocator_constructor, const fallbackAllocator& p_fallback_allocator)
			: m_bucket_count(p_buckets),
			m_bucket_size_step(p_bucket_power_step == 0 ? 1 : p_bucket_power_step)
			m_minimum_bucket_size(jkutil::ceil_pow2(p_smallest_bucket)),
			m_fallback_allocator(p_fallback_allocator)
		{
			JKUTIL_ASSERT((p_smallest_bucket & (p_smallest_bucket - 1)) == 0);
			JKUTIL_ASSERT((m_minimum_bucket_size << (p_buckets * m_bucket_size_step)) != 0 || (p_buckets == 0));
			JKUTIL_ASSERT(p_bucket_power_step != 0);

			std::size_t bucket_size = m_minimum_bucket_size;
			for (std::size_t i = 0; i < m_bucket_count; ++i)
			{
				m_buckets.emplace_back(p_allocator_constructor(i, bucket_size, bucket_size)); //bucket index, bucket allocate size, bucket allocate alignment
				bucket_size = bucket_size << m_bucket_size_step;
			}
		}

		bucket_allocator(const bucket_allocator& p_instance)
		{

		}

		bucket_allocator(bucket_allocator&& p_instance)
		{

		}

		bucket_allocator& operator=(const bucket_allocator& p_rhs)
		{

		}

		bucket_allocator& operator=(bucket_allocator&& p_rhs)
		{

		}

		bool operator==(const bucket_allocator& p_rhs)
		{

		}

		bool operator!=(const bucket_allocator& p_rhs)
		{

		}

		void* allocate(std::size_t p_size, std::size_t p_alignment)
		{
			JKUTIL_ASSERT(p_size <= jkutil::ceil_pow2(p_size) && p_alignment <= p_size);

			if (m_bucket_count == 0 || p_size > (m_minimum_bucket_size << m_bucket_count))
			{

			}
			else
			{
				std::size_t size = jkutil::ceil_pow2(p_size);


			}

		}

		void deallocate(void* p_ptr, std::size_t p_size)
		{

		}

	private:

		std::size_t m_bucket_count;
		std::size_t m_bucket_size_step;
		std::size_t m_minimum_bucket_size;
		jkutil::vector<allocatorType> m_buckets;
		fallbackAllocator m_fallback_allocator;

	};

}

#endif