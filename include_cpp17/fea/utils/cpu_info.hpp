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
#include <cstdint>
#include <string>
#include <vector>

#define FEA_REGION(...)

// References :
// https://en.wikipedia.org/wiki/CPUID
// https://www.scss.tcd.ie/Jeremy.Jones/CS4021/processor-identification-cpuid-instruction-note.pdf

namespace fea {

namespace detail {
void print_bool(const char* str, bool b) {
	printf("%-18s%s\n", str, b ? "true" : "false");
}
} // namespace detail

// Loosely based off :
// https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=vs-2019
// leaf is EAX, subleaf is ECX.
struct cpu_id {
	cpu_id(uint32_t leaf_, uint32_t sub_leaf_)
			: leaf(leaf_)
			, sub_leaf(sub_leaf_) {
#if FEA_WINDOWS
		std::array<int, 4> d = {};
		__cpuidex(d.data(), int(leaf), int(sub_leaf));
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
	cpu_id(uint32_t leaf_)
			: cpu_id(leaf_, 0) {
	}
	cpu_id() = default;

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

struct cpu_info_t {
	cpu_info_t() {
		// Functions
		{
			// calling __cpuid with 0x0 as the function_id argument
			// gets the number of the highest valid function id.
			cpu_id eax0 = cpu_id{ 0 };
			uint32_t highest_leaf = eax0.eax.to_ulong();

			// Vendor ID String (0x0)
			char vendor[13] = { 0 };
			std::array<char, 16> as_string = eax0.to_string();
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

			// Processor Info and Feature Bits (EAX=1)
			if (highest_leaf >= 1) {
				_eax1 = cpu_id{ 1 };
			}

			// TODO : Cache and TLB info.
			if (highest_leaf >= 2) {
				_eax2 = cpu_id{ 2 };
			}

			// Deprecated, used to be serial num.
			if (highest_leaf >= 3) {
				_eax3 = cpu_id{ 3 };
			}

			if (highest_leaf >= 4) {
				_eax4 = cpu_id{ 4 };
			}
			// 0xB
			if (highest_leaf >= 11) {
				_eax0b = cpu_id{ 11 };
			}

			if (highest_leaf >= 6) {
				_eax6 = cpu_id{ 6 };
			}

			// Features Extended (EAX=7)
			if (highest_leaf >= 7) {
				_eax7_ecx0 = cpu_id{ 7 };
				_eax7_ecx1 = cpu_id{ 7, 1 };
			}
		}

		// Extended Functions
		{
			// calling __cpuid with 0x80000000 as the function_id argument
			// gets the number of the highest valid extended id.
			uint32_t highest_leaf = cpu_id{ 0x80000000 }.eax.to_ulong();

			// Extended Processor Info and Feature Bits (0x80000001)
			if (highest_leaf >= 0x80000001) {
				_eax80000001 = cpu_id{ 0x80000001 };
			}

			// Processor Brand String (0x80000002 to 0x80000004)
			char brand[64] = { 0 };
			if (highest_leaf >= 0x80000004) {
				cpu_id eax802 = cpu_id{ 0x80000002 };
				cpu_id eax803 = cpu_id{ 0x80000003 };
				cpu_id eax804 = cpu_id{ 0x80000004 };

				std::array<char, 16> str;
				str = eax802.to_string();
				std::copy(str.begin(), str.end(), &brand[0]);
				str = eax803.to_string();
				std::copy(str.begin(), str.end(), &brand[16]);
				str = eax804.to_string();
				std::copy(str.begin(), str.end(), &brand[32]);

				_brand = brand;
			}

			if (highest_leaf >= 0x80000005) {
				_eax80000005 = cpu_id{ 0x80000005 };
			}
			if (highest_leaf >= 0x80000006) {
				_eax80000006 = cpu_id{ 0x80000006 };
			}
			if (highest_leaf >= 0x80000007) {
				_eax80000007 = cpu_id{ 0x80000007 };
			}
			if (highest_leaf >= 0x80000008) {
				_eax80000008 = cpu_id{ 0x80000008 };
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

	/**
	 * EAX=1 feature bits
	 */
	FEA_REGION(#pragma region EAX1)

	// EAX

	uint8_t stepping_id() const {
		constexpr uint32_t mask = 0x0000'000f;
		uint32_t ret = _eax1.eax.to_ulong();
		return ret & mask;
	}
	uint8_t model() const {
		constexpr uint32_t mask = 0x0000'00f0;
		uint32_t ret = _eax1.eax.to_ulong();
		ret &= mask;
		ret >>= 4;
		return uint8_t(ret);
	}
	uint8_t family_id() const {
		constexpr uint32_t mask = 0x0000'0f00;
		uint32_t ret = _eax1.eax.to_ulong();
		ret &= mask;
		ret >>= 8;
		return uint8_t(ret);
	}
	uint8_t processor_type() const {
		constexpr uint32_t mask = 0b0000'0000'0000'0000'0011'0000'0000'0000;
		uint32_t ret = _eax1.eax.to_ulong();
		ret &= mask;
		ret >>= 12;
		return uint8_t(ret);
	}
	uint8_t extended_model_id() const {
		constexpr uint32_t mask = 0x000f'0000;
		uint32_t ret = _eax1.eax.to_ulong();
		ret &= mask;
		ret >>= 16;
		return uint8_t(ret);
	}
	uint8_t extended_family_id() const {
		constexpr uint32_t mask = 0x0ff0'0000;
		uint32_t ret = _eax1.eax.to_ulong();
		ret &= mask;
		ret >>= 20;
		return uint8_t(ret);
	}


	// EBX

	uint8_t brand_index() const {
		constexpr uint32_t mask = 0x0000'00ff;
		uint32_t ret = _eax1.eax.to_ulong();
		ret &= mask;
		return uint8_t(ret);
	}
	// CLFLUSH line size (Value . 8 = cache line size in bytes).
	uint8_t clflush_line_size() const {
		if (!clfsh()) {
			return 0u;
		}

		constexpr uint32_t mask = 0x0000'ff00;
		uint32_t ret = _eax1.eax.to_ulong();
		ret &= mask;
		ret >>= 8;
		return uint8_t(ret);
	}
	uint8_t num_addressable_logical_ids() const {
		if (!htt()) {
			return 0u;
		}

		constexpr uint32_t mask = 0x00ff'0000;
		uint32_t ret = _eax1.eax.to_ulong();
		ret &= mask;
		ret >>= 16;
		return uint8_t(ret);
	}
	uint8_t local_apic_id() const {
		uint32_t ret = _eax1.eax.to_ulong();
		ret >>= 24;
		return uint8_t(ret);
	}


	// ECX

	// Streaming SIMD Extensions 3
	// The processor supports the Streaming SIMD Extensions 3 instructions.
	bool sse3() const {
		return _eax1.ecx[0];
	}

	// PCLMULDQ Instruction
	// The processor supports PCLMULDQ instruction.
	bool pclmulqdq() const {
		return _eax1.ecx[1];
	}

	// 64-Bit Debug Store
	// Indicates that the processor has the ability to write a history of
	// the 64-bit branch to and from addresses into a memory buffer.
	bool dtes64() const {
		return _eax1.ecx[2];
	}

	// MONITOR/MWAIT
	// The processor supports the MONITOR and MWAIT instructions.
	bool monitor() const {
		return _eax1.ecx[3];
	}

	// CPL Qualified Debug Store
	// The processor supports the extensions to the Debug Store feature to
	// allow for branch message storage qualified by CPL.
	bool ds_cpl() const {
		return _eax1.ecx[4];
	}

	// Virtual Machine Extensions
	// The processor supports Virtualization Technology.
	bool vmx() const {
		return _eax1.ecx[5];
	}

	// Safer Mode Extensions
	// The processor supports Trusted Execution Technology.
	bool smx() const {
		return _eax1.ecx[6];
	}

	// Enhanced SpeedStep Technology
	// The processor supports Enhanced SpeedStep Technology and implements
	// the IA32_PERF_STS and IA32_PERF_CTL registers.
	bool est() const {
		return _eax1.ecx[7];
	}

	// Thermal Monitor 2
	// The processor implements the Thermal Monitor 2 thermal control
	// circuit (TCC).
	bool tm2() const {
		return _eax1.ecx[8];
	}

	// Supplemental Streaming SIMD Extensions 3
	// The processor supports the Supplemental Streaming SIMD Extensions 3
	// instructions.
	bool ssse3() const {
		return _eax1.ecx[9];
	}

	// L1 Context ID
	// The L1 data cache mode can be set to either adaptive mode or shared
	// mode by the BIOS.
	bool cnxt_id() const {
		return _eax1.ecx[10];
	}

	// Silicon Debug interface
	// A value of 1 indicates the processor supports IA32_DEBUG_INTERFACE
	// MSR for silicon debug.
	bool sdbg() const {
		return _eax1.ecx[11];
	}

	// Fused Multiply Add
	// The processor supports FMA extensions using YMM state.
	bool fma() const {
		return _eax1.ecx[12];
	}

	// CMPXCHG16B
	// The processor supports the CMPXCHG16B instruction.
	bool cx16() const {
		return _eax1.ecx[13];
	}

	// xTPR Update Control
	// The processor supports the ability to disable sending Task Priority
	// messages.  When this feature flag is set, Task Priority messages may
	// be disabled.  Bit 23 (Echo TPR disable) in the IA32_MISC_ENABLE MSR
	// controls the sending of Task Priority messages.
	bool xtpr() const {
		return _eax1.ecx[14];
	}

	// Perfmon and Debug Capability
	// The processor supports the Performance Capabilities MSR.
	// IA32_PERF_CAPABILITIES register is MSR 345h.
	bool pdcm() const {
		return _eax1.ecx[15];
	}

	// 16 - reserved

	// Process Context Identifiers
	// The processor supports PCIDs and that software may set CR4.PCIDE to 1
	bool pcid() const {
		return _eax1.ecx[17];
	}

	// Direct Cache Access
	// The processor supports the ability to prefetch data from a memory
	// mapped device.
	bool dca() const {
		return _eax1.ecx[18];
	}

	// Streaming SIMD Extensions 4.1
	// The processor supports the Streaming SIMD Extensions 4.1
	// instructions.
	bool sse41() const {
		return _eax1.ecx[19];
	}

	// Streaming SIMD Extensions 4.2
	// The processor supports the Streaming SIMD Extensions 4.2
	// instructions.
	bool sse42() const {
		return _eax1.ecx[20];
	}

	// Extended xAPIC Support
	// The processor supports x2APIC feature.
	bool x2apic() const {
		return _eax1.ecx[21];
	}

	// MOVBE Instruction
	// The processor supports MOVBE instruction.
	bool movbe() const {
		return _eax1.ecx[22];
	}

	// POPCNT Instruction
	// The processor supports the POPCNT instruction.
	bool popcnt() const {
		return _eax1.ecx[23];
	}

	// Time Stamp Counter Deadline
	// The processor’s local APIC timer supports one-shot operation using a
	// TSC deadline value.
	bool tsc_deadline() const {
		return _eax1.ecx[24];
	}

	// AES Instruction Extensions
	// The processor supports the AES instruction extensions.
	bool aes() const {
		return _eax1.ecx[25];
	}

	// XSAVE/XSTOR States
	// The processor supports the XSAVE/XRSTOR processor extended states
	// feature, the XSETBV/XGETBV instructions, and the
	// XFEATURE_ENABLED_MASK register (XCR0).
	bool xsave() const {
		return _eax1.ecx[26];
	}

	// OS-Enabled Extended State Management
	// A value of 1 indicates that the OS has enabled XSETBV/XGETBV
	// instructions to access the XFEATURE_ENABLED_MASK register (XCR0), and
	// support for processor extended state management using XSAVE/XRSTOR.
	bool osxsave() const {
		return _eax1.ecx[27];
	}

	// Advanced Vector Extensions
	// The processor supports the AVX instruction extensions.
	bool avx() const {
		return _eax1.ecx[28];
	}

	// 16-bit floating-point conversion instructions
	// A value of 1 indicates that the processor supports 16-bit
	// floating-point conversion instructions.
	bool f16c() const {
		return _eax1.ecx[29];
	}

	// RDRAND instruction supported
	// A value of 1 indicates that processor supports RDRAND instruction.
	bool rdrnd() const {
		return _eax1.ecx[30];
	}

	// Hypervisor
	// Hypervisor present (always zero on physical CPUs).
	bool hypervisor() const {
		return _eax1.ecx[31];
	}


	// EDX

	// Floating-point Unit On-Chip
	// The processor contains an FPU that supports the Intel387
	// floating-point instruction set.
	bool fpu() const {
		return _eax1.edx[0];
	}

	// Virtual Mode Extension
	// The processor supports extensions to virtual-8086 mode.
	bool vme() const {
		return _eax1.edx[1];
	}

	// Debugging Extension
	// The processor supports I/O breakpoints, including the CR4.DE bit for
	// enabling debug extensions and optional trapping of access to the DR4
	// and DR5 registers.
	bool de() const {
		return _eax1.edx[2];
	}

	// Page Size Extension
	// The processor supports 4-MB pages.
	bool pse() const {
		return _eax1.edx[3];
	}

	// Time Stamp Counter
	// The RDTSC instruction is supported including the CR4.TSD bit for
	// access/privilege control.
	bool tsc() const {
		return _eax1.edx[4];
	}

	// Model Specific Registers
	// Model Specific Registers are implemented with the RDMSR, WRMSR
	// instructions.
	bool msr() const {
		return _eax1.edx[5];
	}

	// Physical Address Extension
	// Physical addresses greater than 32 bits are supported.
	bool pae() const {
		return _eax1.edx[6];
	}

	// Machine-Check Exception
	// Machine-Check Exception, INT18, and the CR4.MCE enable bit are
	// supported.
	bool mce() const {
		return _eax1.edx[7];
	}

	// CMPXCHG8 Instruction
	// The compare and exchange 8-bytes instruction is supported.
	bool cx8() const {
		return _eax1.edx[8];
	}

	// On-chip APIC Hardware
	// The processor contains a software-accessible local APIC.
	bool apic() const {
		return _eax1.edx[9];
	}

	// 10 - reserved

	// Fast System Call
	// Indicates whether the processor supports the Fast System Call
	// instructions, SYSENTER and SYSEXIT. NOTE: Refer to Section 5.1.2.5
	// for further information regarding SYSENTER/SYSEXIT feature and SEP
	// feature bit.
	bool sep() const {
		return _eax1.edx[11];
	}

	// Memory Type Range Registers
	// The processor supports the Memory Type Range Registers specifically
	// the MTRR_CAP register.
	bool mtrr() const {
		return _eax1.edx[12];
	}

	// Page Global Enable
	// The global bit in the page directory entries (PDEs) and page table
	// entries (PTEs) is supported, indicating TLB entries that are common
	// to different processes and need not be flushed. The CR4.PGE bit
	// controls this feature.
	bool pge() const {
		return _eax1.edx[13];
	}

	// Machine-Check Architecture
	// The Machine-Check Architecture is supported, specifically the MCG_CAP
	// register.
	bool mca() const {
		return _eax1.edx[14];
	}

	// Conditional Move Instruction
	// The processor supports CMOVcc, and if the FPU feature flag (bit 0) is
	// also set, supports the FCMOVCC and FCOMI instructions.
	bool cmov() const {
		return _eax1.edx[15];
	}

	// Page Attribute Table
	// Indicates whether the processor supports the Page Attribute Table.
	// This feature augments the Memory Type Range Registers (MTRRs),
	// allowing an operating system to specify attributes of memory on 4K
	// granularity through a linear address.
	bool pat() const {
		return _eax1.edx[16];
	}

	// 36-bit Page Size Extension
	// Indicates whether the processor supports 4-MB pages that are capable
	// of addressing physical memory beyond 4-GB. This feature indicates
	// that the upper four bits of the physical address of the 4-MB page is
	// encoded by bits 13-16 of the page directory entry.
	bool pse36() const {
		return _eax1.edx[17];
	}

	// Processor serial number is present and enabled
	// The processor supports the 96-bit processor serial number feature,
	// and the feature is enabled.Note: The Pentium 4 and subsequent
	// processor families do not support this feature.
	bool psn() const {
		return _eax1.edx[18];
	}

	// CLFLUSH Instruction
	// Indicates that the processor supports the CLFLUSH instruction.
	bool clfsh() const {
		return _eax1.edx[19];
	}

	// 20 - reserved

	// Debug Store
	// Indicates that the processor supports the ability to write debug
	// information into a memory resident buffer. This feature is used by
	// the branch trace store (BTS) and precise event-based sampling (PEBS)
	// facilities.
	bool ds() const {
		return _eax1.edx[21];
	}

	// Thermal Monitor and Software Controlled Clock Facilities
	// The processor implements internal MSRs that allow processor
	// temperature to be monitored and processor performance to be modulated
	// in predefined duty cycles under software control.
	bool acpi() const {
		return _eax1.edx[22];
	}

	// MMX technology
	// The processor supports the MMX technology instruction set extensions
	// to Intel Architecture.
	bool mmx() const {
		return _eax1.edx[23];
	}

	// FXSAVE and FXSTOR Instructions
	// The FXSAVE and FXRSTOR instructions are supported for fast save and
	// restore of the floating point context. Presence of this bit also
	// indicates that CR4.OSFXSR is available for an operating system to
	// indicate that it supports the FXSAVE and FXRSTOR instructions.
	bool fxsr() const {
		return _eax1.edx[24];
	}

	// Streaming SIMD Extensions
	// The processor supports the Streaming SIMD Extensions to the Intel
	// Architecture.
	bool sse() const {
		return _eax1.edx[25];
	}

	// Streaming SIMD Extensions 2
	// Indicates the processor supports the Streaming SIMD Extensions 2
	// Instructions.
	bool sse2() const {
		return _eax1.edx[26];
	}

	// Self-Snoop
	// The processor supports the management of conflicting memory types by
	// performing a snoop of its own cache structure for transactions issued
	// to the bus.
	bool ss() const {
		return _eax1.edx[27];
	}

	// Multi-Threading
	// The physical processor package is capable of supporting more than one
	// logical processor. This field does not indicate that Hyper-Threading
	// Technology or Core Multi-Processing (CMP) has been enabled for this
	// specific processor. To determine if Hyper-Threading Technology or CMP
	// is supported, compare value returned in EBX[23:16] after executing
	// CPUID with EAX=1.  If the resulting value is > 1, then the processor
	// supports Multi-Threading.
	bool htt() const {
		return _eax1.edx[28];
	}

	// Thermal Monitor
	// The processor implements the Thermal Monitor automatic thermal
	// control circuitry (TCC).
	bool tm() const {
		return _eax1.edx[29];
	}

	// IA64 processor emulating x86
	bool ia64() const {
		return _eax1.edx[30];
	}

	// Pending Break Enable
	// The processor supports the use of the FERR#/PBE# pin when the
	// processor is in the stop-clock state (STPCLK# is asserted) to signal
	// the processor that an interrupt is pending and that the processor
	// should return to normal operation to handle the interrupt. Bit 10
	// (PBE enable) in the IA32_MISC_ENABLE MSR enables this capability.
	bool pbe() const {
		return _eax1.edx[31];
	}
	FEA_REGION(#pragma endregion)


	/**
	 * EAX=7 feature bits
	 */
	FEA_REGION(#pragma region EAX7)

	// EBX

	bool fsgsbase() const {
		return _eax7_ecx0.ebx[0];
	}
	bool ia32_tsc_adjust() const {
		return _eax7_ecx0.ebx[1];
	}
	bool sgx() const {
		return _eax7_ecx0.ebx[2];
	}
	bool bmi1() const {
		return _eax7_ecx0.ebx[3];
	}
	bool hle() const {
		return _eax7_ecx0.ebx[4];
	}
	bool avx2() const {
		return _eax7_ecx0.ebx[5];
	}
	bool fdp_excptn_only() const {
		return _eax7_ecx0.ebx[6];
	}
	bool smep() const {
		return _eax7_ecx0.ebx[7];
	}
	bool bmi2() const {
		return _eax7_ecx0.ebx[8];
	}
	bool erms() const {
		return _eax7_ecx0.ebx[9];
	}
	bool invpcid() const {
		return _eax7_ecx0.ebx[10];
	}
	bool rtm() const {
		return _eax7_ecx0.ebx[11];
	}
	bool pqm() const {
		return _eax7_ecx0.ebx[12];
	}
	bool fpu_cs_ds() const {
		return _eax7_ecx0.ebx[13];
	}
	bool mpx() const {
		return _eax7_ecx0.ebx[14];
	}
	bool pqe() const {
		return _eax7_ecx0.ebx[15];
	}
	bool avx512_f() const {
		return _eax7_ecx0.ebx[16];
	}
	bool avx512_dq() const {
		return _eax7_ecx0.ebx[17];
	}
	bool rdseed() const {
		return _eax7_ecx0.ebx[18];
	}
	bool adx() const {
		return _eax7_ecx0.ebx[19];
	}
	bool smap() const {
		return _eax7_ecx0.ebx[20];
	}
	bool avx512_ifma() const {
		return _eax7_ecx0.ebx[21];
	}
	bool pcommit() const {
		return _eax7_ecx0.ebx[22];
	}
	bool clflushopt() const {
		return _eax7_ecx0.ebx[23];
	}
	bool clwb() const {
		return _eax7_ecx0.ebx[24];
	}
	bool intel_pt() const {
		return _eax7_ecx0.ebx[25];
	}
	bool avx512_pf() const {
		return _eax7_ecx0.ebx[26];
	}
	bool avx512_er() const {
		return _eax7_ecx0.ebx[27];
	}
	bool avx512_cd() const {
		return _eax7_ecx0.ebx[28];
	}
	bool sha() const {
		return _eax7_ecx0.ebx[29];
	}
	bool avx512_bw() const {
		return _eax7_ecx0.ebx[30];
	}
	bool avx512_vl() const {
		return _eax7_ecx0.ebx[31];
	}

	// ECX
	bool prefetchwt1() const {
		return _eax7_ecx0.ecx[0];
	}
	bool avx512_vbmi() const {
		return _eax7_ecx0.ecx[1];
	}
	bool umip() const {
		return _eax7_ecx0.ecx[2];
	}
	bool pku() const {
		return _eax7_ecx0.ecx[3];
	}
	bool ospke() const {
		return _eax7_ecx0.ecx[4];
	}
	bool waitpkg() const {
		return _eax7_ecx0.ecx[5];
	}
	bool avx512_vbmi2() const {
		return _eax7_ecx0.ecx[6];
	}
	bool cet_ss() const {
		return _eax7_ecx0.ecx[7];
	}
	bool gfni() const {
		return _eax7_ecx0.ecx[8];
	}
	bool vaes() const {
		return _eax7_ecx0.ecx[9];
	}
	bool vpclmulqdq() const {
		return _eax7_ecx0.ecx[10];
	}
	bool avx512_vnni() const {
		return _eax7_ecx0.ecx[11];
	}
	bool avx512_bitalg() const {
		return _eax7_ecx0.ecx[12];
	}
	// 13
	bool avx512_vpopcntdq() const {
		return _eax7_ecx0.ecx[14];
	}
	// 15
	bool five_level_paging() const {
		return _eax7_ecx0.ecx[16];
	}
	uint8_t mawau() const {
		constexpr uint32_t mask = 0b0000'0000'0011'1110'0000'0000;
		uint32_t ret = _eax7_ecx0.ecx.to_ulong();
		ret &= mask;
		ret >>= 9;
		return uint8_t(ret);
	}
	bool rdpid() const {
		return _eax7_ecx0.ecx[22];
	}
	// 23 - 24
	bool cldemote() const {
		return _eax7_ecx0.ecx[25];
	}
	// 26
	bool movdiri() const {
		return _eax7_ecx0.ecx[27];
	}
	bool movdir64b() const {
		return _eax7_ecx0.ecx[28];
	}
	bool enqcmd() const {
		return _eax7_ecx0.ecx[29];
	}
	bool sgx_lc() const {
		return _eax7_ecx0.ecx[30];
	}
	bool pks() const {
		return _eax7_ecx0.ecx[31];
	}

	// EDX
	// 0 - 1
	bool avx512_4vnniw() const {
		return _eax7_ecx0.edx[2];
	}
	bool avx512_4fmaps() const {
		return _eax7_ecx0.edx[3];
	}
	bool fsrm() const {
		return _eax7_ecx0.edx[4];
	}
	// 5 - 7
	bool avx512_vp2intersect() const {
		return _eax7_ecx0.edx[8];
	}
	bool srbds_ctrl() const {
		return _eax7_ecx0.edx[9];
	}
	bool md_clear() const {
		return _eax7_ecx0.edx[10];
	}
	// 11 - 12
	bool tsx_force_abort() const {
		return _eax7_ecx0.edx[13];
	}
	bool serialize() const {
		return _eax7_ecx0.edx[14];
	}
	bool hybrid() const {
		return _eax7_ecx0.edx[15];
	}
	bool tsxldtrk() const {
		return _eax7_ecx0.edx[16];
	}
	// 17
	bool pconfig() const {
		return _eax7_ecx0.edx[18];
	}
	bool lbr() const {
		return _eax7_ecx0.edx[19];
	}
	bool cet_ibt() const {
		return _eax7_ecx0.edx[20];
	}
	// 21
	bool amx_bf16() const {
		return _eax7_ecx0.edx[22];
	}
	// 23
	bool amx_tile() const {
		return _eax7_ecx0.edx[24];
	}
	bool amx_int8() const {
		return _eax7_ecx0.edx[25];
	}
	bool spec_ctrl() const {
		return _eax7_ecx0.edx[26];
	}
	bool stibp() const {
		return _eax7_ecx0.edx[27];
	}
	bool l1d_flush() const {
		return _eax7_ecx0.edx[28];
	}
	bool ia32_arch_capabilities() const {
		return _eax7_ecx0.edx[29];
	}
	bool ia32_core_capabilities() const {
		return _eax7_ecx0.edx[30];
	}
	bool ssbd() const {
		return _eax7_ecx0.edx[31];
	}


	// EAX=7, ECX=1
	bool avx512_bf16() const {
		return _eax7_ecx1.eax[5];
	}
	FEA_REGION(#pragma endregion)


	/**
	 * EAX=80000001h feature bits
	 */
	FEA_REGION(#pragma region EAX80000001)

	// ECX

	bool lahf_lm() const {
		return _eax80000001.ecx[0];
	}
	bool cmp_legacy() const {
		return _eax80000001.ecx[1];
	}
	bool svm() const {
		return _eax80000001.ecx[2];
	}
	bool extapic() const {
		return _eax80000001.ecx[3];
	}
	bool cr8_legacy() const {
		return _eax80000001.ecx[4];
	}
	bool abm() const {
		return _eax80000001.ecx[5];
	}
	bool sse4a() const {
		return _eax80000001.ecx[6];
	}
	bool misalignsse() const {
		return _eax80000001.ecx[7];
	}
	bool _3dnowprefetch() const {
		return _eax80000001.ecx[8];
	}
	bool osvw() const {
		return _eax80000001.ecx[9];
	}
	bool ibs() const {
		return _eax80000001.ecx[10];
	}
	bool xop() const {
		return _eax80000001.ecx[11];
	}
	bool skinit() const {
		return _eax80000001.ecx[12];
	}
	bool wdt() const {
		return _eax80000001.ecx[13];
	}
	// 14
	bool lwp() const {
		return _eax80000001.ecx[15];
	}
	bool fma4() const {
		return _eax80000001.ecx[16];
	}
	bool tce() const {
		return _eax80000001.ecx[17];
	}
	// 18
	bool nodeid_msr() const {
		return _eax80000001.ecx[19];
	}
	// 20
	bool tbm() const {
		return _eax80000001.ecx[21];
	}
	bool topoext() const {
		return _eax80000001.ecx[22];
	}
	bool perfctr_core() const {
		return _eax80000001.ecx[23];
	}
	bool perfctr_nb() const {
		return _eax80000001.ecx[24];
	}
	// 25
	bool dbx() const {
		return _eax80000001.ecx[26];
	}
	bool perftsc() const {
		return _eax80000001.ecx[27];
	}
	bool pcx_l2i() const {
		return _eax80000001.ecx[28];
	}
	// 29 - 31


	// EDX

	bool fpu_ext() const {
		return _eax80000001.edx[0];
	}
	bool vme_ext() const {
		return _eax80000001.edx[1];
	}
	bool de_ext() const {
		return _eax80000001.edx[2];
	}
	bool pse_ext() const {
		return _eax80000001.edx[3];
	}
	bool tsc_ext() const {
		return _eax80000001.edx[4];
	}
	bool msr_ext() const {
		return _eax80000001.edx[5];
	}
	bool pae_ext() const {
		return _eax80000001.edx[6];
	}
	bool mce_ext() const {
		return _eax80000001.edx[7];
	}
	bool cx8_ext() const {
		return _eax80000001.edx[8];
	}
	bool apic_ext() const {
		return _eax80000001.edx[9];
	}
	// 10
	bool syscall() const {
		return _eax80000001.edx[11];
	}
	bool mtrr_ext() const {
		return _eax80000001.edx[12];
	}
	bool pge_ext() const {
		return _eax80000001.edx[13];
	}
	bool mca_ext() const {
		return _eax80000001.edx[14];
	}
	bool cmov_ext() const {
		return _eax80000001.edx[15];
	}
	bool pat_ext() const {
		return _eax80000001.edx[16];
	}
	bool pse36_ext() const {
		return _eax80000001.edx[17];
	}
	// 18
	bool mp() const {
		return _eax80000001.edx[19];
	}
	bool nx() const {
		return _eax80000001.edx[20];
	}
	// 21
	bool mmxext() const {
		return _eax80000001.edx[22];
	}
	bool mmx_ext() const {
		return _eax80000001.edx[23];
	}
	bool fxsr_ext() const {
		return _eax80000001.edx[24];
	}
	bool fxsr_opt() const {
		return _eax80000001.edx[25];
	}
	bool pdpe1gb() const {
		return _eax80000001.edx[26];
	}
	bool rdtscp() const {
		return _eax80000001.edx[27];
	}
	// 28
	bool lm() const {
		return _eax80000001.edx[29];
	}
	bool _3dnowext() const {
		return _eax80000001.edx[30];
	}
	bool _3dnow() const {
		return _eax80000001.edx[31];
	}
	FEA_REGION(#pragma endregion)


	/**
	 * Raw registers & unimplemented
	 * If a function you need isn't available, you can construct a
	 * fea::cpu_id object with your desired function leaf and subleaf to get
	 * the custom feature bits.
	 */
	FEA_REGION(#pragma region RAW)

	// INPUT EAX = 01H: Returns Model, Family, Stepping Information
	// INPUT EAX = 01H: Returns Additional Information in EBX
	// INPUT EAX = 01H: Returns Feature Information in ECX and EDX
	const cpu_id& eax1() const {
		return _eax1;
	}

	// INPUT EAX = 02H: TLB/Cache/Prefetch Information Returned in EAX, EBX,
	// ECX, EDX
	const cpu_id& eax2() const {
		return _eax2;
	}

	// INPUT EAX = 03H: Processor Serial Number (Only available on Pentium
	// 3)
	const cpu_id& eax3() const {
		return _eax3;
	}

	// INPUT EAX = 04H: Returns Deterministic Cache Parameters for Each
	// Level
	const cpu_id& eax4() const {
		return _eax4;
	}

	// INPUT EAX = 06H: Returns Thermal and Power Management Features
	const cpu_id& eax6() const {
		return _eax6;
	}

	// INPUT EAX = 07H: Returns Structured Extended Feature Enumeration
	// Information
	const cpu_id& eax7_ecx0() const {
		return _eax7_ecx0;
	}
	const cpu_id& eax7_ecx1() const {
		return _eax7_ecx1;
	}

	// INPUT EAX = 0BH: Returns Extended Topology Information
	const cpu_id& eax0b() const {
		return _eax0b;
	}

	// INPUT EAX=80000001H: Extended Processor Info and Feature Bits
	const cpu_id& eax80000001() const {
		return _eax80000001;
	}

	// 80000002H, 80000003H, 80000004H are decoded in brand string.

	// INPUT EAX=80000005H: L1 Cache and TLB Identifiers
	const cpu_id& eax80000005() const {
		return _eax80000005;
	}

	// INPUT EAX=80000006H: Extended L2 Cache Features
	const cpu_id& eax80000006() const {
		return _eax80000006;
	}

	// INPUT EAX=80000007H: Advanced Power Management Information
	const cpu_id& eax80000007() const {
		return _eax80000007;
	}

	// INPUT EAX=80000008H: Virtual and Physical address Sizes
	const cpu_id& eax80000008() const {
		return _eax80000008;
	}
	FEA_REGION(#pragma endregion)

	// Print all supported feature bits to console.
	void print_all() const {
		printf("%-18s%s\n", "vendor", vendor());
		printf("%-18s%s\n", "brand", brand());
		printf("\n");

		printf("%-18s%x\n", "family_id", family_id());
		printf("%-18s%x\n", "ext_family_id", extended_family_id());
		printf("%-18s%x\n", "model", model());
		printf("%-18s%x\n", "ext_model_id", extended_model_id());
		printf("%-18s%x\n", "processor_type", processor_type());
		printf("%-18s%x\n", "stepping_id", stepping_id());
		printf("\n");

		printf("%-18s%hhu\n", "brand_index", brand_index());
		printf("%-18s%hhu\n", "clflush_line_size", clflush_line_size());
		printf("%-18s%hhu\n", "num_addressable_ids",
				num_addressable_logical_ids());
		printf("%-18s%hhu\n", "local_apic_id", local_apic_id());
		printf("\n");

		printf("eax1 - edx\n");
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
		printf("\n");

		printf("eax1 - ecx\n");
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
		printf("\n");

		printf("eax7_ecx0 - ebx\n");
		detail::print_bool("fsgsbase", fsgsbase());
		detail::print_bool("ia32_tsc_adjust", ia32_tsc_adjust());
		detail::print_bool("sgx", sgx());
		detail::print_bool("bmi1", bmi1());
		detail::print_bool("hle", hle());
		detail::print_bool("avx2", avx2());
		detail::print_bool("fdp_excptn_only", fdp_excptn_only());
		detail::print_bool("smep", smep());
		detail::print_bool("bmi2", bmi2());
		detail::print_bool("erms", erms());
		detail::print_bool("invpcid", invpcid());
		detail::print_bool("rtm", rtm());
		detail::print_bool("pqm", pqm());
		detail::print_bool("fpu_cs_ds", fpu_cs_ds());
		detail::print_bool("mpx", mpx());
		detail::print_bool("pqe", pqe());
		detail::print_bool("avx512f", avx512_f());
		detail::print_bool("avx512dq", avx512_dq());
		detail::print_bool("rdseed", rdseed());
		detail::print_bool("adx", adx());
		detail::print_bool("smap", smap());
		detail::print_bool("avx512ifma", avx512_ifma());
		detail::print_bool("pcommit", pcommit());
		detail::print_bool("clflushopt", clflushopt());
		detail::print_bool("clwb", clwb());
		detail::print_bool("intel_pt", intel_pt());
		detail::print_bool("avx512pf", avx512_pf());
		detail::print_bool("avx512er", avx512_er());
		detail::print_bool("avx512cd", avx512_cd());
		detail::print_bool("sha", sha());
		detail::print_bool("avx512bw", avx512_bw());
		detail::print_bool("avx512vl", avx512_vl());
		printf("\n");


		printf("eax7_ecx0 - ecx\n");
		detail::print_bool("prefetchwt1", prefetchwt1());
		detail::print_bool("avx512_vbmi", avx512_vbmi());
		detail::print_bool("umip", umip());
		detail::print_bool("pku", pku());
		detail::print_bool("ospke", ospke());
		detail::print_bool("waitpkg", waitpkg());
		detail::print_bool("avx512_vbmi2", avx512_vbmi2());
		detail::print_bool("cet_ss", cet_ss());
		detail::print_bool("gfni", gfni());
		detail::print_bool("vaes", vaes());
		detail::print_bool("vpclmulqdq", vpclmulqdq());
		detail::print_bool("avx512_vnni", avx512_vnni());
		detail::print_bool("avx512_bitalg", avx512_bitalg());
		detail::print_bool("avx512_vpopcntdq", avx512_vpopcntdq());
		detail::print_bool("five_level_paging", five_level_paging());
		printf("%-18s%hhu\n", "mawau", mawau());
		detail::print_bool("rdpid", rdpid());
		detail::print_bool("cldemote", cldemote());
		detail::print_bool("movdiri", movdiri());
		detail::print_bool("movdir64b", movdir64b());
		detail::print_bool("enqcmd", enqcmd());
		detail::print_bool("sgx_lc", sgx_lc());
		detail::print_bool("pks", pks());
		printf("\n");

		printf("eax7_ecx0 - edx\n");
		detail::print_bool("avx512_4vnniw", avx512_4vnniw());
		detail::print_bool("avx512_4fmaps", avx512_4fmaps());
		detail::print_bool("fsrm", fsrm());
		detail::print_bool("avx512_vp2intersect", avx512_vp2intersect());
		detail::print_bool("srbds_ctrl", srbds_ctrl());
		detail::print_bool("md_clear", md_clear());
		detail::print_bool("tsx_force_abort", tsx_force_abort());
		detail::print_bool("serialize", serialize());
		detail::print_bool("hybrid", hybrid());
		detail::print_bool("tsxldtrk", tsxldtrk());
		detail::print_bool("pconfig", pconfig());
		detail::print_bool("lbr", lbr());
		detail::print_bool("cet_ibt", cet_ibt());
		detail::print_bool("amx_bf16", amx_bf16());
		detail::print_bool("amx_tile", amx_tile());
		detail::print_bool("amx_int8", amx_int8());
		detail::print_bool("spec_ctrl", spec_ctrl());
		detail::print_bool("stibp", stibp());
		detail::print_bool("l1d_flush", l1d_flush());
		detail::print_bool("ia32_arch_capabilities", ia32_arch_capabilities());
		detail::print_bool("ia32_core_capabilities", ia32_core_capabilities());
		detail::print_bool("ssbd", ssbd());
		printf("\n");

		printf("eax7_ecx1 - eax\n");
		detail::print_bool("avx512_bf16", avx512_bf16());
		printf("\n");

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
		printf("\n");
	}

private:
	std::string _vendor = "";
	std::string _brand = "";
	bool _is_intel = false;
	bool _is_amd = false;

	cpu_id _eax1;
	cpu_id _eax2;
	cpu_id _eax3;
	cpu_id _eax4;
	cpu_id _eax0b;
	cpu_id _eax6;
	cpu_id _eax7_ecx0;
	cpu_id _eax7_ecx1;

	cpu_id _eax80000001;
	cpu_id _eax80000005;
	cpu_id _eax80000006;
	cpu_id _eax80000007;
	cpu_id _eax80000008;
};

inline const cpu_info_t cpu_info;

} // namespace fea
