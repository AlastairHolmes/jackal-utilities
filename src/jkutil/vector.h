/*!
	@file		vector.h
	@author		Alastair Holmes
	@date		27/10/2018
 */

#ifndef JKUTIL_VECTOR_H
#define JKUTIL_VECTOR_H

#include <jkutil\utility.h>
#include <jkutil\allocator.h>
#include <jkutil\array_proxy.h>
#include <type_traits>
#include <vector>

namespace jkutil
{

	template <class elementType, class storableAllocatorType>
	class vector
	{
	private:

		using internal_vector_type = std::vector<elementType,jkutil::allocator_stl_adapter<elementType, storableAllocatorType>>;

	public:

		using value_type = typename internal_vector_type::value_type;
		using allocator_type = storableAllocatorType;
		using size_type = typename internal_vector_type::size_type;
		using difference_type = typename internal_vector_type::difference_type;
		using reference = typename internal_vector_type::reference;
		using const_reference = typename internal_vector_type::const_reference;
		using pointer = typename internal_vector_type::pointer;
		using const_pointer = typename internal_vector_type::const_pointer;
		using iterator = typename internal_vector_type::iterator;
		using const_iterator = typename internal_vector_type::const_iterator;
		using reverse_iterator = typename internal_vector_type::reverse_iterator;
		using const_reverse_iterator = typename internal_vector_type::const_reverse_iterator;

	public:

		vector() = default;

		explicit vector(const storableAllocatorType& p_allocator) noexcept;

		vector(const size_type p_count, const elementType& p_value = elementType(), const storableAllocatorType& p_allocator = storableAllocatorType());

		explicit vector(const size_type p_count, const storableAllocatorType& p_allocator = storableAllocatorType());

		template<class inputIteratorType>
		vector(inputIteratorType p_first, inputIteratorType p_last, const storableAllocatorType& p_allocator = storableAllocatorType());

		vector(const vector&) = default;

		vector(vector&&) noexcept = default;

		template <class otherStorableAllocatorType>
		vector(const vector<elementType, otherStorableAllocatorType>& p_instance, const storableAllocatorType& p_allocator);

		template <class otherStorableAllocatorType>
		vector(vector<elementType, otherStorableAllocatorType>&& p_instance, const storableAllocatorType& p_allocator);

		vector(std::initializer_list<elementType> p_list, const storableAllocatorType& p_allocator = storableAllocatorType());

	public:

		vector& operator=(const vector& p_rhs);

		vector& operator=(vector&& p_rhs);

		template <class otherStorableAllocatorType>
		vector& assign_value(const vector<elementType, otherStorableAllocatorType>& p_instance);

		template <class otherStorableAllocatorType>
		vector& assign_value(vector<elementType, otherStorableAllocatorType>&& p_instance);

		void swap(vector& p_instance);

		template <class otherStorableAllocatorType>
		void swap_value(vector<elementType, otherStorableAllocatorType>& p_instance);

		bool operator==(const vector& p_rhs) const;

		bool operator!=(const vector& p_rhs) const;

		template <class otherStorableAllocator>
		bool operator==(const vector<elementType, otherStorableAllocator>& p_instance) const;

		template <class otherStorableAllocator>
		bool operator!=(const vector<elementType, otherStorableAllocator>& p_instance) const;

	public:
		
		void assign(size_type p_count, const elementType& p_value);
		template <class inputIteratorType>
		void assign(inputIteratorType p_first, inputIteratorType p_last);
		void assign(std::initializer_list<elementType> p_list);

		reference at(size_type p_index);
		const_reference at(size_type p_index) const;

		reference operator[](size_type p_index);
		const_reference operator[](size_type p_index) const;

		reference front();
		const_reference front() const;

		reference back();
		const_reference back() const;

		elementType* data() noexcept;
		const elementType* data() const noexcept;

		iterator begin() noexcept;
		const_iterator begin() const noexcept;
		const_iterator cbegin() const noexcept;

		iterator end() noexcept;
		const_iterator end() const noexcept;
		const_iterator cend() const noexcept;

		reverse_iterator rbegin() noexcept;
		const_reverse_iterator rbegin() const noexcept;
		const_reverse_iterator crbegin() const noexcept;

		reverse_iterator rend() noexcept;
		const_reverse_iterator rend() const noexcept;
		const_reverse_iterator crend() const noexcept;

		bool empty() const noexcept;
		size_type size() const noexcept;
		size_type max_size() const noexcept;
		void reserve(size_type p_capacity);
		size_type capacity() const noexcept;
		void shrink_to_fit();

		void clear() noexcept;

		iterator insert(const_iterator p_position, const elementType& p_element);
		iterator insert(const_iterator p_position, elementType&& p_element);
		iterator insert(const_iterator p_position, size_type p_count, const elementType& p_element);
		template<class inputIteratorType>
		iterator insert(const_iterator p_position, inputIteratorType p_first, inputIteratorType p_last);
		iterator insert(const_iterator p_position, std::initializer_list<elementType> p_list);

		template <class... argumentTypes>
		iterator emplace(const_iterator p_position, argumentTypes&&... p_arguments);

		iterator erase(iterator p_position);
		iterator erase(const_iterator p_position);
		iterator erase(iterator p_first, iterator p_second);
		iterator erase(const_iterator p_first, const_iterator p_second);

		void push_back(const elementType& p_element);
		void push_back(elementType&& p_element);

		template <class... argumentTypes>
		reference emplace_back(argumentTypes&&... p_arguments);

		void pop_back();

		void resize(const size_type p_size);
		void resize(const size_type p_size, const elementType& p_value);

		const storableAllocatorType& get_allocator() const;

		array_proxy<const elementType> to_array_proxy() const;
		array_proxy<elementType> to_array_proxy();

	private:

		template <class otherStorableAllocatorType>
		void assign_value_operation(vector<elementType, otherStorableAllocatorType>&& p_instance);

		template<class otherStorableAllocatorType>
		bool is_same_instance(const vector<elementType, otherStorableAllocatorType>& p_instance) const;

		internal_vector_type m_vector;

	};

	template <class elementType, class storableAllocatorType>
	inline vector<elementType, storableAllocatorType>::vector(const storableAllocatorType& p_allocator) noexcept
		: m_vector(jkutil::allocator_stl_adapter<elementType, storableAllocatorType>(p_allocator))
	{}

	template <class elementType, class storableAllocatorType>
	vector<elementType, storableAllocatorType>::vector(const size_type p_count, const elementType& p_value, const storableAllocatorType& p_allocator)
		: m_vector(p_count, p_value, jkutil::allocator_stl_adapter<elementType, storableAllocatorType>(p_allocator))
	{}

	template <class elementType, class storableAllocatorType>
	vector<elementType, storableAllocatorType>::vector(const size_type p_count, const storableAllocatorType& p_allocator)
		: m_vector(p_count, jkutil::allocator_stl_adapter<elementType, storableAllocatorType>(p_allocator))
	{}

	template <class elementType, class storableAllocatorType>
	template<class inputIteratorType>
	vector<elementType, storableAllocatorType>::vector(inputIteratorType p_first, inputIteratorType p_last, const storableAllocatorType& p_allocator)
		: m_vector(p_first, p_last, jkutil::allocator_stl_adapter<elementType, storableAllocatorType>(p_allocator))
	{}

	template <class elementType, class storableAllocatorType>
	template <class otherStorableAllocatorType>
	vector<elementType, storableAllocatorType>::vector(const vector<elementType, otherStorableAllocatorType>& p_instance, const storableAllocatorType& p_allocator)
		: m_vector(jkutil::allocator_stl_adapter<elementType, storableAllocatorType>(p_allocator))
	{
		assign(p_instance.cbegin(), p_instance.cend());
	}

	template <class elementType, class storableAllocatorType>
	template <class otherStorableAllocatorType>
	vector<elementType, storableAllocatorType>::vector(vector<elementType, otherStorableAllocatorType>&& p_instance, const storableAllocatorType& p_allocator)
		: m_vector(jkutil::allocator_stl_adapter<elementType, storableAllocatorType>(p_allocator))
	{
		assign_value(std::move(p_instance));
	}

	template <class elementType, class storableAllocatorType>
	vector<elementType, storableAllocatorType>::vector(std::initializer_list<elementType> p_list, const storableAllocatorType& p_allocator)
		: m_vector(p_list, jkutil::allocator_stl_adapter<elementType, storableAllocatorType>(p_allocator))
	{}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::operator=(const vector& p_rhs) -> vector&
	{
		m_vector = p_rhs.m_vector;
		return *this;
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::operator=(vector&& p_rhs) -> vector&
	{
		m_vector = std::move(p_rhs.m_vector);
		return *this;
	}

	template <class elementType, class storableAllocatorType>
	template <class otherStorableAllocatorType>
	auto vector<elementType, storableAllocatorType>::assign_value(const vector<elementType, otherStorableAllocatorType>& p_instance) -> vector&
	{
		if (!is_same_instance(p_instance))
		{
			clear();
			reserve(p_instance.size());
			assign(p_instance.cbegin(), p_instance.cend());
		}
		return *this;
	}

	template <class elementType, class storableAllocatorType>
	template <class otherStorableAllocatorType>
	auto vector<elementType, storableAllocatorType>::assign_value(vector<elementType, otherStorableAllocatorType>&& p_instance) -> vector&
	{
		if (!is_same_instance(p_instance))
		{
			if constexpr (std::is_same<storableAllocatorType, otherStorableAllocatorType>::value)
			{
				if ((!storableAllocatorType::propagate_on_container_move_assignment::value) || (get_allocator() == p_instance.get_allocator()))
				{
					*this = std::move(p_instance);
				}
				else
				{
					assign_value_operation(std::move(p_instance));
				}
			}
			else
			{
				assign_value_operation(std::move(p_instance));
			}
			p_instance.clear();
		}
		return *this;
	}

	template <class elementType, class storableAllocatorType>
	void vector<elementType, storableAllocatorType>::swap(vector& p_instance)
	{
		if (storableAllocatorType::propagate_on_container_swap_assignment::value || jkutil::_jkinternal::are_equal(get_allocator(), p_instance.get_allocator()))
		{
			m_vector.swap(p_instance.m_vector);
		}
		else
		{
			swap_value(p_instance);
		}
	}

	template <class elementType, class storableAllocatorType>
	template <class otherStorableAllocatorType>
	void vector<elementType, storableAllocatorType>::swap_value(vector<elementType, otherStorableAllocatorType>& p_instance)
	{
		if (!is_same_instance(p_instance))
		{
			vector<elementType, storableAllocatorType> other(get_allocator());

			other.assign_value(std::move(p_instance));
			p_instance.assign_value(std::move(*this));
			assign_value(std::move(other));
		}
	}

	template <class elementType, class storableAllocatorType>
	bool vector<elementType, storableAllocatorType>::operator==(const vector& p_rhs) const
	{
		return m_vector == p_rhs.m_vector;
	}

	template <class elementType, class storableAllocatorType>
	bool vector<elementType, storableAllocatorType>::operator!=(const vector& p_rhs) const
	{
		return m_vector != p_rhs.m_vector;
	}

	template <class elementType, class storableAllocatorType>
	template <class otherStorableAllocator>
	bool vector<elementType, storableAllocatorType>::operator==(const vector<elementType, otherStorableAllocator>& p_instance) const
	{
		auto this_iterator = cbegin(), this_end_iterator = cend();
		auto other_iterator = p_instance.cbegin(), other_end_iterator = p_instance.cend();

		for (; (this_iterator != this_end_iterator) && (other_iterator != other_end_iterator); ++this_iterator, ++other_iterator)
		{
			if (*this_iterator != *other_iterator)
			{
				break;
			}
		}

		return (this_iterator == this_end_iterator) && (other_iterator == other_end_iterator);
	}

	template <class elementType, class storableAllocatorType>
	template <class otherStorableAllocator>
	bool vector<elementType, storableAllocatorType>::operator!=(const vector<elementType, otherStorableAllocator>& p_instance) const
	{
		return !(this->operator==(p_instance));
	}

	template <class elementType, class storableAllocatorType>
	void vector<elementType, storableAllocatorType>::assign(size_type p_count, const elementType& p_value)
	{
		m_vector.assign(p_count, p_value);
	}

	template <class elementType, class storableAllocatorType>
	template <class inputIteratorType>
	void vector<elementType, storableAllocatorType>::assign(inputIteratorType p_first, inputIteratorType p_last)
	{
		m_vector.assign(p_first, p_last);
	}

	template <class elementType, class storableAllocatorType>
	void vector<elementType, storableAllocatorType>::assign(std::initializer_list<elementType> p_list)
	{
		m_vector.assign(p_list);
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::at(size_type p_index) -> reference
	{
		return m_vector.at(p_index);
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::at(size_type p_index) const -> const_reference
	{
		return m_vector.at(p_index);
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::operator[](size_type p_index) -> reference
	{
		return m_vector[p_index];
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::operator[](size_type p_index) const -> const_reference
	{
		return m_vector[p_index];
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::front() -> reference
	{
		return m_vector.front();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::front() const -> const_reference
	{
		return m_vector.front();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::back() -> reference
	{
		return m_vector.back();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::back() const -> const_reference
	{
		return m_vector.back();
	}

	template <class elementType, class storableAllocatorType>
	elementType* vector<elementType, storableAllocatorType>::data() noexcept
	{
		return m_vector.data();
	}

	template <class elementType, class storableAllocatorType>
	const elementType* vector<elementType, storableAllocatorType>::data() const noexcept
	{
		return m_vector.data();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::begin() noexcept -> iterator
	{
		return m_vector.begin();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::begin() const noexcept -> const_iterator
	{
		return m_vector.begin();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::cbegin() const noexcept -> const_iterator
	{
		return m_vector.cbegin();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::end() noexcept -> iterator
	{
		return m_vector.end();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::end() const noexcept -> const_iterator
	{
		return m_vector.end();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::cend() const noexcept -> const_iterator
	{
		return m_vector.cend();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::rbegin() noexcept -> reverse_iterator
	{
		return m_vector.rbegin();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::rbegin() const noexcept -> const_reverse_iterator
	{
		return m_vector.rbegin();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::crbegin() const noexcept -> const_reverse_iterator
	{
		return m_vector.crbegin();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::rend() noexcept -> reverse_iterator
	{
		return m_vector.rend();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::rend() const noexcept -> const_reverse_iterator
	{
		return m_vector.rend();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::crend() const noexcept -> const_reverse_iterator
	{
		return m_vector.crend();
	}

	template <class elementType, class storableAllocatorType>
	bool vector<elementType, storableAllocatorType>::empty() const noexcept
	{
		return m_vector.empty();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::size() const noexcept -> size_type
	{
		return m_vector.size();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::max_size() const noexcept -> size_type
	{
		return m_vector.max_size();
	}

	template <class elementType, class storableAllocatorType>
	void vector<elementType, storableAllocatorType>::reserve(size_type p_capacity)
	{
		m_vector.reserve(p_capacity);
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::capacity() const noexcept -> size_type
	{
		return m_vector.capacity();
	}

	template <class elementType, class storableAllocatorType>
	void vector<elementType, storableAllocatorType>::shrink_to_fit()
	{
		return m_vector.shrink_to_fit();
	}

	template <class elementType, class storableAllocatorType>
	void vector<elementType, storableAllocatorType>::clear() noexcept
	{
		return m_vector.clear();
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::insert(const_iterator p_position, const elementType& p_element) -> iterator
	{
		return m_vector.insert(p_position, p_element);
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::insert(const_iterator p_position, elementType&& p_element) -> iterator
	{
		return m_vector.insert(p_position, std::move(p_element));
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::insert(const_iterator p_position, size_type p_count, const elementType& p_element) -> iterator
	{
		return m_vector.insert(p_position, p_count, p_element);
	}

	template <class elementType, class storableAllocatorType>
	template<class inputIteratorType>
	auto vector<elementType, storableAllocatorType>::insert(const_iterator p_position, inputIteratorType p_first, inputIteratorType p_last) -> iterator
	{
		return m_vector.insert(p_position, p_first, p_last);
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::insert(const_iterator p_position, std::initializer_list<elementType> p_list) -> iterator
	{
		return m_vector.insert(p_position, p_list);
	}

	template <class elementType, class storableAllocatorType>
	template <class... argumentTypes>
	auto vector<elementType, storableAllocatorType>::emplace(const_iterator p_position, argumentTypes&&... p_arguments) -> iterator
	{
		return m_vector.emplace(p_position, std::forward<argumentTypes>(p_arguments)...);
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::erase(iterator p_position) -> iterator
	{
		return m_vector.erase(p_position);
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::erase(const_iterator p_position) -> iterator
	{
		return m_vector.erase(p_position);
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::erase(iterator p_first, iterator p_second) -> iterator
	{
		return m_vector.erase(p_first, p_second);
	}

	template <class elementType, class storableAllocatorType>
	auto vector<elementType, storableAllocatorType>::erase(const_iterator p_first, const_iterator p_second) -> iterator
	{
		return m_vector.erase(p_first, p_second);
	}

	template <class elementType, class storableAllocatorType>
	void vector<elementType, storableAllocatorType>::push_back(const elementType& p_element)
	{
		m_vector.push_back(p_element);
	}

	template <class elementType, class storableAllocatorType>
	void vector<elementType, storableAllocatorType>::push_back(elementType&& p_element)
	{
		m_vector.push_back(std::move(p_element));
	}

	template <class elementType, class storableAllocatorType>
	template <class... argumentTypes>
	auto vector<elementType, storableAllocatorType>::emplace_back(argumentTypes&&... p_arguments) -> reference
	{
		return m_vector.emplace_back(std::forward<argumentTypes>(p_arguments)...);
	}

	template <class elementType, class storableAllocatorType>
	void vector<elementType, storableAllocatorType>::pop_back()
	{
		m_vector.pop_back();
	}

	template <class elementType, class storableAllocatorType>
	void vector<elementType, storableAllocatorType>::resize(const size_type p_size)
	{
		m_vector.resize(p_size);
	}

	template <class elementType, class storableAllocatorType>
	void vector<elementType, storableAllocatorType>::resize(const size_type p_size, const elementType& p_value)
	{
		m_vector.resize(p_size, p_value);
	}

	template <class elementType, class storableAllocatorType>
	const storableAllocatorType& vector<elementType, storableAllocatorType>::get_allocator() const
	{
		return m_vector.get_allocator().get_allocator();
	}

	template <class elementType, class storableAllocatorType>
	array_proxy<const elementType> vector<elementType, storableAllocatorType>::to_array_proxy() const
	{
		return array_proxy<const elementType>(m_vector.size(), m_vector.data());
	}

	template <class elementType, class storableAllocatorType>
	array_proxy<elementType> vector<elementType, storableAllocatorType>::to_array_proxy()
	{
		return array_proxy<elementType>(m_vector.size(), m_vector.data());
	}

	template <class elementType, class storableAllocatorType>
	template <class otherStorableAllocatorType>
	void vector<elementType, storableAllocatorType>::assign_value_operation(vector<elementType, otherStorableAllocatorType>&& p_instance)
	{
		clear();
		reserve(p_instance.size());
		for (auto& element : p_instance)
		{
			emplace_back(std::move(element));
		}
	}

	template <class elementType, class storableAllocatorType>
	template<class otherStorableAllocatorType>
	bool vector<elementType, storableAllocatorType>::is_same_instance(const vector<elementType, otherStorableAllocatorType>& p_instance) const
	{
		if constexpr (std::is_same<storableAllocatorType, otherStorableAllocatorType>::value)
		{
			return this == &p_instance;
		}
		else
		{
			return false;
		}
	}

}

#endif