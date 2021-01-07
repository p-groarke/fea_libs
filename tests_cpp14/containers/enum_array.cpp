#include <fea/containers/enum_array.hpp>
#include <gtest/gtest.h>

namespace {
TEST(enum_array, basics) {
	enum class e { zero, one, two, three, four, five, count };
	{
		fea::enum_array<size_t, e> arr{};
		EXPECT_EQ(arr.size(), size_t(e::count));

		for (size_t i = 0; i < arr.size(); ++i) {
			arr[e(i)] = i;
		}

		EXPECT_EQ(arr[e::zero], 0u);
		EXPECT_EQ(arr[e::one], 1u);
		EXPECT_EQ(arr[e::two], 2u);
		EXPECT_EQ(arr[e::three], 3u);
		EXPECT_EQ(arr[e::four], 4u);
		EXPECT_EQ(arr[e::five], 5u);

		EXPECT_EQ(arr.at(e::zero), 0u);
		EXPECT_EQ(arr.at(e::one), 1u);
		EXPECT_EQ(arr.at(e::two), 2u);
		EXPECT_EQ(arr.at(e::three), 3u);
		EXPECT_EQ(arr.at(e::four), 4u);
		EXPECT_EQ(arr.at(e::five), 5u);

		EXPECT_EQ(arr.at<e::zero>(), 0u);
		EXPECT_EQ(arr.at<e::one>(), 1u);
		EXPECT_EQ(arr.at<e::two>(), 2u);
		EXPECT_EQ(arr.at<e::three>(), 3u);
		EXPECT_EQ(arr.at<e::four>(), 4u);
		EXPECT_EQ(arr.at<e::five>(), 5u);

#if FEA_CPP17
		EXPECT_EQ(fea::get<e::zero>(arr), 0u);
		EXPECT_EQ(fea::get<e::one>(arr), 1u);
		EXPECT_EQ(fea::get<e::two>(arr), 2u);
		EXPECT_EQ(fea::get<e::three>(arr), 3u);
		EXPECT_EQ(fea::get<e::four>(arr), 4u);
		EXPECT_EQ(fea::get<e::five>(arr), 5u);
#endif
	}

	{
		fea::enum_array<size_t, e, size_t(e::count)> arr{};
		EXPECT_EQ(arr.size(), size_t(e::count));

		for (size_t i = 0; i < arr.size(); ++i) {
			arr[e(i)] = i;
		}

		EXPECT_EQ(arr[e::zero], 0u);
		EXPECT_EQ(arr[e::one], 1u);
		EXPECT_EQ(arr[e::two], 2u);
		EXPECT_EQ(arr[e::three], 3u);
		EXPECT_EQ(arr[e::four], 4u);
		EXPECT_EQ(arr[e::five], 5u);

		EXPECT_EQ(arr.at(e::zero), 0u);
		EXPECT_EQ(arr.at(e::one), 1u);
		EXPECT_EQ(arr.at(e::two), 2u);
		EXPECT_EQ(arr.at(e::three), 3u);
		EXPECT_EQ(arr.at(e::four), 4u);
		EXPECT_EQ(arr.at(e::five), 5u);

		EXPECT_EQ(arr.at<e::zero>(), 0u);
		EXPECT_EQ(arr.at<e::one>(), 1u);
		EXPECT_EQ(arr.at<e::two>(), 2u);
		EXPECT_EQ(arr.at<e::three>(), 3u);
		EXPECT_EQ(arr.at<e::four>(), 4u);
		EXPECT_EQ(arr.at<e::five>(), 5u);

#if FEA_CPP17
		EXPECT_EQ(fea::get<e::zero>(arr), 0u);
		EXPECT_EQ(fea::get<e::one>(arr), 1u);
		EXPECT_EQ(fea::get<e::two>(arr), 2u);
		EXPECT_EQ(fea::get<e::three>(arr), 3u);
		EXPECT_EQ(fea::get<e::four>(arr), 4u);
		EXPECT_EQ(fea::get<e::five>(arr), 5u);
#endif
	}
}

template <auto K, auto V>
struct testit {};

// template <auto K, auto V, template <auto, auto> class test>
// void func(test<K, V>) {
//}
template <auto K, auto V>
void func(testit<K, V>) {
}

template <class...>
struct tup;

template <auto... K, auto... V>
struct tup<testit<K, V>...> {
	// template <auto... K2, auto... V2>
	// tup(testit<K2, V2>... arg) {
	//}
};

template <auto K, auto V>
constexpr auto make_kv() {
}

template <auto... K, auto... V>
constexpr auto make_tup(testit<K, V>...) {
	return tup<testit<K, V>...>{};
}


TEST(enum_array, make_enum_array) {
	enum class e { zero, one, two, three, four, five, count };

	// fea::non_type_type_pair t = { e::zero, true };
	// auto arr = fea::make_enum_array<e>({ e::zero, true });

	// testit t = { e::zero };

	func(testit<e::one, 42>{});

	// tup<testit<e::one, true>, testit<e::two, false>> t;
	// tup t{ testit<e::one, true>{}, testit<e::two, false>{} };

	tup t = make_tup(testit<e::one, true>{}, testit<e::two, false>{});

	// auto k = fea::kv_t<e::one>{ true };

	// auto t2 = fea::make_enum_array<e>(
	//		fea::kv<e::one>(true), fea::kv<e::two>(true));

	// auto t3 = fea::make_enum_array<e>(
	//		fea::kv_nt<e::one, true>{}, fea::kv_nt<e::two, true>{});
}
} // namespace
