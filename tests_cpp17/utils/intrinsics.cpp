#include <fea/meta/static_for.hpp>
#include <fea/meta/tuple.hpp>
#include <fea/utils/intrinsics.hpp>
#include <gtest/gtest.h>
#include <tuple>

namespace {
constexpr auto gen_tup(int val) {
	return std::make_tuple(char(val), (unsigned char)(val), short(val),
			(unsigned short)(val), int(val), (unsigned int)(val), long(val),
			(unsigned long)(val), (long long)(val), (unsigned long long)(val),
			char16_t(val), char32_t(val), wchar_t(val), int8_t(val),
			int16_t(val), int32_t(val), int64_t(val), uint8_t(val),
			uint16_t(val), uint32_t(val), uint64_t(val)
#if FEA_CPP20
												  ,
			char8_t(val)
#endif
	);
}

TEST(intrinsics, make_bitmask) {
	constexpr auto t = gen_tup(0b0111'1000);

	fea::tuple_for_each(
			[](auto v) {
				using T = decltype(v);
				constexpr auto mask = fea::make_bitmask<T, 4, 3>();
				auto mask2 = fea::make_bitmask<T, 4, 3>();
				EXPECT_EQ(mask, mask2);
				EXPECT_EQ(v, mask);
			},
			t);
}

TEST(intrinsics, to_unsigned) {
	// Checks that casted types are correct.
	{
		constexpr auto t = gen_tup(42);
		fea::tuple_for_each(
				[](auto v) {
					auto recieved = fea::to_unsigned(v);
					auto recievedpack = fea::to_unsigned(v);

					using orig_t = decltype(v);
					using T = decltype(recieved);

					static_assert(std::is_same_v<T, decltype(recievedpack)>,
							"intrinsics.cpp : unit test failed");

					if constexpr (sizeof(v) <= 4) {
						static_assert(std::is_same_v<uint32_t, T>,
								"intrinsics.cpp : unit test failed");
					} else {
						static_assert(std::is_same_v<uint64_t, T>,
								"intrinsics.cpp : unit test failed");
					}

					EXPECT_EQ(42, int(recieved));

					if constexpr (sizeof(orig_t) >= 4) {
						EXPECT_EQ(42, int(recievedpack));
					} else {
						// Get the first packed val a few ways.
						constexpr size_t orig_size = sizeof(orig_t);
						constexpr size_t shiftr = 32 - orig_size * 8;

						// Test build_mask at the same time.
						constexpr auto mask
								= fea::make_bitmask<decltype(recievedpack),
										orig_size * 8, shiftr>();
						recievedpack &= mask;

						orig_t test1 = orig_t(recievedpack >> shiftr);
						EXPECT_EQ(42, int(test1));

						constexpr size_t num_vals = 4 / orig_size;
						orig_t* arr = reinterpret_cast<orig_t*>(&recievedpack);
						EXPECT_EQ(42, int(arr[num_vals - 1]));
					}
				},
				t);
	}
}

TEST(intrinsics, count_bits) {
	char smol_test = 0b0010'0000;
	EXPECT_EQ(5u, fea::countr_zero(smol_test));
	EXPECT_EQ(2u, fea::countl_zero(smol_test));

	for (size_t i = 0; i < 8; ++i) {
		int test = 1 << i;

		auto t = gen_tup(test);
		fea::tuple_for_each(
				[&](auto val) {
					using T = decltype(val);
					constexpr size_t bit_size = sizeof(T) * 8;
					if constexpr (bit_size <= fea::arch) {
						EXPECT_EQ(i, fea::countr_zero(val));
						EXPECT_EQ(bit_size - 1 - i, fea::countl_zero(val));
					}
				},
				t);
	}

	for (size_t i = 0; i < 4; ++i) {
		int test = fea::make_bitmask<int>(4, i);

		auto t = gen_tup(test);
		fea::tuple_for_each(
				[&](auto val) {
					using T = decltype(val);
					constexpr size_t bit_size = sizeof(T) * 8;
					if constexpr (bit_size <= fea::arch) {
						EXPECT_EQ(i, fea::countr_zero(val));
						EXPECT_EQ(bit_size - 4 - i, fea::countl_zero(val));
					}
				},
				t);
	}
}
} // namespace
