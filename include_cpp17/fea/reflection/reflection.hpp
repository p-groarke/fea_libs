/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2020, Philippe Groarke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/
#pragma once
#include "fea/reflection/reflection_macros.hpp"

#include <tuple>

namespace fea {
namespace detail {
template <class, size_t, size_t = 0>
struct make_map_t;

template <class descriptor, size_t Count>
struct make_map_t<descriptor, Count, Count> {
	using key = fea::pack_nt<>;
	using type = std::tuple<>;
	using key_pack = fea::pack_nt<>;
	using type_tup = std::tuple<>;
};

// Takes user pack of var_builders and transforms it into type_map (nt).
template <class Vars, size_t Count, size_t Idx>
struct make_map_t {
	// A pack with one key, will be catenated.
	using key = fea::pack_nt<fea::pack_element_t<Idx, Vars>::key>;

	// A tuple with one variable, will be catenated.
	using type = std::tuple<typename fea::pack_element_t<Idx, Vars>::type>;

	// The next recursion.
	using next_t = make_map_t<Vars, Count, Idx + 1>;

	// All the keys.
	using key_pack = fea::pack_cat_t<key, typename next_t::key_pack>;
	// All the types.
	using type_tup
			= decltype(std::tuple_cat(type{}, typename next_t::type_tup{}));

	// Final type_map.
	using type_map = decltype(fea::make_type_map(key_pack{}, type_tup{}));
};

template <class Descriptor>
struct key_to_idx_map {
	using type_map = typename detail::make_map_t<decltype(Descriptor::vars),
			fea::pack_size_v<decltype(Descriptor::vars)>>::type_map;
};
} // namespace detail


// struct blabla {
//	FEA_FUCKING_MACRO(double, patate, size_t, tomate, );
//
//	template <variable b>
//	auto get() {
//	}
//
// private:
//	std::array<std::string, count> _names;
//	std::tuple<> _date;
//};
//
// struct blabla : poutine<blabla> {
//	enum class var {
//		patate,
//		tomate,
//	};
//};
//
// struct patate {
//	struct data {
//		FEA_FUCKING_MACRO(size_t, tomate, double, patate);
//	};
//
//	enum class var {
//		patate,
//		tomate,
//	};
//
//	// option1 doit yourself
//	std::tuple<std::string, double> _mon_data;
//
//	// option2 pass en descriptor
//	using data_t = std::tuple<std::string, double>;
//
//	// option3 std::any fuckit leaveme alone.
//	std::array<std::any, size_t(var::count)> _data;
//};
//
// template <class Descriptor>
// struct bacon {
//	using desc = Descriptor;
//	using var = Descriptor::var;
//	using string_var = Descriptor::gen_strings;
//
//	// convert data_t -> std::tuple<variant<std::string>, variant<double>>
//
//	template <class Func>
//	void get(var v, Func func) {
//	}
//
//	// une shit tonne de fonctions / variables.
//	data_t data;
//};

} // namespace fea
