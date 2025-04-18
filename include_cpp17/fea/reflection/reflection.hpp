/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2025, Philippe Groarke
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
#include "fea/containers/type_map.hpp"
#include "fea/reflection/filters.hpp"
#include "fea/reflection/reflection_macros.hpp"

#include <tuple>

namespace fea {
namespace detail {
template <class, size_t, size_t = 0>
struct make_map_t;

template <class Descriptor, size_t Count>
struct make_map_t<Descriptor, Count, Count> {
	using key = fea::pack_nt<>;
	using type = std::tuple<>;
	using key_pack = fea::pack_nt<>;
	using type_tup = std::tuple<>;
};

// Takes user pack of var_builders and transforms it into type_map (nt).
template <class Descriptor, size_t Count, size_t Idx>
struct make_map_t {
	// A pack with one key, will be catenated.
	using key = fea::pack_nt<std::tuple_element_t<Idx, Descriptor>::key>;

	// A tuple with one variable, will be catenated.
	using type
			= std::tuple<typename std::tuple_element_t<Idx, Descriptor>::type>;

	// The next recursion.
	using next_t = make_map_t<Descriptor, Count, Idx + 1>;

	// All the keys.
	using key_pack = fea::pack_cat_t<key, typename next_t::key_pack>;
	// All the types.
	using type_tup
			= decltype(std::tuple_cat(type{}, typename next_t::type_tup{}));

	// Final type_map.
	using type_map = decltype(fea::make_type_map(key_pack{}, type_tup{}));
};

template <class Descriptor>
struct reflectable_builder {
	static constexpr size_t vars_size
			= std::tuple_size_v<std::decay_t<decltype(Descriptor::vars)>>;

	using type_map = typename detail::make_map_t<decltype(Descriptor::vars),
			vars_size>::type_map;


private:
	template <class... VarBuilders, size_t... Idxes>
	static auto make_init_tuple(const std::tuple<VarBuilders...>& tup,
			std::index_sequence<Idxes...>) {
		return std::make_tuple(std::get<Idxes>(tup).get_init_val()...);
	}

public:
	static auto init_map() {
		return make_init_tuple(
				Descriptor::vars, std::make_index_sequence<vars_size>{});
	}
};
} // namespace detail


// This is a helper struct to assign the type of a var, its order and initial
// value.
template <auto E, class T>
struct var_builder {
	static constexpr auto key = E;
	using type = T;

	constexpr var_builder() = default;
	constexpr var_builder(const T& init_val)
			: _init_val(init_val) {
	}
	constexpr var_builder(T&& init_val)
			: _init_val(std::move(init_val)) {
	}

	constexpr const T& get_init_val() const {
		return _init_val;
	}

private:
	T _init_val{};
};


// Inherit this on your class that will be reflectable.
// Provide a configuration descriptor with the reflectable variables.
template <class Descriptor>
struct reflectable : Descriptor {
private:
	using Descriptor::vars;
	using ref_builder_t = fea::detail::reflectable_builder<Descriptor>;
	using type_map_t = typename ref_builder_t::type_map;
	using front_t = typename type_map_t::front_t;

public:
	using var = typename Descriptor::FEA_REFL_ENAME;
	using Descriptor::var_enum;
	using Descriptor::var_name;
	using Descriptor::var_names;

	// Compile-time, no overhead getters.
	template <var e>
	const auto& get() const {
		return _type_map.template find<e>();
	}
	// Compile-time, no overhead getters.
	template <var e>
	auto& get() {
		return _type_map.template find<e>();
	}

	// Runtime getters with enum.
	template <class Func>
	std::invoke_result_t<Func, const front_t&> get(var e, Func&& func) const {
		return fea::runtime_get(std::forward<Func>(func), e, _type_map);
	}
	// Runtime getters with enum.
	template <class Func>
	std::invoke_result_t<Func, front_t&> get(var e, Func&& func) {
		return fea::runtime_get(std::forward<Func>(func), e, _type_map);
	}

	// Runtime getters with string.
	template <class Func>
	std::invoke_result_t<Func, const front_t&> get(
			const std::string& var_name, Func&& func) const {
		var e = var_enum(var_name);
		return get(e, std::forward<Func>(func));
	}
	// Runtime getters with string.
	template <class Func>
	std::invoke_result_t<Func, front_t&> get(
			const std::string& var_name, Func&& func) {
		var e = var_enum(var_name);
		return get(e, std::forward<Func>(func));
	}

	// Loops on your variables.
	// Passes (std::integral_constant<Enum, E>, const auto& val).
	template <class Func>
	void for_each(Func&& func) const {
		_type_map.for_each(func);
	}
	// Loops on your variables.
	// Passes (std::integral_constant<Enum, E>, auto& val).
	template <class Func>
	void for_each(Func&& func) {
		_type_map.for_each(func);
	}

#if 0
	// Loops on your variables.
	// If values are filtered out, they will be skipped.
	// Passes (std::integral_constant<Enum, E>, const auto& val).
	template <class Func, bool... Vals>
	void for_each(Func&& func, cexpr_var_filter<var, Vals...>) const {
		_type_map.for_each([&](auto key, const auto& val) {
			using filt_t = cexpr_var_filter<var, Vals...>;
			if constexpr (filt_t::template at<key()>()) {
				func(key, val);
			}
		});
	}
	// Loops on your variables.
	// If values are filtered out, they will be skipped.
	// Passes (std::integral_constant<Enum, E>, auto& val).
	template <class Func, bool... Vals>
	void for_each(Func&& func, cexpr_var_filter<var, Vals...>) {
		_type_map.for_each([&](auto key, auto& val) {
			using filt_t = cexpr_var_filter<var, Vals...>;
			if constexpr (filt_t::template at<key()>()) {
				func(key, val);
			}
		});
	}
#endif

	// Loops on your variables.
	// If values are filtered out, they will be skipped.
	// Passes (std::integral_constant<Enum, E>, const auto& val).
	template <class Func>
	void for_each(Func&& func, const var_filter<var>& filter) const {
		_type_map.for_each([&](auto key, const auto& val) {
			if (filter.template at<key()>()) {
				func(key, val);
			}
		});
	}
	// Loops on your variables.
	// If values are filtered out, they will be skipped.
	// Passes (std::integral_constant<Enum, E>, auto& val).
	template <class Func, bool... Vals>
	void for_each(Func&& func, const var_filter<var>& filter) {
		_type_map.for_each([&](auto key, auto& val) {
			if (filter.template at<key()>()) {
				func(key, val);
			}
		});
	}

#if 0
	// Creates a filter which disables all variables.
	// Disabled variables will be skipped in foreach when passing in filter.
	static constexpr auto cexpr_filter_all() {
		return make_var_filter<var, false, size_t(var::count)>();
	}

	// Creates a filter which enables all variables.
	// Disabled variables will be skipped in foreach when passing in filter.
	static constexpr auto cexpr_filter_none() {
		return make_var_filter<var, true, size_t(var::count)>();
	}
#endif

	// Creates a filter which disables all variables.
	// Disabled variables will be skipped in foreach when passing in filter.
	static constexpr var_filter<var> filter_all() {
		return var_filter<var>{};
	}

	// Creates a filter which enables all variables.
	// Disabled variables will be skipped in foreach when passing in filter.
	static constexpr var_filter<var> filter_none() {
		return var_filter<var>{}.enable_all();
	}

private:
	type_map_t _type_map = ref_builder_t::init_map();
};

} // namespace fea
