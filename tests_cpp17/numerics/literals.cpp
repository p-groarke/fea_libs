#include <fea/numerics/literals.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>
#include <type_traits>

namespace {
#define ERR_MSG "literals.cpp : Failed unit test."

TEST(literals, basics) {
	using namespace fea::literals;
	static_assert(std::is_same<int8_t, decltype(42_8)>::value, ERR_MSG);
	static_assert(std::is_same<int16_t, decltype(42_16)>::value, ERR_MSG);
	static_assert(std::is_same<int32_t, decltype(42_32)>::value, ERR_MSG);
	static_assert(std::is_same<int64_t, decltype(42_64)>::value, ERR_MSG);
	static_assert(std::is_same<int8_t, decltype(42_i8)>::value, ERR_MSG);
	static_assert(std::is_same<int16_t, decltype(42_i16)>::value, ERR_MSG);
	static_assert(std::is_same<int32_t, decltype(42_i32)>::value, ERR_MSG);
	static_assert(std::is_same<int64_t, decltype(42_i64)>::value, ERR_MSG);
	static_assert(std::is_same<uint8_t, decltype(42_u8)>::value, ERR_MSG);
	static_assert(std::is_same<uint16_t, decltype(42_u16)>::value, ERR_MSG);
	static_assert(std::is_same<uint32_t, decltype(42_u32)>::value, ERR_MSG);
	static_assert(std::is_same<uint64_t, decltype(42_u64)>::value, ERR_MSG);

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