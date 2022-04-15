#include <array>
#include <fea/maps/type_map.hpp>
#include <fea/utils/platform.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>

namespace {
enum class e {
	one,
	two,
	three,
	four,
	count,
};

TEST(type_map, basics) {
	{
		constexpr fea::pack<int, double> k;
		constexpr std::tuple<short, size_t> v{ short(5), size_t(42) };
		auto m = fea::make_type_map(k, v);

		static_assert(m.template contains<int>(), "type_map.cpp : test failed");
		static_assert(
				!m.template contains<short>(), "type_map.cpp : test failed");

		EXPECT_EQ(m.template find<int>(), short(5));
		EXPECT_EQ(m.template find<double>(), 42u);

		EXPECT_EQ(m.template at<0>(), short(5));
		EXPECT_EQ(m.template at<1>(), 42u);

		EXPECT_EQ(m.template idx<int>(), 0u);
		EXPECT_EQ(m.template idx<double>(), 1u);

		// runtime at
		m.at(0, [](auto v) {
			if constexpr (std::is_same_v<decltype(v), short>) {
				EXPECT_EQ(v, short(5));
			}
		});
		m.at(1, [](auto v) {
			if constexpr (std::is_same_v<decltype(v), size_t>) {
				EXPECT_EQ(v, 42u);
			}
		});

		using find_t = std::decay_t<decltype(m.find<double>())>;
		static_assert(std::is_same<find_t, size_t>::value,
				"type_map.cpp : test failed");

		static_assert(std::is_same_v<decltype(m)::front_t, short>,
				"type_map.cpp : test failed");
		static_assert(std::is_same_v<decltype(m)::back_t, size_t>,
				"type_map.cpp : test failed");

		m.for_each([](auto* ptr, auto& val) {
			using K = std::remove_pointer_t<decltype(ptr)>;
			if constexpr (std::is_same_v<K, short>) {
				EXPECT_EQ(val, short(5));
			} else if constexpr (std::is_same_v<K, size_t>) {
				EXPECT_EQ(val, 42u);
			}
			fea::unused(val);
		});
	}

	{
		auto m = fea::make_type_map(fea::kv_t{ int{}, short(5) },
				fea::kv_t{ double{}, size_t(42) });

		static_assert(m.template contains<int>(), "type_map.cpp : test failed");
		static_assert(
				!m.template contains<short>(), "type_map.cpp : test failed");

		EXPECT_EQ(m.template find<int>(), short(5));
		EXPECT_EQ(m.template find<double>(), 42u);

		EXPECT_EQ(m.template at<0>(), short(5));
		EXPECT_EQ(m.template at<1>(), 42u);

		EXPECT_EQ(m.template idx<int>(), 0u);
		EXPECT_EQ(m.template idx<double>(), 1u);

		// runtime at
		m.at(0, [](auto v) {
			if constexpr (std::is_same_v<decltype(v), short>) {
				EXPECT_EQ(v, short(5));
			}
		});
		m.at(1, [](const auto& v) {
			if constexpr (std::is_same_v<decltype(v), size_t>) {
				EXPECT_EQ(v, 42u);
			}
		});

		using find_t = std::decay_t<decltype(m.find<double>())>;
		static_assert(std::is_same<find_t, size_t>::value,
				"type_map.cpp : test failed");

		static_assert(
				std::is_same_v<decltype(m),
						fea::type_map<fea::pack<int, double>, short, size_t>>,
				"type_map.cpp : test failed");

		m.for_each([](auto* ptr, auto& val) {
			using K = std::remove_pointer_t<decltype(ptr)>;
			if constexpr (std::is_same_v<K, int>) {
				EXPECT_EQ(val, short(5));
			} else if constexpr (std::is_same_v<K, double>) {
				EXPECT_EQ(val, 42u);
			}
		});
	}

	{
		auto m = fea::make_type_map(
				fea::make_kv<int>(short(5)), fea::make_kv<double>(size_t(42)));

		static_assert(m.template contains<int>(), "type_map.cpp : test failed");
		static_assert(
				!m.template contains<short>(), "type_map.cpp : test failed");

		EXPECT_EQ(m.template find<int>(), short(5));
		EXPECT_EQ(m.template find<double>(), 42u);

		EXPECT_EQ(m.template at<0>(), short(5));
		EXPECT_EQ(m.template at<1>(), 42u);

		EXPECT_EQ(m.template idx<int>(), 0u);
		EXPECT_EQ(m.template idx<double>(), 1u);

		// runtime at
		m.at(0, [](auto v) {
			if constexpr (std::is_same_v<decltype(v), short>) {
				EXPECT_EQ(v, short(5));
			}
		});
		m.at(1, [](const auto& v) {
			if constexpr (std::is_same_v<decltype(v), size_t>) {
				EXPECT_EQ(v, 42u);
			}
		});

		using find_t = std::decay_t<decltype(m.find<double>())>;
		static_assert(std::is_same<find_t, size_t>::value,
				"type_map.cpp : test failed");

		m.for_each([](auto* ptr, auto& val) {
			using K = std::remove_pointer_t<decltype(ptr)>;
			if constexpr (std::is_same_v<K, int>) {
				EXPECT_EQ(val, short(5));
			} else if constexpr (std::is_same_v<K, double>) {
				EXPECT_EQ(val, 42u);
			}
		});
	}

	{
		constexpr fea::pack_nt<e::one, e::two> k2;
		constexpr std::tuple<short, size_t> v2{ short(5), size_t(42) };
		auto m = fea::make_type_map(k2, v2);

		static_assert(
				m.template contains<e::one>(), "type_map.cpp : test failed");
		static_assert(
				!m.template contains<e::three>(), "type_map.cpp : test failed");

		EXPECT_EQ(m.template find<e::one>(), short(5));
		EXPECT_EQ(m.template find<e::two>(), 42u);

		EXPECT_EQ(m.template at<0>(), short(5));
		EXPECT_EQ(m.template at<1>(), 42u);

		EXPECT_EQ(m.template idx<e::one>(), 0u);
		EXPECT_EQ(m.template idx<e::two>(), 1u);

		// runtime at
		m.at(0, [](auto v) {
			if constexpr (std::is_same_v<decltype(v), short>) {
				EXPECT_EQ(v, short(5));
			}
		});
		m.at(1, [](const auto& v) {
			if constexpr (std::is_same_v<decltype(v), size_t>) {
				EXPECT_EQ(v, 42u);
			}
		});

		using find_t = std::decay_t<decltype(m.find<e::two>())>;
		static_assert(std::is_same<find_t, size_t>::value,
				"type_map.cpp : test failed");

		m.for_each([](auto k, auto& val) {
			if constexpr (k == e::one) {
				EXPECT_EQ(val, short(5));
			} else if constexpr (k == e::two) {
				EXPECT_EQ(val, 42u);
			}
		});
	}


	{
		auto m = fea::make_type_map(
				fea::make_kv_nt<e::one>(42.f), fea::make_kv_nt<e::two>(42.0));

		static_assert(std::is_same_v<decltype(m),
							  fea::type_map<fea::pack_nt<e::one, e::two>, float,
									  double>>,
				"type_map.cpp : test failed");

		EXPECT_EQ(m.template find<e::one>(), 42.f);
		EXPECT_EQ(m.template find<e::two>(), 42.0);

		EXPECT_EQ(m.template at<0>(), 42.f);
		EXPECT_EQ(m.template at<1>(), 42.0);

		EXPECT_EQ(m.template idx<e::one>(), 0u);
		EXPECT_EQ(m.template idx<e::two>(), 1u);

		// runtime at
		m.at(0, [](auto v) {
			if constexpr (std::is_same_v<decltype(v), float>) {
				EXPECT_EQ(v, 42.f);
			}
		});
		m.at(1, [](const auto& v) {
			if constexpr (std::is_same_v<decltype(v), double>) {
				EXPECT_EQ(v, 42.0);
			}
		});

		m.for_each([](auto k, auto& val) {
			if constexpr (decltype(k)::value == e::one) {
				EXPECT_EQ(val, 42.f);
			} else if constexpr (decltype(k)::value == e::two) {
				EXPECT_EQ(val, 42.0);
			}
		});
	}
}

TEST(type_map, runtime_get) {
	{
		auto m = fea::make_type_map(
				fea::make_kv_nt<e::one>(-42.f), fea::make_kv_nt<e::two>(42.0));

		EXPECT_EQ(m.template find<e::one>(), -42.f);
		EXPECT_EQ(m.template find<e::two>(), 42.0);

		EXPECT_EQ(m.template at<0>(), -42.f);
		EXPECT_EQ(m.template at<1>(), 42.0);

		EXPECT_EQ(m.template idx<e::one>(), 0u);
		EXPECT_EQ(m.template idx<e::two>(), 1u);

		// runtime at
		m.at(0, [](auto v) {
			if constexpr (std::is_same_v<decltype(v), float>) {
				EXPECT_EQ(v, -42.f);
			}
		});
		m.at(1, [](const auto& v) {
			if constexpr (std::is_same_v<decltype(v), double>) {
				EXPECT_EQ(v, 42.0);
			}
		});

		fea::runtime_get(
				[](const auto& val) {
					using T = decltype(val);
					if constexpr (std::is_same_v<T, float>) {
						EXPECT_EQ(val, -42.f);
					} else if constexpr (std::is_same_v<T, double>) {
						EXPECT_EQ(val, 42.0);
					}
				},
				e::two, m);
	}
}
} // namespace
