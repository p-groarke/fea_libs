#include <fea/meta/if_constexpr.hpp>
#include <gtest/gtest.h>

namespace {
TEST(if_constexpr, basics) {
	constexpr bool doit = true;
	constexpr bool dontdoit = false;

	bool answer = false;

	fea::if_constexpr<doit>(
			[&]() { answer = true; }, [&]() { answer = false; });
	EXPECT_TRUE(answer);

	fea::if_constexpr<dontdoit>(
			[&]() { answer = true; }, [&]() { answer = false; });
	EXPECT_FALSE(answer);

	// fea::if_constexpr<doit>([&]() { answer = true; }).else_if<dontdoit>([&]()
	// { 	answer = false;
	//})();

	// static constexpr auto l = []() {};
}
} // namespace
