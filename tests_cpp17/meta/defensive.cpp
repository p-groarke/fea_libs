#include <cstdio>
#include <fea/meta/defensive.hpp>

namespace {
#define FAIL_MSG "defensive.cpp : Test failed."

struct test1 {
	~test1() {
	}
	test1(const test1&) {
	}
	test1(test1&&) noexcept {
	}
	test1& operator=(const test1&) {
		return *this;
	}
	test1& operator=(test1&&) noexcept {
		return *this;
	}
};
static_assert(fea::fulfills_rule_of_5<test1>(), FAIL_MSG);

struct test2 {
	~test2() = default;
	test2(const test2&) = default;
	test2(test2&&) = default;
	test2& operator=(const test2&) = default;
	test2& operator=(test2&&) = default;
};
static_assert(fea::fulfills_rule_of_5<test2>(), FAIL_MSG);
static_assert(fea::fulfills_fast_vector<test2>(), FAIL_MSG);

struct test3 {};
static_assert(fea::fulfills_rule_of_5<test3>(), FAIL_MSG);
static_assert(fea::fulfills_fast_vector<test3>(), FAIL_MSG);

struct test4 {
	test4() {
	}
	~test4() = default;
	test4(const test4&) = default;
	test4(test4&&) = default;
	test4& operator=(const test4&) = default;
	test4& operator=(test4&&) = default;
};
static_assert(fea::fulfills_rule_of_5<test4>(), FAIL_MSG);
static_assert(fea::fulfills_fast_vector<test4>(), FAIL_MSG);

struct test5 {
	test5() = default;
	~test5() {
	}
	test5(const test5&) {
	}
	test5(test5&&) noexcept {
	}
	test5& operator=(const test5&) {
		return *this;
	}
	test5& operator=(test5&&) noexcept {
		return *this;
	}
};
static_assert(fea::fulfills_rule_of_5<test5>(), FAIL_MSG);

struct test6 {};
static_assert(fea::fulfills_fast_vector<test6>(), FAIL_MSG);

struct test7 {
	~test7() = default;
	test7(const test7&) = default;
	test7(test7&&) = delete;
};
static_assert(fea::fulfills_fast_vector<test7>(), FAIL_MSG);

struct test8 {
	test8(const test8&) = delete;
	test8(test8&&) noexcept {
	}
};
static_assert(fea::fulfills_fast_vector<test8>(), FAIL_MSG);

struct test9 {
	~test9() = default;
	test9(const test9&) = delete;
	test9(test9&&) = default;
	test9& operator=(const test9&) = delete;
	test9& operator=(test9&&) = default;
};
static_assert(fea::fulfills_move_only<test9>(), FAIL_MSG);

struct test10 {
	test10(test10&&) = default;
	test10& operator=(test10&&) = default;
};
static_assert(fea::fulfills_move_only<test10>(), FAIL_MSG);

struct test11 {
	test11(test11&&) noexcept {
	}
	test11& operator=(test11&&) noexcept {
		return *this;
	}
};
static_assert(fea::fulfills_move_only<test11>(), FAIL_MSG);

struct test12 {
	test12() = delete;
	~test12() = delete;
	test12(const test12&) = delete;
	test12(test12&&) = delete;
	test12& operator=(const test12&) = delete;
	test12& operator=(test12&&) = delete;
};
static_assert(fea::fulfills_non_constructible<test12>(), FAIL_MSG);
} // namespace
