#include <jkutil\derived.h>
#include <jkutil\allocator.h>
#include <jkutil\memory.h>
#include <jkutil\vector.h>
#include <jkutil\selective_iterator.h>
#include <jkutil\transform_iterator.h>
#include <jkutil\zip.h>
#include <iostream>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

/*
int main()
{
	jkutil::derived_copyable<int, jkutil::allocator> ty = jkutil::derived_copyable<int, jkutil::allocator>(jkutil::allocator());

	ty.emplace<int>(1);
	ty.reset();

	ty.emplace<int32_t>(34);


	std::vector<int, jkutil::allocator_stl_adapter<int, jkutil::allocator>> v = std::vector<int, jkutil::allocator_stl_adapter<int, jkutil::allocator>>(jkutil::allocator_stl_adapter<int, jkutil::allocator>(jkutil::allocator()));

	jkutil::vector<int, jkutil::allocator> g = jkutil::vector<int, jkutil::allocator>(jkutil::allocator());
	jkutil::vector<int, jkutil::allocator> h = jkutil::vector<int, jkutil::allocator>(jkutil::allocator());

	h.push_back(1);
	h.push_back(9);
	h.push_back(10);
	h.push_back(9);
	h.push_back(1);

	g.push_back(7);
	g.push_back(8);
	g.push_back(9);

	auto x = [](const int& a)
	{
		return a == 9;
	};

	auto k = jkutil::make_transform_iterator(h.begin(), x);
	auto k1 = jkutil::make_transform_iterator(h.end(), x);

	auto z = jkutil::make_zip_range(h, jkutil::make_iterator_range(k, k1));

	for (auto i : z)
	{
		std::cout << std::get<0>(i) << " " << std::get<1>(i) << std::endl;
	}

	for (auto i : jkutil::make_iterator_range(k, k1))
	{
		std::cout << "Hello" << i <<  std::endl;
	}

	return 0;
}
*/