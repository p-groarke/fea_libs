#include <array>
#include <fea/maps/type_map.hpp>
#include <fea/utils/platform.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>

namespace {
enum class tm_e {
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
		int ret = m.at(0, [](auto v) {
			if constexpr (std::is_same_v<decltype(v), short>) {
				EXPECT_EQ(v, short(5));
			}
			return int(42);
		});
		EXPECT_EQ(ret, 42);

		ret = m.at(1, [](auto v) {
			if constexpr (std::is_same_v<decltype(v), size_t>) {
				EXPECT_EQ(v, 42u);
			}
			return int(-1);
		});
		EXPECT_EQ(ret, -1);

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
		auto m = fea::make_type_map(
				fea::pack<int, double>{}, std::array<float, 2>{ 42.f, -1.f });
		using t = std::decay_t<decltype(m)>;
		static_assert(
				std::is_same_v<t,
						fea::type_map<fea::pack<int, double>, float, float>>,
				"type_map.cpp : test failed");

		static_assert(m.template contains<int>(), "type_map.cpp : test failed");
		static_assert(
				m.template contains<double>(), "type_map.cpp : test failed");
		EXPECT_EQ(m.template find<int>(), 42.f);
		EXPECT_EQ(m.template find<double>(), -1.f);

		auto m2 = fea::make_type_map(
				fea::pack_nt<0, 1>{}, std::array<int, 2>{ -42, 101 });
		using t2 = std::decay_t<decltype(m2)>;
		static_assert(
				std::is_same_v<t2, fea::type_map<fea::pack_nt<0, 1>, int, int>>,
				"type_map.cpp : test failed");

		static_assert(m2.template contains<0>(), "type_map.cpp : test failed");
		static_assert(m2.template contains<1>(), "type_map.cpp : test failed");
		EXPECT_EQ(m2.template find<0>(), -42);
		EXPECT_EQ(m2.template find<1>(), 101);
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
		{
			int my_int = 0;
			int& ret = m.at(0, [&](auto v) -> int& {
				if constexpr (std::is_same_v<decltype(v), short>) {
					EXPECT_EQ(v, short(5));
				}
				return my_int;
			});
			EXPECT_EQ(ret, my_int);

			++my_int;
			ret = m.at(1, [&](const auto& v) {
				if constexpr (std::is_same_v<decltype(v), size_t>) {
					EXPECT_EQ(v, 42u);
				}
				return my_int;
			});
			EXPECT_EQ(ret, my_int);
		}

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
		constexpr fea::pack_nt<tm_e::one, tm_e::two> k2;
		constexpr std::tuple<short, size_t> v2{ short(5), size_t(42) };
		auto m = fea::make_type_map(k2, v2);

		static_assert(
				m.template contains<tm_e::one>(), "type_map.cpp : test failed");
		static_assert(!m.template contains<tm_e::three>(),
				"type_map.cpp : test failed");

		EXPECT_EQ(m.template find<tm_e::one>(), short(5));
		EXPECT_EQ(m.template find<tm_e::two>(), 42u);

		EXPECT_EQ(m.template at<0>(), short(5));
		EXPECT_EQ(m.template at<1>(), 42u);

		EXPECT_EQ(m.template idx<tm_e::one>(), 0u);
		EXPECT_EQ(m.template idx<tm_e::two>(), 1u);

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

		using find_t = std::decay_t<decltype(m.find<tm_e::two>())>;
		static_assert(std::is_same<find_t, size_t>::value,
				"type_map.cpp : test failed");

		m.for_each([](auto k, auto& val) {
			// Fix VS v141, 32 bits. For some reason it deduces int
			// instead of tm_e in fea::pack_nt.
			constexpr tm_e tmk = tm_e(k());

			if constexpr (tmk == tm_e::one) {
				EXPECT_EQ(val, short(5));
			} else if constexpr (tmk == tm_e::two) {
				EXPECT_EQ(val, 42u);
			}
		});
	}


	{
		auto m = fea::make_type_map(fea::make_kv_nt<tm_e::one>(42.f),
				fea::make_kv_nt<tm_e::two>(42.0));

		static_assert(std::is_same_v<decltype(m),
							  fea::type_map<fea::pack_nt<tm_e::one, tm_e::two>,
									  float, double>>,
				"type_map.cpp : test failed");

		EXPECT_EQ(m.template find<tm_e::one>(), 42.f);
		EXPECT_EQ(m.template find<tm_e::two>(), 42.0);

		EXPECT_EQ(m.template at<0>(), 42.f);
		EXPECT_EQ(m.template at<1>(), 42.0);

		EXPECT_EQ(m.template idx<tm_e::one>(), 0u);
		EXPECT_EQ(m.template idx<tm_e::two>(), 1u);

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
			// Fix VS v141, 32 bits. For some reason it deduces int
			// instead of tm_e in fea::pack_nt.
			constexpr tm_e tmk = tm_e(k());

			if constexpr (tmk == tm_e::one) {
				EXPECT_EQ(val, 42.f);
			} else if constexpr (tmk == tm_e::two) {
				EXPECT_EQ(val, 42.0);
			}
		});
	}
}

TEST(type_map, runtime_get) {
	auto m = fea::make_type_map(fea::make_kv_nt<tm_e::one>(-42.f),
			fea::make_kv_nt<tm_e::two>(42.0));

	static_assert(std::is_same_v<decltype(m),
						  fea::type_map<fea::pack_nt<tm_e::one, tm_e::two>,
								  float, double>>,
			"type_map.cpp : unit test failed");

	EXPECT_EQ(m.template find<tm_e::one>(), -42.f);
	EXPECT_EQ(m.template find<tm_e::two>(), 42.0);

	EXPECT_EQ(m.template at<0>(), -42.f);
	EXPECT_EQ(m.template at<1>(), 42.0);

	EXPECT_EQ(m.template idx<tm_e::one>(), 0u);
	EXPECT_EQ(m.template idx<tm_e::two>(), 1u);

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

	// In VS v141, 32 bits, for some reason it deduces int
	// instead of tm_e in fea::pack_nt.
	// In v142, it complains about ambiguity between int and enum class...
	// Just disable it.
	// fea::runtime_get(
	//		[](const auto& val) {
	//			using T = std::decay_t<decltype(val)>;
	//			if constexpr (std::is_same_v<T, float>) {
	//				EXPECT_EQ(val, -42.f);
	//			} else if constexpr (std::is_same_v<T, double>) {
	//				EXPECT_EQ(val, 42.0);
	//			}
	//		},
	//		int(tm_e::two), m);

#if FEA_VS_YEAR == 0 || FEA_VS_YEAR >= 2022
	fea::runtime_get(
			[](const auto& val) {
				using T = std::decay_t<decltype(val)>;
				if constexpr (std::is_same_v<T, float>) {
					EXPECT_EQ(val, -42.f);
				} else if constexpr (std::is_same_v<T, double>) {
					EXPECT_EQ(val, 42.0);
				}
			},
			tm_e::two, m);
#endif
}
} // namespace
