#include <array>
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

TEST(memory, destroy) {
	static int num_dtors = 0;
	struct obj {
		~obj() {
			++num_dtors;
		}
	};

	// Just check these compile.
	{
		int i = 42;
		fea::destroy_at(&i);
		int arr[4] = { 0, 0, 0, 0 };
		fea::destroy_at(&arr);
		fea::destroy(std::begin(arr), std::end(arr));
	}

	{
		obj o;
		fea::destroy_at(&o);
		EXPECT_EQ(num_dtors, 1);
	}
	EXPECT_EQ(num_dtors, 2);

	{
		obj arr[4] = { {}, {}, {}, {} };
		fea::destroy_at(&arr);
		EXPECT_EQ(num_dtors, 6);
	}

	{
		obj arr[4] = { {}, {}, {}, {} };
		fea::destroy(std::begin(arr), std::end(arr));
		EXPECT_EQ(num_dtors, 14);
	}
}

TEST(memory, copy_or_move) {
	// Check it compiles.
	{
		int i = 42;
		int o = 0;
		fea::copy_or_move(&i, &i + 1, &o);
		EXPECT_EQ(o, 42);
	}

	{
		static int num_ctors = 0;
		static int num_dtors = 0;
		static int num_cpy = 0;
		static int num_mv = 0;

		struct obj {
			obj() {
				++num_ctors;
			}

			~obj() {
				++num_dtors;
			}

			obj(const obj&) {
				++num_cpy;
			}
			obj(obj&&) noexcept {
				++num_mv;
			}

			obj& operator=(const obj&) {
				++num_cpy;
				return *this;
			}
			obj& operator=(obj&&) noexcept {
				++num_mv;
				return *this;
			}
		};

		std::vector<obj> vec(4);
		EXPECT_EQ(num_ctors, 4);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 0);

		std::vector<obj> vec2(4);
		EXPECT_EQ(num_ctors, 8);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 0);

		fea::copy_or_move(vec.begin(), vec.end(), vec2.begin());
		EXPECT_EQ(num_ctors, 8);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 4);
	}

	{
		static int num_ctors = 0;
		static int num_dtors = 0;
		static int num_cpy = 0;
		static int num_mv = 0;

		struct obj2 {
			obj2() {
				++num_ctors;
			}

			~obj2() {
				++num_dtors;
			}

			obj2(const obj2&) {
				++num_cpy;
			}
			obj2& operator=(const obj2&) {
				++num_cpy;
				return *this;
			}

			obj2(obj2&&) = delete;
			obj2& operator=(obj2&&) = delete;
		};

		std::vector<obj2> vec(4);
		EXPECT_EQ(num_ctors, 4);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 0);

		std::vector<obj2> vec2(4);
		EXPECT_EQ(num_ctors, 8);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 0);

		fea::copy_or_move(vec.begin(), vec.end(), vec2.begin());
		EXPECT_EQ(num_ctors, 8);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 4);
		EXPECT_EQ(num_mv, 0);
	}

	{
		static int num_ctors = 0;
		static int num_dtors = 0;
		static int num_cpy = 0;
		static int num_mv = 0;

		struct obj3 {
			obj3() {
				++num_ctors;
			}

			obj3(const obj3&) = default;
			obj3& operator=(const obj3&) = default;
		};
		static_assert(std::is_trivially_copyable<obj3>::value, "");

		std::vector<obj3> vec(4);
		EXPECT_EQ(num_ctors, 4);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 0);

		std::vector<obj3> vec2(4);
		EXPECT_EQ(num_ctors, 8);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 0);

		fea::copy_or_move(vec.begin(), vec.end(), vec2.begin());
		EXPECT_EQ(num_ctors, 8);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 0);
	}
}

TEST(memory, copy_or_move_backward) {
	// Check it compiles.
	{
		int i = 42;
		int o = 0;
		fea::copy_or_move_backward(&i, &i + 1, &o + 1);
		EXPECT_EQ(o, 42);
	}

	{
		static int num_ctors = 0;
		static int num_dtors = 0;
		static int num_cpy = 0;
		static int num_mv = 0;

		struct obj {
			obj() {
				++num_ctors;
			}

			~obj() {
				++num_dtors;
			}

			obj(const obj&) {
				++num_cpy;
			}
			obj(obj&&) noexcept {
				++num_mv;
			}

			obj& operator=(const obj&) {
				++num_cpy;
				return *this;
			}
			obj& operator=(obj&&) noexcept {
				++num_mv;
				return *this;
			}
		};

		std::vector<obj> vec(4);
		EXPECT_EQ(num_ctors, 4);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 0);

		std::vector<obj> vec2(4);
		EXPECT_EQ(num_ctors, 8);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 0);

		fea::copy_or_move_backward(vec.begin(), vec.end(), vec2.end());
		EXPECT_EQ(num_ctors, 8);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 4);
	}

	{
		static int num_ctors = 0;
		static int num_dtors = 0;
		static int num_cpy = 0;
		static int num_mv = 0;

		struct obj2 {
			obj2() {
				++num_ctors;
			}

			~obj2() {
				++num_dtors;
			}

			obj2(const obj2&) {
				++num_cpy;
			}
			obj2& operator=(const obj2&) {
				++num_cpy;
				return *this;
			}

			obj2(obj2&&) = delete;
			obj2& operator=(obj2&&) = delete;
		};

		std::vector<obj2> vec(4);
		EXPECT_EQ(num_ctors, 4);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 0);

		std::vector<obj2> vec2(4);
		EXPECT_EQ(num_ctors, 8);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 0);

		fea::copy_or_move_backward(vec.begin(), vec.end(), vec2.end());
		EXPECT_EQ(num_ctors, 8);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 4);
		EXPECT_EQ(num_mv, 0);
	}

	{
		static int num_ctors = 0;
		static int num_dtors = 0;
		static int num_cpy = 0;
		static int num_mv = 0;

		struct obj3 {
			obj3() {
				++num_ctors;
			}

			obj3(const obj3&) = default;
			obj3& operator=(const obj3&) = default;
		};
		static_assert(std::is_trivially_copyable<obj3>::value, "");

		std::vector<obj3> vec(4);
		EXPECT_EQ(num_ctors, 4);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 0);

		std::vector<obj3> vec2(4);
		EXPECT_EQ(num_ctors, 8);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 0);

		fea::copy_or_move_backward(vec.begin(), vec.end(), vec2.end());
		EXPECT_EQ(num_ctors, 8);
		EXPECT_EQ(num_dtors, 0);
		EXPECT_EQ(num_cpy, 0);
		EXPECT_EQ(num_mv, 0);
	}
}
} // namespace
