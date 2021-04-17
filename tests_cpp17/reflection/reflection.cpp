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

template <class T>
using testf = void (*)(T);

template <class T, auto E>
struct var_builder {
	static constexpr auto key = E;
	using type = T;
};

// template <class Enum>
// struct var_token {
//	using func_t = Enum (*)();
//
//	template <class T, Enum e>
//	constexpr var_token(var_builder<T, e>)
//			: erased([]() { return e; }) {
//	}
//
//	constexpr Enum get_enum_key() const {
//		return erased();
//	}
//
//	// template <class Func>
//	// void reifer(Func func) {
//	//}
//
//
//	func_t erased = nullptr;
//};

struct descriptor {
	FEA_REFLECTION_VARNAMES(potato, tomato, onion);

	// enum variable : unsigned {
	//	potato,
	//	tomato,
	//	count,
	//};

	/*
	make a macro to generate strings for the variable enum,
	that macro also generates a struct, which inherits N structs that contain a
	function pointer of the name of the name
	it could also generate a class which contains the enum, so reflectable can
	inherit that and keep it clean.
	*/

	// static constexpr std::array<var_token<variable>, 2> vars{
	//	var_builder<tomato, std::string>{},
	//	var_builder<potato, double>{},
	//};

	static constexpr auto vars = fea::pack{
		var_builder<std::string, var::tomato>{},
		var_builder<double, var::potato>{},
		var_builder<int, var::onion>{},
	};
};


template <class Descriptor>
struct reflectable : private Descriptor {
public:
	using var = typename Descriptor::FEA_REFL_ENAME;
	using Descriptor::from_string;
	using Descriptor::to_string;
	using Descriptor::var_strings;

	// template <enum_t V>
	// const auto& get() const {
	//	return _map.template find<V>();
	//}
	// template <enum_t V>
	// auto& get() {
	//	return _map.template find<V>();
	//}

	// auto potato = [this]() -> const auto& {
	//	return get<enum_t::potato>();
	//};

	using type_map_t =
			typename fea::detail::key_to_idx_map<Descriptor>::type_map;
	type_map_t _map{};

private:
	// using Descriptor::vars;
};

struct my_struct : reflectable<descriptor> {};

TEST(reflection, basics) {
	using map_t = fea::detail::key_to_idx_map<descriptor>::type_map;
	static_assert(
			fea::pack_size_v<map_t::pack_t> == size_t(my_struct::var::count),
			"reflection.cpp : test failed");

	my_struct t;
	// t._map;

	const auto enum_arr = descriptor::_var_strings;
	EXPECT_EQ(t.var_strings(), enum_arr);
	EXPECT_EQ(t.var_strings().size(), size_t(my_struct::var::count));

	EXPECT_EQ(t.to_string(my_struct::var::potato), "potato");
	EXPECT_EQ(t.to_string<my_struct::var::potato>(), "potato");

	EXPECT_EQ(t.from_string("potato"), my_struct::var::potato);
	EXPECT_EQ(t.from_string("onion"), my_struct::var::onion);

	// int d = t.potato();

	// std::string& str = t.get<my_struct::tomato>();
	// str = "test\n";
	// printf("%s\n", str.c_str());


	// filters
	/*
	fea::var_filter t.filter_all();
	fea::var_filter t.filter_none();

	t.for_each([](){},
		t.filter_all().enable<var::potato>().enable<var::onion>()
	);

	t.for_each([](){},
		t.filter_none().disable<var::potato>().disable<var::onion>()
	);

	*/
}
} // namespace
