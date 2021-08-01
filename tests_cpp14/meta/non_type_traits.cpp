#include <fea/meta/non_type_traits.hpp>
#include <gtest/gtest.h>

namespace {
TEST(non_type_traits, basics) {
	static_assert(fea::max_nt_v<size_t, 1, 0, 42> == 42,
			"non_type_traits.cpp : test failed");
	static_assert(fea::max_nt_v<size_t, 0, 42, 1> == 42,
			"non_type_traits.cpp : test failed");
	static_assert(fea::max_nt_v<size_t, 42, 0, 1> == 42,
			"non_type_traits.cpp : test failed");

	static_assert(fea::min_nt_v<size_t, 1, 42, 0> == 0,
			"non_type_traits.cpp : test failed");
	static_assert(fea::min_nt_v<size_t, 1, 0, 42> == 0,
			"non_type_traits.cpp : test failed");
	static_assert(fea::min_nt_v<size_t, 0, 1, 42> == 0,
			"non_type_traits.cpp : test failed");

	static_assert(fea::max_nt_v<int, 0, -42, 42> == 42,
			"non_type_traits.cpp : test failed");
	static_assert(fea::max_nt_v<int, 0, 42, -42> == 42,
			"non_type_traits.cpp : test failed");
	static_assert(fea::max_nt_v<int, 42, 0, -42> == 42,
			"non_type_traits.cpp : test failed");

	static_assert(fea::min_nt_v<int, 0, 42, -42> == -42,
			"non_type_traits.cpp : test failed");
	static_assert(fea::min_nt_v<int, 0, -42, 42> == -42,
			"non_type_traits.cpp : test failed");
	static_assert(fea::min_nt_v<int, -42, 0, 42> == -42,
			"non_type_traits.cpp : test failed");

	static_assert(fea::max_nt_v<size_t, 1, 1, 1> == 1,
			"non_type_traits.cpp : test failed");
	static_assert(fea::min_nt_v<size_t, 1, 1, 1> == 1,
			"non_type_traits.cpp : test failed");

	static_assert(fea::max_nt_v<int, -1, -1, -1> == -1,
			"non_type_traits.cpp : test failed");
	static_assert(fea::min_nt_v<int, -1, -1, -1> == -1,
			"non_type_traits.cpp : test failed");
}

TEST(non_type_traits, enums) {
	{
		enum class e : unsigned {
			one = 42u,
			two,
			three = 101u,
		};

		static_assert(fea::max_nt_v<e, e::one, e::two, e::three> == e::three,
				"non_type_traits.cpp : test failed");
		static_assert(fea::max_nt_v<e, e::one, e::three, e::two> == e::three,
				"non_type_traits.cpp : test failed");
		static_assert(fea::max_nt_v<e, e::three, e::one, e::two> == e::three,
				"non_type_traits.cpp : test failed");

		static_assert(fea::min_nt_v<e, e::one, e::two, e::three> == e::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_nt_v<e, e::two, e::one, e::three> == e::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_nt_v<e, e::two, e::three, e::one> == e::one,
				"non_type_traits.cpp : test failed");

		static_assert(fea::max_nt_v<e, e::one, e::one, e::one> == e::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_nt_v<e, e::one, e::one, e::one> == e::one,
				"non_type_traits.cpp : test failed");
	}

	{
		enum class e {
			one = -42,
			two = 0,
			three = 42,
		};

		static_assert(fea::max_nt_v<e, e::one, e::two, e::three> == e::three,
				"non_type_traits.cpp : test failed");
		static_assert(fea::max_nt_v<e, e::one, e::three, e::two> == e::three,
				"non_type_traits.cpp : test failed");
		static_assert(fea::max_nt_v<e, e::three, e::one, e::two> == e::three,
				"non_type_traits.cpp : test failed");

		static_assert(fea::min_nt_v<e, e::one, e::two, e::three> == e::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_nt_v<e, e::two, e::one, e::three> == e::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_nt_v<e, e::two, e::three, e::one> == e::one,
				"non_type_traits.cpp : test failed");

		static_assert(fea::max_nt_v<e, e::one, e::one, e::one> == e::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_nt_v<e, e::one, e::one, e::one> == e::one,
				"non_type_traits.cpp : test failed");
	}
}

// Broken in vs2017 and gcc
#if FEA_CPP17
TEST(non_type_traits, basics_cpp17) {

	static_assert(fea::max_v<1u, 0u, 42u> == 42u,
			"non_type_traits.cpp : test failed");
	static_assert(fea::max_v<0u, 42u, 1u> == 42u,
			"non_type_traits.cpp : test failed");
	static_assert(fea::max_v<42u, 0u, 1u> == 42u,
			"non_type_traits.cpp : test failed");

	static_assert(
			fea::min_v<1u, 42u, 0u> == 0u, "non_type_traits.cpp : test failed");
	static_assert(
			fea::min_v<1u, 0u, 42u> == 0u, "non_type_traits.cpp : test failed");
	static_assert(
			fea::min_v<0u, 1u, 42u> == 0u, "non_type_traits.cpp : test failed");

	static_assert(
			fea::max_v<0, -42, 42> == 42, "non_type_traits.cpp : test failed");
	static_assert(
			fea::max_v<0, 42, -42> == 42, "non_type_traits.cpp : test failed");
	static_assert(
			fea::max_v<42, 0, -42> == 42, "non_type_traits.cpp : test failed");

	static_assert(
			fea::min_v<0, 42, -42> == -42, "non_type_traits.cpp : test failed");
	static_assert(
			fea::min_v<0, -42, 42> == -42, "non_type_traits.cpp : test failed");
	static_assert(
			fea::min_v<-42, 0, 42> == -42, "non_type_traits.cpp : test failed");

	static_assert(
			fea::max_v<1u, 1u, 1u> == 1, "non_type_traits.cpp : test failed");
	static_assert(
			fea::min_v<1u, 1u, 1u> == 1, "non_type_traits.cpp : test failed");

	static_assert(
			fea::max_v<-1, -1, -1> == -1, "non_type_traits.cpp : test failed");
	static_assert(
			fea::min_v<-1, -1, -1> == -1, "non_type_traits.cpp : test failed");
}

TEST(non_type_traits, enums_cpp17) {
	{
		enum class e : unsigned {
			one = 42u,
			two,
			three = 101u,
		};

		static_assert(fea::max_v<e::one, e::two, e::three> == e::three,
				"non_type_traits.cpp : test failed");
		static_assert(fea::max_v<e::one, e::three, e::two> == e::three,
				"non_type_traits.cpp : test failed");
		static_assert(fea::max_v<e::three, e::one, e::two> == e::three,
				"non_type_traits.cpp : test failed");

		static_assert(fea::min_v<e::one, e::two, e::three> == e::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_v<e::two, e::one, e::three> == e::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_v<e::two, e::three, e::one> == e::one,
				"non_type_traits.cpp : test failed");

		static_assert(fea::max_v<e::one, e::one, e::one> == e::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_v<e::one, e::one, e::one> == e::one,
				"non_type_traits.cpp : test failed");
	}

	{
		enum class e : int {
			one = -42,
			two = 0,
			three = 42,
		};

		static_assert(
				std::is_enum_v<decltype(fea::max_v<e::one, e::two, e::three>)>,
				"non_type_traits.cpp : test failed");
		static_assert(
				std::is_enum_v<decltype(fea::min_v<e::one, e::two, e::three>)>,
				"non_type_traits.cpp : test failed");

		{
			constexpr e answer = e(fea::max_v<e::one, e::two, e::three>);
			static_assert(
					answer == e::three, "non_type_traits.cpp : test failed");
		}
		static_assert(fea::max_v<e::one, e::three, e::two> == e::three,
				"non_type_traits.cpp : test failed");
		static_assert(fea::max_v<e::three, e::one, e::two> == e::three,
				"non_type_traits.cpp : test failed");

		{
			constexpr e answer = e(fea::min_v<e::one, e::two, e::three>);
			static_assert(
					answer == e::one, "non_type_traits.cpp : test failed");
		}
		static_assert(fea::min_v<e::two, e::one, e::three> == e::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_v<e::two, e::three, e::one> == e::one,
				"non_type_traits.cpp : test failed");

		static_assert(fea::max_v<e::one, e::one, e::one> == e::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_v<e::one, e::one, e::one> == e::one,
				"non_type_traits.cpp : test failed");
	}
}

#endif
} // namespace
