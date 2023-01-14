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
#include "fea/containers/span.hpp"
#include "fea/enum/enum_array.hpp"
#include "fea/utils/platform.hpp"

#if FEA_POSIX
#include <cpuid.h>
#else
#include <intrin.h>
#endif

#if FEA_MACOS
#undef __cpuid
#endif

#include <bitset>
#include <cassert>
#include <cstdint>

namespace fea {
namespace cpuid {
struct leaf_address {
	uint32_t leaf = 0;
	uint32_t subleaf = 0;
};

// A leaf's data register key.
enum class reg_e : uint8_t {
	eax,
	ebx,
	ecx,
	edx,
	count,
};


// Registers returned from a cpuid function call.
// Dereference with cpuid_reg enum.
struct leaf_data {
	static constexpr size_t register_size = 32;

	const std::bitset<register_size>& operator[](reg_e r) const {
		return data[r];
	}
	std::bitset<register_size>& operator[](reg_e r) {
		return data[r];
	}

	fea::enum_array<std::bitset<register_size>, reg_e> data;
};
static_assert(sizeof(leaf_data) == 16, "bleep bloop bzzzzz");


// Returns the requested registers, stored in order eax, ebx, ecx, edx.
leaf_data load_leaf(uint32_t leaf, uint32_t sub_leaf) {
	leaf_data ret;

#if FEA_WINDOWS
	std::array<int, 4> d = {};
	__cpuidex(d.data(), int(leaf), int(sub_leaf));

	ret[reg_e::eax] = uint32_t(d[0]);
	ret[reg_e::ebx] = uint32_t(d[1]);
	ret[reg_e::ecx] = uint32_t(d[2]);
	ret[reg_e::edx] = uint32_t(d[3]);
#else
	uint32_t a = 0, b = 0, c = 0, d = 0;
	__cpuid_count(leaf, sub_leaf, a, b, c, d);

	ret[reg_e::eax] = a;
	ret[reg_e::ebx] = b;
	ret[reg_e::ecx] = c;
	ret[reg_e::edx] = d;
#endif
	return ret;
}

// Returns the requested registers, stored in order eax, ebx, ecx, edx.
leaf_data load_leaf(uint32_t leaf) {
	return load_leaf(leaf, 0);
}

// Returns the requested registers, stored in order eax, ebx, ecx, edx.
leaf_data load_leaf(leaf_address addr) {
	return load_leaf(addr.leaf, addr.subleaf);
}


// Loads all supported leaves.
// Both supported by the library and supported by the cpu.
//
// The output span must be backed by as many items as demanded leaves.
void load_leaves(
		fea::span<const leaf_address> addresses, fea::span<leaf_data> out) {
	assert(addresses.size() == out.size());

	// First, get the highest functions.
	uint32_t max_lo_leaf = 0;
	uint32_t max_hi_leaf = 0;
	{
		leaf_data d = load_leaf(0);
		max_lo_leaf = d[reg_e::eax].to_ulong();
	}
	{
		leaf_data d = load_leaf(0x80000000);
		max_hi_leaf = d[reg_e::eax].to_ulong();
	}

	// Now get all supported functions.
	for (size_t i = 0; i < addresses.size(); ++i) {
		leaf_address addr = addresses[i];
		uint32_t max_leaf = addr.leaf >= 0x80000000 ? max_hi_leaf : max_lo_leaf;

		if (addr.leaf > max_leaf) {
			// Unsupported.
			out[i] = {};
			continue;
		}
		out[i] = load_leaf(addr);
	}
}

} // namespace cpuid
} // namespace fea
