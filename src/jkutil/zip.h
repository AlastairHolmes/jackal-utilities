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

	public:

		bool operator==(const zip_iterator& p_rhs) const;
		bool operator!=(const zip_iterator& p_rhs) const;

		template <class... otherIteratorTypes>
		bool operator==(const zip_iterator<otherIteratorTypes...>& p_rhs) const;
		template <class... otherIteratorTypes>
		bool operator!=(const zip_iterator<otherIteratorTypes...>& p_rhs) const;

		zip_iterator& operator++();
		zip_iterator operator++(int);

		reference operator*() const;
		pointer operator->() const;

	private:

		template <class... otherIteratorTypes, std::size_t... indexValues>
		bool equality_impl(const std::tuple<otherIteratorTypes...>& p_rhs, std::index_sequence<indexValues...>) const;

		std::tuple<iteratorTypes...> m_iterators;

	};

	template <class... iteratorTypes>
	zip_iterator<iteratorTypes...>::zip_iterator(iteratorTypes&&... p_iterators)
		: m_iterators(std::forward<iteratorTypes>(p_iterators)...)
	{
	}

	template <class... iteratorTypes>
	bool zip_iterator<iteratorTypes...>::operator==(const zip_iterator& p_rhs) const
	{
		return equality_impl(p_rhs.m_iterators, std::make_index_sequence<sizeof...(iteratorTypes)>{});
	}

	template <class... iteratorTypes>
	bool zip_iterator<iteratorTypes...>::operator!=(const zip_iterator& p_rhs) const
	{
		return !(*this == p_rhs);
	}

	template <class... iteratorTypes>
	template <class... otherIteratorTypes>
	bool zip_iterator<iteratorTypes...>::operator==(const zip_iterator<otherIteratorTypes...>& p_rhs) const
	{
		static_assert(sizeof...(iteratorTypes) == sizeof...(otherIteratorTypes));
		return equality_impl(p_rhs.m_iterators, std::make_index_sequence<sizeof...(iteratorTypes)>{});
	}

	template <class... iteratorTypes>
	template <class... otherIteratorTypes>
	bool zip_iterator<iteratorTypes...>::operator!=(const zip_iterator<otherIteratorTypes...>& p_rhs) const
	{
		static_assert(sizeof...(iteratorTypes) == sizeof...(otherIteratorTypes));
		return !(*this == p_rhs);
	}

	template <class... iteratorTypes>
	auto zip_iterator<iteratorTypes...>::operator++() -> zip_iterator&
	{
		jkutil::tuple_map(m_iterators, [](auto& p_iterator)
		{
			++p_iterator;
		});
		return *this;
	}

	template <class... iteratorTypes>
	auto zip_iterator<iteratorTypes...>::operator++(int) -> zip_iterator
	{
		zip_iterator copy(*this);
		jkutil::tuple_map(m_iterators, [](auto& p_iterator)
		{
			p_iterator++;
		});
		return copy;
	}

	template <class... iteratorTypes>
	auto zip_iterator<iteratorTypes...>::operator*() const -> reference 
	{
		return jkutil::tuple_map_return(m_iterators, [](auto& p_iterator) -> decltype(*p_iterator)
		{
			return *p_iterator;
		});
	}

	template<class... iteratorTypes>
	inline auto zip_iterator<iteratorTypes...>::operator->() const -> pointer
	{
		return jkutil::tuple_map_return(m_iterators, [](auto& p_iterator) -> decltype(jkutil::arrow_operator(p_iterator))
		{
			return jkutil::arrow_operator(p_iterator);
		});
	}

	template <class... iteratorTypes>
	template <class... otherIteratorTypes, std::size_t... indexValues>
	bool zip_iterator<iteratorTypes...>::equality_impl(const std::tuple<otherIteratorTypes...>& p_rhs, std::index_sequence<indexValues...>) const
	{
		static_assert(sizeof...(iteratorTypes) == sizeof...(otherIteratorTypes));
		return (sizeof...(iteratorTypes) == 0) || ((std::get<indexValues>(m_iterators) == std::get<indexValues>(p_rhs)) || ...);
	}

	template <class... iteratorTypes>
	zip_iterator<std::decay_t<iteratorTypes>...> make_zip_iterator(iteratorTypes&&... p_iterator)
	{
		return zip_iterator<std::decay_t<iteratorTypes>...>(std::forward<iteratorTypes>(p_iterator)...);
	}

	template <class... rangeTypes>
	class zip_range : public iterator_range<zip_iterator<decltype(std::declval<rangeTypes>().begin())...>, zip_iterator<decltype(std::declval<rangeTypes>().end())...>>
	{
	public:

		explicit zip_range(const rangeTypes&... p_ranges);
		explicit zip_range(rangeTypes&... p_ranges);

	};

	template <class... rangeTypes>
	inline zip_range<rangeTypes...>::zip_range(const rangeTypes&... p_ranges)
		: iterator_range<zip_iterator<decltype(std::declval<rangeTypes>().begin())...>, zip_iterator<decltype(std::declval<rangeTypes>().end())...>>(
			zip_iterator<decltype(std::declval<rangeTypes>().begin())...>(p_ranges.begin()...),
			zip_iterator<decltype(std::declval<rangeTypes>().end())...>(p_ranges.end()...))
	{

	}

	template <class... rangeTypes>
	inline zip_range<rangeTypes...>::zip_range(rangeTypes&... p_ranges)
		: iterator_range<zip_iterator<decltype(std::declval(rangeTypes).begin())...>, zip_iterator<decltype(std::declval(rangeTypes).end())...>>(
			zip_iterator<decltype(std::declval<rangeTypes>().begin())...>(p_ranges.begin()...),
			zip_iterator<decltype(std::declval<rangeTypes>().end())...>(p_ranges.end()...))
	{

	}

	template <class... rangeTypes>
	decltype(auto) make_zip_range(rangeTypes&... p_ranges)
	{
		return zip_range<jkutil::iterator_range<
			decltype(std::declval<rangeTypes>().begin()),
			decltype(std::declval<rangeTypes>().end())>...>(
				jkutil::iterator_range<
					decltype(std::declval<rangeTypes>().begin()),
					decltype(std::declval<rangeTypes>().end())>(p_ranges.begin(), p_ranges.end())...);
	}

}

#endif