#include <fea/functional/callback.hpp>
#include <gtest/gtest.h>

namespace {
float test_func(int, double) {
	return 0.f;
}

template <class Func>
float my_callback(const fea::callback<Func, float(int, double)>& sig) {
	return sig(42, 42.0);
}

TEST(callback, basics) {
	auto a = fea::make_callback([](int, double) -> float { return 42.f; });
	auto a2 = fea::make_callback(&test_func);

	float ret = my_callback(a);
	EXPECT_EQ(ret, 42.f);

	ret = my_callback(a2);
	EXPECT_EQ(ret, 0.f);

	ret = my_callback(
			fea::make_callback([](int, double) -> float { return -42.f; }));
	EXPECT_EQ(ret, -42.f);

	ret = my_callback(fea::make_callback(&test_func));
	EXPECT_EQ(ret, 0.f);

	fea::callback t2([](int, double) -> float { return 17.f; });
	fea::callback t3(&test_func);

	ret = my_callback(t2);
	EXPECT_EQ(ret, 17.f);

	ret = my_callback(t3);
	EXPECT_EQ(ret, 0.f);

	ret = my_callback(
			fea::callback{ [](int, double) -> float { return -17.f; } });
	EXPECT_EQ(ret, -17.f);

	ret = my_callback(fea::callback{ &test_func });
	EXPECT_EQ(ret, 0.f);


	ret = my_callback(fea::callback{ [](int, double) { return 42.f; } });
	EXPECT_EQ(ret, 42.f);

	ret = my_callback(fea::callback{ [v = 1.f](int, double) { return v; } });
	EXPECT_EQ(ret, 1.f);
}
} // namespace
