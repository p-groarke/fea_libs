#include <fea/utility/platform.hpp>
#include <gtest/gtest.h>

namespace {
#define FAIL_MSG "platform.cpp : unit test failed"

FEA_PACKED(template <class T> struct packed {
	// comments, with commas
	int a, b;
	/* more comments */
	T t{};
});

template <class T>
struct unpacked {
	int a, b;
	T t{};
};

TEST(platform, packed) {
	static_assert(sizeof(packed<char>) < sizeof(unpacked<char>), FAIL_MSG);
	static_assert(sizeof(packed<char>) == 9, FAIL_MSG);
}
} // namespace
