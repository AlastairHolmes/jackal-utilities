#include <jkutil\any_derived.h>
#include <jkutil\allocator.h>
#include <jkutil\memory.h>

int main()
{
	jkutil::any_derived<int, jkutil::allocator, true> ty = jkutil::any_derived<int, jkutil::allocator, true>(jkutil::allocator());

	ty.emplace<int>(1);

	return 0;
}