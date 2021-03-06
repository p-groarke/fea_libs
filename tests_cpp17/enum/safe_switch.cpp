#include <fea/enum/safe_switch.hpp>
#include <gtest/gtest.h>

namespace {
TEST(enum, safe_switch) {
	enum class e {
		one,
		two,
		three,
		four,
		count,
	};

	int result = 0;
	auto switcher = fea::safe_switch<e>()
							.case_<e::one>([&]() { result = 1; })
							.case_<e::three>([&]() { result = 3; })
							.case_<e::two>([&]() { result = 2; })
							.case_<e::four>([&]() { result = 4; });
	switcher(e::one);
	EXPECT_EQ(result, 1);

	switcher(e::three);
	EXPECT_EQ(result, 3);

	switcher(e::two);
	EXPECT_EQ(result, 2);

	switcher(e::four);
	EXPECT_EQ(result, 4);

#if FEA_DEBUG
	EXPECT_DEATH(switcher(e::count), "");
#endif

	// Using operator () example.
	fea::safe_switch<e>()
			.case_<e::one>([&]() { result = 1; })
			.case_<e::three>([&]() { result = 3; })
			.case_<e::two>([&]() { result = 2; })
			.case_<e::four>([&]() { result = 4; })(e::three);

	EXPECT_EQ(result, 3);
}
} // namespace
