/*!
	@file		transform_iterator.h
	@author		Alastair Holmes
	@date		23/11/2018
 */

#ifndef JKUTIL_TRANSFORM_ITERATOR_H
#define JKUTIL_TRANSFORM_ITERATOR_H

#include <jkutil\utility.h>
#include <iterator>
#include <type_traits>

namespace jkutil
{

	template <class iteratorType, class transformCallableType>
	class transform_iterator
	{
	public:

		static_assert(std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<iteratorType>::iterator_category>);

		using value_type = std::invoke_result_t<transformCallableType, typename std::iterator_traits<iteratorType>::reference>;
		using difference_type = typename std::iterator_traits<iteratorType>::difference_type;
		using reference = std::invoke_result_t<transformCallableType, typename std::iterator_traits<iteratorType>::reference>;
		using pointer = std::invoke_result_t<transformCallableType, typename std::iterator_traits<iteratorType>::reference>;
		using iterator_category = std::input_iterator_tag;

		transform_iterator(const iteratorType& p_iterator, transformCallableType p_callable)
			: m_iterator(p_iterator),
			m_callable(p_callable)
		{

		}

		transform_iterator(iteratorType&& p_iterator, transformCallableType p_callable)
			: m_iterator(std::move(p_iterator)),
			m_callable(p_callable)
		{

		}

		transform_iterator(const transform_iterator&) = default;
		transform_iterator(transform_iterator&&) = default;

		transform_iterator& operator=(const transform_iterator&) = default;
		transform_iterator& operator=(transform_iterator&&) = default;

		bool operator==(const transform_iterator& p_rhs) const
		{
			return m_iterator == p_rhs.m_iterator;
		}

		bool operator!=(const transform_iterator& p_rhs) const
		{
			return m_iterator != p_rhs.m_iterator;
		}

		transform_iterator& operator++()
		{
			++m_iterator;
			return *this;
		}

		transform_iterator operator++(int)
		{
			transform_iterator temp = *this;
			++m_iterator;
			return temp;
		}

		reference operator*() const
		{
			return m_callable(*m_iterator);
		}

		pointer operator->() const
		{
			return m_callable(*m_iterator);
		}

		const iteratorType& get_internal_iterator() const
		{
			return m_iterator;
		}

	private:

		transformCallableType m_callable;
		iteratorType m_iterator;

	};

	template <class iteratorType, class transformCallableType>
	typename std::iterator_traits<iteratorType>::difference_type distance(const transform_iterator<iteratorType, transformCallableType>& p_iterator1, const transform_iterator<iteratorType, transformCallableType>& p_iterator2)
	{
		return std::distance(p_iterator1.get_internal_iterator(), p_iterator2.get_internal_iterator());
	}

	template <class iteratorType, class transformCallableType>
	transform_iterator<std::decay_t<iteratorType>, std::decay_t<transformCallableType>> make_transform_iterator(iteratorType&& p_iterator, transformCallableType&& p_callable)
	{
		return transform_iterator<std::decay_t<iteratorType>, std::decay_t<transformCallableType>>(std::forward<iteratorType>(p_iterator), std::forward<transformCallableType>(p_callable));
	}

	template <class rangeType, class transformCallableType>
	decltype(auto) make_transform_range(rangeType&& p_range, transformCallableType&& p_callable)
	{
		return make_iterator_range(
			make_transform_iterator(p_range.begin(),std::forward<transformCallableType>(p_callable)),
			make_transform_iterator(p_range.end(),std::forward<transformCallableType>(p_callable)));
	}

}

#endif