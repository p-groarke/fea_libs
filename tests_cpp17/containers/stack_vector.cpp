#include <fea/containers/stack_vector.hpp>
#include <gtest/gtest.h>
#include <numeric>
#include <vector>

namespace {

TEST(stack_vector, dtors) {
	static int num_dtors = 0;

	struct test_dtor {
		test_dtor() = default;
		~test_dtor() {
			++num_dtors;
			--v;
			EXPECT_GE(v, 0);
		}

		int v = 1;
	};

	fea::stack_vector<test_dtor, 10> sv;
	sv.push_back({});
	sv.push_back({});
	sv.push_back({});
	sv.push_back({});

	num_dtors = 0;
	sv.clear();
	EXPECT_EQ(num_dtors, 4);
	sv.clear();
	EXPECT_EQ(num_dtors, 4);
	sv.clear();
	EXPECT_EQ(num_dtors, 4);

	sv.shrink_to_fit();
	sv.shrink_to_fit();
	sv.shrink_to_fit();
	EXPECT_EQ(num_dtors, 4);
}

TEST(stack_vector, basics) {
	fea::stack_vector<size_t, 128> v{ std::array<size_t, 3>{ 0u, 1u, 2u } };
	EXPECT_EQ(v.size(), 3u);
	EXPECT_EQ(v.max_size(), 128u);
	EXPECT_FALSE(v.empty());

	EXPECT_EQ(size_t(std::distance(v.begin(), v.end())), 3u);
	EXPECT_EQ(size_t(std::distance(v.cbegin(), v.cend())), 3u);
	EXPECT_EQ(size_t(std::distance(v.rbegin(), v.rend())), 3u);
	EXPECT_EQ(size_t(std::distance(v.crbegin(), v.crend())), 3u);

	EXPECT_EQ(*v.begin(), 0u);
	EXPECT_EQ(*v.cbegin(), 0u);
	EXPECT_EQ(*(v.end() - 1), 2u);
	EXPECT_EQ(*(v.cend() - 1), 2u);
	EXPECT_EQ(*v.rbegin(), 2u);
	EXPECT_EQ(*(v.rend() - 1), 0u);
	EXPECT_EQ(*v.crbegin(), 2u);
	EXPECT_EQ(*(v.crend() - 1), 0u);

	EXPECT_EQ(v.front(), 0u);
	EXPECT_EQ(v.back(), 2u);

	EXPECT_EQ(v[0], 0u);
	EXPECT_EQ(v.at(0), 0u);
	EXPECT_EQ(*v.data(), 0u);

	fea::stack_vector<size_t, 128> v2{ std::array<size_t, 3>{ 3u, 3u, 3u } };
	using std::swap;
	swap(v, v2);
	EXPECT_EQ(v[0], 3u);
	EXPECT_EQ(v[1], 3u);
	EXPECT_EQ(v[2], 3u);

	swap(v, v2);
	EXPECT_EQ(v[0], 0u);
	EXPECT_EQ(v[1], 1u);
	EXPECT_EQ(v[2], 2u);

	v.push_back(3);
	EXPECT_EQ(v.size(), 4u);
	EXPECT_EQ(size_t(std::distance(v.begin(), v.end())), 4u);
	EXPECT_EQ(size_t(std::distance(v.cbegin(), v.cend())), 4u);
	EXPECT_EQ(size_t(std::distance(v.rbegin(), v.rend())), 4u);
	EXPECT_EQ(size_t(std::distance(v.crbegin(), v.crend())), 4u);

	EXPECT_EQ(*(v.end() - 1), 3u);
	EXPECT_EQ(*(v.cend() - 1), 3u);
	EXPECT_EQ(*v.rbegin(), 3u);
	EXPECT_EQ(*v.crbegin(), 3u);

	EXPECT_EQ(v.back(), 3u);

	EXPECT_EQ(v[3], 3u);
	EXPECT_EQ(v.at(3), 3u);
	EXPECT_EQ(*(v.data() + 3), 3u);

	v.pop_back();
	EXPECT_EQ(v.size(), 3u);

	EXPECT_EQ(size_t(std::distance(v.begin(), v.end())), 3u);
	EXPECT_EQ(size_t(std::distance(v.cbegin(), v.cend())), 3u);
	EXPECT_EQ(size_t(std::distance(v.rbegin(), v.rend())), 3u);
	EXPECT_EQ(size_t(std::distance(v.crbegin(), v.crend())), 3u);

	EXPECT_EQ(*(v.end() - 1), 2u);
	EXPECT_EQ(*(v.cend() - 1), 2u);
	EXPECT_EQ(*v.rbegin(), 2u);
	EXPECT_EQ(*v.crbegin(), 2u);

	EXPECT_EQ(v.back(), 2u);

	EXPECT_EQ(v[2], 2u);
	EXPECT_EQ(v.at(2), 2u);
	EXPECT_EQ(*(v.data() + 2), 2u);

	v.resize(5);
	EXPECT_EQ(v.size(), 5u);
	EXPECT_EQ(v.back(), 0u);

	EXPECT_EQ(size_t(std::distance(v.begin(), v.end())), 5u);
	EXPECT_EQ(size_t(std::distance(v.cbegin(), v.cend())), 5u);
	EXPECT_EQ(size_t(std::distance(v.rbegin(), v.rend())), 5u);
	EXPECT_EQ(size_t(std::distance(v.crbegin(), v.crend())), 5u);

	v.resize(3);
	EXPECT_EQ(v.size(), 3u);

	v.resize(5, 5u);
	EXPECT_EQ(v.size(), 5u);
	EXPECT_EQ(v.back(), 5u);
	EXPECT_EQ(v[3], 5u);
	EXPECT_EQ(v[4], 5u);

	v.clear();
	EXPECT_EQ(v.size(), 0u);
	EXPECT_TRUE(v.empty());
}

TEST(stack_vector, ctors) {
	fea::stack_vector<int, 4> v1(2, 42);
	EXPECT_EQ(v1.size(), 2u);
	EXPECT_EQ(v1[0], 42);
	EXPECT_EQ(v1[1], 42);

	const fea::stack_vector<int, 5> v2{ 0, 1, 2 };
	EXPECT_EQ(v2.size(), 3u);
	EXPECT_EQ(v2[0], 0);
	EXPECT_EQ(v2[1], 1);
	EXPECT_EQ(v2[2], 2);

	//{
	//	auto v2(v1);
	//	EXPECT_EQ(v2.size(), 2);
	//	EXPECT_EQ(v2[0], 42);
	//	EXPECT_EQ(v2[1], 42);
	//}

	//{
	//	auto v2 = v1;
	//	EXPECT_EQ(v2.size(), 2);
	//	EXPECT_EQ(v2[0], 42);
	//	EXPECT_EQ(v2[1], 42);
	//}

	//{
	//	auto v2(std::move(v1));
	//	EXPECT_EQ(v2.size(), 2);
	//	EXPECT_EQ(v2[0], 42);
	//	EXPECT_EQ(v2[1], 42);
	//}

	//{
	//	auto v2 = std::move(v1);
	//	EXPECT_EQ(v2.size(), 2);
	//	EXPECT_EQ(v2[0], 42);
	//	EXPECT_EQ(v2[1], 42);
	//}
}

TEST(stack_vector, insert) {
	fea::stack_vector<int, 5> v(5);

	{
		const fea::stack_vector<int, 5> answer{ 0, 1, 2, 3, 4 };
		std::iota(v.begin(), v.end(), 0);
		EXPECT_EQ(v, answer);
	}

	v.pop_back();
	EXPECT_EQ(v.size(), 4u);

	auto it = v.insert(v.begin() + 1, 42);
	EXPECT_EQ(v.size(), 5u);
	EXPECT_EQ(*it, 42);
	EXPECT_EQ(std::distance(v.begin(), it), 1);
	{
		const fea::stack_vector<int, 5> answer{ 0, 42, 1, 2, 3 };
		EXPECT_EQ(v, answer);
	}

	std::iota(v.begin(), v.end(), 0);
	v.pop_back();
	v.pop_back();
	v.pop_back();
	EXPECT_EQ(v.size(), 2u);
	it = v.insert(v.begin() + 1, 3, 42);
	EXPECT_EQ(v.size(), 5u);
	EXPECT_EQ(*it, 42);
	EXPECT_EQ(std::distance(v.begin(), it), 1);
	{
		const fea::stack_vector<int, 5> answer{ 0, 42, 42, 42, 1 };
		EXPECT_EQ(v, answer);
	}

	std::vector<int> cpy{ 42, -42, 42 };
	std::iota(v.begin(), v.end(), 0);
	v.resize(2);
	EXPECT_EQ(v.size(), 2u);
	it = v.insert(v.begin() + 1, cpy.begin(), cpy.end());
	EXPECT_EQ(v.size(), 5u);
	EXPECT_EQ(*it, 42);
	EXPECT_EQ(std::distance(v.begin(), it), 1);
	{
		const fea::stack_vector<int, 5> answer{ 0, 42, -42, 42, 1 };
		EXPECT_EQ(v, answer);
	}

	std::iota(v.begin(), v.end(), 0);
	v.resize(2);
	EXPECT_EQ(v.size(), 2u);
	it = v.insert(v.begin() + 1, { 42, -42, 101 });
	EXPECT_EQ(v.size(), 5u);
	EXPECT_EQ(*it, 42);
	EXPECT_EQ(std::distance(v.begin(), it), 1);
	{
		const fea::stack_vector<int, 5> answer{ 0, 42, -42, 101, 1 };
		EXPECT_EQ(v, answer);
	}

	v.clear();
	EXPECT_EQ(v.size(), 0u);
	EXPECT_TRUE(v.empty());

	it = v.insert(v.end(), { 0, 1, 2, 3, 4 });
	EXPECT_EQ(v.size(), 5u);
	EXPECT_EQ(*it, 0);
	EXPECT_EQ(std::distance(v.begin(), it), 0);
	{
		const fea::stack_vector<int, 5> answer{ 0, 1, 2, 3, 4 };
		EXPECT_EQ(v, answer);
	}
}

TEST(stack_vector, erase) {
	static int num_ctors = 0;
	static int num_dtors = 0;
	static int num_cpy = 0;
	static int num_mv = 0;

	struct obj {
		obj() = default;
		obj(int val)
				: v(val) {
			++num_ctors;
		}

		~obj() {
			++num_dtors;
		}

		obj(const obj& o) {
			++num_cpy;
			v = o.v;
		}

		obj(obj&& o) noexcept {
			++num_mv;
			v = o.v;
		}
		obj& operator=(const obj& o) {
			++num_cpy;
			v = o.v;
			return *this;
		}
		obj& operator=(obj&& o) noexcept {
			++num_mv;
			v = o.v;
			return *this;
		}

		operator int() const {
			return v;
		}

		int v = 0;
	};

	fea::stack_vector<obj, 5> arr{ 0, 1, 2, 3, 4 };
	EXPECT_EQ(arr.size(), 5u);
	EXPECT_EQ(num_ctors, 5);
	EXPECT_EQ(num_dtors, 5);
	EXPECT_EQ(num_cpy, 5); // init-list is const static storage
	EXPECT_EQ(num_mv, 0);

	{
		auto it = arr.erase(arr.begin());
		EXPECT_EQ(arr.size(), 4u);
		EXPECT_EQ(num_ctors, 5);
		EXPECT_EQ(num_dtors, 6);
		EXPECT_EQ(num_cpy, 5);
		EXPECT_EQ(num_mv, 4);
		EXPECT_EQ(it, arr.begin());
		EXPECT_EQ(*it, 1);

		const fea::stack_vector<int, 5> answer{ 1, 2, 3, 4 };
		EXPECT_TRUE(std::equal(
				arr.begin(), arr.end(), answer.begin(), answer.end()));
	}

	{
		auto it = arr.erase(arr.begin() + 2);
		EXPECT_EQ(num_ctors, 5);
		EXPECT_EQ(num_dtors, 7);
		EXPECT_EQ(num_cpy, 5);
		EXPECT_EQ(num_mv, 5);
		EXPECT_EQ(it, arr.begin() + 2);
		EXPECT_EQ(*it, 4);

		const fea::stack_vector<int, 5> answer{ 1, 2, 4 };
		EXPECT_TRUE(std::equal(
				arr.begin(), arr.end(), answer.begin(), answer.end()));
	}

	{
		auto it = arr.erase(arr.begin() + 2);
		EXPECT_EQ(num_ctors, 5);
		EXPECT_EQ(num_dtors, 8);
		EXPECT_EQ(num_cpy, 5);
		EXPECT_EQ(num_mv, 5);
		EXPECT_EQ(it, arr.end());

		const fea::stack_vector<int, 5> answer{ 1, 2 };
		EXPECT_TRUE(std::equal(
				arr.begin(), arr.end(), answer.begin(), answer.end()));
	}

	{
		auto it = arr.erase(arr.begin() + 1);
		EXPECT_EQ(num_ctors, 5);
		EXPECT_EQ(num_dtors, 9);
		EXPECT_EQ(num_cpy, 5);
		EXPECT_EQ(num_mv, 5);
		EXPECT_EQ(it, arr.end());

		const fea::stack_vector<int, 5> answer{ 1 };
		EXPECT_TRUE(std::equal(
				arr.begin(), arr.end(), answer.begin(), answer.end()));
	}

	{
		auto it = arr.erase(arr.begin());
		EXPECT_EQ(num_ctors, 5);
		EXPECT_EQ(num_dtors, 10);
		EXPECT_EQ(num_cpy, 5);
		EXPECT_EQ(num_mv, 5);
		EXPECT_EQ(it, arr.end());

		const fea::stack_vector<int, 5> answer{};
		EXPECT_TRUE(std::equal(
				arr.begin(), arr.end(), answer.begin(), answer.end()));
	}


	num_ctors = 0;
	num_dtors = 0;
	num_cpy = 0;
	num_mv = 0;

	arr = { 0, 1, 2, 3, 4 };
	EXPECT_EQ(arr.size(), 5u);
	EXPECT_EQ(num_ctors, 5);
	EXPECT_EQ(num_dtors, 10);
	EXPECT_EQ(num_cpy, 5); // init-list is const static storage
	EXPECT_EQ(num_mv, 5);

	{
		auto it = arr.erase(arr.begin(), arr.begin());
		EXPECT_EQ(arr.size(), 5u);
		EXPECT_EQ(num_ctors, 5);
		EXPECT_EQ(num_dtors, 10);
		EXPECT_EQ(num_cpy, 5);
		EXPECT_EQ(num_mv, 5);
		EXPECT_EQ(it, arr.begin());
		EXPECT_EQ(*it, 0);

		const fea::stack_vector<int, 5> answer{ 0, 1, 2, 3, 4 };
		EXPECT_TRUE(std::equal(
				arr.begin(), arr.end(), answer.begin(), answer.end()));
	}

	{
		auto it = arr.erase(arr.begin(), arr.begin() + 2);
		EXPECT_EQ(arr.size(), 3u);
		EXPECT_EQ(num_ctors, 5);
		EXPECT_EQ(num_dtors, 12);
		EXPECT_EQ(num_cpy, 5);
		EXPECT_EQ(num_mv, 8);
		EXPECT_EQ(it, arr.begin());
		EXPECT_EQ(*it, 2);

		const fea::stack_vector<int, 5> answer{ 2, 3, 4 };
		EXPECT_TRUE(std::equal(
				arr.begin(), arr.end(), answer.begin(), answer.end()));
	}

	{
		auto it = arr.erase(arr.begin() + 1, arr.end());
		EXPECT_EQ(arr.size(), 1u);
		EXPECT_EQ(num_ctors, 5);
		EXPECT_EQ(num_dtors, 14);
		EXPECT_EQ(num_cpy, 5);
		EXPECT_EQ(num_mv, 8);
		EXPECT_EQ(it, arr.end());

		const fea::stack_vector<int, 5> answer{ 2 };
		EXPECT_TRUE(std::equal(
				arr.begin(), arr.end(), answer.begin(), answer.end()));
	}

	{
		auto it = arr.erase(arr.begin(), arr.end());
		EXPECT_EQ(arr.size(), 0u);
		EXPECT_EQ(num_ctors, 5);
		EXPECT_EQ(num_dtors, 15);
		EXPECT_EQ(num_cpy, 5);
		EXPECT_EQ(num_mv, 8);
		EXPECT_EQ(it, arr.end());

		const fea::stack_vector<int, 5> answer{};
		EXPECT_TRUE(std::equal(
				arr.begin(), arr.end(), answer.begin(), answer.end()));
	}

	{
		auto it = arr.erase(arr.end(), arr.end());
		EXPECT_EQ(arr.size(), 0u);
		EXPECT_EQ(num_ctors, 5);
		EXPECT_EQ(num_dtors, 15);
		EXPECT_EQ(num_cpy, 5);
		EXPECT_EQ(num_mv, 8);
		EXPECT_EQ(it, arr.end());

		const fea::stack_vector<int, 5> answer{};
		EXPECT_TRUE(std::equal(
				arr.begin(), arr.end(), answer.begin(), answer.end()));
	}
}
} // namespace
