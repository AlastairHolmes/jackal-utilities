/*!
	@file		zip_iterator.h
	@author		Alastair Holmes
	@date		28/10/2018
 */

#ifndef JKUTIL_ZIPPER_H
#define JKUTIL_ZIPPER_H

#include <jkutil\utility.h>
#include <cstddef>
#include <tuple>

namespace jkutil
{

	template <class... iteratorTypes>
	class zip_iterator
	{
	public:

		using value_type = std::tuple<typename std::iterator_traits<iteratorTypes>::value_type...>;
		using difference_type = std::common_type_t<typename std::iterator_traits<iteratorTypes>::difference_type...>;
		using reference = std::tuple<typename std::iterator_traits<iteratorTypes>::reference...>;
		using pointer = std::tuple<typename std::iterator_traits<iteratorTypes>::pointer...>;
		using iterator_category = std::input_iterator_tag;

	public:

		zip_iterator(iteratorTypes&&... p_iterators);

		zip_iterator(const zip_iterator&) = default;
		zip_iterator(zip_iterator&&) = default;

		zip_iterator& operator=(const zip_iterator&) = default;
		zip_iterator& operator=(zip_iterator&&) = default;

		bool operator==(const zip_iterator& p_rhs) const;
		bool operator!=(const zip_iterator& p_rhs) const;

		zip_iterator& operator++();
		zip_iterator operator++(int);

		reference operator*() const;
		pointer operator->() const;

	private:

		template <std::size_t... indexValues>
		bool equality_impl(const std::tuple<iteratorTypes...>& p_rhs, std::index_sequence<indexValues...>) const;

		std::tuple<iteratorTypes...> m_iterators;

	};

	template <class... iteratorTypes>
	zip_iterator<iteratorTypes...>::zip_iterator(iteratorTypes&&... p_iterators)
		: m_iterators(std::forward<iteratorTypes>(p_iterators)...)
	{
	}

	template <class... iteratorTypes>
	auto zip_iterator<iteratorTypes...>::operator++() -> zip_iterator&
	{
		tuple_map(m_iterators, [](auto& x)
		{
			++x;
		});
		return *this;
	}

	template <class... iteratorTypes>
	auto zip_iterator<iteratorTypes...>::operator++(int) -> zip_iterator
	{
		zip_iterator copy(*this);
		++(*this);
		return copy;
	}

	template <class... iteratorTypes>
	bool zip_iterator<iteratorTypes...>::operator!=(const zip_iterator& p_rhs) const
	{
		return !(*this == p_rhs);
	}

	template <class... iteratorTypes>
	bool zip_iterator<iteratorTypes...>::operator==(const zip_iterator& p_rhs) const
	{
		return equality_impl(p_rhs.m_iterators, std::make_index_sequence<sizeof...(iteratorTypes)>{});
	}

	template <class... iteratorTypes>
	auto zip_iterator<iteratorTypes...>::operator*() const -> reference 
	{
		return tuple_map_return(m_iterators, [](auto p_iterator) -> decltype(*p_iterator)
		{
			return *p_iterator;
		});
	}

	template<class... iteratorTypes>
	inline auto zip_iterator<iteratorTypes...>::operator->() const -> pointer
	{
		return tuple_map_return(m_iterators, [](auto p_iterator) -> decltype(*p_iterator)
		{
			return jkutil::arrow_operator(p_iterator);
		});
	}

	template <class... iteratorTypes>
	template <std::size_t... indexValues>
	bool zip_iterator<iteratorTypes...>::equality_impl(const std::tuple<iteratorTypes...>& p_rhs, std::index_sequence<indexValues...>) const
	{
		return (sizeof...(iteratorTypes) == 0) || ((std::get<indexValues>(m_iterators) == std::get<indexValues>(p_rhs)) || ...);
	}

	template <class... iteratorTypes>
	zip_iterator<std::decay_t<iteratorTypes>...> make_zip_iterator(iteratorTypes&&... p_iterator)
	{
		return zip_iterator<std::decay_t<iteratorTypes>...>(std::forward<iteratorTypes>(p_iterator)...);
	}

	template <class... zippedContainerTypes>
	class zipper
	{
	public:

		using iterator = zip_iterator<std::conditional_t<std::is_const<zippedContainerTypes>::value, typename zippedContainerTypes::const_iterator, typename zippedContainerTypes::iterator>...>;

		explicit zipper(zippedContainerTypes&... p_containers);

		iterator begin() const;
		iterator end() const;

	private:

		iterator m_begin;
		iterator m_end;

	};

	template <class... zippedContainerTypes>
	zipper<zippedContainerTypes...>::zipper(zippedContainerTypes&... p_containers)
		: m_begin(p_containers.begin()...),
		m_end(p_containers.end()...)
	{

	}

	template <class... zippedContainerTypes>
	auto zipper<zippedContainerTypes...>::begin() const -> iterator
	{
		return m_begin;
	}

	template <class... zippedContainerTypes>
	auto zipper<zippedContainerTypes...>::end() const -> iterator
	{
		return m_end;
	}

	template <class... containerTypes>
	zipper<containerTypes...> make_zipper(containerTypes&... p_containers)
	{
		return zipper<containerTypes...>(p_containers...);
	}

}

#endif