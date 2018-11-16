/*!
	@file		utility.h
	@author		Alastair Holmes
	@date		28/10/2018
 */

#ifndef JKUTIL_UTILITY_H
#define JKUTIL_UTILITY_H

#include <tuple>

namespace jkutil
{

	namespace _jkinternal
	{

		template <class callableType, class tupleType, size_t... indexValues>
		void tuple_map_impl(tupleType& p_tuple, callableType&& p_callable, std::index_sequence<indexValues...>)
		{
			(std::forward<callableType>(p_callable)(std::get<indexValues>(p_tuple)), ...);
		}

		template <class callableType, class tupleType, size_t... indexValues>
		auto tuple_map_return_impl(tupleType& p_tuple, callableType&& p_callable, std::index_sequence<indexValues...>) -> std::tuple<decltype(std::forward<callableType>(p_callable)(std::get<indexValues>(p_tuple)))...>
		{
			return std::forward_as_tuple(std::forward<callableType>(p_callable)(std::get<indexValues>(p_tuple))...);
		}

	}

	template <class callableType, class... incrementableTypes>
	void tuple_map(std::tuple<incrementableTypes...>& p_tuple, callableType&& p_callable)
	{
		_jkinternal::tuple_map_impl(p_tuple, std::forward<callableType>(p_callable), std::make_index_sequence<sizeof...(incrementableTypes)>{});
	}

	template <class callableType, class... incrementableTypes>
	void tuple_map(const std::tuple<incrementableTypes...>& p_tuple, callableType&& p_callable)
	{
		_jkinternal::tuple_map_impl(p_tuple, std::forward<callableType>(p_callable), std::make_index_sequence<sizeof...(incrementableTypes)>{});
	}

	template <class callableType, class... incrementableTypes>
	auto tuple_map_return(std::tuple<incrementableTypes...>& p_tuple, callableType&& p_callable) -> decltype(_jkinternal::tuple_map_return_impl(p_tuple, std::forward<callableType>(p_callable), std::make_index_sequence<sizeof...(incrementableTypes)>{}))
	{
		return _jkinternal::tuple_map_return_impl(p_tuple, std::forward<callableType>(p_callable), std::make_index_sequence<sizeof...(incrementableTypes)>{});
	}

	template <class callableType, class... incrementableTypes>
	auto tuple_map_return(const std::tuple<incrementableTypes...>& p_tuple, callableType&& p_callable) -> decltype(_jkinternal::tuple_map_return_impl(p_tuple, std::forward<callableType>(p_callable), std::make_index_sequence<sizeof...(incrementableTypes)>{}))
	{
		return _jkinternal::tuple_map_return_impl(p_tuple, std::forward<callableType>(p_callable), std::make_index_sequence<sizeof...(incrementableTypes)>{});
	}

	//-----------------------------------//

	namespace _jkinternal
	{

		template <class elementType, class otherElementType>
		bool are_equal(const elementType& p_element, const otherElementType& p_other_element)
		{
			static_assert((!std::is_reference_v<elementType>) && (!std::is_reference_v<otherElementType>));

			if constexpr (std::is_same<elementType, otherElementType>::value)
			{
				return p_element == p_other_element;
			}
			else
			{
				return false;
			}
		}

	}

	template <class iteratorType>
	class iterator_range
	{
	public:

		iterator_range(const iteratorType& p_begin, const iteratorType& p_end)
			: m_begin(p_begin), m_end(p_end)
		{

		}

		iteratorType begin() const
		{
			return m_begin;
		}

		iteratorType end() const
		{
			return m_end;
		}

	private:

		iteratorType m_begin, m_end;

	};

	template <class iteratorType>
	iterator_range<iteratorType> make_iterator_range(const iteratorType& p_begin, const iteratorType& p_end)
	{
		return iterator_range<iteratorType>(p_begin, p_end);
	}

}

#endif