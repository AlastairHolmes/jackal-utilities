/*!
	@file		overloaded.h
	@author		Alastair Holmes
	@date		26/10/2018
 */


#ifndef JKUTIL_OVERLOADED_H
#define JKUTIL_OVERLOADED_H

namespace jkutil
{

	/*!
		@struct overloaded

		@brief A utility to allow easily creation of overloaded lambdas.

		@details The example below creates a lambda \c x with two overloads one for \c int
		and one for \c char.
		@code
			auto x = overloaded{
				[](int p_value)
				{
					return p_value;
				},
				[](char p_value)
				{
					return (int)p_value + 1;
				}};
		@endcode
	*/
	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

}

#endif