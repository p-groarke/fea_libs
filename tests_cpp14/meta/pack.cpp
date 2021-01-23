﻿#include <array>
#include <fea/meta/pack.hpp>
#include <gtest/gtest.h>

namespace {
TEST(pack, basics) {

	{
		constexpr fea::pack<int, double, float> p1
				= fea::make_pack(42, 42.0, 42.f);
		constexpr decltype(p1) p2;

		constexpr fea::pack<int, double, float, int, double, float> p_cat
				= fea::pack_cat(p1, p2);
		constexpr auto p_cat2 = fea::pack_cat(p1, p2);

		static_assert(std::is_same<decltype(p_cat), decltype(p_cat2)>::value,
				"pack.cpp : test failed");

		using p_cat_t = decltype(p_cat);

		static_assert(fea::pack_size_v<p_cat_t> == 6, "pack.cpp : test failed");
		static_assert(
				std::is_same<fea::pack_element_t<0, p_cat_t>, const int>::value,
				"pack.cpp : test failed");
		static_assert(std::is_same<fea::pack_element_t<1, p_cat_t>,
							  const double>::value,
				"pack.cpp : test failed");
		static_assert(std::is_same<fea::pack_element_t<2, p_cat_t>,
							  const float>::value,
				"pack.cpp : test failed");
		static_assert(
				std::is_same<fea::pack_element_t<3, p_cat_t>, const int>::value,
				"pack.cpp : test failed");
		static_assert(std::is_same<fea::pack_element_t<4, p_cat_t>,
							  const double>::value,
				"pack.cpp : test failed");
		static_assert(std::is_same<fea::pack_element_t<5, p_cat_t>,
							  const float>::value,
				"pack.cpp : test failed");

		static_assert(
				fea::pack_idx_v<int, p_cat_t> == 0, "pack.cpp : test failed");
		static_assert(fea::pack_idx_v<double, p_cat_t> == 1,
				"pack.cpp : test failed");
		static_assert(
				fea::pack_idx_v<float, p_cat_t> == 2, "pack.cpp : test failed");

		using int_idxes = fea::pack_idxes_t<int, p_cat_t>;
		static_assert(
				std::is_same<int_idxes, fea::pack_nt<size_t, 0, 3>>::value,
				"pack.cpp : test failed");

		using double_idxes = fea::pack_idxes_t<double, p_cat_t>;
		static_assert(
				std::is_same<double_idxes, fea::pack_nt<size_t, 1, 4>>::value,
				"pack.cpp : test failed");

		using float_idxes = fea::pack_idxes_t<float, p_cat_t>;
		static_assert(
				std::is_same<float_idxes, fea::pack_nt<size_t, 2, 5>>::value,
				"pack.cpp : test failed");

		static_assert(
				fea::pack_contains_v<int, p_cat_t>, "pack.cpp : test failed");
		static_assert(fea::pack_contains_v<double, p_cat_t>,
				"pack.cpp : test failed");
		static_assert(
				fea::pack_contains_v<float, p_cat_t>, "pack.cpp : test failed");
		static_assert(!fea::pack_contains_v<short, p_cat_t>,
				"pack.cpp : test failed");
		static_assert(
				!fea::pack_contains_v<char, p_cat_t>, "pack.cpp : test failed");
	}

	{
		enum class e {
			one,
			two,
			three,
			four,
			count,
		};

		constexpr fea::pack_nt<e, e::one, e::three, e::two> p1;
		constexpr decltype(p1) p2;

		constexpr fea::pack_nt<e, e::one, e::three, e::two, e::one, e::three,
				e::two>
				p_cat = fea::pack_cat(p1, p2);
		constexpr auto p_cat2 = fea::pack_cat(p1, p2);

		static_assert(std::is_same<decltype(p_cat), decltype(p_cat2)>::value,
				"pack.cpp : test failed");

		using p_cat_t = decltype(p_cat);

		static_assert(fea::pack_size_v<p_cat_t> == 6, "pack.cpp : test failed");
		static_assert(fea::pack_element_nt_v<0, p_cat_t> == e::one,
				"pack.cpp : test failed");
		static_assert(fea::pack_element_nt_v<1, p_cat_t> == e::three,
				"pack.cpp : test failed");
		static_assert(fea::pack_element_nt_v<2, p_cat_t> == e::two,
				"pack.cpp : test failed");
		static_assert(fea::pack_element_nt_v<3, p_cat_t> == e::one,
				"pack.cpp : test failed");
		static_assert(fea::pack_element_nt_v<4, p_cat_t> == e::three,
				"pack.cpp : test failed");
		static_assert(fea::pack_element_nt_v<5, p_cat_t> == e::two,
				"pack.cpp : test failed");

#if FEA_CPP17
		static_assert(fea::pack_idx_nt_v2<e::one, p_cat_t> == 0,
				"pack.cpp : test failed");
		static_assert(fea::pack_idx_nt_v2<e::three, p_cat_t> == 1,
				"pack.cpp : test failed");
		static_assert(fea::pack_idx_nt_v2<e::two, p_cat_t> == 2,
				"pack.cpp : test failed");
#endif

		static_assert(fea::pack_idx_nt_v<e, e::one, p_cat_t> == 0,
				"pack.cpp : test failed");
		static_assert(fea::pack_idx_nt_v<e, e::three, p_cat_t> == 1,
				"pack.cpp : test failed");
		static_assert(fea::pack_idx_nt_v<e, e::two, p_cat_t> == 2,
				"pack.cpp : test failed");

#if FEA_CPP17
		using one_indexes = fea::pack_idxes_nt_t2<e::one, p_cat_t>;
		static_assert(
				std::is_same<one_indexes, fea::pack_nt<size_t, 0, 3>>::value,
				"pack.cpp : test failed");

		using three_idxes = fea::pack_idxes_nt_t2<e::three, p_cat_t>;
		static_assert(
				std::is_same<three_idxes, fea::pack_nt<size_t, 1, 4>>::value,
				"pack.cpp : test failed");

		using two_idxes = fea::pack_idxes_nt_t2<e::two, p_cat_t>;
		static_assert(
				std::is_same<two_idxes, fea::pack_nt<size_t, 2, 5>>::value,
				"pack.cpp : test failed");
#endif

		using one_indexes = fea::pack_idxes_nt_t<e, e::one, p_cat_t>;
		static_assert(
				std::is_same<one_indexes, fea::pack_nt<size_t, 0, 3>>::value,
				"pack.cpp : test failed");

		using three_idxes = fea::pack_idxes_nt_t<e, e::three, p_cat_t>;
		static_assert(
				std::is_same<three_idxes, fea::pack_nt<size_t, 1, 4>>::value,
				"pack.cpp : test failed");

		using two_idxes = fea::pack_idxes_nt_t<e, e::two, p_cat_t>;
		static_assert(
				std::is_same<two_idxes, fea::pack_nt<size_t, 2, 5>>::value,
				"pack.cpp : test failed");

#if FEA_CPP17
		static_assert(fea::pack_contains_nt_v2<e::one, p_cat_t>,
				"pack.cpp : test failed");
		static_assert(fea::pack_contains_nt_v2<e::three, p_cat_t>,
				"pack.cpp : test failed");
		static_assert(fea::pack_contains_nt_v2<e::two, p_cat_t>,
				"pack.cpp : test failed");
		static_assert(!fea::pack_contains_nt_v2<e::four, p_cat_t>,
				"pack.cpp : test failed");
		static_assert(!fea::pack_contains_nt_v2<e::count, p_cat_t>,
				"pack.cpp : test failed");
#endif

		static_assert(fea::pack_contains_nt_v<e, e::one, p_cat_t>,
				"pack.cpp : test failed");
		static_assert(fea::pack_contains_nt_v<e, e::three, p_cat_t>,
				"pack.cpp : test failed");
		static_assert(fea::pack_contains_nt_v<e, e::two, p_cat_t>,
				"pack.cpp : test failed");
		static_assert(!fea::pack_contains_nt_v<e, e::four, p_cat_t>,
				"pack.cpp : test failed");
		static_assert(!fea::pack_contains_nt_v<e, e::count, p_cat_t>,
				"pack.cpp : test failed");
	}
}

TEST(pack, splice) {
	static_assert(std::is_same<fea::idx_splice_t<0, int, double, float, short>,
						  int>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_before_t<0, int, double, float, short>,
					fea::pack<>>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_after_t<0, int, double, float, short>,
					fea::pack<double, float, short>>::value,
			"traits.cpp : test failed");

	static_assert(std::is_same<fea::idx_splice_t<1, int, double, float, short>,
						  double>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_before_t<1, int, double, float, short>,
					fea::pack<int>>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_after_t<1, int, double, float, short>,
					fea::pack<float, short>>::value,
			"traits.cpp : test failed");

	static_assert(std::is_same<fea::idx_splice_t<2, int, double, float, short>,
						  float>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_before_t<2, int, double, float, short>,
					fea::pack<int, double>>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_after_t<2, int, double, float, short>,
					fea::pack<short>>::value,
			"traits.cpp : test failed");

	static_assert(std::is_same<fea::idx_splice_t<3, int, double, float, short>,
						  short>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_before_t<3, int, double, float, short>,
					fea::pack<int, double, float>>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_after_t<3, int, double, float, short>,
					fea::pack<>>::value,
			"traits.cpp : test failed");
}

} // namespace
