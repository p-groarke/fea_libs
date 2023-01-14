/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Philippe Groarke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 this
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
#include "fea/performance/cpuid_details/leaf_db.hpp"
#include "fea/performance/cpuid_details/query_db.hpp"

#include <cassert>
#include <cstdio>

namespace fea {
namespace detail {} // namespace detail
#define FEA_CPUID_ADD(name)

struct cpu_info_test {
	// Returns the query's name (the 'key' string).
	static constexpr std::string_view query_name(cpuid::query_e q) {
		return cpuid::to_string(q);
	}

	// Returns the query's value type.
	static constexpr cpuid::query_type_e query_type(cpuid::query_e q) {
		return cpuid::query_db::types[q];
	}

	// Returns a query's value.
	template <cpuid::query_type_e E>
	static auto query_value(cpuid::query_e q) {
		return cpuid::query_db::get_value<E>(q);
	}

	static void print(cpuid::query_e query) {
		std::string_view name = query_name(query);
		printf("%-24.*s", int(name.size()), name.data());

		cpuid::query_type_e type = query_type(query);
		switch (type) {
		case cpuid::query_type_e::bool_: {
			bool v = query_value<cpuid::query_type_e::bool_>(query);
			printf("%s\n", v ? "true" : "false");
		} break;
		case cpuid::query_type_e::uint32_: {
			uint32_t v = query_value<cpuid::query_type_e::uint32_>(query);
			if (cpuid::query_db::is_hex_output(query)) {
				printf("0x%X (%u)\n", v, v);
			} else {
				printf("%u\n", v);
			}
		} break;
		case cpuid::query_type_e::string_: {
			std::string v = query_value<cpuid::query_type_e::string_>(query);
			printf("%s\n", v.c_str());
		} break;
		default: {
			// assert(false);
		} break;
		}
	}

	static void print() {
		for (size_t i = 0; i < size_t(cpuid::query_e::count); ++i) {
			cpuid::query_e q = cpuid::query_e(i);
			print(q);
		}
	}


	// Bla bla
	static uint32_t highest_leaf() {
		static const uint32_t ret
				= cpuid::query_db::get_value<cpuid::query_e::highest_leaf>();
		return ret;
	}

	static const std::string& vendor() {
		static const std::string ret
				= cpuid::query_db::get_value<cpuid::query_e::vendor>();
		return ret;
	}

	// static const typename highest_leaf::type highest_leaf
	//		= get_uint(highest_leaf::lookup);

	// static constexpr std::tuple<std::array<std::string_view, 0>> data;
};
} // namespace fea
