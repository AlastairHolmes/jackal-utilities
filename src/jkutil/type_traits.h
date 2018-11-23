/*!
	@file		type_traits.h
	@author		Alastair Holmes
	@date		26/10/2018
 */

#ifndef JKUTIL_TYPE_TRAITS_H
#define JKUTIL_TYPE_TRAITS_H

#include <type_traits>

namespace jkutil
{

	template <class... Ts>
	struct all_unique : public std::true_type {};

	template <class T, class... Ts>
	struct all_unique<T, Ts...> : public std::bool_constant<
		std::conjunction_v<std::negation<std::is_same<T, Ts>>...> &&
		all_unique<Ts...>::value>
	{};

	template <class... Ts>
	inline constexpr bool all_unique_v = all_unique<Ts...>::value;

}

#endif