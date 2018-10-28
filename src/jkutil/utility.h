/*!
	@file		utility.h
	@author		Alastair Holmes
	@date		28/10/2018
 */

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

}