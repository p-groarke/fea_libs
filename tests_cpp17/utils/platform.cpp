#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>

namespace {
#define fail_msg "platform.cpp : unit test failed"

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
	static_assert(sizeof(packed<char>) < sizeof(unpacked<char>), fail_msg);
	static_assert(sizeof(packed<char>) == 9, fail_msg);
}
} // namespace
