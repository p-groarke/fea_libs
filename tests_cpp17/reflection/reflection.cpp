#include <cstdio>
#include <fea/maps/type_map.hpp>
#include <fea/meta/macros.hpp>
#include <fea/meta/pack.hpp>
#include <fea/meta/static_for.hpp>
#include <fea/meta/tuple.hpp>
#include <fea/reflection/reflection.hpp>
#include <fea/reflection/reflection_macros.hpp>
#include <gtest/gtest.h>

namespace {

struct my_descriptor {
	// Forward declare your variable names.
	// The order here doesn't really matter.
	// An enum class named "var" will be created in your class.
	// The string associated to a variable will match this.
	FEA_REFLECTION_VARNAMES(potato, tomato, onion);

	// Here you declare your variables.
	// Declare the enum value, the type and optionally the initial value.
	// The for_each order will match this order.
	static inline const auto vars = std::tuple{
		fea::var_builder<var::tomato, std::string>{ "Im tomato" },
		fea::var_builder<var::potato, double>{ 42.0 },
		fea::var_builder<var::onion, int>{ -42 },
	};
};


// This is your "final" class. Inherit fea::reflectable and template it with
// your configurator.
struct my_struct : fea::reflectable<my_descriptor> {};


// Tests & examples.
TEST(reflection, basics) {
	using map_t = fea::detail::reflectable_builder<my_descriptor>::type_map;
	static_assert(
			fea::pack_size_v<map_t::pack_t> == size_t(my_struct::var::count),
			"reflection.cpp : test failed");

	my_struct t;

	// const auto enum_arr = descriptor::_var_strings;
	fea::enum_array<std::string, my_struct::var> test_arr{ { "potato", "tomato",
			"onion" } };
	// Get all variable names.
	EXPECT_EQ(t.var_names(), test_arr);
	EXPECT_EQ(t.var_names().size(), size_t(my_struct::var::count));

	// Get a specific variable name, at runtime or compile time.
	EXPECT_EQ(t.var_name(my_struct::var::potato), "potato");
	EXPECT_EQ(t.var_name<my_struct::var::potato>(), "potato");
	EXPECT_EQ(t.var_name(my_struct::var::tomato), "tomato");
	EXPECT_EQ(t.var_name<my_struct::var::tomato>(), "tomato");
	EXPECT_EQ(t.var_name(my_struct::var::onion), "onion");
	EXPECT_EQ(t.var_name<my_struct::var::onion>(), "onion");

	// Get the enum value for a string.
	EXPECT_EQ(t.var_enum("tomato"), my_struct::var::tomato);
	EXPECT_EQ(t.var_enum("potato"), my_struct::var::potato);
	EXPECT_EQ(t.var_enum("onion"), my_struct::var::onion);

	// Initializer values.
	// Compile-time getters.
	EXPECT_EQ(t.get<my_struct::var::tomato>(), "Im tomato");
	EXPECT_EQ(t.get<my_struct::var::potato>(), 42.0);
	EXPECT_EQ(t.get<my_struct::var::onion>(), -42);

	// Runtime getters.
	{
		auto tester = [](auto& v) -> my_struct::var {
			using T = std::decay_t<decltype(v)>;
			if constexpr (std::is_same_v<T, std::string>) {
				EXPECT_EQ(v, "Im tomato");
				return my_struct::var::tomato;
			} else if constexpr (std::is_same_v<T, double>) {
				EXPECT_EQ(v, 42.0);
				return my_struct::var::potato;
			} else if constexpr (std::is_same_v<T, int>) {
				EXPECT_EQ(v, -42);
				return my_struct::var::onion;
			}
		};

		my_struct::var test = t.get(my_struct::var::tomato, tester);
		EXPECT_EQ(test, my_struct::var::tomato);
		test = t.get("tomato", tester);
		EXPECT_EQ(test, my_struct::var::tomato);

		test = t.get(my_struct::var::potato, tester);
		EXPECT_EQ(test, my_struct::var::potato);
		test = t.get("potato", tester);
		EXPECT_EQ(test, my_struct::var::potato);

		test = t.get(my_struct::var::onion, tester);
		EXPECT_EQ(test, my_struct::var::onion);
		test = t.get("onion", tester);
		EXPECT_EQ(test, my_struct::var::onion);
	}

	// For each variable.
	{
		t.for_each([](auto key, const auto& v) {
			using T = std::decay_t<decltype(v)>;

			if constexpr (key == my_struct::var::tomato) {
				static_assert(std::is_same_v<T, std::string>,
						"reflection.cpp : test failed");
				EXPECT_EQ(v, "Im tomato");
			} else if constexpr (key == my_struct::var::potato) {
				static_assert(std::is_same_v<T, double>,
						"reflection.cpp : test failed");
				EXPECT_EQ(v, 42.0);
			} else if constexpr (key == my_struct::var::onion) {
				static_assert(
						std::is_same_v<T, int>, "reflection.cpp : test failed");
				EXPECT_EQ(v, -42);
			}
		});
	}

#if 0
	// Compile-time Filters
	{
		auto all = t.cexpr_filter_all();
		size_t i = 0;
		t.for_each([&](auto, auto&) { ++i; }, all);
		EXPECT_EQ(i, 0u);

		auto none = t.cexpr_filter_none();
		t.for_each([&](auto, auto&) { ++i; }, none);
		EXPECT_EQ(i, 3u);

		auto f1 = t.cexpr_filter_all()
						  .enable<my_struct::var::potato,
								  my_struct::var::tomato>();

		i = 0;
		t.for_each(
				[&](auto key, auto& v) {
					static_assert(key != my_struct::var::onion,
							"reflection.cpp : test failed");

					if constexpr (key == my_struct::var::potato) {
						EXPECT_EQ(v, 42.0);
					} else if constexpr (key == my_struct::var::tomato) {
						EXPECT_EQ(v, "Im tomato");
					}
					++i;
				},
				f1);
		EXPECT_EQ(i, 2u);

		// Other style
		auto f2 = t.cexpr_filter_all()
						  .enable<my_struct::var::potato>()
						  .enable<my_struct::var::onion>();

		i = 0;
		t.for_each(
				[&](auto key, auto& v) {
					static_assert(key != my_struct::var::tomato,
							"reflection.cpp : test failed");

					if constexpr (key == my_struct::var::potato) {
						EXPECT_EQ(v, 42.0);
					} else if constexpr (key == my_struct::var::onion) {
						EXPECT_EQ(v, -42);
					}
					++i;
				},
				f2);
		EXPECT_EQ(i, 2u);


		auto f3 = t.cexpr_filter_none()
						  .disable<my_struct::var::potato,
								  my_struct::var::tomato>();

		i = 0;
		t.for_each(
				[&](auto key, auto& v) {
					static_assert(key != my_struct::var::potato
									&& key != my_struct::var::tomato,
							"reflection.cpp : test failed");

					EXPECT_EQ(v, -42);
					++i;
				},
				f3);
		EXPECT_EQ(i, 1u);

		// Other style
		auto f4 = t.cexpr_filter_none()
						  .disable<my_struct::var::potato>()
						  .disable<my_struct::var::onion>();

		i = 0;
		t.for_each(
				[&](auto key, auto& v) {
					static_assert(key != my_struct::var::potato
									&& key != my_struct::var::onion,
							"reflection.cpp : test failed");

					EXPECT_EQ(v, "Im tomato");
					++i;
				},
				f4);
		EXPECT_EQ(i, 1u);
	}
#endif

	// Runtime filters.
	{
		auto all = t.filter_all();
		size_t i = 0;
		t.for_each([&](auto, auto&) { ++i; }, all);
		EXPECT_EQ(i, 0u);

		auto none = t.filter_none();
		t.for_each([&](auto, auto&) { ++i; }, none);
		EXPECT_EQ(i, 3u);

		auto f1 = t.filter_all()
						  .enable<my_struct::var::potato,
								  my_struct::var::tomato>();

		i = 0;
		t.for_each(
				[&](auto key, auto& v) {
					// static_assert(key != my_struct::var::onion,
					//		"reflection.cpp : test failed");

					if constexpr (key == my_struct::var::potato) {
						EXPECT_EQ(v, 42.0);
					} else if constexpr (key == my_struct::var::tomato) {
						EXPECT_EQ(v, "Im tomato");
					}
					++i;
				},
				f1);
		EXPECT_EQ(i, 2u);

		// Other style
		auto f2 = t.filter_all()
						  .enable<my_struct::var::potato>()
						  .enable<my_struct::var::onion>();

		i = 0;
		t.for_each(
				[&](auto key, auto& v) {
					EXPECT_NE(key(), my_struct::var::tomato);

					if constexpr (key == my_struct::var::potato) {
						EXPECT_EQ(v, 42.0);
					} else if constexpr (key == my_struct::var::onion) {
						EXPECT_EQ(v, -42);
					}
					++i;
				},
				f2);
		EXPECT_EQ(i, 2u);


		auto f3 = t.filter_none()
						  .disable<my_struct::var::potato,
								  my_struct::var::tomato>();

		i = 0;
		t.for_each(
				[&](auto key, auto& v) {
					EXPECT_NE(key(), my_struct::var::potato);
					EXPECT_NE(key(), my_struct::var::tomato);
					if constexpr (key == my_struct::var::onion) {
						EXPECT_EQ(v, -42);
					}
					++i;
				},
				f3);
		EXPECT_EQ(i, 1u);

		// Other style
		auto f4 = t.filter_none()
						  .disable<my_struct::var::potato>()
						  .disable<my_struct::var::onion>();

		i = 0;
		t.for_each(
				[&](auto key, auto& v) {
					EXPECT_NE(key(), my_struct::var::potato);
					EXPECT_NE(key(), my_struct::var::onion);

					if constexpr (key == my_struct::var::tomato) {
						EXPECT_EQ(v, "Im tomato");
					}
					++i;
				},
				f4);
		EXPECT_EQ(i, 1u);
	}
}
} // namespace
