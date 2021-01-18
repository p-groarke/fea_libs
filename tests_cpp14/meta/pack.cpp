#include <array>
#include <fea/meta/pack.hpp>
#include <gtest/gtest.h>

namespace {
TEST(pack, basics) {

	{
		constexpr fea::pack<int, double, float> p1
				= fea::make_pack(42, 42.0, 42.f);
		constexpr auto p2(p1);

		constexpr fea::pack<int, double, float, int, double, float> p_cat
				= fea::pack_cat(p1, p2);
		constexpr auto p_cat2 = fea::pack_cat(p1, p2);

		static_assert(std::is_same<decltype(p_cat), decltype(p_cat2)>::value,
				"pack.cpp : test failed");
	}

	{
		enum class e {
			one,
			two,
			three,
			four,
			count,
		};

		constexpr fea::pack_nt<e, e::one, e::three> p1;
		constexpr auto p2(p1);

		constexpr fea::pack_nt<e, e::one, e::three, e::one, e::three> p_cat
				= fea::pack_cat(p1, p2);
		constexpr auto p_cat2 = fea::pack_cat(p1, p2);

		static_assert(std::is_same<decltype(p_cat), decltype(p_cat2)>::value,
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
