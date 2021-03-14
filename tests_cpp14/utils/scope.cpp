#include <fea/utils/platform.hpp>
#include <fea/utils/scope.hpp>
#include <gtest/gtest.h>

namespace {
TEST(scope, basics) {
	size_t test_var = 0;

	{
		auto e = fea::make_on_exit([&]() { ++test_var; });
	}
	EXPECT_EQ(test_var, 1u);

#if FEA_CPP17
	{
		fea::on_exit e{ [&]() { ++test_var; } };
	}
	EXPECT_EQ(test_var, 2u);
#endif
}

} // namespace
