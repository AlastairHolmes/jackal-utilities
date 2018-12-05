#include <jkutil\test_allocator.h>

namespace jkutil
{

	test_allocator::~test_allocator() noexcept(false)
	{
		if (!m_allocation.empty())
		{
			throw std::runtime_error("Invalid usage of the allocator interface.");
		}
	}

	test_allocator::test_allocator(const test_allocator& p_instance)
	{}

	test_allocator::test_allocator(test_allocator&& p_instance)
		: m_allocation(std::move(p_instance.m_allocation))
	{
		p_instance.m_allocation.clear();
	}

	test_allocator& test_allocator::operator=(const test_allocator& p_rhs)
	{
		return *this;
	}

	test_allocator& test_allocator::operator=(test_allocator&& p_rhs)
	{
		m_allocation = std::move(p_rhs.m_allocation);
		p_rhs.m_allocation.clear();
		return *this;
	}

	void* test_allocator::allocate(std::size_t p_size, std::size_t p_alignment)
	{
		void* ptr = m_allocator.allocate(p_size, p_alignment);
		m_allocation.push_back(std::make_pair(ptr, p_size));
		return ptr;
	}

	void test_allocator::deallocate(void* p_ptr, std::size_t p_size)
	{
		auto position = std::find(m_allocation.begin(), m_allocation.end(), std::make_pair(p_ptr, p_size));
		if (position == m_allocation.end())
		{
			throw std::runtime_error("Invalid usage of the allocator interface.");
		}
		else
		{
			m_allocation.erase(position);
			m_allocator.deallocate(p_ptr, p_size);
		}
	}

	bool test_allocator::operator==(const allocator& p_instance) const
	{
		return false;
	}

	bool test_allocator::operator!=(const allocator& p_instance) const
	{
		return true;
	}

}