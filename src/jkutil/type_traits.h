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

	template <bool... Bs>
	struct variadic_and : public std::true_type
	{};

	template <bool B1, bool... Bs>
	struct variadic_and<B1, Bs...> : public std::bool_constant<B1 && variadic_and<Bs...>::value>
	{};

	template <class... Ts>
	struct all_unique : public std::true_type {};

	template <class T, class... Ts>
	struct all_unique<T, Ts...> : public std::bool_constant<
		variadic_and<(!std::is_same<T, Ts>::value)...>::value && all_unique<Ts...>::value>
	{};

	template <class... comparedTypes>
	struct all_same : public std::false_type {};

	template <class comparedType>
	struct all_same<comparedType> : public std::true_type {};

	template <class comparedType1, class comparedType2, class... comparedTypes>
	struct all_same<comparedType1, comparedType2, comparedTypes...> : public std::conditional<
		std::is_same<comparedType1, comparedType2>::value,
			typename all_same<comparedType2, comparedTypes...>::type,
			std::false_type> {};

}

#endif