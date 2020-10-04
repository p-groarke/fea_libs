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
#include <fea/utils/platform.hpp>

#if FEA_POSIX
#include <cpuid.h>
#else
#include <intrin.h>
#endif

#if FEA_MACOS
#undef __cpuid
#endif

#include <array>
#include <bitset>
#include <string>
#include <vector>

namespace fea {

namespace detail {
void print_bool(const char* str, bool b) {
	printf("%-18s%s\n", str, b ? "true" : "false");
}
} // namespace detail

struct cpu_id final {
	cpu_id() = default;
	cpu_id(const cpu_id&) = default;
	cpu_id(cpu_id&&) = default;

	cpu_id& operator=(const cpu_id&) = default;
	cpu_id& operator=(cpu_id&&) = default;

	cpu_id(uint32_t leaf_)
			: leaf(leaf_) {
#if FEA_WINDOWS
		std::array<int, 4> d = { 0 };
		__cpuid(d.data(), static_cast<int>(leaf));
		eax = d[0];
		ebx = d[1];
		ecx = d[2];
		edx = d[3];
#else
		uint32_t a = 0, b = 0, c = 0, d = 0;
		__get_cpuid(leaf, &a, &b, &c, &d);
		eax = a;
		ebx = b;
		ecx = c;
		edx = d;
#endif
	}

	cpu_id(uint32_t leaf_, uint32_t sub_leaf_)
			: leaf(leaf_)
			, sub_leaf(sub_leaf_) {
#if FEA_WINDOWS
		std::array<int, 4> d = {};
		__cpuidex(d.data(), static_cast<int>(leaf), static_cast<int>(sub_leaf));
		eax = d[0];
		ebx = d[1];
		ecx = d[2];
		edx = d[3];
#else
		uint32_t a = 0, b = 0, c = 0, d = 0;
		__cpuid_count(leaf, sub_leaf, a, b, c, d);
		eax = a;
		ebx = b;
		ecx = c;
		edx = d;
#endif
	}

	std::array<char, 16> to_string() const {
		std::array<char, 16> ret;
		unsigned long a = eax.to_ulong();
		unsigned long b = ebx.to_ulong();
		unsigned long c = ecx.to_ulong();
		unsigned long d = edx.to_ulong();
		const char* a_str = reinterpret_cast<const char*>(&a);
		const char* b_str = reinterpret_cast<const char*>(&b);
		const char* c_str = reinterpret_cast<const char*>(&c);
		const char* d_str = reinterpret_cast<const char*>(&d);
		std::copy(a_str, a_str + 4, ret.begin());
		std::copy(b_str, b_str + 4, ret.begin() + 4);
		std::copy(c_str, c_str + 4, ret.begin() + 8);
		std::copy(d_str, d_str + 4, ret.begin() + 12);
		return ret;
	}

	uint32_t leaf = 0;
	uint32_t sub_leaf = 0;
	std::bitset<32> eax = { 0 };
	std::bitset<32> ebx = { 0 };
	std::bitset<32> ecx = { 0 };
	std::bitset<32> edx = { 0 };
};

struct cpu_info_t final {
	cpu_info_t() {
		{ // Functions
			std::vector<cpu_id> functions = {};

			// calling __cpuid with 0x0 as the function_id argument
			// gets the number of the highest valid function id.
			uint32_t num_funcs = cpu_id{ 0 }.eax.to_ulong();
			functions.reserve(num_funcs);

			// Get all info
			for (uint32_t i = 0; i < num_funcs; ++i) {
				functions.push_back({ i, 0 });
			}

			// Vendor ID String (0x0)
			char vendor[13] = { 0 };
			std::array<char, 16> as_string = functions[0].to_string();
			std::copy(as_string.begin() + 4, as_string.begin() + 8, &vendor[0]);
			std::copy(as_string.begin() + 12, as_string.end(), &vendor[4]);
			std::copy(
					as_string.begin() + 8, as_string.begin() + 12, &vendor[8]);

			_vendor = vendor;

			if (_vendor == "GenuineIntel") {
				_is_intel = true;
			} else if (_vendor == "AuthenticAMD") {
				_is_amd = true;
			}

			// Processor Info and Feature Bits (0x1)
			if (functions.size() > 1) {
				_feature_bits = functions[1];
			}

			// Features Extended (0x7)
			if (functions.size() > 7) {
				_feature_bits_more = functions[7];
			}
		}

		// Extended Functions
		{
			std::vector<cpu_id> ext_functions = {};

			// calling __cpuid with 0x80000000 as the function_id argument
			// gets the number of the highest valid extended id.
			uint32_t highest_ext_func_id = cpu_id{ 0x80000000 }.eax.to_ulong();

			// Get all info
			for (uint32_t i = 0x80000000; i < highest_ext_func_id; ++i) {
				ext_functions.emplace_back(i, 0);
			}

			// Processor Brand String (0x80000004)
			char brand[64] = { 0 };
			if (highest_ext_func_id >= 0x80000004) {
				for (size_t i = 2; i <= 4; ++i) {
					std::array<char, 16> as_string
							= ext_functions[i].to_string();
					size_t pos = (i - 2) * 16;
					std::copy(as_string.begin(), as_string.end(), &brand[pos]);
				}
				_brand = brand;
			}

			// Extended Processor Info and Feature Bits (0x80000001)
			if (highest_ext_func_id >= 0x80000001) {
				_ext_feature_bits = ext_functions[1];
			}
		}
	}

public:
	const char* vendor() const {
		return _vendor.c_str();
	}
	const char* brand() const {
		return _brand.c_str();
	}

	bool intel() const {
		return _is_intel;
	}

	bool amd() const {
		return _is_amd;
	}

	// Reference : https://en.wikipedia.org/wiki/CPUID

	// EAX=1 CPUID feature bits
	// EDX
	bool fpu() const {
		return _feature_bits.edx[0];
	}
	bool vme() const {
		return _feature_bits.edx[1];
	}
	bool de() const {
		return _feature_bits.edx[2];
	}
	bool pse() const {
		return _feature_bits.edx[3];
	}
	bool tsc() const {
		return _feature_bits.edx[4];
	}
	bool msr() const {
		return _feature_bits.edx[5];
	}
	bool pae() const {
		return _feature_bits.edx[6];
	}
	bool mce() const {
		return _feature_bits.edx[7];
	}
	bool cx8() const {
		return _feature_bits.edx[8];
	}
	bool apic() const {
		return _feature_bits.edx[9];
	}
	// 10
	bool sep() const {
		return _feature_bits.edx[11];
	}
	bool mtrr() const {
		return _feature_bits.edx[12];
	}
	bool pge() const {
		return _feature_bits.edx[13];
	}
	bool mca() const {
		return _feature_bits.edx[14];
	}
	bool cmov() const {
		return _feature_bits.edx[15];
	}
	bool pat() const {
		return _feature_bits.edx[16];
	}
	bool pse36() const {
		return _feature_bits.edx[17];
	}
	bool psn() const {
		return _feature_bits.edx[18];
	}
	bool clfsh() const {
		return _feature_bits.edx[19];
	}
	// 20
	bool ds() const {
		return _feature_bits.edx[21];
	}
	bool acpi() const {
		return _feature_bits.edx[22];
	}
	bool mmx() const {
		return _feature_bits.edx[23];
	}
	bool fxsr() const {
		return _feature_bits.edx[24];
	}
	bool sse() const {
		return _feature_bits.edx[25];
	}
	bool sse2() const {
		return _feature_bits.edx[26];
	}
	bool ss() const {
		return _feature_bits.edx[27];
	}
	bool htt() const {
		return _feature_bits.edx[28];
	}
	bool tm() const {
		return _feature_bits.edx[29];
	}
	bool ia64() const {
		return _feature_bits.edx[30];
	}
	bool pbe() const {
		return _feature_bits.edx[31];
	}

	// ECX
	bool sse3() const {
		return _feature_bits.ecx[0];
	}
	bool pclmulqdq() const {
		return _feature_bits.ecx[1];
	}
	bool dtes64() const {
		return _feature_bits.ecx[2];
	}
	bool monitor() const {
		return _feature_bits.ecx[3];
	}
	bool ds_cpl() const {
		return _feature_bits.ecx[4];
	}
	bool vmx() const {
		return _feature_bits.ecx[5];
	}
	bool smx() const {
		return _feature_bits.ecx[6];
	}
	bool est() const {
		return _feature_bits.ecx[7];
	}
	bool tm2() const {
		return _feature_bits.ecx[8];
	}
	bool ssse3() const {
		return _feature_bits.ecx[9];
	}
	bool cnxt_id() const {
		return _feature_bits.ecx[10];
	}
	bool sdbg() const {
		return _feature_bits.ecx[11];
	}
	bool fma() const {
		return _feature_bits.ecx[12];
	}
	bool cx16() const {
		return _feature_bits.ecx[13];
	}
	bool xtpr() const {
		return _feature_bits.ecx[14];
	}
	bool pdcm() const {
		return _feature_bits.ecx[15];
	}
	// 16
	bool pcid() const {
		return _feature_bits.ecx[17];
	}
	bool dca() const {
		return _feature_bits.ecx[18];
	}
	bool sse41() const {
		return _feature_bits.ecx[19];
	}
	bool sse42() const {
		return _feature_bits.ecx[20];
	}
	bool x2apic() const {
		return _feature_bits.ecx[21];
	}
	bool movbe() const {
		return _feature_bits.ecx[22];
	}
	bool popcnt() const {
		return _feature_bits.ecx[23];
	}
	bool tsc_deadline() const {
		return _feature_bits.ecx[24];
	}
	bool aes() const {
		return _feature_bits.ecx[25];
	}
	bool xsave() const {
		return _feature_bits.ecx[26];
	}
	bool osxsave() const {
		return _feature_bits.ecx[27];
	}
	bool avx() const {
		return _feature_bits.ecx[28];
	}
	bool f16c() const {
		return _feature_bits.ecx[29];
	}
	bool rdrnd() const {
		return _feature_bits.ecx[30];
	}
	bool hypervisor() const {
		return _feature_bits.ecx[31];
	}


	// EAX=7 CPUID feature bits
	// EBX
	bool fsgsbase() const {
		return _feature_bits_more.ebx[0];
	}
	bool ia32_tsc_adjust() const {
		return _feature_bits_more.ebx[1];
	}
	bool sgx() const {
		return _feature_bits_more.ebx[2];
	}
	bool bmi1() const {
		return _feature_bits_more.ebx[3];
	}
	bool hle() const {
		return _feature_bits_more.ebx[4];
	}
	bool avx2() const {
		return _feature_bits_more.ebx[5];
	}
	// 6
	bool smep() const {
		return _feature_bits_more.ebx[7];
	}
	bool bmi2() const {
		return _feature_bits_more.ebx[8];
	}
	bool erms() const {
		return _feature_bits_more.ebx[9];
	}
	bool invpcid() const {
		return _feature_bits_more.ebx[10];
	}
	bool rtm() const {
		return _feature_bits_more.ebx[11];
	}
	bool pqm() const {
		return _feature_bits_more.ebx[12];
	}
	// 13
	bool mpx() const {
		return _feature_bits_more.ebx[14];
	}
	bool pqe() const {
		return _feature_bits_more.ebx[15];
	}
	bool avx512f() const {
		return _feature_bits_more.ebx[16];
	}
	bool avx512dq() const {
		return _feature_bits_more.ebx[17];
	}
	bool rdseed() const {
		return _feature_bits_more.ebx[18];
	}
	bool adx() const {
		return _feature_bits_more.ebx[19];
	}
	bool smap() const {
		return _feature_bits_more.ebx[20];
	}
	bool avx512ifma() const {
		return _feature_bits_more.ebx[21];
	}
	bool pcommit() const {
		return _feature_bits_more.ebx[22];
	}
	bool clflushopt() const {
		return _feature_bits_more.ebx[23];
	}
	bool clwb() const {
		return _feature_bits_more.ebx[24];
	}
	bool intel_pt() const {
		return _feature_bits_more.ebx[25];
	}
	bool avx512pf() const {
		return _feature_bits_more.ebx[26];
	}
	bool avx512er() const {
		return _feature_bits_more.ebx[27];
	}
	bool avx512cd() const {
		return _feature_bits_more.ebx[28];
	}
	bool sha() const {
		return _feature_bits_more.ebx[29];
	}
	bool avx512bw() const {
		return _feature_bits_more.ebx[30];
	}
	bool avx512vl() const {
		return _feature_bits_more.ebx[31];
	}

	// ECX
	bool prefetchwt1() const {
		return _feature_bits_more.ecx[0];
	}
	bool avx512vbmi() const {
		return _feature_bits_more.ecx[1];
	}
	bool umip() const {
		return _feature_bits_more.ecx[2];
	}
	bool pku() const {
		return _feature_bits_more.ecx[3];
	}
	bool ospke() const {
		return _feature_bits_more.ecx[4];
	}
	// 5
	bool avx512vbmi2() const {
		return _feature_bits_more.ecx[6];
	}
	// 7
	bool gfni() const {
		return _feature_bits_more.ecx[8];
	}
	bool vaes() const {
		return _feature_bits_more.ecx[9];
	}
	bool vpclmulqdq() const {
		return _feature_bits_more.ecx[10];
	}
	bool avx512vnni() const {
		return _feature_bits_more.ecx[11];
	}
	bool avx512bitalg() const {
		return _feature_bits_more.ecx[12];
	}
	// 13
	bool avx512vpopcntdq() const {
		return _feature_bits_more.ecx[14];
	}
	// 15 - 16
	unsigned short mawau() const {
		uint32_t bits = _feature_bits_more.ecx.to_ulong();
		bits <<= 10;
		bits >>= 27;
		return static_cast<unsigned short>(bits);
	}
	bool rdpid() const {
		return _feature_bits_more.ecx[22];
	}
	// 23 - 29
	bool sgx_lc() const {
		return _feature_bits_more.ecx[30];
	}
	// 30

	// EDX
	// 0 - 1
	bool avx512_4vnniw() const {
		return _feature_bits_more.edx[2];
	}
	bool avx512_4fmaps() const {
		return _feature_bits_more.edx[3];
	}
	// 4 - 17
	bool pconfig() const {
		return _feature_bits_more.edx[19];
	}
	// 19 - 25
	bool spec_ctrl() const {
		return _feature_bits_more.edx[26];
	}
	// 27 - 31


	// EAX=80000001h CPUID feature bits
	// EDX
	bool fpu_ext() const {
		return _ext_feature_bits.edx[0];
	}
	bool vme_ext() const {
		return _ext_feature_bits.edx[1];
	}
	bool de_ext() const {
		return _ext_feature_bits.edx[2];
	}
	bool pse_ext() const {
		return _ext_feature_bits.edx[3];
	}
	bool tsc_ext() const {
		return _ext_feature_bits.edx[4];
	}
	bool msr_ext() const {
		return _ext_feature_bits.edx[5];
	}
	bool pae_ext() const {
		return _ext_feature_bits.edx[6];
	}
	bool mce_ext() const {
		return _ext_feature_bits.edx[7];
	}
	bool cx8_ext() const {
		return _ext_feature_bits.edx[8];
	}
	bool apic_ext() const {
		return _ext_feature_bits.edx[9];
	}
	// 10
	bool syscall() const {
		return _ext_feature_bits.edx[11];
	}
	bool mtrr_ext() const {
		return _ext_feature_bits.edx[12];
	}
	bool pge_ext() const {
		return _ext_feature_bits.edx[13];
	}
	bool mca_ext() const {
		return _ext_feature_bits.edx[14];
	}
	bool cmov_ext() const {
		return _ext_feature_bits.edx[15];
	}
	bool pat_ext() const {
		return _ext_feature_bits.edx[16];
	}
	bool pse36_ext() const {
		return _ext_feature_bits.edx[17];
	}
	// 18
	bool mp() const {
		return _ext_feature_bits.edx[19];
	}
	bool nx() const {
		return _ext_feature_bits.edx[20];
	}
	// 21
	bool mmxext() const {
		return _ext_feature_bits.edx[22];
	}
	bool mmx_ext() const {
		return _ext_feature_bits.edx[23];
	}
	bool fxsr_ext() const {
		return _ext_feature_bits.edx[24];
	}
	bool fxsr_opt() const {
		return _ext_feature_bits.edx[25];
	}
	bool pdpe1gb() const {
		return _ext_feature_bits.edx[26];
	}
	bool rdtscp() const {
		return _ext_feature_bits.edx[27];
	}
	// 28
	bool lm() const {
		return _ext_feature_bits.edx[29];
	}
	bool _3dnowext() const {
		return _ext_feature_bits.edx[30];
	}
	bool _3dnow() const {
		return _ext_feature_bits.edx[31];
	}

	// ECX
	bool lahf_lm() const {
		return _ext_feature_bits.ecx[0];
	}
	bool cmp_legacy() const {
		return _ext_feature_bits.ecx[1];
	}
	bool svm() const {
		return _ext_feature_bits.ecx[2];
	}
	bool extapic() const {
		return _ext_feature_bits.ecx[3];
	}
	bool cr8_legacy() const {
		return _ext_feature_bits.ecx[4];
	}
	bool abm() const {
		return _ext_feature_bits.ecx[5];
	}
	bool sse4a() const {
		return _ext_feature_bits.ecx[6];
	}
	bool misalignsse() const {
		return _ext_feature_bits.ecx[7];
	}
	bool _3dnowprefetch() const {
		return _ext_feature_bits.ecx[8];
	}
	bool osvw() const {
		return _ext_feature_bits.ecx[9];
	}
	bool ibs() const {
		return _ext_feature_bits.ecx[10];
	}
	bool xop() const {
		return _ext_feature_bits.ecx[11];
	}
	bool skinit() const {
		return _ext_feature_bits.ecx[12];
	}
	bool wdt() const {
		return _ext_feature_bits.ecx[13];
	}
	// 14
	bool lwp() const {
		return _ext_feature_bits.ecx[15];
	}
	bool fma4() const {
		return _ext_feature_bits.ecx[16];
	}
	bool tce() const {
		return _ext_feature_bits.ecx[17];
	}
	// 18
	bool nodeid_msr() const {
		return _ext_feature_bits.ecx[19];
	}
	// 20
	bool tbm() const {
		return _ext_feature_bits.ecx[21];
	}
	bool topoext() const {
		return _ext_feature_bits.ecx[22];
	}
	bool perfctr_core() const {
		return _ext_feature_bits.ecx[23];
	}
	bool perfctr_nb() const {
		return _ext_feature_bits.ecx[24];
	}
	// 25
	bool dbx() const {
		return _ext_feature_bits.ecx[26];
	}
	bool perftsc() const {
		return _ext_feature_bits.ecx[27];
	}
	bool pcx_l2i() const {
		return _ext_feature_bits.ecx[28];
	}
	// 29 - 31

	void print_all() const {
		printf("%-18s%s\n", "vendor", vendor());
		printf("%-18s%s\n", "brand", brand());
		detail::print_bool("fpu", fpu());
		detail::print_bool("vme", vme());
		detail::print_bool("de", de());
		detail::print_bool("pse", pse());
		detail::print_bool("tsc", tsc());
		detail::print_bool("msr", msr());
		detail::print_bool("pae", pae());
		detail::print_bool("mce", mce());
		detail::print_bool("cx8", cx8());
		detail::print_bool("apic", apic());
		detail::print_bool("sep", sep());
		detail::print_bool("mtrr", mtrr());
		detail::print_bool("pge", pge());
		detail::print_bool("mca", mca());
		detail::print_bool("cmov", cmov());
		detail::print_bool("pat", pat());
		detail::print_bool("pse36", pse36());
		detail::print_bool("psn", psn());
		detail::print_bool("clfsh", clfsh());
		detail::print_bool("ds", ds());
		detail::print_bool("acpi", acpi());
		detail::print_bool("mmx", mmx());
		detail::print_bool("fxsr", fxsr());
		detail::print_bool("sse", sse());
		detail::print_bool("sse2", sse2());
		detail::print_bool("ss", ss());
		detail::print_bool("htt", htt());
		detail::print_bool("tm", tm());
		detail::print_bool("ia64", ia64());
		detail::print_bool("pbe", pbe());
		detail::print_bool("sse3", sse3());
		detail::print_bool("pclmulqdq", pclmulqdq());
		detail::print_bool("dtes64", dtes64());
		detail::print_bool("monitor", monitor());
		detail::print_bool("ds_cpl", ds_cpl());
		detail::print_bool("vmx", vmx());
		detail::print_bool("smx", smx());
		detail::print_bool("est", est());
		detail::print_bool("tm2", tm2());
		detail::print_bool("ssse3", ssse3());
		detail::print_bool("cnxt_id", cnxt_id());
		detail::print_bool("sdbg", sdbg());
		detail::print_bool("fma", fma());
		detail::print_bool("cx16", cx16());
		detail::print_bool("xtpr", xtpr());
		detail::print_bool("pdcm", pdcm());
		detail::print_bool("pcid", pcid());
		detail::print_bool("dca", dca());
		detail::print_bool("sse41", sse41());
		detail::print_bool("sse42", sse42());
		detail::print_bool("x2apic", x2apic());
		detail::print_bool("movbe", movbe());
		detail::print_bool("popcnt", popcnt());
		detail::print_bool("tsc_deadline", tsc_deadline());
		detail::print_bool("aes", aes());
		detail::print_bool("xsave", xsave());
		detail::print_bool("osxsave", osxsave());
		detail::print_bool("avx", avx());
		detail::print_bool("f16c", f16c());
		detail::print_bool("rdrnd", rdrnd());
		detail::print_bool("hypervisor", hypervisor());
		detail::print_bool("fsgsbase", fsgsbase());
		detail::print_bool("ia32_tsc_adjust", ia32_tsc_adjust());
		detail::print_bool("sgx", sgx());
		detail::print_bool("bmi1", bmi1());
		detail::print_bool("hle", hle());
		detail::print_bool("avx2", avx2());
		detail::print_bool("smep", smep());
		detail::print_bool("bmi2", bmi2());
		detail::print_bool("erms", erms());
		detail::print_bool("invpcid", invpcid());
		detail::print_bool("rtm", rtm());
		detail::print_bool("pqm", pqm());
		detail::print_bool("mpx", mpx());
		detail::print_bool("pqe", pqe());
		detail::print_bool("avx512f", avx512f());
		detail::print_bool("avx512dq", avx512dq());
		detail::print_bool("rdseed", rdseed());
		detail::print_bool("adx", adx());
		detail::print_bool("smap", smap());
		detail::print_bool("avx512ifma", avx512ifma());
		detail::print_bool("pcommit", pcommit());
		detail::print_bool("clflushopt", clflushopt());
		detail::print_bool("clwb", clwb());
		detail::print_bool("intel_pt", intel_pt());
		detail::print_bool("avx512pf", avx512pf());
		detail::print_bool("avx512er", avx512er());
		detail::print_bool("avx512cd", avx512cd());
		detail::print_bool("sha", sha());
		detail::print_bool("avx512bw", avx512bw());
		detail::print_bool("avx512vl", avx512vl());
		detail::print_bool("prefetchwt1", prefetchwt1());
		detail::print_bool("avx512vbmi", avx512vbmi());
		detail::print_bool("umip", umip());
		detail::print_bool("pku", pku());
		detail::print_bool("ospke", ospke());
		detail::print_bool("avx512vbmi2", avx512vbmi2());
		detail::print_bool("gfni", gfni());
		detail::print_bool("vaes", vaes());
		detail::print_bool("vpclmulqdq", vpclmulqdq());
		detail::print_bool("avx512vnni", avx512vnni());
		detail::print_bool("avx512bitalg", avx512bitalg());
		detail::print_bool("avx512vpopcntdq", avx512vpopcntdq());
		printf("%-18s%hu\n", "mawau", mawau());
		detail::print_bool("rdpid", rdpid());
		detail::print_bool("sgx_lc", sgx_lc());
		detail::print_bool("avx512_4vnniw", avx512_4vnniw());
		detail::print_bool("avx512_4fmaps", avx512_4fmaps());
		detail::print_bool("pconfig", pconfig());
		detail::print_bool("spec_ctrl", spec_ctrl());
		detail::print_bool("fpu_ext", fpu_ext());
		detail::print_bool("vme_ext", vme_ext());
		detail::print_bool("de_ext", de_ext());
		detail::print_bool("pse_ext", pse_ext());
		detail::print_bool("tsc_ext", tsc_ext());
		detail::print_bool("msr_ext", msr_ext());
		detail::print_bool("pae_ext", pae_ext());
		detail::print_bool("mce_ext", mce_ext());
		detail::print_bool("cx8_ext", cx8_ext());
		detail::print_bool("apic_ext", apic_ext());
		detail::print_bool("syscall", syscall());
		detail::print_bool("mtrr_ext", mtrr_ext());
		detail::print_bool("pge_ext", pge_ext());
		detail::print_bool("mca_ext", mca_ext());
		detail::print_bool("cmov_ext", cmov_ext());
		detail::print_bool("pat_ext", pat_ext());
		detail::print_bool("pse36_ext", pse36_ext());
		detail::print_bool("mp", mp());
		detail::print_bool("nx", nx());
		detail::print_bool("mmxext", mmxext());
		detail::print_bool("mmx_ext", mmx_ext());
		detail::print_bool("fxsr_ext", fxsr_ext());
		detail::print_bool("fxsr_opt", fxsr_opt());
		detail::print_bool("pdpe1gb", pdpe1gb());
		detail::print_bool("rdtscp", rdtscp());
		detail::print_bool("lm", lm());
		detail::print_bool("3dnowext", _3dnowext());
		detail::print_bool("3dnow", _3dnow());
		detail::print_bool("lahf_lm", lahf_lm());
		detail::print_bool("cmp_legacy", cmp_legacy());
		detail::print_bool("svm", svm());
		detail::print_bool("extapic", extapic());
		detail::print_bool("cr8_legacy", cr8_legacy());
		detail::print_bool("abm", abm());
		detail::print_bool("sse4a", sse4a());
		detail::print_bool("misalignsse", misalignsse());
		detail::print_bool("3dnowprefetch", _3dnowprefetch());
		detail::print_bool("osvw", osvw());
		detail::print_bool("ibs", ibs());
		detail::print_bool("xop", xop());
		detail::print_bool("skinit", skinit());
		detail::print_bool("wdt", wdt());
		detail::print_bool("lwp", lwp());
		detail::print_bool("fma4", fma4());
		detail::print_bool("tce", tce());
		detail::print_bool("nodeid_msr", nodeid_msr());
		detail::print_bool("tbm", tbm());
		detail::print_bool("topoext", topoext());
		detail::print_bool("perfctr_core", perfctr_core());
		detail::print_bool("perfctr_nb", perfctr_nb());
		detail::print_bool("dbx", dbx());
		detail::print_bool("perftsc", perftsc());
		detail::print_bool("pcx_l2i", pcx_l2i());
	}

private:
	std::string _vendor = "";
	std::string _brand = "";
	bool _is_intel = false;
	bool _is_amd = false;
	cpu_id _feature_bits = {};
	cpu_id _feature_bits_more = {};
	cpu_id _ext_feature_bits = {};
};

inline const cpu_info_t cpu_info;

} // namespace fea
