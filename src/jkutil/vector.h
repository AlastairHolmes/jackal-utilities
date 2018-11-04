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

		explicit vector(const storableAllocatorType& p_allocator) noexcept
			: m_vector(jkutil::allocator_stl_adapter<elementType, storableAllocatorType>(p_allocator))
		{}

		vector(const size_type p_count, const elementType& p_value = elementType(), const storableAllocatorType& p_allocator = storableAllocatorType())
			: m_vector(p_count, p_value, jkutil::allocator_stl_adapter<elementType, storableAllocatorType>(p_allocator))
		{}

		explicit vector(const size_type p_count, const storableAllocatorType& p_allocator = storableAllocatorType())
			: m_vector(p_count, jkutil::allocator_stl_adapter<elementType, storableAllocatorType>(p_allocator))
		{}

		template<class inputIteratorType>
		vector(inputIteratorType p_first, inputIteratorType p_last, const storableAllocatorType& p_allocator = storableAllocatorType())
			: m_vector(p_first, p_last, jkutil::allocator_stl_adapter<elementType, storableAllocatorType>(p_allocator))
		{}

		vector(const vector&) = default;

		vector(vector&&) noexcept = default;

		template <class otherStorableAllocatorType>
		vector(const vector<elementType, otherStorableAllocatorType>& p_instance, const storableAllocatorType& p_allocator)
			: m_vector(jkutil::allocator_stl_adapter<elementType, storableAllocatorType>(p_allocator))
		{
			assign(p_instance.cbegin(), p_instance.cend());
		}

		template <class otherStorableAllocatorType>
		vector(vector<elementType, otherStorableAllocatorType>&& p_instance, const storableAllocatorType& p_allocator)
			: m_vector(jkutil::allocator_stl_adapter<elementType, storableAllocatorType>(p_allocator))
		{
			assign_value(std::move(p_instance));
		}

		vector(std::initializer_list<elementType> p_list, const storableAllocatorType& p_allocator = storableAllocatorType())
			: m_vector(p_list, jkutil::allocator_stl_adapter<elementType, storableAllocatorType>(p_allocator))
		{}

	public:

		vector& operator=(const vector& p_rhs)
		{
			m_vector = p_rhs.m_vector;
			return *this;
		}

		vector& operator=(vector&& p_rhs)
		{
			m_vector = std::move(p_rhs.m_vector);
			return *this;
		}

		template <class otherStorableAllocatorType>
		vector& assign_value(const vector<elementType, otherStorableAllocatorType>& p_instance)
		{
			if (!is_same_instance(p_instance))
			{
				clear();
				reserve(p_instance.size());
				assign(p_instance.cbegin(), p_instance.cend());
			}
			return *this;
		}

		template <class otherStorableAllocatorType>
		vector& assign_value(vector<elementType, otherStorableAllocatorType>&& p_instance)
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

		void swap(vector& p_instance)
		{
			if(storableAllocatorType::propagate_on_container_swap_assignment::value || jkutil::_jkinternal::are_equal(get_allocator(), p_instance.get_allocator()))
			{
				m_vector.swap(p_instance.m_vector);
			}
			else
			{
				swap_value(p_instance);
			}
		}

		template <class otherStorableAllocatorType>
		void swap_value(vector<elementType, otherStorableAllocatorType>& p_instance)
		{
			if (!is_same_instance(p_instance))
			{
				vector<elementType, storableAllocatorType> other(get_allocator());

				other.assign_value(std::move(p_instance));
				p_instance.assign_value(std::move(*this));
				assign_value(std::move(other));
			}
		}

		bool operator==(const vector& p_rhs) const
		{
			return m_vector == p_rhs.m_vector;
		}

		bool operator!=(const vector& p_rhs) const
		{
			return m_vector != p_rhs.m_vector;
		}

		template <class otherStorableAllocator>
		bool operator==(const vector<elementType, otherStorableAllocator>& p_instance) const
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

		template <class otherStorableAllocator>
		bool operator!=(const vector<elementType, otherStorableAllocator>& p_instance) const
		{
			return !(this->operator==(p_instance));
		}

	public:
		
		void assign(size_type p_count, const elementType& p_value)
		{
			m_vector.assign(p_count, p_value);
		}

		template <class inputIteratorType>
		void assign(inputIteratorType p_first, inputIteratorType p_last)
		{
			m_vector.assign(p_first, p_last);
		}

		void assign(std::initializer_list<elementType> p_list)
		{
			m_vector.assign(p_list);
		}

		reference at(size_type p_index)
		{
			return m_vector.at(p_index);
		}

		const_reference at(size_type p_index) const
		{
			return m_vector.at(p_index);
		}

		reference operator[](size_type p_index)
		{
			return m_vector[p_index];
		}

		const_reference operator[](size_type p_index) const
		{
			return m_vector[p_index];
		}

		reference front()
		{
			return m_vector.front();
		}

		const_reference front() const
		{
			return m_vector.front();
		}

		reference back()
		{
			return m_vector.back();
		}

		const_reference back() const
		{
			return m_vector.back();
		}

		elementType* data() noexcept
		{
			return m_vector.data();
		}

		const elementType* data() const noexcept
		{
			return m_vector.data();
		}

		iterator begin() noexcept
		{
			return m_vector.begin();
		}

		const_iterator begin() const noexcept
		{
			return m_vector.begin();
		}

		const_iterator cbegin() const noexcept
		{
			return m_vector.cbegin();
		}

		iterator end() noexcept
		{
			return m_vector.end();
		}

		const_iterator end() const noexcept
		{
			return m_vector.end();
		}

		const_iterator cend() const noexcept
		{
			return m_vector.cend();
		}

		reverse_iterator rbegin() noexcept
		{
			return m_vector.rbegin();
		}

		const_reverse_iterator rbegin() const noexcept
		{
			return m_vector.rbegin();
		}

		const_reverse_iterator crbegin() const noexcept
		{
			return m_vector.crbegin();
		}

		reverse_iterator rend() noexcept
		{
			return m_vector.rend();
		}

		const_reverse_iterator rend() const noexcept
		{
			return m_vector.rend();
		}

		const_reverse_iterator crend() const noexcept
		{
			return m_vector.crend();
		}

		bool empty() const noexcept
		{
			return m_vector.empty();
		}

		size_type size() const noexcept
		{
			return m_vector.size();
		}

		size_type max_size() const noexcept
		{
			return m_vector.max_size();
		}

		void reserve(size_type p_capacity)
		{
			m_vector.reserve(p_capacity);
		}

		size_type capacity() const noexcept
		{
			return m_vector.capacity();
		}

		void shrink_to_fit()
		{
			return m_vector.shrink_to_fit();
		}

		void clear() noexcept
		{
			return m_vector.clear();
		}

		iterator insert(const_iterator p_position, const elementType& p_element)
		{
			return m_vector.insert(p_position, p_element);
		}

		iterator insert(const_iterator p_position, elementType&& p_element)
		{
			return m_vector.insert(p_position, std::move(p_element));
		}

		iterator insert(const_iterator p_position, size_type p_count, const elementType& p_element)
		{
			return m_vector.insert(p_position, p_count, p_element);
		}

		template<class inputIteratorType>
		iterator insert(const_iterator p_position, inputIteratorType p_first, inputIteratorType p_last)
		{
			return m_vector.insert(p_position, p_first, p_last);
		}

		iterator insert(const_iterator p_position, std::initializer_list<elementType> p_list)
		{
			return m_vector.insert(p_position, p_list);
		}

		template <class... argumentTypes>
		iterator emplace(const_iterator p_position, argumentTypes&&... p_arguments)
		{
			return m_vector.emplace(p_position, std::forward<argumentTypes>(p_arguments)...);
		}

		iterator erase(iterator p_position)
		{
			return m_vector.erase(p_position);
		}

		iterator erase(const_iterator p_position)
		{
			return m_vector.erase(p_position);
		}

		iterator erase(iterator p_first, iterator p_second)
		{
			return m_vector.erase(p_first, p_second);
		}

		iterator erase(const_iterator p_first, const_iterator p_second)
		{
			return m_vector.erase(p_first, p_second);
		}

		void push_back(const elementType& p_element)
		{
			m_vector.push_back(p_element);
		}

		void push_back(elementType&& p_element)
		{
			m_vector.push_back(std::move(p_element));
		}

		template <class... argumentTypes>
		reference emplace_back(argumentTypes&&... p_arguments)
		{
			return m_vector.emplace_back(std::forward<argumentTypes>(p_arguments)...);
		}

		void pop_back()
		{
			m_vector.pop_back();
		}

		void resize(const size_type p_size)
		{
			m_vector.resize(p_size);
		}

		void resize(const size_type p_size, const elementType& p_value)
		{
			m_vector.resize(p_size, p_value);
		}

		const storableAllocatorType& get_allocator() const
		{
			return m_vector.get_allocator().get_allocator();
		}

		array_proxy<const elementType> to_array_proxy() const
		{
			return array_proxy<const elementType>(m_vector.size(), m_vector.data());
		}

		array_proxy<elementType> to_array_proxy()
		{
			return array_proxy<elementType>(m_vector.size(), m_vector.data());
		}

	private:

		template <class otherStorableAllocatorType>
		void assign_value_operation(vector<elementType, otherStorableAllocatorType>&& p_instance)
		{
			clear();
			reserve(p_instance.size());
			for (auto& element : p_instance)
			{
				emplace_back(std::move(element));
			}
		}

		template<class otherStorableAllocatorType>
		bool is_same_instance(const vector<elementType, otherStorableAllocatorType>& p_instance) const
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

		internal_vector_type m_vector;

	};

}

#endif