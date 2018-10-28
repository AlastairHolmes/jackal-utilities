/*!
	@file		zip_iterator.h
	@author		Alastair Holmes
	@date		28/10/2018
 */

#ifndef JKUTIL_ZIPPER_H
#define JKUTIL_ZIPPER_H

#include <tuple>
#include <jkutil\utility.h>

namespace jkutil
{

	template <class... iteratorTypes>
	class zip_iterator
	{
	public:

		using value_type = std::tuple<typename iteratorTypes::value_type...>;
		using reference = std::tuple<typename iteratorTypes::reference...>;

	public:

		zip_iterator(const iteratorTypes&... p_iterators)
			: m_iterators(p_iterators...)
		{
		}

		zip_iterator(const zip_iterator&) = default;
		zip_iterator(zip_iterator&&) = default;

		zip_iterator& operator=(const zip_iterator&) = default;
		zip_iterator& operator=(zip_iterator&&) = default;

		zip_iterator& operator++()
		{
			tuple_map(m_iterators, [](auto& x)
			{
				++x;
			});
			return *this;
		}

		zip_iterator operator++(int)
		{
			zip_iterator copy(*this);
			++(*this);
			return copy;
		}

		zip_iterator& operator--()
		{
			tuple_map(m_iterators, [](auto& x)
			{
				--x;
			});
			return *this;
		}

		zip_iterator operator--(int)
		{
			zip_iterator copy(*this);
			--(*this);
			return copy;
		}

		bool operator!=(const zip_iterator& p_rhs)
		{
			return !(*this == p_rhs);
		}

		bool operator==(const zip_iterator& p_rhs)
		{
			return equality_impl(p_rhs.m_iterators, std::make_index_sequence<sizeof...(iteratorTypes)>{});
		}

		std::tuple<typename iteratorTypes::reference...> operator*() const
		{
			return tuple_map_return(m_iterators, [](auto p_iterator) -> decltype(*p_iterator)
			{
				return *p_iterator;
			});
		}

	private:

		template <size_t... indexValues>
		bool equality_impl(const std::tuple<iteratorTypes...>& p_rhs, std::index_sequence<indexValues...>) const
		{
			return (sizeof...(iteratorTypes) == 0) || ((std::get<indexValues>(m_iterators) == std::get<indexValues>(p_rhs)) || ...);
		}

		std::tuple<iteratorTypes...> m_iterators;

	};

	template <class... iteratorTypes>
	zip_iterator<iteratorTypes...> make_zip_iterator(const iteratorTypes&... p_iterator)
	{
		return zip_iterator<iteratorTypes...>(p_iterator...);
	}

	template <class... zippedContainerTypes>
	class zipper
	{
	public:

		using iterator = zip_iterator<std::conditional_t<std::is_const<zippedContainerTypes>::value, typename zippedContainerTypes::const_iterator, typename zippedContainerTypes::iterator>...>;

		explicit zipper(zippedContainerTypes&... p_containers)
			: m_begin(p_containers.begin()...),
			m_end(p_containers.end()...)
		{

		}

		iterator begin() const
		{
			return m_begin;
		}

		iterator end() const
		{
			return m_end;
		}

	private:

		iterator m_begin;
		iterator m_end;

	};

	template <class... containerTypes>
	zipper<containerTypes...> make_zipper(containerTypes&... p_containers)
	{
		return zipper<containerTypes...>(p_containers...);
	}

}

#endif