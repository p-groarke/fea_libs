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
#include "fea/descriptors/descriptor_map.hpp"
#include "fea/enum/enum_array.hpp"
#include "fea/performance/cpuid_details/leaf.hpp"

#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <string>

// References :
// https://en.wikipedia.org/wiki/CPUID
// https://www.scss.tcd.ie/Jeremy.Jones/CS4021/processor-identification-cpuid-instruction-note.pdf
// https://cdrdv2.intel.com/v1/dl/getContent/671110 (Intel Manual Vol. 2)

namespace fea {
namespace cpuid {
// The supported leaves to query.
// Subleaves are encoded in this key as well.
enum class leaf_e : uint8_t {
	eax0,
	eax1,
	eax2,
	eax3,
	eax4,
	eax6,
	eax7,
	eax7_ecx1,
	eaxbh,
	eaxdh_ecx1,
	eax12h,
	eax14h,
	eax19h,
	eax80000000h,
	eax80000001h,
	eax80000002h,
	eax80000003h,
	eax80000004h,
	eax80000005h,
	eax80000006h,
	eax80000007h,
	eax80000008h,
	eax8000001fh,
	eax80000021h,
	eax8fffffffh,
	count,
};

namespace detail {
struct eax0_desc {
	// Our key.
	static constexpr leaf_e key = leaf_e::eax0;

	// Our leaf, passed to cpuid call.
	static constexpr uint32_t leaf = 0x0;

	// Our subleaf, passed to cpuid call, 0 if none.
	static constexpr uint32_t subleaf = 0;
};

struct eax1_desc {
	static constexpr leaf_e key = leaf_e::eax1;
	static constexpr uint32_t leaf = 0x1;
	static constexpr uint32_t subleaf = 0;
};

struct eax2_desc {
	static constexpr leaf_e key = leaf_e::eax2;
	static constexpr uint32_t leaf = 0x2;
	static constexpr uint32_t subleaf = 0;
};

struct eax3_desc {
	static constexpr leaf_e key = leaf_e::eax3;
	static constexpr uint32_t leaf = 0x3;
	static constexpr uint32_t subleaf = 0;
};

struct eax4_desc {
	static constexpr leaf_e key = leaf_e::eax4;
	static constexpr uint32_t leaf = 0x4;
	static constexpr uint32_t subleaf = 0;
};

struct eax6_desc {
	static constexpr leaf_e key = leaf_e::eax6;
	static constexpr uint32_t leaf = 0x6;
	static constexpr uint32_t subleaf = 0;
};

struct eax7_desc {
	static constexpr leaf_e key = leaf_e::eax7;
	static constexpr uint32_t leaf = 0x7;
	static constexpr uint32_t subleaf = 0;
};

struct eax7_ecx1_desc {
	static constexpr leaf_e key = leaf_e::eax7_ecx1;
	static constexpr uint32_t leaf = 0x7;
	static constexpr uint32_t subleaf = 1;
};

struct eaxbh_desc {
	static constexpr leaf_e key = leaf_e::eaxbh;
	static constexpr uint32_t leaf = 0xb;
	static constexpr uint32_t subleaf = 0;
};

struct eaxdh_ecx1_desc {
	static constexpr leaf_e key = leaf_e::eaxdh_ecx1;
	static constexpr uint32_t leaf = 0xd;
	static constexpr uint32_t subleaf = 1;
};

struct eax12h_desc {
	static constexpr leaf_e key = leaf_e::eax12h;
	static constexpr uint32_t leaf = 0x12;
	static constexpr uint32_t subleaf = 0;
};

struct eax14h_desc {
	static constexpr leaf_e key = leaf_e::eax14h;
	static constexpr uint32_t leaf = 0x14;
	static constexpr uint32_t subleaf = 0;
};

struct eax19h_desc {
	static constexpr leaf_e key = leaf_e::eax19h;
	static constexpr uint32_t leaf = 0x19;
	static constexpr uint32_t subleaf = 0;
};

struct eax80000000h_desc {
	static constexpr leaf_e key = leaf_e::eax80000000h;
	static constexpr uint32_t leaf = 0x80000000;
	static constexpr uint32_t subleaf = 0;
};

struct eax80000001h_desc {
	static constexpr leaf_e key = leaf_e::eax80000001h;
	static constexpr uint32_t leaf = 0x80000001;
	static constexpr uint32_t subleaf = 0;
};

struct eax80000002h_desc {
	static constexpr leaf_e key = leaf_e::eax80000002h;
	static constexpr uint32_t leaf = 0x80000002;
	static constexpr uint32_t subleaf = 0;
};

struct eax80000003h_desc {
	static constexpr leaf_e key = leaf_e::eax80000003h;
	static constexpr uint32_t leaf = 0x80000003;
	static constexpr uint32_t subleaf = 0;
};

struct eax80000004h_desc {
	static constexpr leaf_e key = leaf_e::eax80000004h;
	static constexpr uint32_t leaf = 0x80000004;
	static constexpr uint32_t subleaf = 0;
};

struct eax80000005h_desc {
	static constexpr leaf_e key = leaf_e::eax80000005h;
	static constexpr uint32_t leaf = 0x80000005;
	static constexpr uint32_t subleaf = 0;
};

struct eax80000006h_desc {
	static constexpr leaf_e key = leaf_e::eax80000006h;
	static constexpr uint32_t leaf = 0x80000006;
	static constexpr uint32_t subleaf = 0;
};

struct eax80000007h_desc {
	static constexpr leaf_e key = leaf_e::eax80000007h;
	static constexpr uint32_t leaf = 0x80000007;
	static constexpr uint32_t subleaf = 0;
};

struct eax80000008h_desc {
	static constexpr leaf_e key = leaf_e::eax80000008h;
	static constexpr uint32_t leaf = 0x80000008;
	static constexpr uint32_t subleaf = 0;
};

struct eax8000001fh_desc {
	static constexpr leaf_e key = leaf_e::eax8000001fh;
	static constexpr uint32_t leaf = 0x8000001f;
	static constexpr uint32_t subleaf = 0;
};

struct eax80000021h_desc {
	static constexpr leaf_e key = leaf_e::eax80000021h;
	static constexpr uint32_t leaf = 0x80000021;
	static constexpr uint32_t subleaf = 0;
};

struct eax8fffffffh_desc {
	static constexpr leaf_e key = leaf_e::eax8fffffffh;
	static constexpr uint32_t leaf = 0x8fffffff;
	static constexpr uint32_t subleaf = 0;
};


// clang-format off
using leaf_db = fea::descriptor_map<
	leaf_e // key enum
	, eax0_desc // the descriptors, in key order
	, eax1_desc
	, eax2_desc
	, eax3_desc
	, eax4_desc
	, eax6_desc
	, eax7_desc
	, eax7_ecx1_desc
	, eaxbh_desc
	, eaxdh_ecx1_desc
	, eax12h_desc
	, eax14h_desc
	, eax19h_desc
	, eax80000000h_desc
	, eax80000001h_desc
	, eax80000002h_desc
	, eax80000003h_desc
	, eax80000004h_desc
	, eax80000005h_desc
	, eax80000006h_desc
	, eax80000007h_desc
	, eax80000008h_desc
	, eax8000001fh_desc
	, eax80000021h_desc
	, eax8fffffffh_desc
>;
// clang-format on

fea::enum_array<leaf_data, leaf_e> load_leaves_imp(
		const fea::enum_array<leaf_address, leaf_e>& addresses) {
	fea::enum_array<leaf_data, leaf_e> ret{};
	load_leaves(fea::span<const leaf_address>{ addresses },
			fea::span<leaf_data>{ ret });
	return ret;
}
} // namespace detail

struct leaf_db : detail::leaf_db {
	// Enum array of leaf addresses.
	static constexpr auto addresses = make_enum_array([](auto desc) {
		return leaf_address{ desc.leaf, desc.subleaf };
	});

	// Enum array of uint32_t leaves.
	static constexpr auto leaves
			= make_enum_array([](auto desc) { return desc.leaf; });

	// Enum array of uint32_t subleaves.
	static constexpr auto subleaves
			= make_enum_array([](auto desc) { return desc.subleaf; });

	// The runtime supported leaf data.
	// This library only deals with constants, not runtime sensor / frequency
	// information.
	inline static const fea::enum_array<leaf_data, leaf_e> data
			= detail::load_leaves_imp(addresses);
};

// struct lookup {
//	leaf_e mkey = leaf_e::count;
//	uint32_t mleaf = 0;
//	uint32_t msubleaf = 0;
// };

// static constexpr std::array<lookup, size_t(leaf_e::count)> lookups{
//	lookup{ leaf_e::eax0, 0, 0 },
//	lookup{ leaf_e::eax1, 1, 0 },
//	lookup{ leaf_e::eax2, 2, 0 },
//	lookup{ leaf_e::eax3, 3, 0 },
//	lookup{ leaf_e::eax4, 4, 0 },
//	lookup{ leaf_e::eax6, 6, 0 },
//	lookup{ leaf_e::eax7, 7, 0 },
//	lookup{ leaf_e::eax7_ecx1, 7, 1 },
//	lookup{ leaf_e::eaxbh, 0xb, 0 },
//	lookup{ leaf_e::eaxdh_ecx1, 0xd, 1 },
//	lookup{ leaf_e::eax12h, 0x12, 0 },
//	lookup{ leaf_e::eax14h, 0x14, 0 },
//	lookup{ leaf_e::eax19h, 0x19, 0 },
//	lookup{ leaf_e::eax80000000h, 0x80000000, 0 },
//	lookup{ leaf_e::eax80000001h, 0x80000001, 0 },
//	lookup{ leaf_e::eax80000002h, 0x80000002, 0 },
//	lookup{ leaf_e::eax80000003h, 0x80000003, 0 },
//	lookup{ leaf_e::eax80000004h, 0x80000004, 0 },
//	lookup{ leaf_e::eax80000005h, 0x80000005, 0 },
//	lookup{ leaf_e::eax80000006h, 0x80000006, 0 },
//	lookup{ leaf_e::eax80000007h, 0x80000007, 0 },
//	lookup{ leaf_e::eax80000008h, 0x80000008, 0 },
//	lookup{ leaf_e::eax8000001fh, 0x8000001f, 0 },
//	lookup{ leaf_e::eax80000021h, 0x80000021, 0 },
//	lookup{ leaf_e::eax8fffffffh, 0x8fffffff, 0 },
// };

} // namespace cpuid
} // namespace fea
