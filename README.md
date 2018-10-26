# jackal-utilities
A small set of utilities built on the STL. These originally where built-on [EASTL](https://github.com/electronicarts/EASTL), but now use the STL. I will transition functionality as I require it for other projects.

## Allocator design
This library uses a style of allocator that allows stateful allocators and has ability of STL to manually control allocator propagation on a per allocator type level, and the simplicity of implementation of EASTL. The example below shows the libraries standard allocator that uses 'malloc()' and 'free()':

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