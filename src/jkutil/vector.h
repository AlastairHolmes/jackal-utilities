/*!
	@file		vector.h
	@author		Alastair Holmes
	@date		27/10/2018
 */

#ifndef JKUTIL_VECTOR_H
#define JKUTIL_VECTOR_H

#include <vector>
#include <jkutil\allocator.h>

namespace jkutil
{

	template <class elementType, class storableAllocatorType>
	class vector
	{
	private:

		using internal_vector_type = std::vector<elementType,jkutil::allocator_stl_adapter<elementType, storableAllocatorType>>;

	public:

		using value_type = typename internal_vector_type::value_type;
		using allocator_type = storableAllocatorType;
		using size_type = typename internal_vector_type::size_type;
		using difference_type = typename internal_vector_type::difference_type;
		using reference = typename internal_vector_type::reference;
		using const_reference = typename internal_vector_type::const_reference;
		using pointer = typename internal_vector_type::pointer;
		using const_pointer = typename internal_vector_type::const_pointer;
		using iterator = typename internal_vector_type::iterator;
		using const_iterator = typename internal_vector_type::const_iterator;
		using reverse_iterator = typename internal_vector_type::reverse_iterator;
		using const_reverse_iterator = typename internal_vector_type::const_reverse_iterator;

	public:

		vector();
		vector(const storableAllocatorType& p_allocator);

		vector(const vector&) = default;
		vector(vector&&) = default;

		template <class otherStorableAllocatorType>
		vector(const vector<elementType,otherStorableAllocatorType>& p_instance, const storableAllocatorType& p_allocator);

		template <class otherStorableAllocatorType>
		vector(vector<elementType, otherStorableAllocatorType>&& p_instance, const storableAllocatorType& p_allocator);

	public:

		vector& operator=(const vector& p_rhs);
		vector& operator=(vector&& p_rhs);

		template <class otherStorableAllocatorType>
		vector& assign_value(const vector<elementType, otherStorableAllocatorType>& p_instance);
		template <class otherStorableAllocatorType>
		vector& assign_value(vector<elementType, otherStorableAllocatorType>&& p_instance);

		void swap(vector& p_instance);
		template <class otherStorableAllocatorType>
		void swap_value(vector<elementType, otherStorableAllocatorType>& p_instance);

		bool operator==(const vector& p_rhs) const;
		bool operator!=(const vector& p_rhs) const;

	public:
		
		//assign

		//at

		//operator[]

		//front

		//back

		//data

		//begin
		//cbegin

		//end
		//cend

		//rbegin
		//crbegin

		//rend
		//crend

		//empty

		//size

		//max_size

		//reserve

		//capacity

		//shrink_to_fit

		//clear

		//insert

		//emplace

		//erase

		//push_back

		//emplace_back

		//pop_back

		//resize

		const storableAllocatorType& get_allocator() const;

	private:

		internal_vector_type m_vector;

	};

}

#endif