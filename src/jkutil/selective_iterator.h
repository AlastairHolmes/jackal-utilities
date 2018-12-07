/*!
	@file		selective_iterator.h
	@author		Alastair Holmes
	@date		23/11/2018
 */

#ifndef JKUTIL_SELECTIVE_ITERATOR_H
#define JKUTIL_SELECTIVE_ITERATOR_H

#include <jkutil\utility.h>
#include <iterator>
#include <type_traits>
#include <optional>

namespace jkutil
{

	namespace _jkinternal
	{

		template <class iteratorType, class selectorCallableType, class iteratorCategoryTag = typename std::iterator_traits<iteratorType>::iterator_category>
		class selective_iterator
		{};

		template <class iteratorType, class selectorCallableType>
		class selective_iterator<iteratorType, selectorCallableType, std::input_iterator_tag>
		{
		public:

			using value_type = typename std::iterator_traits<iteratorType>::value_type;
			using difference_type = typename std::iterator_traits<iteratorType>::difference_type;
			using reference = typename std::iterator_traits<iteratorType>::reference;
			using pointer = typename std::iterator_traits<iteratorType>::pointer;
			using iterator_category = std::input_iterator_tag;

			selective_iterator(const iteratorType& p_iterator, const iteratorType& p_end_iterator, selectorCallableType p_callable)
				: m_iterator(p_iterator),
				m_end_iterator(p_end_iterator),
				m_callable(p_callable)
			{
				pre_find();
			}

			selective_iterator(iteratorType&& p_iterator, iteratorType&& p_end_iterator, selectorCallableType p_callable)
				: m_iterator(p_iterator),
				m_end_iterator(p_end_iterator),
				m_callable(p_callable)
			{
				pre_find();
			}

			selective_iterator(const selective_iterator&) = default;
			selective_iterator(selective_iterator&&) = default;

			selective_iterator& operator=(const selective_iterator&) = default;
			selective_iterator& operator=(selective_iterator&&) = default;

			bool operator==(const selective_iterator& p_rhs) const
			{
				return m_iterator == p_rhs.m_iterator;
			}

			bool operator!=(const selective_iterator& p_rhs) const
			{
				return m_iterator != p_rhs.m_iterator;
			}

			selective_iterator& operator++()
			{
				++m_iterator;
				pre_find();
				return *this;
			}

			selective_iterator operator++(int)
			{
				selective_iterator temp = *this;
				m_iterator++;
				post_find();
				return temp;
			}

			reference operator*() const
			{
				return *m_iterator;
			}

			pointer operator->() const
			{
				return jkutil::arrow_operator(m_iterator);
			}

			const iteratorType& get_internal_iterator() const
			{
				return m_iterator;
			}

		private:

			void pre_find()
			{
				for (; m_iterator != m_end_iterator; ++m_iterator)
				{
					if (m_callable(*m_iterator))
					{
						break;
					}
				}
			}

			void post_find()
			{
				for (; m_iterator != m_end_iterator; m_iterator++)
				{
					if (m_callable(*m_iterator))
					{
						break;
					}
				}
			}

			selectorCallableType m_callable;
			iteratorType m_iterator;
			iteratorType m_end_iterator;
		};

		template <class iteratorType, class selectorCallableType>
		class selective_iterator<iteratorType, selectorCallableType, std::forward_iterator_tag>
		{
		public:

			using value_type = typename std::iterator_traits<iteratorType>::value_type;
			using difference_type = typename std::iterator_traits<iteratorType>::difference_type;
			using reference = typename std::iterator_traits<iteratorType>::reference;
			using pointer = typename std::iterator_traits<iteratorType>::pointer;
			using iterator_category = std::forward_iterator_tag;

			selective_iterator() = default;

			selective_iterator(const iteratorType& p_iterator, const iteratorType& p_end_iterator, selectorCallableType p_callable)
				: m_iterator(p_iterator),
				m_end_iterator(p_end_iterator),
				m_callable(p_callable)
			{
				find();
			}

			selective_iterator(iteratorType&& p_iterator, iteratorType&& p_end_iterator, selectorCallableType p_callable)
				: m_iterator(p_iterator),
				m_end_iterator(p_end_iterator),
				m_callable(p_callable)
			{
				find();
			}

			selective_iterator(const selective_iterator&) = default;
			selective_iterator(selective_iterator&&) = default;

			selective_iterator& operator=(const selective_iterator&) = default;
			selective_iterator& operator=(selective_iterator&&) = default;

			bool operator==(const selective_iterator& p_rhs) const
			{
				return m_iterator == p_rhs.m_iterator;
			}

			bool operator!=(const selective_iterator& p_rhs) const
			{
				return m_iterator != p_rhs.m_iterator;
			}

			selective_iterator& operator++()
			{
				++m_iterator;
				find();
				return *this;
			}

			selective_iterator operator++(int)
			{
				selective_iterator temp = *this;
				++m_iterator;
				find();
				return temp;
			}

			reference operator*() const
			{
				return *m_iterator;
			}

			pointer operator->() const
			{
				return jkutil::arrow_operator(m_iterator);
			}

			const iteratorType& get_internal_iterator() const
			{
				return m_iterator;
			}

			decltype(auto) end() const
			{
				selective_iterator<iteratorType, selectorCallableType, std::forward_iterator_tag> temp = *this;
				temp.m_iterator = temp.m_end_iterator;
				return temp;
			}

		protected:

			void find()
			{
				for (; m_iterator != m_end_iterator; ++m_iterator)
				{
					if (m_callable.value()(*m_iterator))
					{
						break;
					}
				}
			}

			std::optional<selectorCallableType> m_callable;
			iteratorType m_iterator;
			iteratorType m_end_iterator;

		};

		template <class iteratorType, class selectorCallableType>
		class selective_iterator<iteratorType, selectorCallableType, std::bidirectional_iterator_tag> : public selective_iterator<iteratorType, selectorCallableType, std::forward_iterator_tag>
		{
		public:

			using iterator_category = std::bidirectional_iterator_tag;
			using selective_iterator<iteratorType, selectorCallableType, std::forward_iterator_tag>::selective_iterator;

			selective_iterator& operator--()
			{
				--m_iterator;
				find();
				return *this;
			}

			selective_iterator operator--(int)
			{
				selective_iterator temp = *this;
				--m_iterator;
				find();
				return temp;
			}

			decltype(auto) end() const
			{
				selective_iterator<iteratorType, selectorCallableType, std::bidirectional_iterator_tag> temp = *this;
				temp.m_iterator = temp.m_end_iterator;
				return temp;
			}

		};

		template <class iteratorType, class selectorCallableType>
		class selective_iterator<iteratorType, selectorCallableType, std::random_access_iterator_tag> : public selective_iterator<iteratorType, selectorCallableType, std::bidirectional_iterator_tag>
		{
		public:

			using selective_iterator<iteratorType, selectorCallableType, std::bidirectional_iterator_tag>::selective_iterator;

			decltype(auto) end() const
			{
				selective_iterator<iteratorType, selectorCallableType, std::random_access_iterator_tag> temp = *this;
				temp.m_iterator = temp.m_end_iterator;
				return temp;
			}

		};

	}

	template <class iteratorType, class selectorCallableType>
	using selective_iterator = _jkinternal::selective_iterator<iteratorType, selectorCallableType>;

	template <class iteratorType, class selectorCallableType>
	selective_iterator<std::decay_t<iteratorType>, std::decay_t<selectorCallableType>> make_selective_iterator(iteratorType&& p_iterator, iteratorType&& p_end_iterator, selectorCallableType&& p_callable)
	{
		return selective_iterator<std::decay_t<iteratorType>, std::decay_t<selectorCallableType>>(std::forward<iteratorType>(p_iterator), std::forward<iteratorType>(p_end_iterator), std::forward<selectorCallableType>(p_callable));
	}

	template <class rangeType, class selectorCallableType>
	decltype(auto) make_selective_range(rangeType&& p_range, selectorCallableType&& p_callable)
	{
		return make_iterator_range(
			make_selective_iterator(p_range.begin(), p_range.end(), std::forward<selectorCallableType>(p_callable)),
			make_selective_iterator(p_range.end(), p_range.end(), std::forward<selectorCallableType>(p_callable)));
	}

}

#endif