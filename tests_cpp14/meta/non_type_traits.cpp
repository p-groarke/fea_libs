#include <fea/meta/non_type_traits.hpp>
#include <gtest/gtest.h>

namespace {
enum class e1 : unsigned {
	one = 42u,
	two,
	three = 101u,
};
enum class e2 {
	one = -42,
	two = 0,
	three = 42,
};

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
		static_assert(
				fea::max_nt_v<e1, e1::one, e1::two, e1::three> == e1::three,
				"non_type_traits.cpp : test failed");
		static_assert(
				fea::max_nt_v<e1, e1::one, e1::three, e1::two> == e1::three,
				"non_type_traits.cpp : test failed");
		static_assert(
				fea::max_nt_v<e1, e1::three, e1::one, e1::two> == e1::three,
				"non_type_traits.cpp : test failed");

		static_assert(fea::min_nt_v<e1, e1::one, e1::two, e1::three> == e1::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_nt_v<e1, e1::two, e1::one, e1::three> == e1::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_nt_v<e1, e1::two, e1::three, e1::one> == e1::one,
				"non_type_traits.cpp : test failed");

		static_assert(fea::max_nt_v<e1, e1::one, e1::one, e1::one> == e1::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_nt_v<e1, e1::one, e1::one, e1::one> == e1::one,
				"non_type_traits.cpp : test failed");
	}

	{
		static_assert(
				fea::max_nt_v<e2, e2::one, e2::two, e2::three> == e2::three,
				"non_type_traits.cpp : test failed");
		static_assert(
				fea::max_nt_v<e2, e2::one, e2::three, e2::two> == e2::three,
				"non_type_traits.cpp : test failed");
		static_assert(
				fea::max_nt_v<e2, e2::three, e2::one, e2::two> == e2::three,
				"non_type_traits.cpp : test failed");

		static_assert(fea::min_nt_v<e2, e2::one, e2::two, e2::three> == e2::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_nt_v<e2, e2::two, e2::one, e2::three> == e2::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_nt_v<e2, e2::two, e2::three, e2::one> == e2::one,
				"non_type_traits.cpp : test failed");

		static_assert(fea::max_nt_v<e2, e2::one, e2::one, e2::one> == e2::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_nt_v<e2, e2::one, e2::one, e2::one> == e2::one,
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
		static_assert(fea::max_v<e1::one, e1::two, e1::three> == e1::three,
				"non_type_traits.cpp : test failed");
		static_assert(fea::max_v<e1::one, e1::three, e1::two> == e1::three,
				"non_type_traits.cpp : test failed");
		static_assert(fea::max_v<e1::three, e1::one, e1::two> == e1::three,
				"non_type_traits.cpp : test failed");

		static_assert(fea::min_v<e1::one, e1::two, e1::three> == e1::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_v<e1::two, e1::one, e1::three> == e1::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_v<e1::two, e1::three, e1::one> == e1::one,
				"non_type_traits.cpp : test failed");

		static_assert(fea::max_v<e1::one, e1::one, e1::one> == e1::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_v<e1::one, e1::one, e1::one> == e1::one,
				"non_type_traits.cpp : test failed");
	}

	{
		static_assert(std::is_enum_v<decltype(
							  fea::max_v<e2::one, e2::two, e2::three>)>,
				"non_type_traits.cpp : test failed");
		// static_assert(std::is_enum_v<decltype(
		//					  fea::min_v<e2::one, e2::two, e2::three>)>,
		//		"non_type_traits.cpp : test failed");

		{
			constexpr e2 answer = e2(fea::max_v<e2::one, e2::two, e2::three>);
			static_assert(
					answer == e2::three, "non_type_traits.cpp : test failed");
		}
		static_assert(fea::max_v<e2::one, e2::three, e2::two> == e2::three,
				"non_type_traits.cpp : test failed");
		static_assert(fea::max_v<e2::three, e2::one, e2::two> == e2::three,
				"non_type_traits.cpp : test failed");

		{
			constexpr e2 answer = e2(fea::min_v<e2::one, e2::two, e2::three>);
			static_assert(
					answer == e2::one, "non_type_traits.cpp : test failed");
		}
		static_assert(e2(fea::min_v<e2::two, e2::one, e2::three>) == e2::one,
				"non_type_traits.cpp : test failed");
		static_assert(e2(fea::min_v<e2::two, e2::three, e2::one>) == e2::one,
				"non_type_traits.cpp : test failed");

		static_assert(fea::max_v<e2::one, e2::one, e2::one> == e2::one,
				"non_type_traits.cpp : test failed");
		static_assert(fea::min_v<e2::one, e2::one, e2::one> == e2::one,
				"non_type_traits.cpp : test failed");
	}
}

#endif
} // namespace
