/*!
	@file		any_derived.h
	@author		Alastair Holmes
	@date		26/10/2018
 */


#ifndef JKUTIL_ANY_DERIVED_H
#define JKUTIL_ANY_DERIVED_H

#include <jkutil\object_container.h>
#include <jkutil\allocator_pointer.h>
#include <jkutil\allocator.h>
#include <type_traits>
#include <utility>
#include <functional>

namespace jkutil
{

	//Add copyable and moveable tests of elementType

	//Stores an instance of any type derived from baseType (Publically inherits baseType).

	template <class baseType, class allocatorType, bool isCopyable = false>
	class any_derived
	{};

	template <class baseType, class allocatorType>
	class any_derived<baseType, allocatorType, false>
	{
	private:

		template <class baseType, class allocatorType, bool otherIsCopyable>
		friend class any_derived;

	public:
		
		template <class elementType, class... argumentTypes>
		any_derived(const allocatorType& p_allocator, std::in_place_type_t<elementType> p_in_place, argumentTypes&&... p_arguments);

		template <class otherAllocatorType>
		any_derived(any_derived<baseType, otherAllocatorType, false>&& p_instance, const allocatorType& p_allocator);

		any_derived(const allocatorType& p_allocator);

		any_derived(const any_derived&) = delete;
		any_derived(any_derived&& p_instance);

		~any_derived();

	public:

		any_derived& operator=(const any_derived&) = delete;
		any_derived& operator=(any_derived&& p_rhs);

		template <class otherAllocatorType>
		any_derived& assign_value(any_derived<baseType, otherAllocatorType, false>&& p_instance);

		template <class elementType, class... argumentTypes>
		void emplace(argumentTypes&&... p_arguments);

		void reset();

		void swap(any_derived& p_instance);

		template <class otherAllocatorType>
		void swap_value(any_derived<baseType, otherAllocatorType, false>& p_instance);

		bool has_value() const;
		const baseType* get() const;
		baseType* get();
		size_t size() const;

		const allocatorType& get_allocator() const;

	private:

		template<class otherAllocatorType>
		void assign_value_operation(any_derived<baseType, otherAllocatorType, false>&& p_instance);

		template<class otherAllocatorType>
		void swap_value_operation(any_derived<baseType, otherAllocatorType, false>& p_instance);

		template<class otherAllocatorType>
		bool is_same_instance(const any_derived<baseType, otherAllocatorType, false>& p_instance) const;

		bool has_self_reference() const;
		virtual_allocator_adapter<allocator_pointer<allocatorType>> get_abstract_allocator();
		general_moveable_object_container<baseType>* container_move(virtual_allocator& p_allocator);
		void set_container(general_moveable_object_container<baseType>* p_container);
		void lose_container();

		baseType* m_element;
		general_moveable_object_container<baseType>* m_container;
		allocatorType m_allocator;

	};

	template<class baseType, class allocatorType>
	template<class elementType, class... argumentTypes>
	inline any_derived<baseType, allocatorType, false>::any_derived(const allocatorType& p_allocator, std::in_place_type_t<elementType> p_in_place, argumentTypes&&... p_arguments)
		: any_derived(p_allocator)
	{
		p_in_place;
		emplace<elementType>(std::forward<argumentTypes>(p_arguments)...);
	}

	template<class baseType, class allocatorType>
	template<class otherAllocatorType>
	inline any_derived<baseType, allocatorType, false>::any_derived(any_derived<baseType, otherAllocatorType, false>&& p_instance, const allocatorType& p_allocator)
		: any_derived(p_allocator)
	{
		assign_value_operation(p_instance);
	}

	template<class baseType, class allocatorType>
	inline any_derived<baseType, allocatorType, false>::any_derived(const allocatorType& p_allocator)
		: m_element(nullptr), m_container(nullptr), m_allocator(p_allocator)
	{
	}

	template<class baseType, class allocatorType>
	inline any_derived<baseType, allocatorType, false>::any_derived(any_derived&& p_instance)
		: m_element(p_instance.m_element), m_container(p_instance.m_container), m_allocator(std::move(p_instance.m_allocator))
	{
		p_instance.lose_container();
	}

	template<class baseType, class allocatorType>
	inline any_derived<baseType, allocatorType, false>::~any_derived()
	{
		reset();
	}

	template<class baseType, class allocatorType>
	inline auto any_derived<baseType, allocatorType, false>::operator=(any_derived&& p_rhs) -> any_derived&
	{
		if (this != &p_rhs)
		{
			reset();

			if constexpr (allocatorType::propagate_on_container_move_assignment::value)
			{
				m_allocator = std::move(p_rhs.m_allocator);
				m_container = p_rhs.m_container;
				m_element = p_rhs.m_element;
				p_rhs.lose_container();
			}
			else if (allocatorType::is_always_equal::value || m_allocator == p_rhs.m_allocator)
			{
				m_container = p_rhs.m_container;
				m_element = p_rhs.m_element;
				p_rhs.lose_container();
			}
			else if (p_rhs.has_value())
			{
				set_container(p_rhs.container_move(get_abstract_allocator()));
				p_rhs.reset();
			}

			JKUTIL_ASSERT(!has_self_reference());
		}

		return *this;
	}

	template<class baseType, class allocatorType>
	template <class otherAllocatorType>
	auto any_derived<baseType, allocatorType, false>::assign_value(any_derived<baseType, otherAllocatorType, false>&& p_instance) -> any_derived&
	{
		if (!is_same_instance(p_instance))
		{
			assign_value_operation(p_instance);
			JKUTIL_ASSERT(!has_self_reference());
		}
		return *this;
	}

	template<class baseType, class allocatorType>
	template<class elementType, class ...argumentTypes>
	inline void any_derived<baseType, allocatorType, false>::emplace(argumentTypes&&... p_arguments)
	{
		reset();
		set_container(jkutil::create<moveable_object_container<elementType, baseType>>(m_allocator, std::forward<argumentTypes>(p_arguments)...));
	}

	template<class baseType, class allocatorType>
	inline void any_derived<baseType, allocatorType, false>::reset()
	{
		if (m_container != nullptr)
		{
			m_container->destroy(get_abstract_allocator());
			lose_container();
		}
	}

	template<class baseType, class allocatorType>
	inline void any_derived<baseType, allocatorType, false>::swap(any_derived& p_instance)
	{
		if (!is_same_instance(p_instance))
		{
			if constexpr(allocatorType::propagate_on_container_swap::value)
			{
				std::swap(m_allocator, p_instance.m_allocator);
				std::swap(m_container, p_instance.m_container);
				std::swap(m_element, p_instance.m_element);
			}
			else if (allocatorType::is_always_equal::value || m_allocator == p_instance.m_allocator)
			{
				std::swap(m_container, p_instance.m_container);
				std::swap(m_element, p_instance.m_element);
			}
			else
			{
				swap_value(p_instance);
			}

			JKUTIL_ASSERT(!has_self_reference());
		}
	}

	template<class baseType, class allocatorType>
	template <class otherAllocatorType>
	void any_derived<baseType, allocatorType, false>::swap_value(any_derived<baseType, otherAllocatorType, false>& p_instance)
	{
		if (!is_same_instance(p_instance))
		{
			swap_value_operation(p_instance);
			JKUTIL_ASSERT(!has_self_reference() && !p_instance.has_self_reference());
		}
	}

	template<class baseType, class allocatorType>
	inline bool any_derived<baseType, allocatorType, false>::has_value() const
	{
		return (m_container != nullptr);
	}

	template<class baseType, class allocatorType>
	inline const baseType* any_derived<baseType, allocatorType, false>::get() const
	{
		return m_element;
	}

	template<class baseType, class allocatorType>
	inline baseType* any_derived<baseType, allocatorType, false>::get()
	{
		return m_element;
	}

	template<class baseType, class allocatorType>
	inline size_t any_derived<baseType, allocatorType, false>::size() const
	{
		return (m_container) ? m_container->size() : 0;
	}

	template<class baseType, class allocatorType>
	inline const allocatorType& any_derived<baseType, allocatorType, false>::get_allocator() const
	{
		return m_allocator;
	}

	//----------

	template<class baseType, class allocatorType>
	template<class otherAllocatorType>
	inline void any_derived<baseType, allocatorType, false>::assign_value_operation(any_derived<baseType, otherAllocatorType, false>&& p_instance)
	{
		reset();
		set_container(p_instance.container_move(get_abstract_allocator()));
		p_instance.reset();
	}

	template<class baseType, class allocatorType>
	template<class otherAllocatorType>
	inline void any_derived<baseType, allocatorType, false>::swap_value_operation(any_derived<baseType, otherAllocatorType, false>& p_instance)
	{
		auto this_value = container_move(p_instance.get_abstract_allocator());
		auto instance_value = p_instance.container_move(get_abstract_allocator());

		reset();
		p_instance.reset();

		set_container(instance_value);
		p_instance.set_container(this_value);
	}

	template<class baseType, class allocatorType>
	template<class otherAllocatorType>
	inline bool any_derived<baseType, allocatorType, false>::is_same_instance(const any_derived<baseType, otherAllocatorType, false>& p_instance) const
	{
		if constexpr (std::is_same<allocatorType, otherAllocatorType>::value)
		{
			return this != &p_instance;
		}
		else
		{
			return false;
		}
	}

	template<class baseType, class allocatorType>
	inline bool any_derived<baseType, allocatorType, false>::has_self_reference() const
	{
		// Cannot refer to itself. This will only catch self references; it will not detect the equally invalid case of cyclic references (A set of any_deriveds that circularly refer to each other). E.g. A(B(C(A(...)))) where A,B,C,A are any_derived or structures that use any_deriveds.

		std::less_equal<const char*> le_op;
		std::less_equal<const char*> l_op;
		return m_element &&
			le_op(reinterpret_cast<const char*>(m_element), reinterpret_cast<const char*>(this)) &&
			l_op(reinterpret_cast<const char*>(this), reinterpret_cast<const char*>(m_element) + size());
	}

	template<class baseType, class allocatorType>
	inline virtual_allocator_adapter<allocator_pointer<allocatorType>> any_derived<baseType, allocatorType, false>::get_abstract_allocator()
	{
		return virtual_allocator_adapter<allocator_pointer<allocatorType>>(allocator_pointer<allocatorType>(&m_allocator));
	}

	template<class baseType, class allocatorType>
	inline general_moveable_object_container<baseType>* any_derived<baseType, allocatorType, false>::container_move(virtual_allocator& p_allocator)
	{
		if (m_container)
		{
			return m_container->move(p_allocator);
		}
		else
		{
			return nullptr;
		}
	}

	template<class baseType, class allocatorType>
	inline void any_derived<baseType, allocatorType, false>::set_container(general_moveable_object_container<baseType>* p_container)
	{
		m_container = p_container;
		if (m_container)
		{
			m_element = m_container->get();
		}
		else
		{
			m_element = nullptr;
		}
	}
	
	template<class baseType, class allocatorType>
	inline void any_derived<baseType, allocatorType, false>::lose_container()
	{
		m_container = nullptr;
		m_element = nullptr;
	}

	//----------------------------------------------------//

	template <class baseType, class allocatorType>
	class any_derived<baseType, allocatorType, true>
	{
	private:

		template <class baseType, class allocatorType, bool otherIsCopyable>
		friend class any_derived;

	public:

		template <class elementType, class... argumentTypes>
		any_derived(const allocatorType& p_allocator, std::in_place_type_t<elementType> p_in_place, argumentTypes&&... p_arguments);

		any_derived(const allocatorType& p_allocator);

		any_derived(const any_derived& p_instance);
		any_derived(any_derived&& p_instance);

		template <class otherAllocatorType>
		any_derived(const any_derived<baseType, otherAllocatorType, true>& p_instance, const allocatorType& p_allocator);

		template <class otherAllocatorType>
		any_derived(any_derived<baseType, otherAllocatorType, true>&& p_instance, const allocatorType& p_allocator);

		~any_derived();

	public:

		any_derived& operator=(const any_derived& p_rhs);
		any_derived& operator=(any_derived&& p_rhs);

		template <class otherAllocatorType>
		any_derived& assign_value(const any_derived<baseType, otherAllocatorType, true>& p_instance);

		template <class otherAllocatorType>
		any_derived& assign_value(any_derived<baseType, otherAllocatorType, true>&& p_instance);

		template <class elementType, class... argumentTypes>
		void emplace(argumentTypes&&... p_arguments);

		void reset();

		void swap(any_derived& p_instance);

		template <class otherAllocatorType>
		void swap_value(any_derived<baseType, otherAllocatorType, true>& p_instance);

		bool has_value() const;
		const baseType* get() const;
		baseType* get();
		size_t size() const;

		const allocatorType& get_allocator() const;

	private:

		template<class otherAllocatorType>
		void assign_value_operation(const any_derived<baseType, otherAllocatorType, true>& p_instance);

		template<class otherAllocatorType>
		void assign_value_operation(any_derived<baseType, otherAllocatorType, true>&& p_instance);

		template<class otherAllocatorType>
		void swap_value_operation(any_derived<baseType, otherAllocatorType, true>& p_instance);

		template<class otherAllocatorType>
		bool is_same_instance(const any_derived<baseType, otherAllocatorType, true>& p_instance) const;

		bool has_self_reference() const;
		virtual_allocator_adapter<allocator_pointer<allocatorType>> get_abstract_allocator();
		general_cloneable_object_container<baseType>* container_clone(virtual_allocator& p_allocator) const;
		general_cloneable_object_container<baseType>* container_move(virtual_allocator& p_allocator);
		void set_container(general_cloneable_object_container<baseType>* p_container);
		void lose_container();

		baseType* m_element;
		general_cloneable_object_container<baseType>* m_container;
		allocatorType m_allocator;

	};

	template<class baseType, class allocatorType>
	template<class elementType, class... argumentTypes>
	inline any_derived<baseType, allocatorType, true>::any_derived(const allocatorType& p_allocator, std::in_place_type_t<elementType> p_in_place, argumentTypes&&... p_arguments)
		: any_derived(p_allocator)
	{
		p_in_place;
		emplace<elementType>(std::forward<argumentTypes>(p_arguments)...);
	}

	template<class baseType, class allocatorType>
	inline any_derived<baseType, allocatorType, true>::any_derived(const allocatorType& p_allocator)
		: m_element(nullptr), m_container(nullptr), m_allocator(p_allocator)
	{
	}

	template<class baseType, class allocatorType>
	inline any_derived<baseType, allocatorType, true>::any_derived(const any_derived& p_instance)
		: any_derived(p_instance.m_allocator)
	{
		set_container(p_instance.container_clone(get_abstract_allocator()));
	}

	template<class baseType, class allocatorType>
	inline any_derived<baseType, allocatorType, true>::any_derived(any_derived&& p_instance)
		: m_element(p_instance.m_element), m_container(p_instance.m_container), m_allocator(std::move(p_instance.m_allocator))
	{
		p_instance.lose_container();
	}

	template<class baseType, class allocatorType>
	template<class otherAllocatorType>
	inline any_derived<baseType, allocatorType, true>::any_derived(const any_derived<baseType, otherAllocatorType, true>& p_instance, const allocatorType& p_allocator)
		: any_derived(p_allocator)
	{
		assign_value_operation(p_instance);
	}

	template<class baseType, class allocatorType>
	template<class otherAllocatorType>
	inline any_derived<baseType, allocatorType, true>::any_derived(any_derived<baseType, otherAllocatorType, true>&& p_instance, const allocatorType& p_allocator)
		: any_derived(p_allocator)
	{
		assign_value_operation(p_instance);
	}

	template<class baseType, class allocatorType>
	inline any_derived<baseType, allocatorType, true>::~any_derived()
	{
		reset();
	}

	template<class baseType, class allocatorType>
	inline auto any_derived<baseType, allocatorType, true>::operator=(const any_derived& p_rhs) -> any_derived&
	{
		if (!is_same_instance(p_rhs))
		{
			reset();
			if constexpr (allocatorType::propagate_on_container_copy_assignment::value)
			{
				m_allocator = p_rhs.m_allocator;
			}
			set_container(p_rhs.container_clone(get_abstract_allocator()));
		}
		return *this;
	}

	template<class baseType, class allocatorType>
	inline auto any_derived<baseType, allocatorType, true>::operator=(any_derived&& p_rhs) -> any_derived&
	{
		if (!is_same_instance(p_rhs))
		{
			reset();

			if constexpr (allocatorType::propagate_on_container_move_assignment::value)
			{
				m_allocator = std::move(p_rhs.m_allocator);
				m_container = p_rhs.m_container;
				m_element = p_rhs.m_element;
				p_rhs.lose_container();
			}
			else if (allocatorType::is_always_equal::value || m_allocator == p_rhs.m_allocator)
			{
				m_container = p_rhs.m_container;
				m_element = p_rhs.m_element;
				p_rhs.lose_container();
			}
			else if (p_rhs.has_value())
			{
				set_container(p_rhs.container_move(get_abstract_allocator()));
				p_rhs.reset();
			}
		}
		return *this;
	}

	template<class baseType, class allocatorType>
	template<class otherAllocatorType>
	inline auto any_derived<baseType, allocatorType, true>::assign_value(const any_derived<baseType, otherAllocatorType, true>& p_instance) -> any_derived&
	{
		if (!is_same_instance(p_instance))
		{
			assign_value_operation(p_instance);
			JKUTIL_ASSERT(!has_self_reference());
		}
		return *this;
	}

	template<class baseType, class allocatorType>
	template<class otherAllocatorType>
	inline auto any_derived<baseType, allocatorType, true>::assign_value(any_derived<baseType, otherAllocatorType, true>&& p_instance) -> any_derived&
	{
		if (!is_same_instance(p_instance))
		{
			assign_value_operation(p_instance);
			JKUTIL_ASSERT(!has_self_reference());
		}
		return *this;
	}

	template<class baseType, class allocatorType>
	template<class elementType, class ...argumentTypes>
	inline void any_derived<baseType, allocatorType, true>::emplace(argumentTypes&&... p_arguments)
	{
		reset();
		set_container(jkutil::create<cloneable_object_container<elementType, baseType>>(m_allocator, std::forward<argumentTypes>(p_arguments)...));
	}

	template<class baseType, class allocatorType>
	inline void any_derived<baseType, allocatorType, true>::reset()
	{
		if (m_container != nullptr)
		{
			m_container->destroy(get_abstract_allocator());
			lose_container();
		}
	}

	template<class baseType, class allocatorType>
	inline void any_derived<baseType, allocatorType, true>::swap(any_derived& p_instance)
	{
		if (!is_same_instance(p_instance))
		{
			if constexpr (allocatorType::propagate_on_container_swap::value)
			{
				std::swap(m_allocator, p_instance.m_allocator);
				std::swap(m_container, p_instance.m_container);
				std::swap(m_element, p_instance.m_element);
			}
			else if (allocatorType::is_always_equal::value || m_allocator == p_instance.m_allocator)
			{
				std::swap(m_container, p_instance.m_container);
				std::swap(m_element, p_instance.m_element);
			}
			else
			{
				swap_value(p_instance);
			}

			JKUTIL_ASSERT(!has_self_reference());
		}
	}

	template<class baseType, class allocatorType>
	template<class otherAllocatorType>
	inline void any_derived<baseType, allocatorType, true>::swap_value(any_derived<baseType, otherAllocatorType, true>& p_instance)
	{
		if (!is_same_instance(p_instance))
		{
			swap_value_operation(p_instance);
			JKUTIL_ASSERT(!has_self_reference() && !p_instance.has_self_reference());
		}
	}

	template<class baseType, class allocatorType>
	inline bool any_derived<baseType, allocatorType, true>::has_value() const
	{
		return (m_container != nullptr);
	}

	template<class baseType, class allocatorType>
	inline const baseType* any_derived<baseType, allocatorType, true>::get() const
	{
		return m_element;
	}

	template<class baseType, class allocatorType>
	inline baseType* any_derived<baseType, allocatorType, true>::get()
	{
		return m_element;
	}

	template<class baseType, class allocatorType>
	inline size_t any_derived<baseType, allocatorType, true>::size() const
	{
		return (m_container) ? m_container->size() : 0;
	}

	template<class baseType, class allocatorType>
	inline const allocatorType& any_derived<baseType, allocatorType, true>::get_allocator() const
	{
		return m_allocator;
	}

	//--------------------------------//

	template<class baseType, class allocatorType>
	template<class otherAllocatorType>
	inline void any_derived<baseType, allocatorType, true>::assign_value_operation(const any_derived<baseType, otherAllocatorType, true>& p_instance)
	{
		reset();
		set_container(p_instance.container_clone(get_abstract_allocator()));
	}

	template<class baseType, class allocatorType>
	template<class otherAllocatorType>
	inline void any_derived<baseType, allocatorType, true>::assign_value_operation(any_derived<baseType, otherAllocatorType, true>&& p_instance)
	{
		reset();
		set_container(p_instance.container_move(get_abstract_allocator()));
		p_instance.reset();
	}

	template<class baseType, class allocatorType>
	template<class otherAllocatorType>
	inline void any_derived<baseType, allocatorType, true>::swap_value_operation(any_derived<baseType, otherAllocatorType, true>& p_instance)
	{
		auto this_value = container_move(p_instance.get_abstract_allocator());
		auto instance_value = p_instance.container_move(get_abstract_allocator());

		reset();
		p_instance.reset();

		set_container(instance_value);
		p_instance.set_container(this_value);
	}

	template<class baseType, class allocatorType>
	template<class otherAllocatorType>
	inline bool any_derived<baseType, allocatorType, true>::is_same_instance(const any_derived<baseType, otherAllocatorType, true>& p_instance) const
	{
		if constexpr (std::is_same<allocatorType, otherAllocatorType>::value)
		{
			return this != &p_instance;
		}
		else
		{
			return false;
		}
	}

	template<class baseType, class allocatorType>
	inline bool any_derived<baseType, allocatorType, true>::has_self_reference() const
	{
		// Cannot refer to itself. This will only catch self references; it will not detect the equally invalid case of cyclic references (A set of any_deriveds that circularly refer to each other). E.g. A(B(C(A(...)))) where A,B,C,A are any_derived or structures that use any_deriveds.

		std::less_equal<const char*> le_op;
		std::less_equal<const char*> l_op;
		return m_element &&
			le_op(reinterpret_cast<const char*>(m_element), reinterpret_cast<const char*>(this)) &&
			l_op(reinterpret_cast<const char*>(this), reinterpret_cast<const char*>(m_element) + size());
	}

	template<class baseType, class allocatorType>
	inline virtual_allocator_adapter<allocator_pointer<allocatorType>> any_derived<baseType, allocatorType, true>::get_abstract_allocator()
	{
		return virtual_allocator_adapter<allocator_pointer<allocatorType>>(allocator_pointer<allocatorType>(&m_allocator));
	}

	template<class baseType, class allocatorType>
	inline general_cloneable_object_container<baseType>* any_derived<baseType, allocatorType, true>::container_clone(virtual_allocator& p_allocator) const
	{
		return m_container ? m_container->clone(p_allocator) : nullptr;
	}

	template<class baseType, class allocatorType>
	inline general_cloneable_object_container<baseType>* any_derived<baseType, allocatorType, true>::container_move(virtual_allocator& p_allocator)
	{
		return m_container ? m_container->move(p_allocator) : nullptr;
	}

	template<class baseType, class allocatorType>
	inline void any_derived<baseType, allocatorType, true>::set_container(general_cloneable_object_container<baseType>* p_container)
	{
		m_container = p_container;
		if (m_container)
		{
			m_element = m_container->get();
		}
		else
		{
			m_element = nullptr;
		}
	}

	template<class baseType, class allocatorType>
	inline void any_derived<baseType, allocatorType, true>::lose_container()
	{
		m_container = nullptr;
		m_element = nullptr;
	}

}

#endif