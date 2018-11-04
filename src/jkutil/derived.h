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

	template <class baseType, class storableAllocatorType>
	class derived;

	template <class baseType, class storableAllocatorType>
	class derived_copyable;

	/*!
		@brief Used to abstractly move emplace an object into a derived object.
	*/
	template <class baseType>
	class derived_abstract_move_emplacer
	{
	public:

		/*!
			@brief Move an object into \p p_derived.
		*/
		virtual void move_to(derived<baseType, allocator_pointer<virtual_allocator>>& p_derived) = 0;

	};

	/*!
		@brief Used to abstractly copy emplace an object into a derived object.
	*/
	template <class baseType>
	class derived_abstract_copy_emplacer
	{
	public:

		/*!
			@brief Copy an object into \p p_derived.
		*/
		virtual void clone_to(derived<baseType, allocator_pointer<virtual_allocator>>& p_derived) const = 0;

	};

	/*!
		@brief Used to abstractly move emplace an object into a derived_copyable object.
	*/
	template <class baseType>
	class derived_copyable_abstract_move_emplacer
	{
	public:

		/*!
			@brief Move an object into \p p_derived.
		*/
		virtual void move_to(derived_copyable<baseType, allocator_pointer<virtual_allocator>>& p_derived) = 0;

	};

	/*!
		@brief Used to abstractly copy emplace an object into a derived_copyable object.
	*/
	template <class baseType>
	class derived_copyable_abstract_copy_emplacer
	{
	public:

		/*!
			@brief Copy an object into \p p_derived.
		*/
		virtual void clone_to(derived_copyable<baseType, allocator_pointer<virtual_allocator>>& p_derived) const = 0;

	};

	template <class baseType, class storableAllocatorType>
	class derived
	{
	private:

		template <class otherBaseType, class otherStorableAllocatorType>
		friend class derived;

	public:
		
		template <class elementType, class... argumentTypes>
		derived(const storableAllocatorType& p_allocator, std::in_place_type_t<elementType> p_in_place, argumentTypes&&... p_arguments);

		template <class otherStorableAllocatorType>
		derived(derived<baseType, otherStorableAllocatorType>&& p_instance, const storableAllocatorType& p_allocator);

		derived(const storableAllocatorType& p_allocator);

		derived(const derived&) = delete;
		derived(derived&& p_instance);

		~derived();

	public:

		derived& operator=(const derived&) = delete;
		derived& operator=(derived&& p_rhs);

		template <class otherStorableAllocatorType>
		derived& assign_value(derived<baseType, otherStorableAllocatorType>&& p_instance);

		template <class elementType, class... argumentTypes>
		void emplace(argumentTypes&&... p_arguments);

		/*!
			@brief Allows abstract copy emplacement into the derived object (Abstract from the allocator type).
			@see derived_abstract_copy_emplacer
		*/
		void abstract_emplace(const derived_abstract_copy_emplacer<baseType>& p_emplacer);

		/*!
			@brief Allows abstract move emplacement into the derived object (Abstract from the allocator type).
			@see derived_abstract_move_emplacer
		*/
		void abstract_emplace(derived_abstract_move_emplacer<baseType>&& p_emplacer);

		void reset();

		void swap(derived& p_instance);

		template <class otherStorableAllocatorType>
		void swap_value(derived<baseType, otherStorableAllocatorType>& p_instance);

		bool has_value() const;
		const baseType* get() const;
		baseType* get();
		size_t size() const;

		const storableAllocatorType& get_allocator() const;

	private:

		template<class otherStorableAllocatorType>
		void assign_value_operation(derived<baseType, otherStorableAllocatorType>&& p_instance);

		template<class otherStorableAllocatorType>
		void swap_value_operation(derived<baseType, otherStorableAllocatorType>& p_instance);

		template<class otherStorableAllocatorType>
		bool is_same_instance(const derived<baseType, otherStorableAllocatorType>& p_instance) const;

		bool has_self_reference() const;
		virtual_allocator_adapter<allocator_pointer<storableAllocatorType>> get_abstract_allocator();
		jkutil::_jkinternal::general_moveable_object_container<baseType>* container_move(virtual_allocator& p_allocator);
		void set_container(jkutil::_jkinternal::general_moveable_object_container<baseType>* p_container);
		void lose_container();

		baseType* m_element;
		jkutil::_jkinternal::general_moveable_object_container<baseType>* m_container;
		storableAllocatorType m_allocator;

	};

	template<class baseType, class storableAllocatorType>
	template<class elementType, class... argumentTypes>
	inline derived<baseType, storableAllocatorType>::derived(const storableAllocatorType& p_allocator, std::in_place_type_t<elementType> p_in_place, argumentTypes&&... p_arguments)
		: derived(p_allocator)
	{
		p_in_place;
		emplace<elementType>(std::forward<argumentTypes>(p_arguments)...);
	}

	template<class baseType, class storableAllocatorType>
	template<class otherStorableAllocatorType>
	inline derived<baseType, storableAllocatorType>::derived(derived<baseType, otherStorableAllocatorType>&& p_instance, const storableAllocatorType& p_allocator)
		: derived(p_allocator)
	{
		assign_value_operation(p_instance);
	}

	template<class baseType, class storableAllocatorType>
	inline derived<baseType, storableAllocatorType>::derived(const storableAllocatorType& p_allocator)
		: m_element(nullptr), m_container(nullptr), m_allocator(p_allocator)
	{
	}

	template<class baseType, class storableAllocatorType>
	inline derived<baseType, storableAllocatorType>::derived(derived&& p_instance)
		: m_element(p_instance.m_element), m_container(p_instance.m_container), m_allocator(std::move(p_instance.m_allocator))
	{
		p_instance.lose_container();
	}

	template<class baseType, class storableAllocatorType>
	inline derived<baseType, storableAllocatorType>::~derived()
	{
		reset();
	}

	template<class baseType, class storableAllocatorType>
	inline auto derived<baseType, storableAllocatorType>::operator=(derived&& p_rhs) -> derived&
	{
		if (this != &p_rhs)
		{
			reset();

			if constexpr (storableAllocatorType::propagate_on_container_move_assignment::value)
			{
				m_allocator = std::move(p_rhs.m_allocator);
				m_container = p_rhs.m_container;
				m_element = p_rhs.m_element;
				p_rhs.lose_container();
			}
			else if (storableAllocatorType::is_always_equal::value || m_allocator == p_rhs.m_allocator)
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

	template<class baseType, class storableAllocatorType>
	template <class otherStorableAllocatorType>
	auto derived<baseType, storableAllocatorType>::assign_value(derived<baseType, otherStorableAllocatorType>&& p_instance) -> derived&
	{
		if (!is_same_instance(p_instance))
		{
			assign_value_operation(p_instance);
			JKUTIL_ASSERT(!has_self_reference());
		}
		return *this;
	}

	template<class baseType, class storableAllocatorType>
	template<class elementType, class ...argumentTypes>
	inline void derived<baseType, storableAllocatorType>::emplace(argumentTypes&&... p_arguments)
	{
		reset();
		set_container(jkutil::create<jkutil::_jkinternal::moveable_object_container<elementType, baseType>>(m_allocator, std::forward<argumentTypes>(p_arguments)...));
	}

	template<class baseType, class storableAllocatorType>
	inline void derived<baseType, storableAllocatorType>::abstract_emplace(const derived_abstract_copy_emplacer<baseType>& p_emplacer)
	{
		reset();

		auto allocator = get_abstract_allocator();
		derived<baseType, allocator_pointer<virtual_allocator>> temp = derived<baseType, allocator_pointer<virtual_allocator>>(&allocator);
		p_emplacer.clone_to(temp);

		std::swap(m_container, temp.m_container);
		std::swap(m_element, temp.m_element);
	}

	template<class baseType, class storableAllocatorType>
	inline void derived<baseType, storableAllocatorType>::abstract_emplace(derived_abstract_move_emplacer<baseType>&& p_emplacer)
	{
		reset();

		auto allocator = get_abstract_allocator();
		derived<baseType, allocator_pointer<virtual_allocator>> temp = derived<baseType, allocator_pointer<virtual_allocator>>(&allocator);
		p_emplacer.move_to(temp);

		std::swap(m_container, temp.m_container);
		std::swap(m_element, temp.m_element);
	}

	template<class baseType, class storableAllocatorType>
	inline void derived<baseType, storableAllocatorType>::reset()
	{
		if (m_container != nullptr)
		{
			m_container->destroy(get_abstract_allocator());
			lose_container();
		}
	}

	template<class baseType, class storableAllocatorType>
	inline void derived<baseType, storableAllocatorType>::swap(derived& p_instance)
	{
		if (!is_same_instance(p_instance))
		{
			if constexpr(storableAllocatorType::propagate_on_container_swap::value)
			{
				std::swap(m_allocator, p_instance.m_allocator);
				std::swap(m_container, p_instance.m_container);
				std::swap(m_element, p_instance.m_element);
			}
			else if (storableAllocatorType::is_always_equal::value || m_allocator == p_instance.m_allocator)
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

	template<class baseType, class storableAllocatorType>
	template <class otherStorableAllocatorType>
	void derived<baseType, storableAllocatorType>::swap_value(derived<baseType, otherStorableAllocatorType>& p_instance)
	{
		if (!is_same_instance(p_instance))
		{
			swap_value_operation(p_instance);
			JKUTIL_ASSERT(!has_self_reference() && !p_instance.has_self_reference());
		}
	}

	template<class baseType, class storableAllocatorType>
	inline bool derived<baseType, storableAllocatorType>::has_value() const
	{
		return (m_container != nullptr);
	}

	template<class baseType, class storableAllocatorType>
	inline const baseType* derived<baseType, storableAllocatorType>::get() const
	{
		return m_element;
	}

	template<class baseType, class storableAllocatorType>
	inline baseType* derived<baseType, storableAllocatorType>::get()
	{
		return m_element;
	}

	template<class baseType, class storableAllocatorType>
	inline size_t derived<baseType, storableAllocatorType>::size() const
	{
		return (m_container) ? m_container->size() : 0;
	}

	template<class baseType, class storableAllocatorType>
	inline const storableAllocatorType& derived<baseType, storableAllocatorType>::get_allocator() const
	{
		return m_allocator;
	}

	//----------

	template<class baseType, class storableAllocatorType>
	template<class otherStorableAllocatorType>
	inline void derived<baseType, storableAllocatorType>::assign_value_operation(derived<baseType, otherStorableAllocatorType>&& p_instance)
	{
		reset();
		set_container(p_instance.container_move(get_abstract_allocator()));
		p_instance.reset();
	}

	template<class baseType, class storableAllocatorType>
	template<class otherStorableAllocatorType>
	inline void derived<baseType, storableAllocatorType>::swap_value_operation(derived<baseType, otherStorableAllocatorType>& p_instance)
	{
		auto this_value = container_move(p_instance.get_abstract_allocator());
		auto instance_value = p_instance.container_move(get_abstract_allocator());

		reset();
		p_instance.reset();

		set_container(instance_value);
		p_instance.set_container(this_value);
	}

	template<class baseType, class storableAllocatorType>
	template<class otherStorableAllocatorType>
	inline bool derived<baseType, storableAllocatorType>::is_same_instance(const derived<baseType, otherStorableAllocatorType>& p_instance) const
	{
		if constexpr (std::is_same<storableAllocatorType, otherStorableAllocatorType>::value)
		{
			return this == &p_instance;
		}
		else
		{
			return false;
		}
	}

	template<class baseType, class storableAllocatorType>
	inline bool derived<baseType, storableAllocatorType>::has_self_reference() const
	{
		// Cannot refer to itself. This will only catch self references; it will not detect the equally invalid case of cyclic references (A set of deriveds that circularly refer to each other). E.g. A(B(C(A(...)))) where A,B,C,A are derived or structures that use deriveds.

		std::less_equal<const char*> le_op;
		std::less_equal<const char*> l_op;
		return m_element &&
			le_op(reinterpret_cast<const char*>(m_element), reinterpret_cast<const char*>(this)) &&
			l_op(reinterpret_cast<const char*>(this), reinterpret_cast<const char*>(m_element) + size());
	}

	template<class baseType, class storableAllocatorType>
	inline virtual_allocator_adapter<allocator_pointer<storableAllocatorType>> derived<baseType, storableAllocatorType>::get_abstract_allocator()
	{
		return virtual_allocator_adapter<allocator_pointer<storableAllocatorType>>(allocator_pointer<storableAllocatorType>(&m_allocator));
	}

	template<class baseType, class storableAllocatorType>
	inline jkutil::_jkinternal::general_moveable_object_container<baseType>* derived<baseType, storableAllocatorType>::container_move(virtual_allocator& p_allocator)
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

	template<class baseType, class storableAllocatorType>
	inline void derived<baseType, storableAllocatorType>::set_container(jkutil::_jkinternal::general_moveable_object_container<baseType>* p_container)
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
	
	template<class baseType, class storableAllocatorType>
	inline void derived<baseType, storableAllocatorType>::lose_container()
	{
		m_container = nullptr;
		m_element = nullptr;
	}

	//----------------------------------------------------//

	template <class baseType, class storableAllocatorType>
	class derived_copyable
	{
	private:

		template <class otherBaseType, class otherStorableAllocatorType>
		friend class derived_copyable;

	public:

		derived_copyable(const storableAllocatorType& p_allocator);

		template <class elementType, class... argumentTypes>
		derived_copyable(const storableAllocatorType& p_allocator, std::in_place_type_t<elementType> p_in_place, argumentTypes&&... p_arguments);

		derived_copyable(const derived_copyable& p_instance);
		derived_copyable(derived_copyable&& p_instance);

		template <class otherStorableAllocatorType>
		derived_copyable(const derived_copyable<baseType, otherStorableAllocatorType>& p_instance, const storableAllocatorType& p_allocator);

		template <class otherStorableAllocatorType>
		derived_copyable(derived_copyable<baseType, otherStorableAllocatorType>&& p_instance, const storableAllocatorType& p_allocator);

		~derived_copyable();

	public:

		derived_copyable& operator=(const derived_copyable& p_rhs);
		derived_copyable& operator=(derived_copyable&& p_rhs);

		template <class otherStorableAllocatorType>
		derived_copyable& assign_value(const derived_copyable<baseType, otherStorableAllocatorType>& p_instance);
		template <class otherStorableAllocatorType>
		derived_copyable& assign_value(derived_copyable<baseType, otherStorableAllocatorType>&& p_instance);

		void swap(derived_copyable& p_instance);
		template <class otherStorableAllocatorType>
		void swap_value(derived_copyable<baseType, otherStorableAllocatorType>& p_instance);

	public:

		template <class elementType, class... argumentTypes>
		void emplace(argumentTypes&&... p_arguments);

		void abstract_emplace(const derived_copyable_abstract_copy_emplacer<baseType>& p_emplacer);
		void abstract_emplace(derived_copyable_abstract_move_emplacer<baseType>&& p_emplacer);

		void reset();

		bool has_value() const;
		const baseType* get() const;
		baseType* get();
		size_t size() const;

		const storableAllocatorType& get_allocator() const;

	private:

		template<class otherStorableAllocatorType>
		void assign_value_operation(const derived_copyable<baseType, otherStorableAllocatorType>& p_instance);

		template<class otherStorableAllocatorType>
		void assign_value_operation(derived_copyable<baseType, otherStorableAllocatorType>&& p_instance);

		template<class otherStorableAllocatorType>
		void swap_value_operation(derived_copyable<baseType, otherStorableAllocatorType>& p_instance);

		template<class otherStorableAllocatorType>
		bool is_same_instance(const derived_copyable<baseType, otherStorableAllocatorType>& p_instance) const;

		bool has_self_reference() const;
		virtual_allocator_adapter<allocator_pointer<storableAllocatorType>> get_abstract_allocator();
		jkutil::_jkinternal::general_cloneable_object_container<baseType>* container_clone(virtual_allocator& p_allocator) const;
		jkutil::_jkinternal::general_cloneable_object_container<baseType>* container_move(virtual_allocator& p_allocator);
		void set_container(jkutil::_jkinternal::general_cloneable_object_container<baseType>* p_container);
		void lose_container();

		baseType* m_element;
		jkutil::_jkinternal::general_cloneable_object_container<baseType>* m_container;
		storableAllocatorType m_allocator;

	};

	template<class baseType, class storableAllocatorType>
	template<class elementType, class... argumentTypes>
	inline derived_copyable<baseType, storableAllocatorType>::derived_copyable(const storableAllocatorType& p_allocator, std::in_place_type_t<elementType> p_in_place, argumentTypes&&... p_arguments)
		: derived_copyable(p_allocator)
	{
		p_in_place;
		emplace<elementType>(std::forward<argumentTypes>(p_arguments)...);
	}

	template<class baseType, class storableAllocatorType>
	inline derived_copyable<baseType, storableAllocatorType>::derived_copyable(const storableAllocatorType& p_allocator)
		: m_element(nullptr), m_container(nullptr), m_allocator(p_allocator)
	{
	}

	template<class baseType, class storableAllocatorType>
	inline derived_copyable<baseType, storableAllocatorType>::derived_copyable(const derived_copyable& p_instance)
		: derived_copyable(p_instance.m_allocator)
	{
		set_container(p_instance.container_clone(get_abstract_allocator()));
	}

	template<class baseType, class storableAllocatorType>
	inline derived_copyable<baseType, storableAllocatorType>::derived_copyable(derived_copyable&& p_instance)
		: m_element(p_instance.m_element), m_container(p_instance.m_container), m_allocator(std::move(p_instance.m_allocator))
	{
		p_instance.lose_container();
	}

	template<class baseType, class storableAllocatorType>
	template<class otherStorableAllocatorType>
	inline derived_copyable<baseType, storableAllocatorType>::derived_copyable(const derived_copyable<baseType, otherStorableAllocatorType>& p_instance, const storableAllocatorType& p_allocator)
		: derived_copyable(p_allocator)
	{
		assign_value_operation(p_instance);
	}

	template<class baseType, class storableAllocatorType>
	template<class otherStorableAllocatorType>
	inline derived_copyable<baseType, storableAllocatorType>::derived_copyable(derived_copyable<baseType, otherStorableAllocatorType>&& p_instance, const storableAllocatorType& p_allocator)
		: derived_copyable(p_allocator)
	{
		assign_value_operation(p_instance);
	}

	template<class baseType, class storableAllocatorType>
	inline derived_copyable<baseType, storableAllocatorType>::~derived_copyable()
	{
		reset();
	}

	template<class baseType, class storableAllocatorType>
	inline auto derived_copyable<baseType, storableAllocatorType>::operator=(const derived_copyable& p_rhs) -> derived_copyable&
	{
		if (!is_same_instance(p_rhs))
		{
			reset();
			if constexpr (storableAllocatorType::propagate_on_container_copy_assignment::value)
			{
				m_allocator = p_rhs.m_allocator;
			}
			set_container(p_rhs.container_clone(get_abstract_allocator()));
		}
		return *this;
	}

	template<class baseType, class storableAllocatorType>
	inline auto derived_copyable<baseType, storableAllocatorType>::operator=(derived_copyable&& p_rhs) -> derived_copyable&
	{
		if (!is_same_instance(p_rhs))
		{
			reset();

			if constexpr (storableAllocatorType::propagate_on_container_move_assignment::value)
			{
				m_allocator = std::move(p_rhs.m_allocator);
				m_container = p_rhs.m_container;
				m_element = p_rhs.m_element;
				p_rhs.lose_container();
			}
			else if (storableAllocatorType::is_always_equal::value || m_allocator == p_rhs.m_allocator)
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

	template<class baseType, class storableAllocatorType>
	template<class otherStorableAllocatorType>
	inline auto derived_copyable<baseType, storableAllocatorType>::assign_value(const derived_copyable<baseType, otherStorableAllocatorType>& p_instance) -> derived_copyable&
	{
		if (!is_same_instance(p_instance))
		{
			assign_value_operation(p_instance);
			JKUTIL_ASSERT(!has_self_reference());
		}
		return *this;
	}

	template<class baseType, class storableAllocatorType>
	template<class otherStorableAllocatorType>
	inline auto derived_copyable<baseType, storableAllocatorType>::assign_value(derived_copyable<baseType, otherStorableAllocatorType>&& p_instance) -> derived_copyable&
	{
		if (!is_same_instance(p_instance))
		{
			assign_value_operation(p_instance);
			JKUTIL_ASSERT(!has_self_reference());
		}
		return *this;
	}

	template<class baseType, class storableAllocatorType>
	template<class elementType, class ...argumentTypes>
	inline void derived_copyable<baseType, storableAllocatorType>::emplace(argumentTypes&&... p_arguments)
	{
		reset();
		set_container(jkutil::create<jkutil::_jkinternal::cloneable_object_container<elementType, baseType>>(m_allocator, std::forward<argumentTypes>(p_arguments)...));
	}

	template<class baseType, class storableAllocatorType>
	inline void derived_copyable<baseType, storableAllocatorType>::abstract_emplace(const derived_copyable_abstract_copy_emplacer<baseType>& p_emplacer)
	{
		reset();

		auto allocator = get_abstract_allocator();
		derived_copyable<baseType, allocator_pointer<virtual_allocator>> temp = derived_copyable<baseType, allocator_pointer<virtual_allocator>>(&allocator);
		p_emplacer.clone_to(temp);

		std::swap(m_container, temp.m_container);
		std::swap(m_element, temp.m_element);
	}

	template<class baseType, class storableAllocatorType>
	inline void derived_copyable<baseType, storableAllocatorType>::abstract_emplace(derived_copyable_abstract_move_emplacer<baseType>&& p_emplacer)
	{
		reset();

		auto allocator = get_abstract_allocator();
		derived_copyable<baseType, allocator_pointer<virtual_allocator>> temp = derived_copyable<baseType, allocator_pointer<virtual_allocator>>(&allocator);
		p_emplacer.move_to(temp);

		std::swap(m_container, temp.m_container);
		std::swap(m_element, temp.m_element);
	}

	template<class baseType, class storableAllocatorType>
	inline void derived_copyable<baseType, storableAllocatorType>::reset()
	{
		if (m_container != nullptr)
		{
			m_container->destroy(get_abstract_allocator());
			lose_container();
		}
	}

	template<class baseType, class storableAllocatorType>
	inline void derived_copyable<baseType, storableAllocatorType>::swap(derived_copyable& p_instance)
	{
		if (!is_same_instance(p_instance))
		{
			if constexpr (storableAllocatorType::propagate_on_container_swap::value)
			{
				std::swap(m_allocator, p_instance.m_allocator);
				std::swap(m_container, p_instance.m_container);
				std::swap(m_element, p_instance.m_element);
			}
			else if (storableAllocatorType::is_always_equal::value || m_allocator == p_instance.m_allocator)
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

	template<class baseType, class storableAllocatorType>
	template<class otherStorableAllocatorType>
	inline void derived_copyable<baseType, storableAllocatorType>::swap_value(derived_copyable<baseType, otherStorableAllocatorType>& p_instance)
	{
		if (!is_same_instance(p_instance))
		{
			swap_value_operation(p_instance);
			JKUTIL_ASSERT(!has_self_reference() && !p_instance.has_self_reference());
		}
	}

	template<class baseType, class storableAllocatorType>
	inline bool derived_copyable<baseType, storableAllocatorType>::has_value() const
	{
		return (m_container != nullptr);
	}

	template<class baseType, class storableAllocatorType>
	inline const baseType* derived_copyable<baseType, storableAllocatorType>::get() const
	{
		return m_element;
	}

	template<class baseType, class storableAllocatorType>
	inline baseType* derived_copyable<baseType, storableAllocatorType>::get()
	{
		return m_element;
	}

	template<class baseType, class storableAllocatorType>
	inline size_t derived_copyable<baseType, storableAllocatorType>::size() const
	{
		return (m_container) ? m_container->size() : 0;
	}

	template<class baseType, class storableAllocatorType>
	inline const storableAllocatorType& derived_copyable<baseType, storableAllocatorType>::get_allocator() const
	{
		return m_allocator;
	}

	//--------------------------------//

	template<class baseType, class storableAllocatorType>
	template<class otherStorableAllocatorType>
	inline void derived_copyable<baseType, storableAllocatorType>::assign_value_operation(const derived_copyable<baseType, otherStorableAllocatorType>& p_instance)
	{
		reset();
		set_container(p_instance.container_clone(get_abstract_allocator()));
	}

	template<class baseType, class storableAllocatorType>
	template<class otherStorableAllocatorType>
	inline void derived_copyable<baseType, storableAllocatorType>::assign_value_operation(derived_copyable<baseType, otherStorableAllocatorType>&& p_instance)
	{
		reset();
		set_container(p_instance.container_move(get_abstract_allocator()));
		p_instance.reset();
	}

	template<class baseType, class storableAllocatorType>
	template<class otherStorableAllocatorType>
	inline void derived_copyable<baseType, storableAllocatorType>::swap_value_operation(derived_copyable<baseType, otherStorableAllocatorType>& p_instance)
	{
		auto this_value = container_move(p_instance.get_abstract_allocator());
		auto instance_value = p_instance.container_move(get_abstract_allocator());

		reset();
		p_instance.reset();

		set_container(instance_value);
		p_instance.set_container(this_value);
	}

	template<class baseType, class storableAllocatorType>
	template<class otherStorableAllocatorType>
	inline bool derived_copyable<baseType, storableAllocatorType>::is_same_instance(const derived_copyable<baseType, otherStorableAllocatorType>& p_instance) const
	{
		if constexpr (std::is_same<storableAllocatorType, otherStorableAllocatorType>::value)
		{
			return this == &p_instance;
		}
		else
		{
			return false;
		}
	}

	template<class baseType, class storableAllocatorType>
	inline bool derived_copyable<baseType, storableAllocatorType>::has_self_reference() const
	{
		// Cannot refer to itself. This will only catch self references; it will not detect the equally invalid case of cyclic references (A set of derived_copyables that circularly refer to each other). E.g. A(B(C(A(...)))) where A,B,C,A are derived_copyable or structures that use derived_copyables.

		std::less_equal<const char*> le_op;
		std::less_equal<const char*> l_op;
		return m_element &&
			le_op(reinterpret_cast<const char*>(m_element), reinterpret_cast<const char*>(this)) &&
			l_op(reinterpret_cast<const char*>(this), reinterpret_cast<const char*>(m_element) + size());
	}

	template<class baseType, class storableAllocatorType>
	inline virtual_allocator_adapter<allocator_pointer<storableAllocatorType>> derived_copyable<baseType, storableAllocatorType>::get_abstract_allocator()
	{
		return virtual_allocator_adapter<allocator_pointer<storableAllocatorType>>(allocator_pointer<storableAllocatorType>(&m_allocator));
	}

	template<class baseType, class storableAllocatorType>
	inline jkutil::_jkinternal::general_cloneable_object_container<baseType>* derived_copyable<baseType, storableAllocatorType>::container_clone(virtual_allocator& p_allocator) const
	{
		return m_container ? m_container->clone(p_allocator) : nullptr;
	}

	template<class baseType, class storableAllocatorType>
	inline jkutil::_jkinternal::general_cloneable_object_container<baseType>* derived_copyable<baseType, storableAllocatorType>::container_move(virtual_allocator& p_allocator)
	{
		return m_container ? m_container->move(p_allocator) : nullptr;
	}

	template<class baseType, class storableAllocatorType>
	inline void derived_copyable<baseType, storableAllocatorType>::set_container(jkutil::_jkinternal::general_cloneable_object_container<baseType>* p_container)
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

	template<class baseType, class storableAllocatorType>
	inline void derived_copyable<baseType, storableAllocatorType>::lose_container()
	{
		m_container = nullptr;
		m_element = nullptr;
	}

}

#endif