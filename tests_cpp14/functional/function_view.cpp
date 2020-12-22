#include <fea/functional/function_view.hpp>
#include <gtest/gtest.h>

namespace {
TEST(function_view, basics) {
	bool test_passed = false;
	{
		fea::function_view<void()> fn = [&]() { test_passed = true; };
		fn();
		EXPECT_TRUE(test_passed);
	}

	test_passed = false;
	{
		fea::function_view<void(int)> fn = [&, val = 42](int i) {
			if (val == i) {
				test_passed = true;
			}
		};
		fn(42);
		EXPECT_TRUE(test_passed);
	}
}
} // namespace
