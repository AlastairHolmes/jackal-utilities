/*!
	@file		bitwise.h
	@author		Alastair Holmes
	@date		26/10/2018
 */

#ifndef JKUTIL_BITWISE_H
#define JKUTIL_BITWISE_H

#include <cstdint>
#include <limits>

namespace jkutil
{

	/*!
		@brief Returns a value with all the bits, that are more or equally significant to the highest set bit in /p p_bits, set.

		@details Examples in 5 bits (For simplicity):
		Input      | Output
		---------- | -------
		00010 (2)  | 11110
		00000 (0)  | 00000
		10000 (16) | 10000
		00001 (1)  | 11111
	*/
	constexpr std::uint32_t all_highest_bits(std::uint32_t p_bits)
	{
		std::uint32_t mask = p_bits;
		mask |= mask >> 1;
		mask |= mask >> 2;
		mask |= mask >> 4;
		mask |= mask >> 8;
		mask |= mask >> 16;
		return mask;
	}

	constexpr std::uint64_t all_highest_bits(std::uint64_t p_bits)
	{
		std::uint64_t mask = p_bits;
		mask |= mask >> 1;
		mask |= mask >> 2;
		mask |= mask >> 4;
		mask |= mask >> 8;
		mask |= mask >> 16;
		mask |= mask >> 32;
		return mask;
	}

	/*!
		@brief Returns a value with only the highest value bit in \p p_bits set.

		@details Examples in 5 bits (For simplicity):
		Input      | Output
		---------- | -------
		01100 (12) | 01000
		11111 (31) | 10000
		00000 (0)  | 00000
		00111 (7)  | 00100
	*/
	constexpr std::uint32_t set_highest_bit(std::uint32_t p_bits)
	{
		std::uint32_t temp = all_highest_bits(p_bits);
		return temp ^ (temp >> 1);
	}

	constexpr std::uint64_t set_highest_bit(std::uint64_t p_bits)
	{
		std::uint64_t temp = all_highest_bits(p_bits);
		return temp ^ (temp >> 1);
	}

	/*!
		@brief Returns a value with all the bits, that are less or equally significant to the lowest set bit in \p p_bits, set.

		@details Examples in 5 bits (For simplicity):
		Input      | Output
		---------- | -------
		00010 (2)  | 00011
		00000 (0)  | 00000
		10000 (16) | 11111
		00001 (1)  | 00001
	*/
	constexpr std::uint32_t all_lowest_bits(std::uint32_t p_bits)
	{
		std::uint32_t mask = p_bits;
		mask |= mask << 1;
		mask |= mask << 2;
		mask |= mask << 4;
		mask |= mask << 8;
		mask |= mask << 16;
		return mask;
	}

	constexpr std::uint64_t all_lowest_bits(std::uint64_t p_bits)
	{
		std::uint64_t mask = p_bits;
		mask |= mask << 1;
		mask |= mask << 2;
		mask |= mask << 4;
		mask |= mask << 8;
		mask |= mask << 16;
		mask |= mask << 32;
		return mask;
	}

	/*!
		@brief Returns a value with only the lowest value bit in \p p_bits set. 

		@details Examples in 5 bits (For simplicity):
		Input      | Output
		---------- | -------
		01100 (12) | 00100
		11111 (31) | 00001
		00000 (0)  | 00000
		00111 (7)  | 00001
	*/
	constexpr std::uint32_t set_lowest_bit(std::uint32_t p_bits)
	{
		std::uint32_t temp = all_lowest_bits(p_bits);
		return temp ^ (temp << 1);
	}

	constexpr std::uint64_t set_lowest_bit(std::uint64_t p_bits)
	{
		std::uint64_t temp = all_lowest_bits(p_bits);
		return temp ^ (temp << 1);
	}

	/*!
		@brief Rounds up to the nearest power of two
		@details For any number more than '2^31' (32 bit) or '2^63' (64 bit) returns '2^31' or '2^63' respectively.
	*/
	constexpr std::uint32_t ceil_pow2(std::uint32_t p_number)
	{
		std::uint32_t result = (p_number < 2) ? 1 : set_highest_bit(p_number - 1) << 1;
		return result == 0 ? set_highest_bit(std::numeric_limits<std::uint32_t>::max()) : result;
	}

	constexpr std::uint64_t ceil_pow2(std::uint64_t p_number)
	{
		std::uint64_t result = (p_number < 2) ? 1 : set_highest_bit(p_number - 1) << 1;
		return result == 0 ? set_highest_bit(std::numeric_limits<std::uint64_t>::max()) : result;
	}

}

#endif