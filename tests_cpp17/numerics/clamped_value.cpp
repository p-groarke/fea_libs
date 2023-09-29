#include <fea/meta/tuple.hpp>
#include <fea/numerics/clamped_value.hpp>
#include <gtest/gtest.h>

namespace {
TEST(clamped_value, basics) {
	{
		fea::clamp_v<int> t(42, 0, 100);
		EXPECT_EQ(t, int(42));
		EXPECT_EQ(t.minimum(), 0);
		EXPECT_EQ(t.maximum(), 100);
		EXPECT_TRUE(t < 50);
		EXPECT_TRUE(t <= 50);
		EXPECT_FALSE(t > 50);
		EXPECT_FALSE(t >= 50);
		EXPECT_FALSE(t == 50);
		EXPECT_TRUE(t != 50);

		++t;
		EXPECT_EQ(t, 43);
		EXPECT_EQ(t++, 43);
		EXPECT_EQ(t, 44);

		--t;
		EXPECT_EQ(t, 43);
		EXPECT_EQ(t--, 43);
		EXPECT_EQ(t, 42);

		t += 1;
		EXPECT_EQ(t, 43);

		t -= 1;
		EXPECT_EQ(t, 42);

		t = 1;
		t *= 2;
		EXPECT_EQ(t, 2);
		t /= 2;
		EXPECT_EQ(t, 1);


		t = 200;
		EXPECT_EQ(t, t.maximum());

		++t;
		EXPECT_EQ(t, t.maximum());
		EXPECT_EQ(t++, t.maximum());
		EXPECT_EQ(t, t.maximum());

		t += 1;
		EXPECT_EQ(t, t.maximum());

		t *= 2;
		EXPECT_EQ(t, t.maximum());

		t = -200;
		t.minimum(10);
		EXPECT_EQ(t, t.minimum());

		--t;
		EXPECT_EQ(t, t.minimum());
		EXPECT_EQ(t--, t.minimum());
		EXPECT_EQ(t, t.minimum());

		t -= 1;
		EXPECT_EQ(t, t.minimum());

		t /= 2;
		EXPECT_EQ(t, t.minimum());


		t = 42;
		t.minimum(50);
		EXPECT_EQ(t.minimum(), 50);
		EXPECT_EQ(int(t), 50);
		EXPECT_EQ(t.maximum(), 100);

		t.minimum(0);
		t.maximum(30);
		EXPECT_EQ(t.minimum(), 0);
		EXPECT_EQ(t.get(), 30);
		EXPECT_EQ(t.maximum(), 30);

		fea::clamp_v<int> tcmp(50, 0, 42);
		EXPECT_EQ(tcmp, 42);
		EXPECT_TRUE(t < tcmp);
		EXPECT_TRUE(t <= tcmp);
		EXPECT_FALSE(t > tcmp);
		EXPECT_FALSE(t >= tcmp);
		EXPECT_FALSE(t == tcmp);
		EXPECT_TRUE(t != tcmp);
	}

	{
		std::tuple<char, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t,
				uint32_t, uint64_t, float, double, long double>
				test_types{};

		// std::tuple<uint8_t, uint16_t, uint32_t, uint64_t, float, double,
		//		long double>
		//		test_types{};

		fea::tuple_for_each(
				[](auto test) {
					using T = decltype(test);
					constexpr bool is_unsigned = std::is_unsigned_v<T>;

					{
						fea::clamp_v<T> v(T(5), T(0), T(10));

						v -= (std::numeric_limits<T>::max)();
						EXPECT_EQ(v, v.minimum());

						v += (std::numeric_limits<T>::max)();
						EXPECT_EQ(v, v.maximum());

						if constexpr (is_unsigned) {
							auto v_bak = v;
							v -= (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v_bak);

							v += (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v_bak);
						} else {
							v -= (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.minimum());
						}
					}
					{
						fea::clamp_v<T> v(T(10), T(5), T(15));

						v -= (std::numeric_limits<T>::max)();
						EXPECT_EQ(v, v.minimum());

						v += (std::numeric_limits<T>::max)();
						EXPECT_EQ(v, v.maximum());

						if constexpr (is_unsigned) {
							auto v_bak = v;
							v -= (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v_bak);

							v += (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v_bak);
						} else {
							v -= (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.minimum());
						}
					}

					if constexpr (!is_unsigned) {
						{
							fea::clamp_v<T> v(T(0), T(-20), T(20));

							v -= (std::numeric_limits<T>::max)();
							EXPECT_EQ(v, v.minimum());

							v -= (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::max)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.minimum());
						}
						{
							fea::clamp_v<T> v(T(-10), T(-20), T(0));

							v -= (std::numeric_limits<T>::max)();
							EXPECT_EQ(v, v.minimum());

							v -= (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::max)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.minimum());
						}
						{
							fea::clamp_v<T> v(T(-15), T(-20), T(-10));

							v -= (std::numeric_limits<T>::max)();
							EXPECT_EQ(v, v.minimum());

							v -= (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::max)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.minimum());
						}
					}
				},
				test_types);
	}

	// unsigned screwyness
	{
		fea::clamp_v<uint8_t> v(5u, 0u, 10u);
		v -= 10u;
		EXPECT_EQ(v, v.minimum());

		v += 20u;
		EXPECT_EQ(v, v.maximum());

		v -= (std::numeric_limits<uint8_t>::max)();
		EXPECT_EQ(v, v.minimum());

		v += (std::numeric_limits<uint8_t>::max)();
		EXPECT_EQ(v, v.maximum());
	}

	{
		fea::clamp_v<char> v(0, -10, 10);

		v -= (std::numeric_limits<char>::max)();
		EXPECT_EQ(v, v.minimum());

		v -= (std::numeric_limits<char>::lowest)();
		EXPECT_EQ(v, v.maximum());

		v += (std::numeric_limits<char>::max)();
		EXPECT_EQ(v, v.maximum());

		v += (std::numeric_limits<char>::lowest)();
		EXPECT_EQ(v, v.minimum());
	}
}

TEST(clamped_value, template_basics) {
	{
		fea::clamp_v<int, 0, 100> t(42);
		EXPECT_EQ(t, int(42));
		EXPECT_EQ(t.minimum(), 0);
		EXPECT_EQ(t.maximum(), 100);
		EXPECT_TRUE(t < 50);
		EXPECT_TRUE(t <= 50);
		EXPECT_FALSE(t > 50);
		EXPECT_FALSE(t >= 50);
		EXPECT_FALSE(t == 50);
		EXPECT_TRUE(t != 50);

		++t;
		EXPECT_EQ(t, 43);
		EXPECT_EQ(t++, 43);
		EXPECT_EQ(t, 44);

		--t;
		EXPECT_EQ(t, 43);
		EXPECT_EQ(t--, 43);
		EXPECT_EQ(t, 42);

		t += 1;
		EXPECT_EQ(t, 43);

		t -= 1;
		EXPECT_EQ(t, 42);

		t = 1;
		t *= 2;
		EXPECT_EQ(t, 2);
		t /= 2;
		EXPECT_EQ(t, 1);


		t = 200;
		EXPECT_EQ(t, t.maximum());

		++t;
		EXPECT_EQ(t, t.maximum());
		EXPECT_EQ(t++, t.maximum());
		EXPECT_EQ(t, t.maximum());

		t += 1;
		EXPECT_EQ(t, t.maximum());

		t *= 2;
		EXPECT_EQ(t, t.maximum());
	}

	{
		fea::clamp_v<int, 10, 100> t(42);
		t = -200;
		EXPECT_EQ(t, t.minimum());

		--t;
		EXPECT_EQ(t, t.minimum());
		EXPECT_EQ(t--, t.minimum());
		EXPECT_EQ(t, t.minimum());

		t -= 1;
		EXPECT_EQ(t, t.minimum());

		t /= 2;
		EXPECT_EQ(t, t.minimum());
	}

	{
		fea::clamp_v<int, 0, 30> t(42);
		EXPECT_EQ(t.minimum(), 0);
		EXPECT_EQ(t.get(), 30);
		EXPECT_EQ(t.maximum(), 30);

		fea::clamp_v<int, 0, 42> tcmp(50);
		EXPECT_EQ(tcmp, 42);
		EXPECT_TRUE(t < tcmp);
		EXPECT_TRUE(t <= tcmp);
		EXPECT_FALSE(t > tcmp);
		EXPECT_FALSE(t >= tcmp);
		EXPECT_FALSE(t == tcmp);
		EXPECT_TRUE(t != tcmp);
	}

	{
		std::tuple<char, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t,
				uint32_t, uint64_t, float, double, long double>
				test_types{};

		// std::tuple<uint8_t, uint16_t, uint32_t, uint64_t, float, double,
		//		long double>
		//		test_types{};

		fea::tuple_for_each(
				[](auto test) {
					using T = decltype(test);
					constexpr bool is_unsigned = std::is_unsigned_v<T>;

					{
						fea::clamp_v<T, T(0), T(10)> v(T(5));

						v -= (std::numeric_limits<T>::max)();
						EXPECT_EQ(v, v.minimum());

						v += (std::numeric_limits<T>::max)();
						EXPECT_EQ(v, v.maximum());

						if constexpr (is_unsigned) {
							auto v_bak = v;
							v -= (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v_bak);

							v += (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v_bak);
						} else {
							v -= (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.minimum());
						}
					}
					{
						fea::clamp_v<T, T(5), T(15)> v(T(10));

						v -= (std::numeric_limits<T>::max)();
						EXPECT_EQ(v, v.minimum());

						v += (std::numeric_limits<T>::max)();
						EXPECT_EQ(v, v.maximum());

						if constexpr (is_unsigned) {
							auto v_bak = v;
							v -= (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v_bak);

							v += (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v_bak);
						} else {
							v -= (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.minimum());
						}
					}

					if constexpr (!is_unsigned) {
						{
							fea::clamp_v<T, T(-20), T(20)> v(T(0));

							v -= (std::numeric_limits<T>::max)();
							EXPECT_EQ(v, v.minimum());

							v -= (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::max)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.minimum());
						}
						{
							fea::clamp_v<T, T(-20), T(0)> v(T(-10));

							v -= (std::numeric_limits<T>::max)();
							EXPECT_EQ(v, v.minimum());

							v -= (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::max)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.minimum());
						}
						{
							fea::clamp_v<T, T(-20), T(-10)> v(T(-15));

							v -= (std::numeric_limits<T>::max)();
							EXPECT_EQ(v, v.minimum());

							v -= (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::max)();
							EXPECT_EQ(v, v.maximum());

							v += (std::numeric_limits<T>::lowest)();
							EXPECT_EQ(v, v.minimum());
						}
					}
				},
				test_types);
	}

	// unsigned screwyness
	{
		fea::clamp_v<uint8_t, 0u, 10u> v(5u);
		v -= 10u;
		EXPECT_EQ(v, v.minimum());

		v += 20u;
		EXPECT_EQ(v, v.maximum());

		v -= (std::numeric_limits<uint8_t>::max)();
		EXPECT_EQ(v, v.minimum());

		v += (std::numeric_limits<uint8_t>::max)();
		EXPECT_EQ(v, v.maximum());
	}

	{
		fea::clamp_v<char, -10, 10> v(0);

		v -= (std::numeric_limits<char>::max)();
		EXPECT_EQ(v, v.minimum());

		v -= (std::numeric_limits<char>::lowest)();
		EXPECT_EQ(v, v.maximum());

		v += (std::numeric_limits<char>::max)();
		EXPECT_EQ(v, v.maximum());

		v += (std::numeric_limits<char>::lowest)();
		EXPECT_EQ(v, v.minimum());
	}
}
} // namespace
