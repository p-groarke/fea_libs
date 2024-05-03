#include <array>
#include <fea/meta/static_for.hpp>
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

TEST(tuple, runtime_get) {
	{
		constexpr std::tuple<int32_t, uint32_t, int64_t, int8_t, uint8_t> tup
				= std::make_tuple(1, 42u, -42, int8_t(2), uint8_t(5));
		using tup_t = decltype(tup);

		{
			uintptr_t offset = fea::tuple_offset<0>(tup);

			int32_t i = fea::offset_get<int32_t>(offset, tup);
			EXPECT_EQ(i, 1);
		}
		{
			uintptr_t offset = fea::tuple_offset<1>(tup);

			uint32_t i = fea::offset_get<uint32_t>(offset, tup);
			EXPECT_EQ(i, 42u);
		}
		{
			uintptr_t offset = fea::tuple_offset<2>(tup);

			int64_t i = fea::offset_get<int64_t>(offset, tup);
			EXPECT_EQ(i, -42);
		}
		{
			uintptr_t offset = fea::tuple_offset<3>(tup);

			int8_t i = fea::offset_get<int8_t>(offset, tup);
			EXPECT_EQ(i, 2);
		}
		{
			uintptr_t offset = fea::tuple_offset<4>(tup);

			uint8_t i = fea::offset_get<uint8_t>(offset, tup);
			EXPECT_EQ(i, 5u);
		}

		auto lookup = fea::make_offset_lookup(tup);
		fea::static_for<std::tuple_size<tup_t>::value>([&](auto idx) {
			EXPECT_EQ(lookup[idx], fea::tuple_offset<idx>(tup));
			size_t off2 = fea::tuple_offset<idx>(tup);
			EXPECT_EQ(lookup[idx], off2);
		});

		{
			const int32_t* ptr = (int32_t*)fea::runtime_get(0, tup);
			EXPECT_EQ(*ptr, 1);
		}
		{
			const uint32_t* ptr = (uint32_t*)fea::runtime_get(1, tup);
			EXPECT_EQ(*ptr, 42u);
		}
		{
			const int64_t* ptr = (int64_t*)fea::runtime_get(2, tup);
			EXPECT_EQ(*ptr, -42);
		}
		{
			const int8_t* ptr = (int8_t*)fea::runtime_get(3, tup);
			EXPECT_EQ(*ptr, 2);
		}
		{
			const uint8_t* ptr = (uint8_t*)fea::runtime_get(4, tup);
			EXPECT_EQ(*ptr, 5u);
		}
	}
}

#if FEA_CPP17
TEST(tuple, runtime_get_cpp17) {
	{
		const std::tuple<int32_t, uint32_t, int64_t, int8_t, uint8_t> tup
				= std::make_tuple(1, 42u, -42, int8_t(2), uint8_t(5));

		int32_t i32 = 0;
		uint32_t ui32 = 0;
		int64_t i64 = 0;
		int8_t i8 = 0;
		uint8_t ui8 = 0;

		auto tester = [&, ret = 42](const auto& v) -> const int& {
			using T = std::decay_t<decltype(v)>;
			if constexpr (std::is_same_v<T, int32_t>) {
				i32 = v;
			} else if constexpr (std::is_same_v<T, uint32_t>) {
				ui32 = v;
			} else if constexpr (std::is_same_v<T, int64_t>) {
				i64 = v;
			} else if constexpr (std::is_same_v<T, int8_t>) {
				i8 = v;
			} else if constexpr (std::is_same_v<T, uint8_t>) {
				ui8 = v;
			}
			return ret;
		};

		const int& ret = fea::runtime_get(tester, 0, tup);
		EXPECT_EQ(i32, 1);
		EXPECT_EQ(ret, 42);

		fea::runtime_get(tester, 1, tup);
		EXPECT_EQ(ui32, 42u);

		fea::runtime_get(tester, 2, tup);
		EXPECT_EQ(i64, -42);

		fea::runtime_get(tester, 3, tup);
		EXPECT_EQ(i8, 2);

		fea::runtime_get(tester, 4, tup);
		EXPECT_EQ(ui8, 5);
	}

	{
		std::tuple<int32_t, uint32_t, int64_t, int8_t, uint8_t> tup
				= std::make_tuple(1, 42u, -42, int8_t(2), uint8_t(5));

		int32_t i32 = 0;
		uint32_t ui32 = 0;
		int64_t i64 = 0;
		int8_t i8 = 0;
		uint8_t ui8 = 0;

		auto tester = [&, ret = 42](auto& v) mutable -> int& {
			using T = std::decay_t<decltype(v)>;
			if constexpr (std::is_same_v<T, int32_t>) {
				i32 = v;
			} else if constexpr (std::is_same_v<T, uint32_t>) {
				ui32 = v;
			} else if constexpr (std::is_same_v<T, int64_t>) {
				i64 = v;
			} else if constexpr (std::is_same_v<T, int8_t>) {
				i8 = v;
			} else if constexpr (std::is_same_v<T, uint8_t>) {
				ui8 = v;
			}
			return ret;
		};

		int& ret = fea::runtime_get(tester, 0, tup);
		EXPECT_EQ(i32, 1);
		EXPECT_EQ(ret, 42);

		fea::runtime_get(tester, 1, tup);
		EXPECT_EQ(ui32, 42u);

		fea::runtime_get(tester, 2, tup);
		EXPECT_EQ(i64, -42);

		fea::runtime_get(tester, 3, tup);
		EXPECT_EQ(i8, 2);

		fea::runtime_get(tester, 4, tup);
		EXPECT_EQ(ui8, 5);
	}
}
#endif
} // namespace
