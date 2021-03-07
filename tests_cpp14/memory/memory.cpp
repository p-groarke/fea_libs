#include <fea/memory/memory.hpp>
#include <gtest/gtest.h>
#include <vector>

namespace {
struct cpy {
	cpy() = default;
	cpy(const cpy&) = default;
	cpy(cpy&&) = delete;
};
struct mv {};
TEST(memory, basics) {
	cpy c1;
	static_assert(
			std::is_lvalue_reference<decltype(fea::maybe_move(c1))>::value,
			"memory.cpp : test failed");
	static_assert(
			!std::is_rvalue_reference<decltype(fea::maybe_move(c1))>::value,
			"memory.cpp : test failed");
	mv m1;
	static_assert(
			!std::is_lvalue_reference<decltype(fea::maybe_move(m1))>::value,
			"memory.cpp : test failed");
	static_assert(
			std::is_rvalue_reference<decltype(fea::maybe_move(m1))>::value,
			"memory.cpp : test failed");

	{
		std::vector<cpy> vec(1);
		using cpy_it_t = decltype(vec.begin());
		using mv_it_t = decltype(std::make_move_iterator(vec.begin()));

		auto it = fea::maybe_make_move_iterator(vec.begin());
		static_assert(std::is_same<decltype(it), cpy_it_t>::value,
				"memory.cpp : test failed");
		static_assert(!std::is_same<decltype(it), mv_it_t>::value,
				"memory.cpp : test failed");
	}

	{
		std::vector<mv> vec(1);
		using cpy_it_t = decltype(vec.begin());
		using mv_it_t = decltype(std::make_move_iterator(vec.begin()));

		auto it = fea::maybe_make_move_iterator(vec.begin());
		static_assert(!std::is_same<decltype(it), cpy_it_t>::value,
				"memory.cpp : test failed");
		static_assert(std::is_same<decltype(it), mv_it_t>::value,
				"memory.cpp : test failed");
	}
}
} // namespace
