/*!
	@file		bucket_allocator.h
	@author		Alastair Holmes
	@date		21/11/2018
 */

#ifndef JKUTIL_BUCKET_ALLOCATOR_H
#define JKUTIL_BUCKET_ALLOCATOR_H

#include <jkutil\vector.h>
#include <jkutil\bitwise.h>
#include <jkutil\memory.h>

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
			: m_bucket_size_step(p_bucket_power_step == 0 ? 1 : p_bucket_power_step),
			m_minimum_bucket_size(jkutil::downto_pow2(p_smallest_bucket)),
			m_minimum_bucket_size_log(jkutil::int_log2(m_minimum_bucket_size)),
			m_buckets(),
			m_fallback_allocator(p_fallback_allocator)
		{
			std::size_t bucket_size = m_minimum_bucket_size;
			for (std::size_t i = 0; i < calculate_bucket_count(p_buckets, m_minimum_bucket_size, m_bucket_size_step); ++i)
			{
				m_buckets.emplace_back(p_allocator_constructor(i, bucket_size, bucket_size)); //bucket index, bucket allocate size, bucket allocate alignment
				bucket_size = bucket_size << m_bucket_size_step;
			}
		}

		bucket_allocator(const bucket_allocator&) = default;
		bucket_allocator(bucket_allocator&&) = default;

		bucket_allocator& operator=(const bucket_allocator&) = default;
		bucket_allocator& operator=(bucket_allocator&&) = default;

		bool operator==(const bucket_allocator& p_rhs) const
		{
			return m_bucket_size_step == p_rhs.m_bucket_size_step &&
				m_minimum_bucket_size == p_rhs.m_minimum_bucket_size &&
				m_buckets == p_rhs.m_buckets &&
				m_fallback_allocator == p_rhs.m_fallback_allocator;
		}

		bool operator!=(const bucket_allocator& p_rhs) const
		{
			return m_bucket_size_step != p_rhs.m_bucket_size_step ||
				m_minimum_bucket_size != p_rhs.m_minimum_bucket_size ||
				m_buckets != p_rhs.m_buckets ||
				m_fallback_allocator != p_rhs.m_fallback_allocator;
		}

		void* allocate(std::size_t p_size, std::size_t p_alignment)
		{
			std::size_t bucket_index = 0;
			std::size_t log_size = jkutil::int_ceil_log2(p_size);

			if (log_size > m_minimum_bucket_size_log)
			{
				bucket_index = (((log_size - m_minimum_bucket_size_log) - 1) / m_bucket_size_step) + 1; //(log_size - m_minimum_bucket_size_log) / m_bucket_size_step (ROUND UP)
			}

			return bucket_allocate(bucket_index, p_size, p_alignment);
		}

		void deallocate(void* p_ptr, std::size_t p_size)
		{
			std::size_t bucket_index = 0;
			std::size_t log_size = jkutil::int_ceil_log2(p_size);

			if (log_size > m_minimum_bucket_size_log)
			{
				bucket_index = (((log_size - m_minimum_bucket_size_log) - 1) / m_bucket_size_step) + 1; //(log_size - m_minimum_bucket_size_log) / m_bucket_size_step (ROUND UP)
			}

			bucket_deallocate(bucket_index, p_ptr, p_size);
		}

	private:

		void* bucket_allocate(std::size_t p_bucket_index, std::size_t p_size, std::size_t p_alignment)
		{
			if (p_bucket_index < m_buckets.size())
			{
				std::size_t bucket_size = m_minimum_bucket_size << (p_bucket_index * m_bucket_size_step);
				JKUTIL_ASSERT(bucket_size >= p_size);
				return jkutil::memory_allocate(m_buckets.at(p_bucket_index), bucket_size, bucket_size);
			}
			else
			{
				return jkutil::memory_allocate(m_fallback_allocator, p_size, p_alignment);
			}
		}

		void bucket_deallocate(std::size_t p_bucket_index, void* p_ptr, std::size_t p_size)
		{
			if (p_bucket_index < m_buckets.size())
			{
				std::size_t bucket_size = m_minimum_bucket_size << (p_bucket_index * m_bucket_size_step);
				JKUTIL_ASSERT(bucket_size >= p_size);
				jkutil::memory_deallocate(m_buckets.at(p_bucket_index), p_ptr, bucket_size);
			}
			else
			{
				jkutil::memory_deallocate(m_fallback_allocator, p_ptr, p_size);
			}
		}

		static std::size_t calculate_bucket_count(std::size_t p_maximum_buckets, std::size_t p_minimum_bucket_size, std::size_t p_bucket_size_step)
		{
			JKUTIL_ASSERT(jkutil::is_pow2(p_minimum_bucket_size) && p_bucket_size_step > 0);
			std::size_t log_min_bucket = jkutil::int_log2(p_minimum_bucket_size); //Note: Always, std::numeric_limits<std::size_t>::digits > log_min_bucket
			//lmb is the zero-based index of the minimum bucket.
			std::size_t bit_count = log_min_bucket + (p_maximum_buckets * p_bucket_size_step) - 1; //bit_count is the zero-based index of the maximum bucket.
			return (static_cast<std::size_t>(std::numeric_limits<std::size_t>::digits) > bit_count) ? p_maximum_buckets : ((static_cast<std::size_t>(std::numeric_limits<std::size_t>::digits) - log_min_bucket) / p_bucket_size_step);
		}

		std::size_t m_bucket_size_step;
		std::size_t m_minimum_bucket_size;
		std::size_t m_minimum_bucket_size_log;
		jkutil::vector<allocatorType> m_buckets;
		fallbackAllocator m_fallback_allocator;

	};

}

#endif