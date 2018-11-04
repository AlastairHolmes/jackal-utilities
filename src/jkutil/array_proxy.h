/*!
	@file		array_proxy.h
	@author		Alastair Holmes
	@date		26/10/2018
 */

#ifndef JKUTIL_ARRAY_PROXY_H
#define JKUTIL_ARRAY_PROXY_H

#include <utility>
#include <limits>
#include <stdexcept>

namespace jkutil
{

	/*!
		@class array_proxy

		@brief The purpose of array_proxy is to allow easy passing and usage (throught iterators) of raw arrays.

		@note Out of range accesses will throw a std::out_of_range.
		@note The move constructor and assignment resets the moved instance, this is for safety purposes, as it makes it harder to
		accidently refer to resources you didn't intend when moving.

		@tparam elementType The type of the elements in the array.
	*/
	template <typename elementType>
	class array_proxy
	{
	public:

		typedef elementType																	value_type;
		typedef elementType*																pointer;
		typedef const elementType*															const_pointer;
		typedef elementType&																reference;
		typedef const elementType&															const_reference;
		typedef elementType*																iterator;
		typedef const elementType*															const_iterator;
		typedef size_t																		size_type;
		typedef ptrdiff_t																	difference_type;

	public:

		array_proxy() noexcept;
		array_proxy(std::nullptr_t) noexcept;
		array_proxy(size_type p_count, pointer p_ptr) noexcept;

		array_proxy(const array_proxy&) = default;
		array_proxy(array_proxy&& p_instance) noexcept;

		array_proxy& operator=(const array_proxy&) = default;
		array_proxy& operator=(array_proxy&& p_rhs) noexcept;

		void swap(array_proxy& p_rhs) noexcept;

		bool operator==(const array_proxy& p_rhs) const noexcept;
		bool operator!=(const array_proxy& p_rhs) const noexcept;

		reference operator[](size_type p_index) const;
		reference at(size_type p_index) const;

		iterator begin() const noexcept;
		iterator end() const noexcept;

		const_iterator cbegin() const noexcept;
		const_iterator cend() const noexcept;

		reference front() const;
		reference back() const;

		bool empty() const noexcept;
		size_type size() const noexcept;
		pointer data() const noexcept;

		size_type max_size() const noexcept;

	private:

		void check_out_of_range(bool p_error, const char* p_message) const;

		size_type m_size;
		pointer m_data;

	};

	template <class elementType>
	inline void swap(array_proxy<elementType>& p_lhs, array_proxy<elementType>& p_rhs) noexcept
	{
		p_lhs.swap(p_rhs);
	}

	template <class elementType>
	array_proxy<elementType> make_array_proxy(size_t p_count, elementType* p_elements)
	{
		return array_proxy<elementType>(p_count, p_elements);
	}

	template<typename elementType>
	inline array_proxy<elementType>::array_proxy() noexcept
		: m_size(static_cast<size_type>(0)), m_data(nullptr)
	{
	}

	template<typename elementType>
	inline array_proxy<elementType>::array_proxy(std::nullptr_t) noexcept
		: array_proxy()
	{
	}

	template<typename elementType>
	inline array_proxy<elementType>::array_proxy(size_type p_size, pointer p_data) noexcept
		: m_size(p_size), m_data(p_data)
	{
	}

	template<typename elementType>
	inline array_proxy<elementType>::array_proxy(array_proxy&& p_instance) noexcept
		: m_size(p_instance.m_size), m_data(p_instance.m_data)
	{
		p_instance.m_size = static_cast<size_type>(0);
		p_instance.m_data = nullptr;
	}

	template<typename elementType>
	inline array_proxy<elementType>& array_proxy<elementType>::operator=(array_proxy&& p_rhs) noexcept
	{
		if (this != &p_rhs)
		{
			m_size = p_rhs.m_size;
			m_data = p_rhs.m_data;
			p_rhs.m_size = static_cast<size_type>(0);
			p_rhs.m_data = nullptr;
		}
		return *this;
	}

	template<typename elementType>
	inline void array_proxy<elementType>::swap(array_proxy<elementType>& p_rhs) noexcept
	{
		if (this != &p_rhs)
		{
			auto temp = *this;
			*this = std::move(p_rhs);
			p_rhs = std::move(temp);
		}
	}

	template<typename elementType>
	inline bool array_proxy<elementType>::operator==(const array_proxy& p_rhs) const noexcept
	{
		return (m_data == p_rhs.m_data) && (m_size == p_rhs.m_size);
	}

	template<typename elementType>
	inline bool array_proxy<elementType>::operator!=(const array_proxy& p_rhs) const noexcept
	{
		return !(*this == p_rhs);
	}

	template<typename elementType>
	inline auto array_proxy<elementType>::operator[](size_type p_index) const -> reference
	{
		check_out_of_range(p_index >= m_size, "array_proxy::operator[]() - p_index is out of range.");
		return m_data[p_index];
	}

	template<typename elementType>
	inline auto array_proxy<elementType>::at(size_type p_index) const -> reference
	{
		check_out_of_range(p_index >= m_size, "array_proxy::at() - p_index is out of range.");
		return m_data[p_index];
	}

	template<typename elementType>
	inline auto array_proxy<elementType>::begin() const noexcept -> iterator
	{
		return m_size == 0 ? nullptr : &m_data[0];
	}

	template<typename elementType>
	inline auto array_proxy<elementType>::end() const noexcept -> iterator
	{
		return m_size == 0 ? nullptr : &m_data[m_size];
	}

	template<typename elementType>
	inline auto array_proxy<elementType>::cbegin() const noexcept -> const_iterator
	{
		return m_size == 0 ? nullptr : &m_data[0];
	}

	template<typename elementType>
	inline auto array_proxy<elementType>::cend() const noexcept -> const_iterator
	{
		return m_size == 0 ? nullptr : &m_data[m_size];
	}

	template<typename elementType>
	inline auto array_proxy<elementType>::front() const -> reference
	{
		check_out_of_range(m_size == 0, "array_proxy::front() - array_proxy is empty.");
		return m_ptr[0];
	}

	template<typename elementType>
	inline auto array_proxy<elementType>::back() const -> reference
	{
		check_out_of_range(m_size == 0, "array_proxy::back() - array_proxy is empty.");
		return m_ptr[m_size - 1];
	}

	template<typename elementType>
	inline bool array_proxy<elementType>::empty() const noexcept
	{
		return (m_size == 0);
	}

	template<typename elementType>
	inline auto array_proxy<elementType>::size() const noexcept -> size_type
	{
		return m_size;
	}

	template<typename elementType>
	inline auto array_proxy<elementType>::data() const noexcept -> pointer
	{
		return m_data;
	}

	template<typename elementType>
	inline auto array_proxy<elementType>::max_size() const noexcept -> size_type
	{
		return static_cast<size_type>(std::numeric_limits<difference_type>::max());
	}

	template<typename elementType>
	inline void array_proxy<elementType>::check_out_of_range(bool p_error, const char* p_message) const
	{
		if (p_error)
		{
			throw std::out_of_range(p_message);
		}
	}

}

#endif