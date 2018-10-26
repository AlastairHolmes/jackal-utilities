# jackal-utilities
A small set of cross-platform utilities and containers built on the STL. These originally were built-on [EASTL](https://github.com/electronicarts/EASTL), but now use the STL. I will transition functionality as I require it for other projects.

All the memory allocation this library does can be handled by your own custom allocators, similarly to stl e.g. std::vector<int,customAllocatorType>.

## Allocator design
This library uses a style of allocator that allows stateful allocators and has, like STL, the ability to manually control allocator propagation on a per allocator type level, and EASTL's allocator's ease of implementation. The library includes a utility to wrap up allocators that match this library's style and adapt them to STL Allocators (See STL '[Allocator](https://en.cppreference.com/w/cpp/named_req/Allocator)' Named Requirement). The example below shows the library's standard allocator that uses 'malloc()' and 'free()':

```cpp
namespace jkutil
{
	class allocator
	{
	public:

		using propagate_on_container_copy_assignment = std::false_type;
		using propagate_on_container_move_assignment = std::false_type;
		using propagate_on_container_swap = std::false_type;
		using is_always_equal = std::true_type;

		void* allocate(size_t p_size, size_t p_alignment)
		{
			void* memory = jkutil::aligned_malloc(p_size,p_alignment);

			if (!memory)
			{
				throw std::bad_alloc();
			}

			return memory;
		}
		
		void deallocate(void* p_ptr, size_t p_size)
		{
			jkutil::aligned_free(p_ptr, p_size);
		}
		
		bool operator==(const allocator& p_instance) const
		{
			return true;
		}
		
		bool operator!=(const allocator& p_instance) const
		{
			return false;
		}

	};
}
```

## Documentation
This project uses Doxygen to generate its documentation, this can be done by building the project's target named 'Documentation'.

## Containers
Our containers are very similar to STL's and are mostly just wrappers (That adapt the allocator), there are some differences:
	- swap() is not guarenteed to not invalidate iterators. swap() will invalidate iterators if and only if the two container's allocators are non-propagating on swap (propagate_on_container_swap = std::false_type) and not equal. In STL this is undefined behaviour; most implementations will assert in this case. This modification allows proper use of non-propagating statefuls allocators without potentially undefined behaviour. The only down side is the swap() function is no longer noexcept; the allocator can throw during swap, swap will throw.
	- containers have the addition functions swap_value(container_type<otherAllocatorType>&), assign_value(const container_type<otherAllocatorType>&), assign_value(container_type<otherAllocatorType>&&). These function are equivalent to the swap(), copy assignment, and move assignment except they do not propagate the container's allocators even if the allocators set propagation true. Note these function allow swap and assignment between containers of the same template that use different types of allocator (Which cannot be done with STL or EASTL).
	- STL AllocatorAwareContainers have a copy and move constructor:
	```
	container_type(const container_type& p_other, const allocatorType& p_allocator);
	container_type(container_type&& p_other, const allocatorType& p_allocator);
	```
	These do copy and move construction as normal except using the p_allocator you specified rather than the allocator in p_other. We change these to:
	```
	template <class otherAllocatorType>
	container_type(const container_type<otherAllocatorType>& p_other, const allocatorType& p_allocator);	
	template <class otherAllocatorType>
	container_type(container_type<otherAllocatorType>&& p_other, const allocatorType& p_allocator);
	```
	This allows containers to be constructed using containers that have different allocator types (Which cannot be done with STL or EASTL).