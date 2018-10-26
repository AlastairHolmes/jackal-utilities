/*!
	@file		object_container.h
	@author		Alastair Holmes
	@date		26/10/2018
 */


#ifndef JKUTIL_OBJECT_CONTAINER_H
#define JKUTIL_OBJECT_CONTAINER_H

#include <jkutil\allocator.h>
#include <jkutil\memory.h>
#include <jkutil\type_traits.h>

namespace jkutil
{

	template <class baseType>
	class general_object_container
	{
	public:

		virtual const baseType* get() const = 0;
		virtual baseType* get() = 0;
		virtual size_t size() const = 0;

		virtual void destroy(virtual_allocator& p_allocator) = 0;

	};

	//----------------------//

	template <class elementType, class baseType>
	class object_container : public general_object_container<baseType>
	{
	public:

		static_assert(
			!std::is_reference<elementType>::value &&
			std::is_destructible<elementType>::value);

	public:

		template <class... argumentTypes>
		object_container(argumentTypes&&... p_arguments);

		object_container(const object_container&) = delete;
		object_container(object_container&&) = delete;

		object_container& operator=(const object_container&) = delete;
		object_container& operator=(object_container&&) = delete;

		~object_container() = default;

	public:

		virtual const baseType* get() const override final;
		virtual baseType* get() override final;
		virtual size_t size() const override final;

		virtual void destroy(virtual_allocator& p_allocator) override final;

	private:

		elementType m_element;

	};

	template<class elementType, class baseType>
	template<class ...argumentTypes>
	inline object_container<elementType, baseType>::object_container(argumentTypes&&... p_arguments)
		: m_element(std::forward<argumentTypes>(p_arguments)...)
	{
	}

	template<class elementType, class baseType>
	inline const baseType* object_container<elementType, baseType>::get() const
	{
		return static_cast<const baseType*>(&m_element);
	}

	template<class elementType, class baseType>
	inline baseType* object_container<elementType, baseType>::get()
	{
		return static_cast<baseType*>(&m_element);
	}

	template<class elementType, class baseType>
	inline size_t object_container<elementType, baseType>::size() const
	{
		return sizeof(elementType);
	}

	template<class elementType, class baseType>
	inline void object_container<elementType, baseType>::destroy(virtual_allocator& p_allocator)
	{
		jkutil::destroy<object_container<elementType, baseType>>(p_allocator, this);
	}
	//-----------------------------//

	template <class baseType>
	class general_moveable_object_container : public general_object_container<baseType>
	{
	public:

		virtual general_moveable_object_container* move(virtual_allocator& p_allocator) = 0;

	};

	//-----------------------------//

	template <class elementType, class baseType>
	class moveable_object_container : public general_moveable_object_container<baseType>
	{
	public:

		static_assert(
			!std::is_reference<elementType>::value &&
			std::is_move_constructible<elementType>::value &&
			std::is_destructible<elementType>::value);

	public:

		template <class... argumentTypes>
		moveable_object_container(argumentTypes&&... p_arguments);

		moveable_object_container(const moveable_object_container&) = delete;
		moveable_object_container(moveable_object_container&&) = delete;

		moveable_object_container& operator=(const moveable_object_container&) = delete;
		moveable_object_container& operator=(moveable_object_container&&) = delete;

		~moveable_object_container() = default;

	public:

		virtual const baseType* get() const override final;
		virtual baseType* get() override final;
		virtual size_t size() const override final;

		virtual void destroy(virtual_allocator& p_allocator) override final;

		virtual general_moveable_object_container<baseType>* move(virtual_allocator& p_allocator) override final;

	private:

		elementType m_element;

	};

	template<class elementType, class baseType>
	template<class... argumentTypes>
	inline moveable_object_container<elementType, baseType>::moveable_object_container(argumentTypes&&... p_arguments)
		: m_element(std::forward<argumentTypes>(p_arguments)...)
	{
	}

	template<class elementType, class baseType>
	inline const baseType * moveable_object_container<elementType, baseType>::get() const
	{
		return static_cast<const baseType*>(&m_element);
	}

	template<class elementType, class baseType>
	inline baseType * moveable_object_container<elementType, baseType>::get()
	{
		return static_cast<baseType*>(&m_element);
	}

	template<class elementType, class baseType>
	inline size_t moveable_object_container<elementType, baseType>::size() const
	{
		return sizeof(elementType);
	}

	template<class elementType, class baseType>
	inline void moveable_object_container<elementType, baseType>::destroy(virtual_allocator & p_allocator)
	{
		jkutil::destroy<moveable_object_container<elementType, baseType>>(p_allocator, this);
	}

	template<class elementType, class baseType>
	inline general_moveable_object_container<baseType>* moveable_object_container<elementType, baseType>::move(virtual_allocator& p_allocator)
	{
		return jkutil::create<moveable_object_container<elementType, baseType>>(p_allocator, std::move(m_element));
	}

	//-----------------------------//

	template <class baseType>
	class general_cloneable_object_container : public general_moveable_object_container<baseType>
	{
	public:

		virtual general_cloneable_object_container* clone(virtual_allocator& p_allocator) const = 0;

	};

	//-------------------------------//

	template <class elementType, class baseType>
	class cloneable_object_container : public general_cloneable_object_container<baseType>
	{
	public:

		static_assert(
			!std::is_reference<elementType>::value &&
			std::is_move_constructible<elementType>::value &&
			std::is_copy_constructible<elementType>::value &&
			std::is_destructible<elementType>::value);

	public:

		template <class... argumentTypes>
		cloneable_object_container(argumentTypes&&... p_arguments);

		cloneable_object_container(const cloneable_object_container&) = delete;
		cloneable_object_container(cloneable_object_container&&) = delete;

		cloneable_object_container& operator=(const cloneable_object_container&) = delete;
		cloneable_object_container& operator=(cloneable_object_container&&) = delete;

		~cloneable_object_container() = default;

	public:

		virtual const baseType* get() const override final;
		virtual baseType* get() override final;
		virtual size_t size() const override final;

		virtual void destroy(virtual_allocator& p_allocator) override final;

		virtual general_cloneable_object_container<baseType>* clone(virtual_allocator& p_allocator) const override final;
		virtual general_cloneable_object_container<baseType>* move(virtual_allocator& p_allocator) override final;

	private:

		elementType m_element;
	};

	template<class elementType, class baseType>
	template<class... argumentTypes>
	inline cloneable_object_container<elementType, baseType>::cloneable_object_container(argumentTypes&&... p_arguments)
		: m_element(std::forward<argumentTypes>(p_arguments)...)
	{
	}

	template<class elementType, class baseType>
	inline const baseType* cloneable_object_container<elementType, baseType>::get() const
	{
		return static_cast<const baseType*>(&m_element);
	}

	template<class elementType, class baseType>
	inline baseType* cloneable_object_container<elementType, baseType>::get()
	{
		return static_cast<baseType*>(&m_element);
	}

	template<class elementType, class baseType>
	inline size_t cloneable_object_container<elementType, baseType>::size() const
	{
		return sizeof(elementType);
	}

	template<class elementType, class baseType>
	inline void cloneable_object_container<elementType, baseType>::destroy(virtual_allocator& p_allocator)
	{
		jkutil::destroy<cloneable_object_container<elementType, baseType>>(p_allocator, this);
	}

	template<class elementType, class baseType>
	inline general_cloneable_object_container<baseType>* cloneable_object_container<elementType, baseType>::clone(virtual_allocator& p_allocator) const
	{
		return jkutil::create<cloneable_object_container<elementType, baseType>>(p_allocator, m_element);
	}

	template<class elementType, class baseType>
	inline general_cloneable_object_container<baseType>* cloneable_object_container<elementType, baseType>::move(virtual_allocator& p_allocator)
	{
		return jkutil::create<cloneable_object_container<elementType, baseType>>(p_allocator, std::move(m_element));
	}

	//-------------------------------//

	template <class baseType>
	class general_self_allocated_object_container
	{
	public:

		~general_self_allocated_object_container() = default;

	public:

		virtual const baseType* get() const = 0;
		virtual baseType* get() = 0;
		virtual size_t size() const = 0;

		virtual void destroy() = 0;

	};

	template <class elementType, class allocatorType, class baseType>
	class self_allocated_object_container : public general_self_allocated_object_container<baseType>
	{
	public:

		static_assert(
			!std::is_reference<elementType>::value &&
			std::is_destructible<elementType>::value);

		template <class... argumentTypes>
		self_allocated_object_container(const allocatorType& p_allocator, argumentTypes&&... p_arguments);

		self_allocated_object_container(const self_allocated_object_container<elementType, allocatorType, baseType>&) = delete;
		self_allocated_object_container(self_allocated_object_container<elementType, allocatorType, baseType>&&) = delete;

		self_allocated_object_container& operator=(const self_allocated_object_container&) = delete;
		self_allocated_object_container& operator=(self_allocated_object_container&&) = delete;

	public:

		virtual const baseType* get() const override final;
		virtual baseType* get() override final;

		virtual size_t size() const override final;

		virtual void destroy() override final;

	private:

		elementType m_element;
		allocatorType m_allocator;

	};

	template<class elementType, class allocatorType, class baseType>
	template<class ...argumentTypes>
	inline self_allocated_object_container<elementType, allocatorType, baseType>::self_allocated_object_container(const allocatorType& p_allocator, argumentTypes&& ...p_arguments)
		: m_allocator(p_allocator), m_element(std::forward<argumentTypes>(p_arguments)...)
	{
	}

	template<class elementType, class allocatorType, class baseType>
	inline const baseType* self_allocated_object_container<elementType, allocatorType, baseType>::get() const
	{
		return static_cast<const baseType*>(&m_element);
	}

	template<class elementType, class allocatorType, class baseType>
	inline baseType* self_allocated_object_container<elementType, allocatorType, baseType>::get()
	{
		return static_cast<baseType*>(&m_element);
	}

	template<class elementType, class allocatorType, class baseType>
	inline size_t self_allocated_object_container<elementType, allocatorType, baseType>::size() const
	{
		return sizeof(elementType);
	}

	template<class elementType, class allocatorType, class baseType>
	inline void self_allocated_object_container<elementType, allocatorType, baseType>::destroy()
	{
		allocatorType allocator(std::move(m_allocator));
		jkutil::destroy<self_allocated_object_container<elementType, allocatorType, baseType>>(allocator, this);
	}

}

#endif