#include <fea/numerics/clamp_cast.hpp>
#include <gtest/gtest.h>

namespace {
using namespace fea;

#define test_hi_to_lo(T, U) \
	{ \
		T t = std::numeric_limits<T>::lowest(); \
		U u = clamp_cast<U>(t); \
		EXPECT_EQ(u, std::numeric_limits<U>::lowest()); \
\
		t = std::numeric_limits<T>::max(); \
		u = clamp_cast<U>(t); \
		EXPECT_EQ(u, std::numeric_limits<U>::max()); \
	}

#define test_lo_to_hi(U, T) \
	{ \
		T t = std::numeric_limits<T>::lowest(); \
		U u = clamp_cast<U>(t); \
		EXPECT_EQ(u, std::numeric_limits<U>::lowest()); \
\
		t = std::numeric_limits<T>::max(); \
		u = clamp_cast<U>(t); \
		EXPECT_EQ(u, std::numeric_limits<U>::max()); \
	}

#define test_lo_to_hi_unsigned(U, T) \
	{ \
		T t = std::numeric_limits<T>::lowest(); \
		U u = clamp_cast<U>(t); \
		EXPECT_EQ(u, U{ 0 }); \
\
		t = std::numeric_limits<T>::max(); \
		u = clamp_cast<U>(t); \
		EXPECT_EQ(u, std::numeric_limits<U>::max()); \
	}

#define test_hi_unsigned_to_lo(T, U) \
	{ \
		U u = std::numeric_limits<U>::lowest(); \
		T t = clamp_cast<T>(u); \
		EXPECT_EQ(t, T{ 0 }); \
\
		t = std::numeric_limits<T>::max(); \
		u = clamp_cast<U>(t); \
		EXPECT_EQ(u, std::numeric_limits<U>::max()); \
	}


template <class T, class U>
void debug_hi_to_lo() {
	T t = std::numeric_limits<T>::lowest();
	U u = clamp_cast<U>(t);
	EXPECT_EQ(u, std::numeric_limits<U>::lowest());

	t = std::numeric_limits<T>::max();
	u = clamp_cast<U>(t);
	EXPECT_EQ(u, std::numeric_limits<U>::max());
}

template <class T, class U>
void debug_hi_unsigned_to_lo() {
	U u = std::numeric_limits<U>::lowest();
	T t = clamp_cast<T>(u);
	EXPECT_EQ(t, T{ 0 });

	t = std::numeric_limits<T>::max();
	u = clamp_cast<U>(t);
	EXPECT_EQ(u, std::numeric_limits<U>::max());
}

template <class U, class T>
void debug_lo_to_hi() {
	T t = std::numeric_limits<T>::lowest();
	U u = clamp_cast<U>(t);
	EXPECT_EQ(u, std::numeric_limits<U>::lowest());

	t = std::numeric_limits<T>::max();
	u = clamp_cast<U>(t);
	EXPECT_EQ(u, std::numeric_limits<U>::max());
}

template <class U, class T>
void debug_lo_to_hi_unsigned() {
	U u = std::numeric_limits<U>::lowest();
	T t = clamp_cast<T>(u);
	EXPECT_EQ(t, T{ 0 });

	t = std::numeric_limits<T>::max();
	u = clamp_cast<U>(t);
	EXPECT_EQ(u, std::numeric_limits<U>::max());
}

TEST(clamp_cast, example) {
	char c = -42;
	unsigned char uc = fea::clamp_cast<unsigned char>(c); // 0
	EXPECT_EQ(uc, 0u);

	uc = 255;
	c = fea::clamp_cast<char>(uc); // 127
	EXPECT_EQ(c, 127);
}

TEST(clamp_cast, uint_to_int) {
	test_hi_unsigned_to_lo(uint64_t, int64_t);
	test_hi_unsigned_to_lo(uint64_t, int32_t);
	test_hi_unsigned_to_lo(uint64_t, int16_t);
	test_hi_unsigned_to_lo(uint64_t, int8_t);

	test_lo_to_hi(uint32_t, int64_t);
	test_hi_unsigned_to_lo(uint32_t, int32_t);
	test_hi_unsigned_to_lo(uint32_t, int16_t);
	test_hi_unsigned_to_lo(uint32_t, int8_t);

	test_lo_to_hi(uint16_t, int64_t);
	test_lo_to_hi(uint16_t, int32_t);
	test_hi_unsigned_to_lo(uint16_t, int16_t);
	test_hi_unsigned_to_lo(uint16_t, int8_t);

	test_lo_to_hi(uint8_t, int64_t);
	test_lo_to_hi(uint8_t, int32_t);
	test_lo_to_hi(uint8_t, int16_t);
	test_hi_unsigned_to_lo(uint8_t, int8_t);
}

TEST(clamp_cast, int_to_int) {
	test_hi_to_lo(int64_t, int32_t);
	test_hi_to_lo(int64_t, int16_t);
	test_hi_to_lo(int64_t, int8_t);

	test_lo_to_hi(int32_t, int64_t);
	test_hi_to_lo(int32_t, int16_t);
	test_hi_to_lo(int32_t, int8_t);

	test_lo_to_hi(int16_t, int64_t);
	test_lo_to_hi(int16_t, int32_t);
	test_hi_to_lo(int16_t, int8_t);

	test_lo_to_hi(int8_t, int64_t);
	test_lo_to_hi(int8_t, int32_t);
	test_lo_to_hi(int8_t, int16_t);
}

TEST(clamp_cast, float_to_float) {
	test_hi_to_lo(long double, double);
	test_hi_to_lo(long double, float);

	test_lo_to_hi(double, long double);
	test_hi_to_lo(double, float);

	test_lo_to_hi(float, long double);
	test_lo_to_hi(float, double);
}

TEST(clamp_cast, float_to_int) {
	test_hi_to_lo(long double, int64_t);
	test_hi_to_lo(long double, int32_t);
	test_hi_to_lo(long double, int16_t);
	test_hi_to_lo(long double, int8_t);

	test_hi_to_lo(double, int64_t);
	test_hi_to_lo(double, int32_t);
	test_hi_to_lo(double, int16_t);
	test_hi_to_lo(double, int8_t);

	test_hi_to_lo(float, int64_t);
	test_hi_to_lo(float, int32_t);
	test_hi_to_lo(float, int16_t);
	test_hi_to_lo(float, int8_t);
}

TEST(clamp_cast, int_to_float) {
	test_lo_to_hi(int64_t, long double);
	test_lo_to_hi(int64_t, double);
	test_lo_to_hi(int64_t, float);

	test_lo_to_hi(int32_t, long double);
	test_lo_to_hi(int32_t, double);
	test_lo_to_hi(int32_t, float);

	test_lo_to_hi(int16_t, long double);
	test_lo_to_hi(int16_t, double);
	test_lo_to_hi(int16_t, float);

	test_lo_to_hi(int8_t, long double);
	test_lo_to_hi(int8_t, double);
	test_lo_to_hi(int8_t, float);
}

TEST(clamp_cast, uint_to_uint) {
	test_hi_to_lo(uint64_t, uint32_t);
	test_hi_to_lo(uint64_t, uint16_t);
	test_hi_to_lo(uint64_t, uint8_t);

	test_lo_to_hi(uint32_t, uint64_t);
	test_hi_to_lo(uint32_t, uint16_t);
	test_hi_to_lo(uint32_t, uint8_t);

	test_lo_to_hi(uint16_t, uint64_t);
	test_lo_to_hi(uint16_t, uint32_t);
	test_hi_to_lo(uint16_t, uint8_t);

	test_lo_to_hi(uint8_t, uint64_t);
	test_lo_to_hi(uint8_t, uint32_t);
	test_lo_to_hi(uint8_t, uint16_t);
}

TEST(clamp_cast, int_to_uint) {
	test_lo_to_hi_unsigned(int64_t, uint64_t);
	test_hi_to_lo(int64_t, uint32_t);
	test_hi_to_lo(int64_t, uint16_t);
	test_hi_to_lo(int64_t, uint8_t);

	test_lo_to_hi_unsigned(int32_t, uint64_t);
	test_lo_to_hi_unsigned(int32_t, uint32_t);
	test_hi_to_lo(int32_t, uint16_t);
	test_hi_to_lo(int32_t, uint8_t);

	test_lo_to_hi_unsigned(int16_t, uint64_t);
	test_lo_to_hi_unsigned(int16_t, uint32_t);
	test_lo_to_hi_unsigned(int16_t, uint16_t);
	test_hi_to_lo(int16_t, uint8_t);

	test_lo_to_hi_unsigned(int8_t, uint64_t);
	test_lo_to_hi_unsigned(int8_t, uint32_t);
	test_lo_to_hi_unsigned(int8_t, uint16_t);
	test_lo_to_hi_unsigned(int8_t, uint8_t);
}

TEST(clamp_cast, float_to_uint) {
	test_hi_to_lo(long double, uint64_t);
	test_hi_to_lo(long double, uint32_t);
	test_hi_to_lo(long double, uint16_t);
	test_hi_to_lo(long double, uint8_t);

	test_hi_to_lo(double, uint64_t);
	test_hi_to_lo(double, uint32_t);
	test_hi_to_lo(double, uint16_t);
	test_hi_to_lo(double, uint8_t);

	test_hi_to_lo(float, uint64_t);
	test_hi_to_lo(float, uint32_t);
	test_hi_to_lo(float, uint16_t);
	test_hi_to_lo(float, uint8_t);
}

TEST(clamp_cast, uint_to_float) {
	test_lo_to_hi(uint64_t, long double);
	test_lo_to_hi(uint64_t, double);
	test_lo_to_hi(uint64_t, float);

	test_lo_to_hi(uint32_t, long double);
	test_lo_to_hi(uint32_t, double);
	test_lo_to_hi(uint32_t, float);

	test_lo_to_hi(uint16_t, long double);
	test_lo_to_hi(uint16_t, double);
	test_lo_to_hi(uint16_t, float);

	test_lo_to_hi(uint8_t, long double);
	test_lo_to_hi(uint8_t, double);
	test_lo_to_hi(uint8_t, float);
}
} // namespace
