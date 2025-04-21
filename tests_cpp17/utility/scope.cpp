#include <fea/utility/platform.hpp>
#include <fea/utility/scope.hpp>
#include <gtest/gtest.h>

namespace {
TEST(scope, basics) {
	size_t test_var = 0;

	{
		auto e = fea::make_on_exit([&]() { ++test_var; });
	}
	EXPECT_EQ(test_var, 1u);

	{
		fea::on_exit e{ [&]() { ++test_var; } };
	}
	EXPECT_EQ(test_var, 2u);
}

} // namespace
