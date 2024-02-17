#include <fea/numerics/literals.hpp>
#include <gtest/gtest.h>

namespace {
#define ERR_MSG "literals.cpp : Failed unit test."

TEST(literals, basics) {
	using namespace fea::literals;
	static_assert(std::is_same_v<int8_t, decltype(42_8)>, ERR_MSG);
	static_assert(std::is_same_v<int16_t, decltype(42_16)>, ERR_MSG);
	static_assert(std::is_same_v<int32_t, decltype(42_32)>, ERR_MSG);
	static_assert(std::is_same_v<int64_t, decltype(42_64)>, ERR_MSG);
	static_assert(std::is_same_v<int8_t, decltype(42_i8)>, ERR_MSG);
	static_assert(std::is_same_v<int16_t, decltype(42_i16)>, ERR_MSG);
	static_assert(std::is_same_v<int32_t, decltype(42_i32)>, ERR_MSG);
	static_assert(std::is_same_v<int64_t, decltype(42_i64)>, ERR_MSG);
	static_assert(std::is_same_v<uint8_t, decltype(42_u8)>, ERR_MSG);
	static_assert(std::is_same_v<uint16_t, decltype(42_u16)>, ERR_MSG);
	static_assert(std::is_same_v<uint32_t, decltype(42_u32)>, ERR_MSG);
	static_assert(std::is_same_v<uint64_t, decltype(42_u64)>, ERR_MSG);

#if FEA_CPP23
#if defined(__STDCPP_FLOAT16_T__)
	static_assert(std::is_same_v<float16_t, decltype(42.0_16)>, ERR_MSG);
	static_assert(std::is_same_v<float16_t, decltype(42.0_f16)>, ERR_MSG);
#endif

	static_assert(std::is_same_v<float32_t, decltype(42.0_32)>, ERR_MSG);
	static_assert(std::is_same_v<float64_t, decltype(42.0_64)>, ERR_MSG);
	static_assert(std::is_same_v<float32_t, decltype(42.0_f32)>, ERR_MSG);
	static_assert(std::is_same_v<float64_t, decltype(42.0_f64)>, ERR_MSG);
#endif
}
} // namespace