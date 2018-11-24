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
		decltype(auto) tuple_map_return_impl(tupleType& p_tuple, callableType&& p_callable, std::index_sequence<indexValues...>)
		{
			return std::tuple<decltype(std::forward<callableType>(p_callable)(std::get<indexValues>(p_tuple)))...>(std::forward<callableType>(p_callable)(std::get<indexValues>(p_tuple))...);
		}

	}

	template <class callableType, class... elementTypes>
	void tuple_map(std::tuple<elementTypes...>& p_tuple, callableType&& p_callable)
	{
		_jkinternal::tuple_map_impl(p_tuple, std::forward<callableType>(p_callable), std::make_index_sequence<sizeof...(elementTypes)>{});
	}

	template <class callableType, class... elementTypes>
	void tuple_map(const std::tuple<elementTypes...>& p_tuple, callableType&& p_callable)
	{
		_jkinternal::tuple_map_impl(p_tuple, std::forward<callableType>(p_callable), std::make_index_sequence<sizeof...(elementTypes)>{});
	}

	template <class callableType, class... elementTypes>
	decltype(auto) tuple_map_return(std::tuple<elementTypes...>& p_tuple, callableType&& p_callable)
	{
		return _jkinternal::tuple_map_return_impl(p_tuple, std::forward<callableType>(p_callable), std::make_index_sequence<sizeof...(elementTypes)>{});
	}

	template <class callableType, class... elementTypes>
	decltype(auto) tuple_map_return(const std::tuple<elementTypes...>& p_tuple, callableType&& p_callable)
	{
		return _jkinternal::tuple_map_return_impl(p_tuple, std::forward<callableType>(p_callable), std::make_index_sequence<sizeof...(elementTypes)>{});
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
	template <class iteratorBeginType, class iteratorEndType>
	class iterator_range
	{
	public:

		iterator_range(const iteratorBeginType& p_begin, const iteratorEndType& p_end)
			: m_begin(p_begin), m_end(p_end)
		{}

		iterator_range(iteratorBeginType&& p_begin, iteratorEndType&& p_end)
			: m_begin(std::move(p_begin)), m_end(std::move(p_end))
		{}

		iteratorBeginType begin() const
		{
			return m_begin;
		}

		iteratorEndType end() const
		{
			return m_end;
		}

	private:

		iteratorBeginType m_begin;
		iteratorEndType m_end;

	};

	template <class iteratorBeginType, class iteratorEndType>
	iterator_range<std::decay_t<iteratorBeginType>, std::decay_t<iteratorEndType>> make_iterator_range(iteratorBeginType&& p_begin, iteratorEndType&& p_end)
	{
		return iterator_range<std::decay_t<iteratorBeginType>, std::decay_t<iteratorEndType>>(std::forward<iteratorBeginType>(p_begin), std::forward<iteratorEndType>(p_end));
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