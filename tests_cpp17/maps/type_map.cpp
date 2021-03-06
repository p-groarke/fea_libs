#include <array>
#include <fea/maps/type_map.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>

namespace {
TEST(type_map, basics) {
	{
		constexpr fea::pack<int, double> k;
		constexpr std::tuple<short, size_t> v{ short(0), size_t(42) };
		auto m = fea::make_type_map(k, v);

		static_assert(m.template contains<int>(), "type_map.cpp : test failed");
		static_assert(
				!m.template contains<short>(), "type_map.cpp : test failed");

		EXPECT_EQ(m.template find<double>(), 42u);

		using find_t = std::decay_t<decltype(m.find<double>())>;
		static_assert(std::is_same<find_t, size_t>::value,
				"type_map.cpp : test failed");

		static_assert(std::is_same_v<decltype(m)::front_t, short>,
				"type_map.cpp : test failed");
		static_assert(std::is_same_v<decltype(m)::back_t, size_t>,
				"type_map.cpp : test failed");

		m.for_each([](auto* ptr, [[maybe_unused]] auto& val) {
			using K = std::remove_pointer_t<decltype(ptr)>;
			if constexpr (std::is_same_v<K, short>) {
				EXPECT_EQ(val, short(0));
			} else if constexpr (std::is_same_v<K, size_t>) {
				EXPECT_EQ(val, 42u);
			}
		});
	}

	{
		auto m = fea::make_type_map(fea::kv_t{ int{}, short(0) },
				fea::kv_t{ double{}, size_t(42) });

		static_assert(m.template contains<int>(), "type_map.cpp : test failed");
		static_assert(
				!m.template contains<short>(), "type_map.cpp : test failed");

		EXPECT_EQ(m.template find<double>(), 42u);

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
				EXPECT_EQ(val, short(0));
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

		EXPECT_EQ(m.template find<double>(), 42u);

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
		enum class e {
			one,
			two,
			three,
			four,
			count,
		};

		constexpr fea::pack_nt<e::one, e::two> k2;
		constexpr std::tuple<short, size_t> v2{ short(0), size_t(42) };
		auto m = fea::make_type_map(k2, v2);

		static_assert(
				m.template contains<e::one>(), "type_map.cpp : test failed");
		static_assert(
				!m.template contains<e::three>(), "type_map.cpp : test failed");

		EXPECT_EQ(m.template find<e::two>(), 42u);

		using find_t = std::decay_t<decltype(m.find<e::two>())>;
		static_assert(std::is_same<find_t, size_t>::value,
				"type_map.cpp : test failed");

		m.for_each([](auto k, auto& val) {
			if constexpr (k == e::one) {
				EXPECT_EQ(val, short(0));
			} else if constexpr (k == e::two) {
				EXPECT_EQ(val, 42u);
			}
		});
	}


	{
		enum class e {
			one,
			two,
			three,
			four,
			count,
		};

		auto tmap = fea::make_type_map(
				fea::make_kv_nt<e::one>(42.f), fea::make_kv_nt<e::two>(42.0));

		static_assert(std::is_same_v<decltype(tmap),
							  fea::type_map<fea::pack_nt<e::one, e::two>, float,
									  double>>,
				"type_map.cpp : test failed");

		tmap.for_each([](auto k, auto& val) {
			if constexpr (decltype(k)::value == e::one) {
				EXPECT_EQ(val, 42.f);
			} else if constexpr (decltype(k)::value == e::two) {
				EXPECT_EQ(val, 42.0);
			}
		});
	}
}

#if FEA_CPP20
TEST(type_map, runtime_get) {

	// TODO :
	//{
	//	auto m = fea::make_type_map(fea::kv_t{ int{}, short(0) },
	//			fea::kv_t{ double{}, size_t(42) });

	//	//fea::runtime_get([](auto& val) {},
	//}

	{
		enum class e {
			one,
			two,
			three,
			four,
			count,
		};

		auto tmap = fea::make_type_map(
				fea::make_kv_nt<e::one>(-42.f), fea::make_kv_nt<e::two>(42.0));

		fea::runtime_get(
				[](const auto& val) {
					using T = decltype(val);
					if constexpr (std::is_same_v<T, float>) {
						EXPECT_EQ(val, -42.f);
					} else if constexpr (std::is_same_v<T, double>) {
						EXPECT_EQ(val, 42.0);
					}
				},
				e::two, tmap);
	}
}
#endif
} // namespace
