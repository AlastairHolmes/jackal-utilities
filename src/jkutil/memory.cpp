#include <jkutil\memory.h>
#include <jkutil\assert.h>
#include <jkutil\bitwise.h>

#include <limits>
#include <memory>

namespace jkutil
{

	void* aligned_malloc(size_t p_size, size_t p_alignment)
	{
		using align_header_t = uint16_t;

		JKUTIL_ASSERT(
			p_alignment <= std::numeric_limits<align_header_t>::max && //Check the request alignment in less than the maximum possible alignment than can be guarenteed
			jkutil::ceil_pow2(p_alignment) == p_alignment); //Check p_alignment is a power of two.

		//Ensures the alignment offset header is also aligned.
		const size_t alignment = p_alignment > alignof(align_header_t) ? p_alignment : alignof(align_header_t);

		//Ensures a more an zero size is always allocated ensuring unique pointer values.
		const size_t size = p_size == 0 ? 1 : p_size;

		//The maximum additional size neede dto guarentee alignment and space for the alignment offset header.
		const size_t max_header_size = sizeof(align_header_t) + (alignment - 1);

		//The total memory to allocate.
		const size_t memory_size = size + max_header_size;

		void* memory = reinterpret_cast<char*>(malloc(memory_size));

		if (memory)
		{

			//This takes the memory from malloc and aligns it putting the result in aligned_ptr
			//and decreases remaining_space by the number of bytes used to align it (Not including
			//the sizeof(align_header_t)).
			void* aligned_ptr = reinterpret_cast<char*>(memory) + sizeof(align_header_t); //Ensures there is space in front of the aligned_ptr to put the alignment header.
			size_t total_space = memory_size - sizeof(align_header_t);
			size_t remaining_space = total_space;
			const void* result = std::align(alignment, size, aligned_ptr, remaining_space);
			JKUTIL_ASSERT(result);//Something has seriously gone wrong if this assert fails.
				//std::align only returns nullptr if there isn't space to do the alignment, but it is guarenteed to have enough space.

			//The header size including the bytes used to align and the sizeof(align_header_t) for the storage of the header_size.
			size_t header_size = reinterpret_cast<char*>(aligned_ptr) - reinterpret_cast<char*>(memory);
			JKUTIL_ASSERT(header_size == (total_space - remaining_space) + sizeof(align_header_t)); //Check to ensure align_header_size was calculated correctly.

			//Put the header_size in the header.
			*(reinterpret_cast<align_header_t*>(aligned_ptr) - 1) = static_cast<align_header_t>(header_size);

			memory = aligned_ptr;
		}

		return memory;
	}

	void aligned_free(void* p_ptr, size_t p_size)
	{
		using align_header_t = uint16_t;

		if (p_ptr)
		{
			const size_t size = p_size == 0 ? 1 : p_size;

			//The pointer to the header
			align_header_t* const header = reinterpret_cast<align_header_t*>(p_ptr) - 1;
			const size_t header_size = *header;

			//Gets the original memory return from malloc in aligned_malloc().
			void* const memory = reinterpret_cast<char*>(p_ptr) - header_size;
			free(memory);
		}
	}

	void memory_copy(void* p_destination, const void* p_source, size_t p_size)
	{
		JKUTIL_ASSERT(p_size == 0 || (p_source != nullptr && p_destination != nullptr));

		if (p_size != 0 && p_destination != nullptr && p_source != nullptr)
		{
			memcpy(p_destination, p_source, p_size);
		}
	}

}