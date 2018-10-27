/*!
	@file		aligned.h
	@author		Alastair Holmes
	@date		25/10/2018
 */

#ifndef JKUTIL_ALIGNED_H
#define JKUTIL_ALIGNED_H

#include <jkutil\bitwise.h>

namespace jkutil
{

	/*!
		@struct aligned

		@brief A simple object wrapper for safely over-aligning objects.

		@details This struct's original purpose was to allow the creation of
		arrays of overaligned objects, which can't be done with \c alignas().
		The wrapped object will always at a minimum be aligned to \c alignof(elementType),
		even if this is more than the specified \c elementAlignment.

		@tparam elementType Type of the wrapped object.
		@tparam elementAlignment The minimum alignment of the public member variable
		\c value; the actual alignment will always be equal to or more than this.
		Must be a power of two.

		@attention \c elementAlignment must be a power of two. 
	*/
	template <class elementType, size_t elementAlignment = alignof(elementType)>
	struct aligned
	{

		static_assert((set_highest_bit(elementAlignment) == elementAlignment) && (elementAlignment != 0), "jkutil::aligned: elementAlignment must be a power of two.");

		/// @brief Default constructs the member #value.
		aligned() = default;

		aligned(const elementType& p_element);
		aligned(elementType&& p_element);

		aligned(const aligned&) = default;
		aligned(aligned&&) = default;

		aligned& operator=(const aligned&) = default;
		aligned& operator=(aligned&&) = default;

		/// @brief The public member variable \c value is over-aligned by atleast the specified \c elementAlignment.
		alignas((elementAlignment < alignof(elementType)) ? alignof(elementType) : elementAlignment) elementType value;

	};

	template<class elementType, size_t elementAlignment>
	inline aligned<elementType, elementAlignment>::aligned(const elementType& p_element)
		: value(p_element)
	{
	}

	template<class elementType, size_t elementAlignment>
	inline aligned<elementType, elementAlignment>::aligned(elementType&& p_element)
		: value(std::move(p_element))
	{
	}

}

#endif