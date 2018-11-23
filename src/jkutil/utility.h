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

		template <class callableType, class tupleType, std::size_t... indexValues>
		void tuple_map_impl(tupleType& p_tuple, callableType&& p_callable, std::index_sequence<indexValues...>)
		{
			(std::forward<callableType>(p_callable)(std::get<indexValues>(p_tuple)), ...);
		}

		template <class callableType, class tupleType, std::size_t... indexValues>
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

	//To allow usage of range based for-loop with only iterators.
	template <class iteratorType>
	class iterator_range
	{
	public:

		using value_type = typename std::iterator_traits<iteratorType>::value_type;
		using const_iterator = iteratorType;
		using iterator = iteratorType;

		iterator_range(const iteratorType& p_begin, const iteratorType& p_end)
			: m_begin(p_begin), m_end(p_end)
		{}

		iterator_range(iteratorType&& p_begin, iteratorType&& p_end)
			: m_begin(std::move(p_begin)), m_end(std::move(p_end))
		{}

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
	iterator_range<std::decay_t<iteratorType>> make_iterator_range(iteratorType&& p_begin, iteratorType&& p_end)
	{
		return iterator_range<std::decay_t<iteratorType>>(std::forward<iteratorType>(p_begin), std::forward<iteratorType>(p_end));
	}

	template <class objectType>
	decltype(auto) arrow_operator(const objectType& p_object)
	{
		return p_object.operator->();
	}

	template <class objectType>
	decltype(auto) arrow_operator(const objectType* p_object)
	{
		return p_object;
	}

	template <class objectType>
	decltype(auto) arrow_operator(objectType& p_object)
	{
		return p_object.operator->();
	}

	template <class objectType>
	decltype(auto) arrow_operator(objectType* p_object)
	{
		return p_object;
	}

}

#endif