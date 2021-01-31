#include <fea/utils/scope.hpp>
#include <gtest/gtest.h>

namespace {
TEST(scope, basics) {
	size_t test_var = 0;

	{
		fea::on_exit e{ [&]() { ++test_var; } };
	}
	EXPECT_EQ(test_var, 1u);
}

} // namespace
