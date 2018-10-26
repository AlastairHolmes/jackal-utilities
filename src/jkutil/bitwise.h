#ifndef JKUTIL_BITWISE_H
#define JKUTIL_BITWISE_H

#include <stdint.h>

namespace jkutil
{

	//Returns a value with all the bits, that are more or equally significant to the highest set bit in 'p_bits', set.
	// Examples (5 bits for simplicity, rather than the actual 32 or 64 bits):
		//00010 -> 11110
		//00000 -> 00000
		//10000 -> 10000
		//00001 -> 11111
	constexpr uint32_t all_highest_bits(uint32_t p_bits);
	constexpr uint64_t all_highest_bits(uint64_t p_bits);

	//Returns a value with only the highest value bit in 'p_bits' set. 
	//Examples (5 bits for simplicity, rather than the actual 32 or 64 bits):
		// 01100 -> 01000
		// 11111 -> 10000
		// 00000 -> 00000
		// 00111 -> 00100
	constexpr uint32_t set_highest_bit(uint32_t p_bits);
	constexpr uint64_t set_highest_bit(uint64_t p_bits);

	//Returns a value with all the bits, that are less or equally significant to the lowest set bit in 'p_bits', set.
	//Examples (5 bits for simplicity, rather than the actual 32 or 64 bits):
		//00010 -> 00011
		//00000 -> 00000
		//10000 -> 11111
		//00001 -> 00001
	constexpr uint32_t all_lowest_bits(uint32_t p_bits);
	constexpr uint64_t all_lowest_bits(uint64_t p_bits);

	//Returns a value with only the lowest value bit in 'p_bits' set. 
		//Examples (5 bits for simplicity, rather than the actual 32 or 64 bits):
		// 01100 -> 00100
		// 11111 -> 00001
		// 00000 -> 00000
		// 00111 -> 00001
	constexpr uint32_t set_lowest_bit(uint32_t p_bits);
	constexpr uint64_t set_lowest_bit(uint64_t p_bits);

	//Rounds up to the nearest power of two (For any number more than '2^31' (32 bit) or '2^63' (64 bit) returns '2^31' or '2^63' respectively.
	constexpr uint32_t ceil_pow2(uint32_t p_number);
	constexpr uint64_t ceil_pow2(uint64_t p_number);

	constexpr uint32_t all_highest_bits(uint32_t p_bits)
	{
		uint32_t mask = p_bits;
		mask |= mask >> 1;
		mask |= mask >> 2;
		mask |= mask >> 4;
		mask |= mask >> 8;
		mask |= mask >> 16;
		return mask;
	}

	constexpr uint64_t all_highest_bits(uint64_t p_bits)
	{
		uint64_t mask = p_bits;
		mask |= mask >> 1;
		mask |= mask >> 2;
		mask |= mask >> 4;
		mask |= mask >> 8;
		mask |= mask >> 16;
		mask |= mask >> 32;
		return mask;
	}

	constexpr uint32_t set_highest_bit(uint32_t p_bits)
	{
		uint32_t temp = all_highest_bits(p_bits);
		return temp ^ (temp >> 1);
	}

	constexpr uint64_t set_highest_bit(uint64_t p_bits)
	{
		uint64_t temp = all_highest_bits(p_bits);
		return temp ^ (temp >> 1);
	}

	constexpr uint32_t all_lowest_bits(uint32_t p_bits)
	{
		uint32_t mask = p_bits;
		mask |= mask << 1;
		mask |= mask << 2;
		mask |= mask << 4;
		mask |= mask << 8;
		mask |= mask << 16;
		return mask;
	}

	constexpr uint64_t all_lowest_bits(uint64_t p_bits)
	{
		uint64_t mask = p_bits;
		mask |= mask << 1;
		mask |= mask << 2;
		mask |= mask << 4;
		mask |= mask << 8;
		mask |= mask << 16;
		mask |= mask << 32;
		return mask;
	}

	constexpr uint32_t set_lowest_bit(uint32_t p_bits)
	{
		uint32_t temp = all_lowest_bits(p_bits);
		return temp ^ (temp << 1);
	}

	constexpr uint64_t set_lowest_bit(uint64_t p_bits)
	{
		uint64_t temp = all_lowest_bits(p_bits);
		return temp ^ (temp << 1);
	}

	constexpr uint32_t ceil_pow2(uint32_t p_number)
	{
		if (p_number < 2)
		{
			return 1;
		}
		return set_highest_bit(p_number - 1) << 1;
	}

	constexpr uint64_t ceil_pow2(uint64_t p_number)
	{
		if (p_number < 2)
		{
			return 1;
		}
		return set_highest_bit(p_number - 1) << 1;
	}

}

#endif