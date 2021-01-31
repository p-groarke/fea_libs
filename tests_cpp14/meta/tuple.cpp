#include <array>
#include <fea/meta/tuple.hpp>
#include <gtest/gtest.h>

namespace {
TEST(tuple, basics) {
	{
		auto tup = fea::make_tuple_from_count<int, 4>();
		static_assert(std::is_same<decltype(tup),
							  std::tuple<int, int, int, int>>::value,
				"tuple.cpp : test failed");

		auto tup2 = fea::make_tuple_from_count<int, 0>();
		static_assert(std::is_same<decltype(tup2), std::tuple<>>::value,
				"tuple.cpp : test failed");
	}

	{
		using tup = std::tuple<int, double, float, short>;
		static_assert(
				fea::tuple_idx_v<int, tup> == 0, "tuple.cpp : test failed");
		static_assert(
				fea::tuple_idx_v<double, tup> == 1, "tuple.cpp : test failed");
		static_assert(
				fea::tuple_idx_v<float, tup> == 2, "tuple.cpp : test failed");
		static_assert(
				fea::tuple_idx_v<short, tup> == 3, "tuple.cpp : test failed");
	}

	{
		using tup = std::tuple<int, double, float, short>;
		static_assert(
				fea::tuple_contains_v<int, tup>, "tuple.cpp : test failed");
		static_assert(
				fea::tuple_contains_v<double, tup>, "tuple.cpp : test failed");
		static_assert(
				fea::tuple_contains_v<float, tup>, "tuple.cpp : test failed");
		static_assert(
				fea::tuple_contains_v<short, tup>, "tuple.cpp : test failed");

		static_assert(!fea::tuple_contains_v<unsigned, tup>,
				"tuple.cpp : test failed");
		static_assert(
				!fea::tuple_contains_v<char, tup>, "tuple.cpp : test failed");
		static_assert(!fea::tuple_contains_v<std::tuple<>, tup>,
				"tuple.cpp : test failed");
		static_assert(!fea::tuple_contains_v<unsigned long long, tup>,
				"tuple.cpp : test failed");
	}

	{

#if FEA_CPP17
#define FEA_CEXPR constexpr
#else
#define FEA_CEXPR
#endif

		std::tuple<int, double, float, short> tup{};
		std::array<std::string, 4> visited;
		size_t idx = 0;

		fea::tuple_for_each(
				[&](auto v) {
					std::string str{};
					if FEA_CEXPR (std::is_same<decltype(v), int>::value) {
						str = "int";
					} else if FEA_CEXPR (std::is_same<decltype(v),
												 double>::value) {
						str = "double";
					} else if FEA_CEXPR (std::is_same<decltype(v),
												 float>::value) {
						str = "float";
					} else if FEA_CEXPR (std::is_same<decltype(v),
												 short>::value) {
						str = "short";
					} else {
						EXPECT_TRUE(false);
					}
					visited[idx++] = str;
					// visited[idx++] = std::string{ typeid(v).name() };
				},
				tup);

		const std::array<std::string, 4> expected{ "int", "double", "float",
			"short" };
		EXPECT_EQ(visited, expected);
	}

	{
		std::tuple<int, double, float, short> tup{};
		fea::apply(
				[&](auto... args) {
					EXPECT_EQ(sizeof...(args), 4u);
					static_assert(std::is_same<std::tuple<decltype(args)...>,
										  decltype(tup)>::value,
							"tuple.cpp : test failed");
				},
				tup);
	}

	{
		using tup = fea::tuple_type_cat_t<std::tuple<int, double>,
				std::tuple<float, short>>;

		static_assert(
				std::is_same<tup, std::tuple<int, double, float, short>>::value,
				"tuple.cpp : test failed");
	}
}
} // namespace
