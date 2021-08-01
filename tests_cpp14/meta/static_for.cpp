#include <fea/meta/static_for.hpp>
#include <gtest/gtest.h>

namespace {

template <class... Args>
int foldit(Args... args) {
	int ret = 0;
	auto add = [&](auto i) { ret += i; };
	fea::fold(add, args...);
	return ret;
}

TEST(static_for, basics) {
	{
		int ans = foldit(0, 1, 2, 3);
		EXPECT_EQ(ans, 6);
	}

	{
		int ans = 0;
		fea::static_for<4>([&](auto v) { ans += int(v()); });
		EXPECT_EQ(ans, 6);
	}

	{
		int ans = 0;
		fea::apply_indexes<4>([&](auto... ic) {
			auto l = [&](auto v) { ans += int(v()); };

			fea::fold(l, ic...);
		});
		EXPECT_EQ(ans, 6);
	}
}
} // namespace
