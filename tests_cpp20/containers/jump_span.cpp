#include <fea/containers/jump_span.hpp>
#include <fea/numerics/random.hpp>
#include <gtest/gtest.h>
#include <numeric>

namespace {
TEST(jump_span, basics) {
	{
		fea::jump_span<const int> js;
		EXPECT_EQ(js.begin(), js.end());
		EXPECT_EQ(js.rbegin(), js.rend());
		EXPECT_TRUE(js.data().empty());
		EXPECT_EQ(js.size(), 0);
		EXPECT_EQ(js.size_bytes(), 0);
		EXPECT_TRUE(js.empty());
	}

	auto basics1 = []<class T>() {
		std::vector<std::decay_t<T>> v(20);
		std::iota(v.begin(), v.end(), 0);

		fea::jump_span<T> js{
			{ v.begin(), v.begin() + 5 },
			{ v.begin() + 5, v.begin() + 10 },
			{ v.begin() + 10, v.begin() + 15 },
			{ v.begin() + 15, v.begin() + 20 },
		};

		EXPECT_EQ(*(js.begin()), *(js.begin().operator->()));

		EXPECT_TRUE(js.begin() + 5 == js.end() - 15);
		EXPECT_TRUE(js.begin() + 15 == js.end() - 5);
		EXPECT_TRUE(js.begin() < js.end());
		EXPECT_FALSE(js.end() < js.begin());
		EXPECT_FALSE(js.begin() > js.end());
		EXPECT_TRUE(js.end() > js.begin());
		EXPECT_NE(js.begin(), js.end());
		EXPECT_NE(js.rbegin(), js.rend());

		EXPECT_EQ(*(js.begin()), 0);
		EXPECT_EQ((js.begin() + 20), js.end());
		EXPECT_EQ((js.end() - 20), js.begin());
		EXPECT_EQ(*(js.begin() + 7), 7);
		EXPECT_EQ(*(js.begin() + 13), 13);
		EXPECT_EQ(*(js.end() - 7), 13);
		EXPECT_EQ(*(js.end() - 13), 7);

		EXPECT_EQ(std::distance(js.begin(), js.end()), 20);
		EXPECT_EQ(std::distance(js.end(), js.begin()), -20);

		EXPECT_EQ((js.begin() + 13) - (js.begin() + 5), 8);
		EXPECT_EQ((js.begin() + 5) - (js.begin() + 13), -8);

		EXPECT_EQ(js.front(), 0);
		EXPECT_EQ(js.back(), 19);
		EXPECT_EQ(js[13], 13);
		EXPECT_FALSE(js.data().empty());
		EXPECT_EQ(js.data().size(), 4);
		EXPECT_EQ(js.size(), 20);
		EXPECT_EQ(js.size_bytes(), 20 * sizeof(int));
		EXPECT_FALSE(js.empty());

		{
			auto it = js.begin();
			EXPECT_EQ(*(++it), 1);
			EXPECT_EQ(*(it++), 1);
			EXPECT_EQ(*(it), 2);
			EXPECT_EQ(*(--it), 1);
			EXPECT_EQ(*(it--), 1);
			EXPECT_EQ(*(it), 0);
		}

		EXPECT_FALSE(fea::are_contiguous(js.begin(), js.end()));
		EXPECT_TRUE(fea::are_contiguous(js.begin(), js.begin() + 2));
		EXPECT_FALSE(fea::are_contiguous(js.end(), js.end() - 1));
		EXPECT_TRUE(fea::are_contiguous(js.end() - 1, js.end() - 2));

		int expected = 0;
		for (int i : js) {
			EXPECT_EQ(i, expected);
			++expected;
		}
		for (auto it = js.rbegin(); it != js.rend(); ++it) {
			--expected;
			EXPECT_EQ(*it, expected);
		}
		for (size_t i = 0; i < js.size(); ++i) {
			EXPECT_EQ(js[i], expected);
			++expected;
		}

		// Do some light fuzzing.
		auto it = js.begin();
		constexpr size_t fuzz_count = 1000;
		for (size_t i = 0; i < fuzz_count; ++i) {
			size_t end = std::distance(it, js.end() - 1);
			size_t off = fea::random_val(size_t(0), end);
			it += off;
			assert(it >= js.begin() && it < js.end());
			EXPECT_EQ(std::distance(js.begin(), it), *it);
			EXPECT_GE(it, js.begin());
			EXPECT_LT(it, js.end());

			auto it2 = it;
			size_t beg = std::distance(js.begin(), it2);
			off = fea::random_val(size_t(0), beg);
			it2 -= off;
			assert(it >= js.begin() && it < js.end());
			EXPECT_EQ(std::distance(js.begin(), it), *it);
			EXPECT_GE(it, js.begin());
			EXPECT_LT(it, js.end());

			ptrdiff_t diff1 = it - it2;
			ptrdiff_t diff2 = it2 - it;
			EXPECT_TRUE(std::abs(diff1) <= 20);
			EXPECT_TRUE(std::abs(diff2) <= 20);
			if (it == it2) {
				EXPECT_EQ(diff1, 0);
				EXPECT_EQ(diff2, 0);
			} else if (it < it2) {
				EXPECT_LT(diff1, 0);
				EXPECT_GT(diff2, 0);
			} else {
				EXPECT_GT(diff1, 0);
				EXPECT_LT(diff2, 0);
			}

			it = it2;
		}
	};

	basics1.template operator()<const int>();
	basics1.template operator()<int>();

	// Mutate
	{
		std::vector<int> v(20);
		fea::jump_span<int> js{
			{ v.begin(), v.begin() + 5 },
			{ v.begin() + 5, v.begin() + 10 },
			{ v.begin() + 10, v.begin() + 15 },
			{ v.begin() + 15, v.begin() + 20 },
		};
		std::iota(js.begin(), js.end(), 0);

		int expected = 0;
		for (int i : js) {
			EXPECT_EQ(i, expected);
			++expected;
		}

		js[7] = 42;
		EXPECT_EQ(*(js.begin() + 7), 42);

		auto it = js.begin() + 13;
		*it = -42;
		EXPECT_EQ(js[13], -42);
	}
}

} // namespace