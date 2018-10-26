#ifndef JKUTIL_MEMORY_H
#define JKUTIL_MEMORY_H

namespace jkutil
{

	void* aligned_malloc(size_t p_size, size_t p_alignment);
	void aligned_free(void* p_ptr, size_t p_size);

	template <class allocatorType>
	inline void* memory_allocate(allocatorType& p_allocator, size_t p_size, size_t p_alignment)
	{
		return p_allocator.allocate(p_size, p_alignment);
	}

	template <class allocatorType>
	inline void memory_deallocate(allocatorType& p_allocator, void* p_memory, size_t p_size)
	{
		p_allocator.deallocate(p_memory, p_size);
	}

	template <class constructType, class... argumentTypes>
	inline constructType* construct(void* p_memory, argumentTypes&&... p_arguments)
	{
		return new (p_memory) constructType(std::forward<argumentTypes>(p_arguments)...);
	}

	template <class deconstructType>
	inline void destruct(deconstructType* p_object)
	{
		p_object->~deconstructType();
	}

	template <class allocatorType>
	class deallocate_guard
	{
	public:

		deallocate_guard(allocatorType& p_allocator, void* p_memory, size_t p_size, bool p_enabled = true);

		deallocate_guard(const deallocate_guard&) = delete;
		deallocate_guard(deallocate_guard&& p_instance);

		deallocate_guard& operator=(const deallocate_guard&) = delete;
		deallocate_guard& operator=(deallocate_guard&&) = delete;

		~deallocate_guard();

		void disable();
		void enable();

	private:

		void run();

		bool m_run;
		bool m_enabled;
		allocatorType& m_allocator;

	protected:

		allocatorType& get_allocator();

		void* m_memory;
		size_t m_size;
	};

	template<class allocatorType>
	inline deallocate_guard<allocatorType>::deallocate_guard(allocatorType& p_allocator, void* p_memory, size_t p_size, bool p_enabled)
		: m_run(false),
		m_enabled(p_enabled),
		m_memory(p_memory),
		m_size(p_size),
		m_allocator(p_allocator)
	{
	}

	template<class allocatorType>
	inline deallocate_guard<allocatorType>::deallocate_guard(deallocate_guard&& p_instance)
		: m_run(p_instance.m_run),
		m_enabled(p_instance.m_enabled),
		m_memory(p_instance.m_memory),
		m_size(p_instance.m_size),
		m_allocator(p_instance.m_allocator)
	{
		p_instance.m_run = true;
		p_instance.m_active = false;
		p_instance.m_memory = nullptr;
		p_instance.m_size = 0;
	}

	template<class allocatorType>
	inline deallocate_guard<allocatorType>::~deallocate_guard()
	{
		run();
	}

	template<class allocatorType>
	inline void deallocate_guard<allocatorType>::disable()
	{
		m_enabled = false;
	}

	template<class allocatorType>
	inline void deallocate_guard<allocatorType>::enable()
	{
		m_enabled = true;
	}

	template<class allocatorType>
	inline allocatorType& deallocate_guard<allocatorType>::get_allocator()
	{
		return m_allocator;
	}

	template<class allocatorType>
	inline void deallocate_guard<allocatorType>::run()
	{
		if (m_enabled && !m_run)
		{
			memory_deallocate(m_allocator, m_memory, m_size);
			m_enabled = false;
		}
		m_run = true;
	}

	template <class allocatorType>
	class allocate_guard : public deallocate_guard<allocatorType>
	{
	public:

		allocate_guard(allocatorType& p_allocator, size_t p_size, size_t p_alignment, bool p_enabled = true);

		void* data() const;
		size_t size() const;

	};

	template<class allocatorType>
	inline allocate_guard<allocatorType>::allocate_guard(allocatorType& p_allocator, size_t p_size, size_t p_alignment, bool p_enabled)
		: deallocate_guard<allocatorType>(p_allocator, nullptr, p_size, false)
	{
		m_memory = memory_allocate(p_allocator, p_size, p_alignment);
		if (p_enabled)
		{
			enable();
		}
	}

	template<class allocatorType>
	inline void* allocate_guard<allocatorType>::data() const
	{
		return m_memory;
	}

	template<class allocatorType>
	inline size_t allocate_guard<allocatorType>::size() const
	{
		return m_size;
	}

	template <class constructType, class allocatorType, class... argumentTypes>
	inline constructType* create(allocatorType& p_allocator, argumentTypes&&... p_arguments)
	{
		allocate_guard<allocatorType> memory = allocate_guard<allocatorType>(p_allocator, sizeof(constructType), alignof(constructType));
		jkutil::construct<constructType>(memory.data(), std::forward<argumentTypes>(p_arguments)...);
		memory.disable();
		return static_cast<constructType*>(memory.data());
	}

	template <class deconstructType, class allocatorType>
	inline void destroy(allocatorType& p_allocator, deconstructType* p_element)
	{
		deallocate_guard<allocatorType> guard(p_allocator, p_element, sizeof(deconstructType));
		jkutil::destruct<deconstructType>(p_element);
	}

}

#endif