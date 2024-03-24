#include <fea/meta/return_overload.hpp>
#include <fea/numerics/literals.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>
#include <string>
#include <string_view>

using namespace fea::literals;

#if FEA_MACOS
// Clang complains about braces around lambdas, which is a bug (potentially
// fixed in recent versions).
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif

namespace {
#if !FEA_MACOS
#define DISABLE_MAC(...) __VA_ARGS__
#else
#define DISABLE_MAC(...)
#endif

auto example_func() {
	// clang-format off
	return fea::return_overload{
		// When overloading with both char and std::string,
		// use signed char to disambiguate std::string operator=.
		// On apple-clang, this doesn't work, which means
		// you cannot mix std::string and chars in that situation.
		[]() -> signed char { return 0; },
		[]() -> unsigned char { return 1; },
		[]() -> short { return 2; },
		[]() -> unsigned short { return 3; },
		[]() -> int { return 4; },
		[]() -> unsigned int { return 5; },
		[]() -> float { return 6.f; },
		[]() -> std::string { return "string"; },
	};
	// clang-format on
}

TEST(return_overload, basics) {
	signed char c = example_func();
	unsigned char uc = example_func();
	short s = example_func();
	unsigned short us = example_func();
	int i = example_func();
	unsigned int ui = example_func();
	float f = example_func();
	std::string str = example_func();

	EXPECT_EQ(c, 0_i8);
	EXPECT_EQ(uc, 1_u8);
	EXPECT_EQ(s, 2_i16);
	EXPECT_EQ(us, 3_u16);
	EXPECT_EQ(i, 4_i32);
	EXPECT_EQ(ui, 5_u32);
	EXPECT_EQ(f, 6.f);
	EXPECT_EQ(str, "string");

	// operator=
	c = example_func();
	uc = example_func();
	s = example_func();
	us = example_func();
	i = example_func();
	ui = example_func();
	f = example_func();
#if !FEA_MACOS
	str = example_func();
#endif

	EXPECT_EQ(c, 0_i8);
	EXPECT_EQ(uc, 1_u8);
	EXPECT_EQ(s, 2_i16);
	EXPECT_EQ(us, 3_u16);
	EXPECT_EQ(i, 4_i32);
	EXPECT_EQ(ui, 5_u32);
	EXPECT_EQ(f, 6.f);
#if !FEA_MACOS
	EXPECT_EQ(str, "string");
#endif
}

auto example_func(int mi) {
	return fea::return_overload{
		[mi]() -> signed char { return 0 + (signed char)(mi); },
		[mi]() -> unsigned char { return 1 + (unsigned char)(mi); },
		[mi]() -> short { return 2 + short(mi); },
		[mi]() -> unsigned short { return 3 + (unsigned short)(mi); },
		[mi]() -> int { return 4 + mi; },
		[mi]() -> unsigned int { return 5 + (unsigned char)(mi); },
		[mi]() -> float { return 6.f + float(mi); },
		[mi]() -> std::string { return "string" + std::to_string(mi); },
	};
}

TEST(return_overload, args) {
	signed char c = example_func(42);
	unsigned char uc = example_func(42);
	short s = example_func(42);
	unsigned short us = example_func(42);
	int i = example_func(42);
	unsigned int ui = example_func(42);
	float f = example_func(42);
	std::string str = example_func(42);

	EXPECT_EQ(c, 42_i8);
	EXPECT_EQ(uc, 43_u8);
	EXPECT_EQ(s, 44_i16);
	EXPECT_EQ(us, 45_u16);
	EXPECT_EQ(i, 46_i32);
	EXPECT_EQ(ui, 47_u32);
	EXPECT_EQ(f, 48.f);
	EXPECT_EQ(str, "string42");

	// operator=
	c = example_func(42);
	uc = example_func(42);
	s = example_func(42);
	us = example_func(42);
	i = example_func(42);
	ui = example_func(42);
	f = example_func(42);
#if !FEA_MACOS
	str = example_func(42);
#endif

	EXPECT_EQ(c, 42_i8);
	EXPECT_EQ(uc, 43_u8);
	EXPECT_EQ(s, 44_i16);
	EXPECT_EQ(us, 45_u16);
	EXPECT_EQ(i, 46_i32);
	EXPECT_EQ(ui, 47_u32);
	EXPECT_EQ(f, 48.f);
#if !FEA_MACOS
	EXPECT_EQ(str, "string42");
#endif
}

struct obj {
	auto func() const {
		return fea::return_overload{
			[this]() -> const signed char& { return c; },
			[this]() -> const unsigned char& { return uc; },
			[this]() -> const short& { return s; },
			[this]() -> const unsigned short& { return us; },
			[this]() -> const int& { return i; },
			[this]() -> const unsigned int& { return ui; },
			[this]() -> const float& { return f; },
			[this]() -> const std::string& { return str; },
		};
	}
	auto func() {
		return fea::return_overload{
			[this]() -> signed char& {
				c += 10_i8;
				return c;
			},
			[this]() -> unsigned char& {
				uc += 10_u8;
				return uc;
			},
			[this]() -> short& {
				s += 10_i16;
				return s;
			},
			[this]() -> unsigned short& {
				us += 10_u16;
				return us;
			},
			[this]() -> int& {
				i += 10_i32;
				return i;
			},
			[this]() -> unsigned int& {
				ui += 10_u32;
				return ui;
			},
			[this]() -> float& {
				f += 10.f;
				return f;
			},
			[this]() -> std::string& {
				str += " string";
				return str;
			},
		};
	}

	auto const_overload_func() const {
		return fea::return_overload{
			[this]() -> const signed char& { return c; },
			[this]() -> const unsigned char& { return uc; },
			[this]() -> const short& { return s; },
			[this]() -> const unsigned short& { return us; },
			[this]() -> const int& { return i; },
			[this]() -> const unsigned int& { return ui; },
			[this]() -> const float& { return f; },
			[this]() -> const std::string& { return str; },
		};
	}

	auto const_overload_func() {
		// This simply tests const overloading.
		// However, in this situation, the mutable version will always be
		// picked on the return type.
		return fea::return_overload{
			[this]() -> const signed char& { return c; },
			[this]() -> const unsigned char& { return uc; },
			[this]() -> const short& { return s; },
			[this]() -> const unsigned short& { return us; },
			[this]() -> const int& { return i; },
			[this]() -> const unsigned int& { return ui; },
			[this]() -> const float& { return f; },
			[this]() -> const std::string& { return str; },
			[this]() mutable -> signed char& {
				c += 10_i8;
				return c;
			},
			[this]() mutable -> unsigned char& {
				uc += 10_u8;
				return uc;
			},
			[this]() mutable -> short& {
				s += 10_i16;
				return s;
			},
			[this]() mutable -> unsigned short& {
				us += 10_u16;
				return us;
			},
			[this]() mutable -> int& {
				i += 10_i32;
				return i;
			},
			[this]() mutable -> unsigned int& {
				ui += 10_u32;
				return ui;
			},
			[this]() mutable -> float& {
				f += 10.f;
				return f;
			},
			[this]() mutable -> std::string& {
				str += " string";
				return str;
			},
		};
	}

	signed char c = 0_i8;
	unsigned char uc = 1_u8;
	short s = 2_i16;
	unsigned short us = 3_u16;
	int i = 4_32;
	unsigned int ui = 5_u32;
	float f = 6.f;
	std::string str = "string";
};

TEST(return_overload, struct_basics) {
	{
		const obj o;

		signed char c = o.func();
		unsigned char uc = o.func();
		short s = o.func();
		unsigned short us = o.func();
		int i = o.func();
		unsigned int ui = o.func();
		float f = o.func();
		std::string str = o.func();

		EXPECT_EQ(c, 0_i8);
		EXPECT_EQ(uc, 1_u8);
		EXPECT_EQ(s, 2_i16);
		EXPECT_EQ(us, 3_u16);
		EXPECT_EQ(i, 4_i32);
		EXPECT_EQ(ui, 5_u32);
		EXPECT_EQ(f, 6.f);
		EXPECT_EQ(str, "string");

		// operator=
		c = o.func();
		uc = o.func();
		s = o.func();
		us = o.func();
		i = o.func();
		ui = o.func();
		f = o.func();
#if !FEA_MACOS
		str = o.func();
#endif

		EXPECT_EQ(c, 0_i8);
		EXPECT_EQ(uc, 1_u8);
		EXPECT_EQ(s, 2_i16);
		EXPECT_EQ(us, 3_u16);
		EXPECT_EQ(i, 4_i32);
		EXPECT_EQ(ui, 5_u32);
		EXPECT_EQ(f, 6.f);
#if !FEA_MACOS
		EXPECT_EQ(str, "string");
#endif
	}

	{
		obj o;

		signed char c = o.func();
		unsigned char uc = o.func();
		short s = o.func();
		unsigned short us = o.func();
		int i = o.func();
		unsigned int ui = o.func();
		float f = o.func();
		std::string str = o.func();

		EXPECT_EQ(c, 10_i8);
		EXPECT_EQ(uc, 11_u8);
		EXPECT_EQ(s, 12_i16);
		EXPECT_EQ(us, 13_u16);
		EXPECT_EQ(i, 14_i32);
		EXPECT_EQ(ui, 15_u32);
		EXPECT_EQ(f, 16.f);
		EXPECT_EQ(str, "string string");

		EXPECT_EQ(o.c, 10_i8);
		EXPECT_EQ(o.uc, 11_u8);
		EXPECT_EQ(o.s, 12_i16);
		EXPECT_EQ(o.us, 13_u16);
		EXPECT_EQ(o.i, 14_i32);
		EXPECT_EQ(o.ui, 15_u32);
		EXPECT_EQ(o.f, 16.f);
		EXPECT_EQ(o.str, "string string");

		// operator=
		c = o.func();
		uc = o.func();
		s = o.func();
		us = o.func();
		i = o.func();
		ui = o.func();
		f = o.func();
#if !FEA_MACOS
		str = o.func();
#endif

		EXPECT_EQ(c, 20_i8);
		EXPECT_EQ(uc, 21_u8);
		EXPECT_EQ(s, 22_i16);
		EXPECT_EQ(us, 23_u16);
		EXPECT_EQ(i, 24_i32);
		EXPECT_EQ(ui, 25_u32);
		EXPECT_EQ(f, 26.f);
#if !FEA_MACOS
		EXPECT_EQ(str, "string string string");
#endif

		EXPECT_EQ(o.c, 20_i8);
		EXPECT_EQ(o.uc, 21_u8);
		EXPECT_EQ(o.s, 22_i16);
		EXPECT_EQ(o.us, 23_u16);
		EXPECT_EQ(o.i, 24_i32);
		EXPECT_EQ(o.ui, 25_u32);
		EXPECT_EQ(o.f, 26.f);
#if !FEA_MACOS
		EXPECT_EQ(o.str, "string string string");
#endif
	}
}

TEST(return_overload, const_overload) {
	obj o;

	{
		const obj& co = o;
		const signed char& c = co.const_overload_func();
		const unsigned char& uc = co.const_overload_func();
		const short& s = co.const_overload_func();
		const unsigned short& us = co.const_overload_func();
		const int& i = co.const_overload_func();
		const unsigned int& ui = co.const_overload_func();
		const float& f = co.const_overload_func();
		const std::string& str = co.const_overload_func();

		EXPECT_EQ(c, 0_i8);
		EXPECT_EQ(uc, 1_u8);
		EXPECT_EQ(s, 2_i16);
		EXPECT_EQ(us, 3_u16);
		EXPECT_EQ(i, 4_i32);
		EXPECT_EQ(ui, 5_u32);
		EXPECT_EQ(f, 6.f);
		EXPECT_EQ(str, "string");
	}

	{
		signed char& c = o.const_overload_func();
		unsigned char& uc = o.const_overload_func();
		short& s = o.const_overload_func();
		unsigned short& us = o.const_overload_func();
		int& i = o.const_overload_func();
		unsigned int& ui = o.const_overload_func();
		float& f = o.const_overload_func();
		std::string& str = o.const_overload_func();

		EXPECT_EQ(c, 10_i8);
		EXPECT_EQ(uc, 11_u8);
		EXPECT_EQ(s, 12_i16);
		EXPECT_EQ(us, 13_u16);
		EXPECT_EQ(i, 14_i32);
		EXPECT_EQ(ui, 15_u32);
		EXPECT_EQ(f, 16.f);
		EXPECT_EQ(str, "string string");

		EXPECT_EQ(o.c, 10_i8);
		EXPECT_EQ(o.uc, 11_u8);
		EXPECT_EQ(o.s, 12_i16);
		EXPECT_EQ(o.us, 13_u16);
		EXPECT_EQ(o.i, 14_i32);
		EXPECT_EQ(o.ui, 15_u32);
		EXPECT_EQ(o.f, 16.f);
		EXPECT_EQ(o.str, "string string");
	}
}

TEST(return_overload, string_stringview) {
	auto r = fea::return_overload{
		[]() -> std::string_view { return "sv"; },
		[]() -> std::string { return "string"; },
	};

	std::string_view sv = r;
	std::string str = r;

	EXPECT_EQ(sv, "sv");
	EXPECT_EQ(str, "string");
}
} // namespace

#if FEA_MACOS
#pragma clang diagnostic pop
#endif
