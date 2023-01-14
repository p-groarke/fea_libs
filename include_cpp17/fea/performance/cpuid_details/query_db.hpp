/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Philippe Groarke
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
#include "fea/containers/stack_vector.hpp"
#include "fea/descriptors/descriptor_map.hpp"
#include "fea/enum/string_enum.hpp"
#include "fea/performance/cpuid_details/leaf_db.hpp"
#include "fea/performance/intrinsics.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

// References :
// https://en.wikipedia.org/wiki/CPUID
// https://www.scss.tcd.ie/Jeremy.Jones/CS4021/processor-identification-cpuid-instruction-note.pdf
// https://cdrdv2.intel.com/v1/dl/getContent/671110 (Intel Manual Vol. 2)

namespace fea {
namespace cpuid {
enum class query_type_e : uint8_t {
	bool_,
	uint32_,
	string_,
	count,
};

// using query_types_t = std::tuple<bool, uint32_t, std::string>;
// static_assert(std::tuple_size_v<query_types_t> ==
// size_t(query_type_e::count), 		"fea::cpuid : Missing query type.");
//
// template <query_type_e E>
// using to_query_type = std::tuple_element_t<size_t(E), query_types_t>;


// clang-format off
FEA_STRING_ENUM_WITH_COUNT(query_e, uint16_t,
	highest_leaf,
	vendor
)
// clang-format on


namespace detail {
struct register_lookup {
	// Our leaf.
	leaf_e leaf = leaf_e::eax0;

	// The register to get our value.
	reg_e reg = reg_e::eax;

	// The value's first bit in the register.
	uint8_t bit_begin = 0;

	// The value's end bit (one past) in the register.
	uint8_t bit_end = 32;
};

struct query_description {
	// static constexpr size_t max_lookups = 3;

	// The key this descriptor represents.
	query_e key = query_e::count;

	// The expected return type.
	query_type_e type = query_type_e::count;

	// A query will typically have 1 lookup, but can have more.
	fea::stack_vector<register_lookup, 4> lookups{};

	// If the type is uint32_t, set this to true to print as hexadecimal.
	// You can ignore this for non-uint32 types.
	bool print_hex = false;
};

inline constexpr std::array<query_description, size_t(query_e::count)>
		query_map{
			query_description{
					query_e::highest_leaf,
					query_type_e::uint32_,
					{
							register_lookup{ leaf_e::eax0, reg_e::eax, 0, 32 },
					},
			},
			query_description{
					query_e::vendor,
					query_type_e::string_,
					{
							register_lookup{ leaf_e::eax0, reg_e::ebx, 0, 32 },
							register_lookup{ leaf_e::eax0, reg_e::edx, 0, 32 },
							register_lookup{ leaf_e::eax0, reg_e::ecx, 0, 32 },
					},
			},
		};

struct highest_leaf_desc {
	static constexpr query_e key = query_e::highest_leaf;

	static constexpr query_type_e type = query_type_e::uint32_;

	// If more than 1, you must implement lookupN.
	static constexpr uint8_t num_lookups = 1;

	static constexpr register_lookup lookup{
		// Our leaf.
		leaf_e::eax0,

		// The register to get our value.
		reg_e::eax,

		// The value's first bit in the register.
		0,

		// The value's end bit (one past) in the register.
		32,
	};

	// If the type is uint32_t, set this to true to print as hexadecimal.
	// You can ignore this for non-uint32 types.
	static constexpr bool print_hex = true;
};

struct vendor_desc {
	static constexpr query_e key = query_e::vendor;
	static constexpr query_type_e type = query_type_e::string_;
	static constexpr uint8_t num_lookups = 3;

	static constexpr register_lookup lookup{ leaf_e::eax0, reg_e::ebx, 0, 32 };
	static constexpr register_lookup lookup2{ leaf_e::eax0, reg_e::edx, 0, 32 };
	static constexpr register_lookup lookup3{ leaf_e::eax0, reg_e::ecx, 0, 32 };
};


// clang-format off
using query_db = fea::descriptor_map<
	query_e // the key enum
	, highest_leaf_desc // the descriptors, in enum order
	, vendor_desc
>;
// clang-format on


template <leaf_e LeafE, reg_e RegE, uint8_t BitBegin, uint8_t BitEnd>
bool lookup_bool() {
	// bit_end is ignored for bools.
	static_assert(
			BitEnd == 0, "fea::cpuid : bit_end is ignored for bool queries.");

	const leaf_data& runtime_data = cpuid::leaf_db::data[LeafE];
	return runtime_data[RegE][BitBegin];
}

bool lookup_bool(const register_lookup& lookup) {
	// bit_end is ignored for bools.
	assert(lookup.bit_end == 0);

	const leaf_data& runtime_data = cpuid::leaf_db::data[lookup.leaf];
	return runtime_data[lookup.reg][lookup.bit_begin];
}

template <leaf_e LeafE, reg_e RegE, uint8_t BitBegin, uint8_t BitEnd>
uint32_t lookup_uint32() {
	constexpr size_t reg_size = cpuid::leaf_data::register_size;
	static_assert(reg_size == 32,
			"fea::cpuid : Register size is expected to be 32. Refactor "
			"to_ulong.");

	constexpr size_t bit_size = BitEnd - BitBegin;
	static_assert(bit_size > 1,
			"fea::cpuid : uint32_t query expects a size bigger than 1. Use "
			"bool type for single bits.");

	const cpuid::leaf_data& runtime_data = cpuid::leaf_db::data[LeafE];
	uint32_t ret = runtime_data[RegE].to_ulong();

	if constexpr (bit_size != reg_size) {
		// Mask needed.
		constexpr uint32_t mask
				= fea::make_bitmask<uint32_t, bit_size, BitBegin>();
		ret &= mask;
		ret >>= BitBegin;
	}

	return ret;
}

uint32_t lookup_uint32(const register_lookup& lookup) {
	constexpr size_t reg_size = cpuid::leaf_data::register_size;
	static_assert(reg_size == 32,
			"fea::cpuid : Register size is expected to be 32. Refactor "
			"to_ulong.");

	size_t bit_size = lookup.bit_end - lookup.bit_begin;
	assert(bit_size > 1);

	const cpuid::leaf_data& runtime_data = cpuid::leaf_db::data[lookup.leaf];
	uint32_t ret = runtime_data[lookup.reg].to_ulong();

	if (bit_size != reg_size) {
		// Mask needed.
		uint32_t mask = fea::make_bitmask<uint32_t>(bit_size, lookup.bit_begin);
		ret &= mask;
		ret >>= lookup.bit_begin;
	}

	return ret;
}

} // namespace detail

struct query_db : detail::query_db {
	//// All the query_type_e.
	// static constexpr auto types
	//		= make_enum_array([](auto desc) { return desc.type; });

	// static constexpr auto num_lookups
	//		= make_enum_array([](auto desc) { return desc.num_lookups; });

	// static constexpr auto lookups
	//		= make_enum_array([](auto desc) { return desc.lookup; });

	// static constexpr auto print_hexes = make_enum_array([](auto desc) {
	//	using desc_t = decltype(desc);
	//	if constexpr (desc_t::type != query_type_e::uint32_) {
	//		return false;
	//	} else {
	//		return desc.print_hex;
	//	}
	// });

	// static constexpr auto registers
	//		= make_enum_array([](auto desc) { return desc.reg; });

	// static constexpr auto bit_begins
	//		= make_enum_array([](auto desc) { return desc.bit_begin; });

	// static constexpr auto bit_ends
	//		= make_enum_array([](auto desc) { return desc.bit_end; });

	// Get a bool value for provided query.
	template <query_e Query>
	static std::enable_if_t<types[Query] == query_type_e::bool_, bool>
	get_value() {
		using desc_t = std::tuple_element_t<size_t(Query), desc_tup_t>;
		static_assert(desc_t::num_lookups == 1,
				"fea::cpuid : Bool queries only support 1 lookup.");

		constexpr detail::register_lookup lookup = desc_t::lookup;
		return detail::lookup_bool<lookup.leaf, lookup.reg, lookup.bit_begin,
				lookup.bit_end>();
	}

	// Get a uint32_t value for provided query.
	template <query_e Query>
	static std::enable_if_t<types[Query] == query_type_e::uint32_, uint32_t>
	get_value() {
		using desc_t = std::tuple_element_t<size_t(Query), desc_tup_t>;
		static_assert(desc_t::num_lookups == 1,
				"fea::cpuid : uint32 queries only support 1 lookup.");

		constexpr detail::register_lookup lookup = desc_t::lookup;
		return detail::lookup_uint32<lookup.leaf, lookup.reg, lookup.bit_begin,
				lookup.bit_end>();
	}

	// Get a string value for provided query.
	template <query_e Query>
	static std::enable_if_t<types[Query] == query_type_e::string_, std::string>
	get_value() {
		using desc_t = std::tuple_element_t<size_t(Query), desc_tup_t>;

		constexpr size_t numl = desc_t::num_lookups;
		static_assert(numl <= 4, "fea::cpuid : TODO add more lookups");

		std::string ret;
		ret.reserve(numl * 4 + 1); // 4 chars per lookup

		auto add_to_string = [](uint32_t v, std::string& ret) {
			assert(ret.capacity() > ret.size() + 4);
			const char* as_str = reinterpret_cast<const char*>(&v);
			ret.insert(ret.end(), as_str, as_str + 4);
		};

		if constexpr (numl >= 1) {
			// First, get as uint32_t.
			constexpr detail::register_lookup l = desc_t::lookup;
			uint32_t v = detail::lookup_uint32<l.leaf, l.reg, l.bit_begin,
					l.bit_end>();

			// And add to string.
			add_to_string(v, ret);
		}
		if constexpr (numl >= 2) {
			// First, get as uint32_t.
			constexpr detail::register_lookup l = desc_t::lookup2;
			uint32_t v = detail::lookup_uint32<l.leaf, l.reg, l.bit_begin,
					l.bit_end>();
			add_to_string(v, ret);
		}
		if constexpr (numl >= 3) {
			// First, get as uint32_t.
			constexpr detail::register_lookup l = desc_t::lookup3;
			uint32_t v = detail::lookup_uint32<l.leaf, l.reg, l.bit_begin,
					l.bit_end>();
			add_to_string(v, ret);
		}
		if constexpr (numl >= 4) {
			// First, get as uint32_t.
			constexpr detail::register_lookup l = desc_t::lookup4;
			uint32_t v = detail::lookup_uint32<l.leaf, l.reg, l.bit_begin,
					l.bit_end>();
			add_to_string(v, ret);
		}

		return ret;
	}

	template <query_type_e E>
	static std::enable_if_t<E == query_type_e::bool_, bool> get_value(
			query_e query) {
		assert(num_lookups[query] == 1);
		return detail::lookup_bool(lookups[query]);
	}

	template <query_type_e E>
	static std::enable_if_t<E == query_type_e::uint32_, uint32_t> get_value(
			query_e query) {
		assert(num_lookups[query] == 1);
		return detail::lookup_uint32(lookups[query]);
	}

	template <query_type_e E>
	static std::enable_if_t<E == query_type_e::string_, std::string> get_value(
			query_e query) {
		size_t numl = num_lookups[query];

		std::string ret;
		ret.reserve(numl * 4 + 1); // 4 chars per lookup

		auto add_to_string = [](uint32_t v, std::string& ret) {
			assert(ret.capacity() > ret.size() + 4);
			const char* as_str = reinterpret_cast<const char*>(&v);
			ret.insert(ret.end(), as_str, as_str + 4);
		};

		switch (numl) {
		case 1: {
			uint32_t v = detail::lookup_uint32(lookups[query]);
			add_to_string(v, ret);
		} break;
		case 2: {
			uint32_t v = detail::lookup_uint32(lookups2[query]);
			add_to_string(v, ret);
		} break;
		case 3: {
			// uint32_t v = detail::lookup_uint32(lookups3[query]);
			// add_to_string(v, ret);
		} break;
		case 4: {
		} break;
		default: {
			assert(false); // TODO : add more lookups
		} break;
		}

		return ret;
	}

	static bool is_hex_output(query_e q) {
		return print_hexes[q];
	}
};

} // namespace cpuid

// template <cpuid::query_type_e>
// struct data_getter;
//
// template <>
// struct data_getter<bool> {
//	template <class Desc>
//	static bool get() {
//		// bit_end is ignored for bools.
//		static_assert(Desc::bit_end == 0,
//				"fea::cpuid : bit_end is ignored for bool queries.");
//		static_assert(Desc::num_lookups == 1,
//				"fea::cpuid : Bool queries only support 1 lookup.");
//
//		const cpuid::leaf_data& runtime_data = cpuid::leaf_db::data[Desc::leaf];
//		return runtime_data[Desc::reg][Desc::bit_begin];
//	}
// };
//
// template <>
// struct data_getter<uint32_t> {
//	template <class Desc>
//	static uint32_t get() {
//		constexpr size_t bit_size = Desc::bit_end - Desc::bit_begin;
//		constexpr size_t reg_size = cpuid::leaf_data::register_size;
//
//		static_assert(reg_size == 32,
//				"fea::cpuid : Register size is expected to be 32. Refactor "
//				"to_ulong.");
//		static_assert(bit_size > 1,
//				"fea::cpuid : uint32_t query expects a size bigger than 1. Use "
//				"bool type for single bits.");
//		static_assert(Desc::num_lookups == 1,
//				"fea::cpuid : uint32 queries only support 1 lookup.");
//
//		const cpuid::leaf_data& runtime_data = cpuid::leaf_db::data[Desc::leaf];
//
//		if constexpr (bit_size == reg_size) {
//			// No mask needed.
//			return runtime_data[Desc::reg].to_ulong();
//		} else {
//			constexpr uint32_t mask
//					= fea::make_bitmask<uint32_t, bit_size, Desc::bit_begin>();
//			uint32_t ret = runtime_data[Desc::reg].to_ulong() & mask;
//			return ret >> Desc::bit_begin;
//		}
//
//
//		// if constexpr (bit_size == 0) {
//		//	return {};
//		// } else if constexpr (bit_size == 1 || Desc::bit_end == 0) {
//		//	// bit_end = 0 is a shortcut for single bits.
//		//	return Desc::type{ d[Desc::reg][Desc::bit_begin] };
//		// } else if constexpr (bit_size == cpuid::leaf_data::register_size) {
//		//	static_assert(cpuid::leaf_data::register_size == 32,
//		//			"fea::cpu_info : Need to update to_ulong.");
//
//		//	return Desc::type{ d[Desc::reg].to_ulong() };
//		//}
//	}
// };
} // namespace fea
