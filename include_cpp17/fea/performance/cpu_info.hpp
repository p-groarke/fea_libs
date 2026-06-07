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
#include "fea/utility/platform.hpp"
#if !FEA_ARM

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
#include <cstdio>
#include <string>

// References :
// https://en.wikipedia.org/wiki/CPUID
// https://www.scss.tcd.ie/Jeremy.Jones/CS4021/processor-identification-cpuid-instruction-note.pdf

namespace fea {
// Loosely based off :
// https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=vs-2019
// leaf is EAX, subleaf is ECX.
struct cpu_id {
	cpu_id() = default;
	inline cpu_id(uint32_t leaf_, uint32_t sub_leaf_);
	inline cpu_id(uint32_t leaf_);

	[[nodiscard]]
	inline std::array<char, 16> to_string() const;

	uint32_t leaf = 0;
	uint32_t sub_leaf = 0;
	std::bitset<32> eax = { 0 };
	std::bitset<32> ebx = { 0 };
	std::bitset<32> ecx = { 0 };
	std::bitset<32> edx = { 0 };
};

struct cpu_info_t {
	inline cpu_info_t();

	[[nodiscard]]
	inline const char* vendor() const;
	[[nodiscard]]
	inline const char* brand() const;

	[[nodiscard]]
	inline bool intel() const;
	[[nodiscard]]
	inline bool amd() const;

	/**
	 * EAX=1 feature bits
	 */
	FEA_REGION(#pragma region EAX1)

	// EAX
	[[nodiscard]]
	inline uint8_t stepping_id() const;

	[[nodiscard]]
	inline uint8_t model() const;

	[[nodiscard]]
	inline uint8_t family_id() const;

	[[nodiscard]]
	inline uint8_t processor_type() const;

	[[nodiscard]]
	inline uint8_t extended_model_id() const;

	[[nodiscard]]
	inline uint8_t extended_family_id() const;

	// EBX
	[[nodiscard]]
	inline uint8_t brand_index() const;

	// CLFLUSH line size (Value . 8 = cache line size in bytes).
	[[nodiscard]]
	inline uint8_t clflush_line_size() const;

	[[nodiscard]]
	inline uint8_t num_addressable_logical_ids() const;

	[[nodiscard]]
	inline uint8_t local_apic_id() const;

	// ECX

	// Streaming SIMD Extensions 3
	// The processor supports the Streaming SIMD Extensions 3 instructions.
	[[nodiscard]]
	inline bool sse3() const;

	// PCLMULDQ Instruction
	// The processor supports PCLMULDQ instruction.
	[[nodiscard]]
	inline bool pclmulqdq() const;

	// 64-Bit Debug Store
	// Indicates that the processor has the ability to write a history of
	// the 64-bit branch to and from addresses into a memory buffer.
	[[nodiscard]]
	inline bool dtes64() const;

	// MONITOR/MWAIT
	// The processor supports the MONITOR and MWAIT instructions.
	[[nodiscard]]
	inline bool monitor() const;

	// CPL Qualified Debug Store
	// The processor supports the extensions to the Debug Store feature to
	// allow for branch message storage qualified by CPL.
	[[nodiscard]]
	inline bool ds_cpl() const;

	// Virtual Machine Extensions
	// The processor supports Virtualization Technology.
	[[nodiscard]]
	inline bool vmx() const;

	// Safer Mode Extensions
	// The processor supports Trusted Execution Technology.
	[[nodiscard]]
	inline bool smx() const;

	// Enhanced SpeedStep Technology
	// The processor supports Enhanced SpeedStep Technology and implements
	// the IA32_PERF_STS and IA32_PERF_CTL registers.
	[[nodiscard]]
	inline bool est() const;

	// Thermal Monitor 2
	// The processor implements the Thermal Monitor 2 thermal control
	// circuit (TCC).
	[[nodiscard]]
	inline bool tm2() const;

	// Supplemental Streaming SIMD Extensions 3
	// The processor supports the Supplemental Streaming SIMD Extensions 3
	// instructions.
	[[nodiscard]]
	inline bool ssse3() const;

	// L1 Context ID
	// The L1 data cache mode can be set to either adaptive mode or shared
	// mode by the BIOS.
	[[nodiscard]]
	inline bool cnxt_id() const;

	// Silicon Debug interface
	// A value of 1 indicates the processor supports IA32_DEBUG_INTERFACE
	// MSR for silicon debug.
	[[nodiscard]]
	inline bool sdbg() const;

	// Fused Multiply Add
	// The processor supports FMA extensions using YMM state.
	[[nodiscard]]
	inline bool fma() const;

	// CMPXCHG16B
	// The processor supports the CMPXCHG16B instruction.
	[[nodiscard]]
	inline bool cx16() const;

	// xTPR Update Control
	// The processor supports the ability to disable sending Task Priority
	// messages.  When this feature flag is set, Task Priority messages may
	// be disabled.  Bit 23 (Echo TPR disable) in the IA32_MISC_ENABLE MSR
	// controls the sending of Task Priority messages.
	[[nodiscard]]
	inline bool xtpr() const;

	// Perfmon and Debug Capability
	// The processor supports the Performance Capabilities MSR.
	// IA32_PERF_CAPABILITIES register is MSR 345h.
	[[nodiscard]]
	inline bool pdcm() const;

	// 16 - reserved

	// Process Context Identifiers
	// The processor supports PCIDs and that software may set CR4.PCIDE to 1
	[[nodiscard]]
	inline bool pcid() const;

	// Direct Cache Access
	// The processor supports the ability to prefetch data from a memory
	// mapped device.
	[[nodiscard]]
	inline bool dca() const;

	// Streaming SIMD Extensions 4.1
	// The processor supports the Streaming SIMD Extensions 4.1
	// instructions.
	[[nodiscard]]
	inline bool sse41() const;

	// Streaming SIMD Extensions 4.2
	// The processor supports the Streaming SIMD Extensions 4.2
	// instructions.
	[[nodiscard]]
	inline bool sse42() const;

	// Extended xAPIC Support
	// The processor supports x2APIC feature.
	[[nodiscard]]
	inline bool x2apic() const;

	// MOVBE Instruction
	// The processor supports MOVBE instruction.
	[[nodiscard]]
	inline bool movbe() const;

	// POPCNT Instruction
	// The processor supports the POPCNT instruction.
	[[nodiscard]]
	inline bool popcnt() const;

	// Time Stamp Counter Deadline
	// The processor’s local APIC timer supports one-shot operation using a
	// TSC deadline value.
	[[nodiscard]]
	inline bool tsc_deadline() const;

	// AES Instruction Extensions
	// The processor supports the AES instruction extensions.
	[[nodiscard]]
	inline bool aes() const;

	// XSAVE/XSTOR States
	// The processor supports the XSAVE/XRSTOR processor extended states
	// feature, the XSETBV/XGETBV instructions, and the
	// XFEATURE_ENABLED_MASK register (XCR0).
	[[nodiscard]]
	inline bool xsave() const;

	// OS-Enabled Extended State Management
	// A value of 1 indicates that the OS has enabled XSETBV/XGETBV
	// instructions to access the XFEATURE_ENABLED_MASK register (XCR0), and
	// support for processor extended state management using XSAVE/XRSTOR.
	[[nodiscard]]
	inline bool osxsave() const;

	// Advanced Vector Extensions
	// The processor supports the AVX instruction extensions.
	[[nodiscard]]
	inline bool avx() const;

	// 16-bit floating-point conversion instructions
	// A value of 1 indicates that the processor supports 16-bit
	// floating-point conversion instructions.
	[[nodiscard]]
	inline bool f16c() const;

	// RDRAND instruction supported
	// A value of 1 indicates that processor supports RDRAND instruction.
	[[nodiscard]]
	inline bool rdrnd() const;

	// Hypervisor
	// Hypervisor present (always zero on physical CPUs).
	[[nodiscard]]
	inline bool hypervisor() const;


	// EDX

	// Floating-point Unit On-Chip
	// The processor contains an FPU that supports the Intel387
	// floating-point instruction set.
	[[nodiscard]]
	inline bool fpu() const;

	// Virtual Mode Extension
	// The processor supports extensions to virtual-8086 mode.
	[[nodiscard]]
	inline bool vme() const;

	// Debugging Extension
	// The processor supports I/O breakpoints, including the CR4.DE bit for
	// enabling debug extensions and optional trapping of access to the DR4
	// and DR5 registers.
	[[nodiscard]]
	inline bool de() const;

	// Page Size Extension
	// The processor supports 4-MB pages.
	[[nodiscard]]
	inline bool pse() const;

	// Time Stamp Counter
	// The RDTSC instruction is supported including the CR4.TSD bit for
	// access/privilege control.
	[[nodiscard]]
	inline bool tsc() const;

	// Model Specific Registers
	// Model Specific Registers are implemented with the RDMSR, WRMSR
	// instructions.
	[[nodiscard]]
	inline bool msr() const;

	// Physical Address Extension
	// Physical addresses greater than 32 bits are supported.
	[[nodiscard]]
	inline bool pae() const;

	// Machine-Check Exception
	// Machine-Check Exception, INT18, and the CR4.MCE enable bit are
	// supported.
	[[nodiscard]]
	inline bool mce() const;

	// CMPXCHG8 Instruction
	// The compare and exchange 8-bytes instruction is supported.
	[[nodiscard]]
	inline bool cx8() const;

	// On-chip APIC Hardware
	// The processor contains a software-accessible local APIC.
	[[nodiscard]]
	inline bool apic() const;

	// 10 - reserved

	// Fast System Call
	// Indicates whether the processor supports the Fast System Call
	// instructions, SYSENTER and SYSEXIT. NOTE: Refer to Section 5.1.2.5
	// for further information regarding SYSENTER/SYSEXIT feature and SEP
	// feature bit.
	[[nodiscard]]
	inline bool sep() const;

	// Memory Type Range Registers
	// The processor supports the Memory Type Range Registers specifically
	// the MTRR_CAP register.
	[[nodiscard]]
	inline bool mtrr() const;

	// Page Global Enable
	// The global bit in the page directory entries (PDEs) and page table
	// entries (PTEs) is supported, indicating TLB entries that are common
	// to different processes and need not be flushed. The CR4.PGE bit
	// controls this feature.
	[[nodiscard]]
	inline bool pge() const;

	// Machine-Check Architecture
	// The Machine-Check Architecture is supported, specifically the MCG_CAP
	// register.
	[[nodiscard]]
	inline bool mca() const;

	// Conditional Move Instruction
	// The processor supports CMOVcc, and if the FPU feature flag (bit 0) is
	// also set, supports the FCMOVCC and FCOMI instructions.
	[[nodiscard]]
	inline bool cmov() const;

	// Page Attribute Table
	// Indicates whether the processor supports the Page Attribute Table.
	// This feature augments the Memory Type Range Registers (MTRRs),
	// allowing an operating system to specify attributes of memory on 4K
	// granularity through a linear address.
	[[nodiscard]]
	inline bool pat() const;

	// 36-bit Page Size Extension
	// Indicates whether the processor supports 4-MB pages that are capable
	// of addressing physical memory beyond 4-GB. This feature indicates
	// that the upper four bits of the physical address of the 4-MB page is
	// encoded by bits 13-16 of the page directory entry.
	[[nodiscard]]
	inline bool pse36() const;

	// Processor serial number is present and enabled
	// The processor supports the 96-bit processor serial number feature,
	// and the feature is enabled.Note: The Pentium 4 and subsequent
	// processor families do not support this feature.
	[[nodiscard]]
	inline bool psn() const;

	// CLFLUSH Instruction
	// Indicates that the processor supports the CLFLUSH instruction.
	[[nodiscard]]
	inline bool clfsh() const;

	// 20 - reserved

	// Debug Store
	// Indicates that the processor supports the ability to write debug
	// information into a memory resident buffer. This feature is used by
	// the branch trace store (BTS) and precise event-based sampling (PEBS)
	// facilities.
	[[nodiscard]]
	inline bool ds() const;

	// Thermal Monitor and Software Controlled Clock Facilities
	// The processor implements internal MSRs that allow processor
	// temperature to be monitored and processor performance to be modulated
	// in predefined duty cycles under software control.
	[[nodiscard]]
	inline bool acpi() const;

	// MMX technology
	// The processor supports the MMX technology instruction set extensions
	// to Intel Architecture.
	[[nodiscard]]
	inline bool mmx() const;

	// FXSAVE and FXSTOR Instructions
	// The FXSAVE and FXRSTOR instructions are supported for fast save and
	// restore of the floating point context. Presence of this bit also
	// indicates that CR4.OSFXSR is available for an operating system to
	// indicate that it supports the FXSAVE and FXRSTOR instructions.
	[[nodiscard]]
	inline bool fxsr() const;

	// Streaming SIMD Extensions
	// The processor supports the Streaming SIMD Extensions to the Intel
	// Architecture.
	[[nodiscard]]
	inline bool sse() const;

	// Streaming SIMD Extensions 2
	// Indicates the processor supports the Streaming SIMD Extensions 2
	// Instructions.
	[[nodiscard]]
	inline bool sse2() const;

	// Self-Snoop
	// The processor supports the management of conflicting memory types by
	// performing a snoop of its own cache structure for transactions issued
	// to the bus.
	[[nodiscard]]
	inline bool ss() const;

	// Multi-Threading
	// The physical processor package is capable of supporting more than one
	// logical processor. This field does not indicate that Hyper-Threading
	// Technology or Core Multi-Processing (CMP) has been enabled for this
	// specific processor. To determine if Hyper-Threading Technology or CMP
	// is supported, compare value returned in EBX[23:16] after executing
	// CPUID with EAX=1.  If the resulting value is > 1, then the processor
	// supports Multi-Threading.
	[[nodiscard]]
	inline bool htt() const;

	// Thermal Monitor
	// The processor implements the Thermal Monitor automatic thermal
	// control circuitry (TCC).
	[[nodiscard]]
	inline bool tm() const;

	// IA64 processor emulating x86
	[[nodiscard]]
	inline bool ia64() const;

	// Pending Break Enable
	// The processor supports the use of the FERR#/PBE# pin when the
	// processor is in the stop-clock state (STPCLK# is asserted) to signal
	// the processor that an interrupt is pending and that the processor
	// should return to normal operation to handle the interrupt. Bit 10
	// (PBE enable) in the IA32_MISC_ENABLE MSR enables this capability.
	[[nodiscard]]
	inline bool pbe() const;
	FEA_REGION(#pragma endregion)


	/**
	 * EAX=7 feature bits
	 */
	FEA_REGION(#pragma region EAX7)

	// EBX

	[[nodiscard]]
	inline bool fsgsbase() const;
	[[nodiscard]]
	inline bool ia32_tsc_adjust() const;
	[[nodiscard]]
	inline bool sgx() const;
	[[nodiscard]]
	inline bool bmi1() const;
	[[nodiscard]]
	inline bool hle() const;
	[[nodiscard]]
	inline bool avx2() const;
	[[nodiscard]]
	inline bool fdp_excptn_only() const;
	[[nodiscard]]
	inline bool smep() const;
	[[nodiscard]]
	inline bool bmi2() const;
	[[nodiscard]]
	inline bool erms() const;
	[[nodiscard]]
	inline bool invpcid() const;
	[[nodiscard]]
	inline bool rtm() const;
	[[nodiscard]]
	inline bool pqm() const;
	[[nodiscard]]
	inline bool fpu_csds() const;
	[[nodiscard]]
	inline bool mpx() const;
	[[nodiscard]]
	inline bool pqe() const;
	[[nodiscard]]
	inline bool avx512_f() const;
	[[nodiscard]]
	inline bool avx512_dq() const;
	[[nodiscard]]
	inline bool rdseed() const;
	[[nodiscard]]
	inline bool adx() const;
	[[nodiscard]]
	inline bool smap() const;
	[[nodiscard]]
	inline bool avx512_ifma() const;
	[[nodiscard]]
	inline bool pcommit() const;
	[[nodiscard]]
	inline bool clflushopt() const;
	[[nodiscard]]
	inline bool clwb() const;
	[[nodiscard]]
	inline bool intel_pt() const;
	[[nodiscard]]
	inline bool avx512_pf() const;
	[[nodiscard]]
	inline bool avx512_er() const;
	[[nodiscard]]
	inline bool avx512_cd() const;
	[[nodiscard]]
	inline bool sha() const;
	[[nodiscard]]
	inline bool avx512_bw() const;
	[[nodiscard]]
	inline bool avx512_vl() const;

	// ECX
	[[nodiscard]]
	inline bool prefetchwt1() const;
	[[nodiscard]]
	inline bool avx512_vbmi() const;
	[[nodiscard]]
	inline bool umip() const;
	[[nodiscard]]
	inline bool pku() const;
	[[nodiscard]]
	inline bool ospke() const;
	[[nodiscard]]
	inline bool waitpkg() const;
	[[nodiscard]]
	inline bool avx512_vbmi2() const;
	[[nodiscard]]
	inline bool cet_ss() const;
	[[nodiscard]]
	inline bool gfni() const;
	[[nodiscard]]
	inline bool vaes() const;
	[[nodiscard]]
	inline bool vpclmulqdq() const;
	[[nodiscard]]
	inline bool avx512_vnni() const;
	[[nodiscard]]
	inline bool avx512_bitalg() const;
	// 13
	[[nodiscard]]
	inline bool avx512_vpopcntdq() const;
	// 15
	[[nodiscard]]
	inline bool five_level_paging() const;
	uint8_t mawau() const;
	[[nodiscard]]
	inline bool rdpid() const;
	// 23 - 24
	[[nodiscard]]
	inline bool cldemote() const;
	// 26
	[[nodiscard]]
	inline bool movdiri() const;
	[[nodiscard]]
	inline bool movdir64b() const;
	[[nodiscard]]
	inline bool enqcmd() const;
	[[nodiscard]]
	inline bool sgx_lc() const;
	[[nodiscard]]
	inline bool pks() const;

	// EDX
	// 0 - 1
	[[nodiscard]]
	inline bool avx512_4vnniw() const;
	[[nodiscard]]
	inline bool avx512_4fmaps() const;
	[[nodiscard]]
	inline bool fsrm() const;
	// 5 - 7
	[[nodiscard]]
	inline bool avx512_vp2intersect() const;
	[[nodiscard]]
	inline bool srbds_ctrl() const;
	[[nodiscard]]
	inline bool md_clear() const;
	// 11 - 12
	[[nodiscard]]
	inline bool tsx_force_abort() const;
	[[nodiscard]]
	inline bool serialize() const;
	[[nodiscard]]
	inline bool hybrid() const;
	[[nodiscard]]
	inline bool tsxldtrk() const;
	// 17
	[[nodiscard]]
	inline bool pconfig() const;
	[[nodiscard]]
	inline bool lbr() const;
	[[nodiscard]]
	inline bool cet_ibt() const;
	// 21
	[[nodiscard]]
	inline bool amx_bf16() const;
	// 23
	[[nodiscard]]
	inline bool amx_tile() const;
	[[nodiscard]]
	inline bool amx_int8() const;
	[[nodiscard]]
	inline bool spec_ctrl() const;
	[[nodiscard]]
	inline bool stibp() const;
	[[nodiscard]]
	inline bool l1d_flush() const;
	[[nodiscard]]
	inline bool ia32_arch_capabilities() const;
	[[nodiscard]]
	inline bool ia32_core_capabilities() const;
	[[nodiscard]]
	inline bool ssbd() const;


	// EAX=7, ECX=1
	[[nodiscard]]
	inline bool avx512_bf16() const;
	FEA_REGION(#pragma endregion)


	/**
	 * EAX=80000001h feature bits
	 */
	FEA_REGION(#pragma region EAX80000001)

	// ECX

	[[nodiscard]]
	inline bool lahf_lm() const;
	[[nodiscard]]
	inline bool cmp_legacy() const;
	[[nodiscard]]
	inline bool svm() const;
	[[nodiscard]]
	inline bool extapic() const;
	[[nodiscard]]
	inline bool cr8_legacy() const;
	[[nodiscard]]
	inline bool abm() const;
	[[nodiscard]]
	inline bool sse4a() const;
	[[nodiscard]]
	inline bool misalignsse() const;
	[[nodiscard]]
	inline bool _3dnowprefetch() const;
	[[nodiscard]]
	inline bool osvw() const;
	[[nodiscard]]
	inline bool ibs() const;
	[[nodiscard]]
	inline bool xop() const;
	[[nodiscard]]
	inline bool skinit() const;
	[[nodiscard]]
	inline bool wdt() const;
	// 14
	[[nodiscard]]
	inline bool lwp() const;
	[[nodiscard]]
	inline bool fma4() const;
	[[nodiscard]]
	inline bool tce() const;
	// 18
	[[nodiscard]]
	inline bool nodeid_msr() const;
	// 20
	[[nodiscard]]
	inline bool tbm() const;
	[[nodiscard]]
	inline bool topoext() const;
	[[nodiscard]]
	inline bool perfctr_core() const;
	[[nodiscard]]
	inline bool perfctr_nb() const;
	// 25
	[[nodiscard]]
	inline bool dbx() const;
	[[nodiscard]]
	inline bool perftsc() const;
	[[nodiscard]]
	inline bool pcx_l2i() const;
	// 29 - 31


	// EDX

	[[nodiscard]]
	inline bool fpu_ext() const;
	[[nodiscard]]
	inline bool vme_ext() const;
	[[nodiscard]]
	inline bool de_ext() const;
	[[nodiscard]]
	inline bool pse_ext() const;
	[[nodiscard]]
	inline bool tsc_ext() const;
	[[nodiscard]]
	inline bool msr_ext() const;
	[[nodiscard]]
	inline bool pae_ext() const;
	[[nodiscard]]
	inline bool mce_ext() const;
	[[nodiscard]]
	inline bool cx8_ext() const;
	[[nodiscard]]
	inline bool apic_ext() const;
	// 10
	[[nodiscard]]
	inline bool syscall() const;
	[[nodiscard]]
	inline bool mtrr_ext() const;
	[[nodiscard]]
	inline bool pge_ext() const;
	[[nodiscard]]
	inline bool mca_ext() const;
	[[nodiscard]]
	inline bool cmov_ext() const;
	[[nodiscard]]
	inline bool pat_ext() const;
	[[nodiscard]]
	inline bool pse36_ext() const;
	// 18
	[[nodiscard]]
	inline bool mp() const;
	[[nodiscard]]
	inline bool nx() const;
	// 21
	[[nodiscard]]
	inline bool mmxext() const;
	[[nodiscard]]
	inline bool mmx_ext() const;
	[[nodiscard]]
	inline bool fxsr_ext() const;
	[[nodiscard]]
	inline bool fxsr_opt() const;
	[[nodiscard]]
	inline bool pdpe1gb() const;
	[[nodiscard]]
	inline bool rdtscp() const;
	// 28
	[[nodiscard]]
	inline bool lm() const;
	[[nodiscard]]
	inline bool _3dnowext() const;
	[[nodiscard]]
	inline bool _3dnow() const;

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
	[[nodiscard]]
	inline const cpu_id& eax1() const;

	// INPUT EAX = 02H: TLB/Cache/Prefetch Information Returned in EAX, EBX,
	// ECX, EDX
	[[nodiscard]]
	inline const cpu_id& eax2() const;

	// INPUT EAX = 03H: Processor Serial Number (Only available on Pentium
	// 3)
	[[nodiscard]]
	inline const cpu_id& eax3() const;

	// INPUT EAX = 04H: Returns Deterministic Cache Parameters for Each
	// Level
	[[nodiscard]]
	inline const cpu_id& eax4() const;

	// INPUT EAX = 06H: Returns Thermal and Power Management Features
	[[nodiscard]]
	inline const cpu_id& eax6() const;

	// INPUT EAX = 07H: Returns Structured Extended Feature Enumeration
	// Information
	[[nodiscard]]
	inline const cpu_id& eax7_ecx0() const;
	[[nodiscard]]
	inline const cpu_id& eax7_ecx1() const;

	// INPUT EAX = 0BH: Returns Extended Topology Information
	[[nodiscard]]
	inline const cpu_id& eax0b() const;

	// INPUT EAX=80000001H: Extended Processor Info and Feature Bits
	[[nodiscard]]
	inline const cpu_id& eax80000001() const;

	// 80000002H, 80000003H, 80000004H are decoded in brand string.

	// INPUT EAX=80000005H: L1 Cache and TLB Identifiers
	[[nodiscard]]
	inline const cpu_id& eax80000005() const;

	// INPUT EAX=80000006H: Extended L2 Cache Features
	[[nodiscard]]
	inline const cpu_id& eax80000006() const;

	// INPUT EAX=80000007H: Advanced Power Management Information
	[[nodiscard]]
	inline const cpu_id& eax80000007() const;

	// INPUT EAX=80000008H: Virtual and Physical address Sizes
	[[nodiscard]]
	inline const cpu_id& eax80000008() const;

	FEA_REGION(#pragma endregion)

	// Print all supported feature bits to console.
	inline void print_all() const;

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


// Implementation
namespace fea {
cpu_id::cpu_id(uint32_t leaf_, uint32_t sub_leaf_)
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

cpu_id::cpu_id(uint32_t leaf_)
		: cpu_id(leaf_, 0) {
}

std::array<char, 16> cpu_id::to_string() const {
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

cpu_info_t::cpu_info_t() {
	// Functions
	{
		// calling __cpuid with 0x0 as the function_id argument
		// gets the number of the highest valid function id.
		cpu_id eax0 = cpu_id{ 0 };
		uint32_t highest_leaf = eax0.eax.to_ulong();

		// Vendor ID String (0x0)
		std::array<char, 13> vendor{};
		std::array<char, 16> as_string = eax0.to_string();

		std::copy(as_string.begin() + 4, as_string.begin() + 8, vendor.begin());
		std::copy(as_string.begin() + 12, as_string.end(), vendor.begin() + 4);
		std::copy(as_string.begin() + 8, as_string.begin() + 12,
				vendor.begin() + 8);

		_vendor = vendor.data();

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

		if (highest_leaf >= 6) {
			_eax6 = cpu_id{ 6 };
		}

		// Features Extended (EAX=7)
		if (highest_leaf >= 7) {
			_eax7_ecx0 = cpu_id{ 7 };
			_eax7_ecx1 = cpu_id{ 7, 1 };
		}

		// 0x0B
		if (highest_leaf >= 11) {
			_eax0b = cpu_id{ 11 };
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
		std::array<char, 64> brand{};
		if (highest_leaf >= 0x80000004) {
			cpu_id eax802 = cpu_id{ 0x80000002 };
			cpu_id eax803 = cpu_id{ 0x80000003 };
			cpu_id eax804 = cpu_id{ 0x80000004 };

			std::array<char, 16> str;
			str = eax802.to_string();
			std::copy(str.begin(), str.end(), brand.begin());
			str = eax803.to_string();
			std::copy(str.begin(), str.end(), brand.begin() + 16);
			str = eax804.to_string();
			std::copy(str.begin(), str.end(), brand.begin() + 32);

			_brand = brand.data();
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

const char* cpu_info_t::vendor() const {
	return _vendor.c_str();
}
const char* cpu_info_t::brand() const {
	return _brand.c_str();
}

bool cpu_info_t::intel() const {
	return _is_intel;
}
bool cpu_info_t::amd() const {
	return _is_amd;
}

/**
 * EAX=1 feature bits
 */

// EAX

uint8_t cpu_info_t::stepping_id() const {
	constexpr uint32_t mask = 0x0000'000f;
	uint32_t ret = _eax1.eax.to_ulong();
	return ret & mask;
}
uint8_t cpu_info_t::model() const {
	constexpr uint32_t mask = 0x0000'00f0;
	uint32_t ret = _eax1.eax.to_ulong();
	ret &= mask;
	ret >>= 4;
	return uint8_t(ret);
}
uint8_t cpu_info_t::family_id() const {
	constexpr uint32_t mask = 0x0000'0f00;
	uint32_t ret = _eax1.eax.to_ulong();
	ret &= mask;
	ret >>= 8;
	return uint8_t(ret);
}
uint8_t cpu_info_t::processor_type() const {
	constexpr uint32_t mask = 0b0000'0000'0000'0000'0011'0000'0000'0000;
	uint32_t ret = _eax1.eax.to_ulong();
	ret &= mask;
	ret >>= 12;
	return uint8_t(ret);
}
uint8_t cpu_info_t::extended_model_id() const {
	constexpr uint32_t mask = 0x000f'0000;
	uint32_t ret = _eax1.eax.to_ulong();
	ret &= mask;
	ret >>= 16;
	return uint8_t(ret);
}
uint8_t cpu_info_t::extended_family_id() const {
	constexpr uint32_t mask = 0x0ff0'0000;
	uint32_t ret = _eax1.eax.to_ulong();
	ret &= mask;
	ret >>= 20;
	return uint8_t(ret);
}


// EBX

uint8_t cpu_info_t::brand_index() const {
	constexpr uint32_t mask = 0x0000'00ff;
	uint32_t ret = _eax1.eax.to_ulong();
	ret &= mask;
	return uint8_t(ret);
}
// CLFLUSH line size (Value . 8 = cache line size in bytes).
uint8_t cpu_info_t::clflush_line_size() const {
	if (!clfsh()) {
		return 0u;
	}

	constexpr uint32_t mask = 0x0000'ff00;
	uint32_t ret = _eax1.eax.to_ulong();
	ret &= mask;
	ret >>= 8;
	return uint8_t(ret);
}
uint8_t cpu_info_t::num_addressable_logical_ids() const {
	if (!htt()) {
		return 0u;
	}

	constexpr uint32_t mask = 0x00ff'0000;
	uint32_t ret = _eax1.eax.to_ulong();
	ret &= mask;
	ret >>= 16;
	return uint8_t(ret);
}
uint8_t cpu_info_t::local_apic_id() const {
	uint32_t ret = _eax1.eax.to_ulong();
	ret >>= 24;
	return uint8_t(ret);
}


// ECX

// Streaming SIMD Extensions 3
// The processor supports the Streaming SIMD Extensions 3 instructions.
bool cpu_info_t::sse3() const {
	return _eax1.ecx[0];
}

// PCLMULDQ Instruction
// The processor supports PCLMULDQ instruction.
bool cpu_info_t::pclmulqdq() const {
	return _eax1.ecx[1];
}

// 64-Bit Debug Store
// Indicates that the processor has the ability to write a history of
// the 64-bit branch to and from addresses into a memory buffer.
bool cpu_info_t::dtes64() const {
	return _eax1.ecx[2];
}

// MONITOR/MWAIT
// The processor supports the MONITOR and MWAIT instructions.
bool cpu_info_t::monitor() const {
	return _eax1.ecx[3];
}

// CPL Qualified Debug Store
// The processor supports the extensions to the Debug Store feature to
// allow for branch message storage qualified by CPL.
bool cpu_info_t::ds_cpl() const {
	return _eax1.ecx[4];
}

// Virtual Machine Extensions
// The processor supports Virtualization Technology.
bool cpu_info_t::vmx() const {
	return _eax1.ecx[5];
}

// Safer Mode Extensions
// The processor supports Trusted Execution Technology.
bool cpu_info_t::smx() const {
	return _eax1.ecx[6];
}

// Enhanced SpeedStep Technology
// The processor supports Enhanced SpeedStep Technology and implements
// the IA32_PERF_STS and IA32_PERF_CTL registers.
bool cpu_info_t::est() const {
	return _eax1.ecx[7];
}

// Thermal Monitor 2
// The processor implements the Thermal Monitor 2 thermal control
// circuit (TCC).
bool cpu_info_t::tm2() const {
	return _eax1.ecx[8];
}

// Supplemental Streaming SIMD Extensions 3
// The processor supports the Supplemental Streaming SIMD Extensions 3
// instructions.
bool cpu_info_t::ssse3() const {
	return _eax1.ecx[9];
}

// L1 Context ID
// The L1 data cache mode can be set to either adaptive mode or shared
// mode by the BIOS.
bool cpu_info_t::cnxt_id() const {
	return _eax1.ecx[10];
}

// Silicon Debug interface
// A value of 1 indicates the processor supports IA32_DEBUG_INTERFACE
// MSR for silicon debug.
bool cpu_info_t::sdbg() const {
	return _eax1.ecx[11];
}

// Fused Multiply Add
// The processor supports FMA extensions using YMM state.
bool cpu_info_t::fma() const {
	return _eax1.ecx[12];
}

// CMPXCHG16B
// The processor supports the CMPXCHG16B instruction.
bool cpu_info_t::cx16() const {
	return _eax1.ecx[13];
}

// xTPR Update Control
// The processor supports the ability to disable sending Task Priority
// messages.  When this feature flag is set, Task Priority messages may
// be disabled.  Bit 23 (Echo TPR disable) in the IA32_MISC_ENABLE MSR
// controls the sending of Task Priority messages.
bool cpu_info_t::xtpr() const {
	return _eax1.ecx[14];
}

// Perfmon and Debug Capability
// The processor supports the Performance Capabilities MSR.
// IA32_PERF_CAPABILITIES register is MSR 345h.
bool cpu_info_t::pdcm() const {
	return _eax1.ecx[15];
}

// 16 - reserved

// Process Context Identifiers
// The processor supports PCIDs and that software may set CR4.PCIDE to 1
bool cpu_info_t::pcid() const {
	return _eax1.ecx[17];
}

// Direct Cache Access
// The processor supports the ability to prefetch data from a memory
// mapped device.
bool cpu_info_t::dca() const {
	return _eax1.ecx[18];
}

// Streaming SIMD Extensions 4.1
// The processor supports the Streaming SIMD Extensions 4.1
// instructions.
bool cpu_info_t::sse41() const {
	return _eax1.ecx[19];
}

// Streaming SIMD Extensions 4.2
// The processor supports the Streaming SIMD Extensions 4.2
// instructions.
bool cpu_info_t::sse42() const {
	return _eax1.ecx[20];
}

// Extended xAPIC Support
// The processor supports x2APIC feature.
bool cpu_info_t::x2apic() const {
	return _eax1.ecx[21];
}

// MOVBE Instruction
// The processor supports MOVBE instruction.
bool cpu_info_t::movbe() const {
	return _eax1.ecx[22];
}

// POPCNT Instruction
// The processor supports the POPCNT instruction.
bool cpu_info_t::popcnt() const {
	return _eax1.ecx[23];
}

// Time Stamp Counter Deadline
// The processor’s local APIC timer supports one-shot operation using a
// TSC deadline value.
bool cpu_info_t::tsc_deadline() const {
	return _eax1.ecx[24];
}

// AES Instruction Extensions
// The processor supports the AES instruction extensions.
bool cpu_info_t::aes() const {
	return _eax1.ecx[25];
}

// XSAVE/XSTOR States
// The processor supports the XSAVE/XRSTOR processor extended states
// feature, the XSETBV/XGETBV instructions, and the
// XFEATURE_ENABLED_MASK register (XCR0).
bool cpu_info_t::xsave() const {
	return _eax1.ecx[26];
}

// OS-Enabled Extended State Management
// A value of 1 indicates that the OS has enabled XSETBV/XGETBV
// instructions to access the XFEATURE_ENABLED_MASK register (XCR0), and
// support for processor extended state management using XSAVE/XRSTOR.
bool cpu_info_t::osxsave() const {
	return _eax1.ecx[27];
}

// Advanced Vector Extensions
// The processor supports the AVX instruction extensions.
bool cpu_info_t::avx() const {
	return _eax1.ecx[28];
}

// 16-bit floating-point conversion instructions
// A value of 1 indicates that the processor supports 16-bit
// floating-point conversion instructions.
bool cpu_info_t::f16c() const {
	return _eax1.ecx[29];
}

// RDRAND instruction supported
// A value of 1 indicates that processor supports RDRAND instruction.
bool cpu_info_t::rdrnd() const {
	return _eax1.ecx[30];
}

// Hypervisor
// Hypervisor present (always zero on physical CPUs).
bool cpu_info_t::hypervisor() const {
	return _eax1.ecx[31];
}


// EDX

// Floating-point Unit On-Chip
// The processor contains an FPU that supports the Intel387
// floating-point instruction set.
bool cpu_info_t::fpu() const {
	return _eax1.edx[0];
}

// Virtual Mode Extension
// The processor supports extensions to virtual-8086 mode.
bool cpu_info_t::vme() const {
	return _eax1.edx[1];
}

// Debugging Extension
// The processor supports I/O breakpoints, including the CR4.DE bit for
// enabling debug extensions and optional trapping of access to the DR4
// and DR5 registers.
bool cpu_info_t::de() const {
	return _eax1.edx[2];
}

// Page Size Extension
// The processor supports 4-MB pages.
bool cpu_info_t::pse() const {
	return _eax1.edx[3];
}

// Time Stamp Counter
// The RDTSC instruction is supported including the CR4.TSD bit for
// access/privilege control.
bool cpu_info_t::tsc() const {
	return _eax1.edx[4];
}

// Model Specific Registers
// Model Specific Registers are implemented with the RDMSR, WRMSR
// instructions.
bool cpu_info_t::msr() const {
	return _eax1.edx[5];
}

// Physical Address Extension
// Physical addresses greater than 32 bits are supported.
bool cpu_info_t::pae() const {
	return _eax1.edx[6];
}

// Machine-Check Exception
// Machine-Check Exception, INT18, and the CR4.MCE enable bit are
// supported.
bool cpu_info_t::mce() const {
	return _eax1.edx[7];
}

// CMPXCHG8 Instruction
// The compare and exchange 8-bytes instruction is supported.
bool cpu_info_t::cx8() const {
	return _eax1.edx[8];
}

// On-chip APIC Hardware
// The processor contains a software-accessible local APIC.
bool cpu_info_t::apic() const {
	return _eax1.edx[9];
}

// 10 - reserved

// Fast System Call
// Indicates whether the processor supports the Fast System Call
// instructions, SYSENTER and SYSEXIT. NOTE: Refer to Section 5.1.2.5
// for further information regarding SYSENTER/SYSEXIT feature and SEP
// feature bit.
bool cpu_info_t::sep() const {
	return _eax1.edx[11];
}

// Memory Type Range Registers
// The processor supports the Memory Type Range Registers specifically
// the MTRR_CAP register.
bool cpu_info_t::mtrr() const {
	return _eax1.edx[12];
}

// Page Global Enable
// The global bit in the page directory entries (PDEs) and page table
// entries (PTEs) is supported, indicating TLB entries that are common
// to different processes and need not be flushed. The CR4.PGE bit
// controls this feature.
bool cpu_info_t::pge() const {
	return _eax1.edx[13];
}

// Machine-Check Architecture
// The Machine-Check Architecture is supported, specifically the MCG_CAP
// register.
bool cpu_info_t::mca() const {
	return _eax1.edx[14];
}

// Conditional Move Instruction
// The processor supports CMOVcc, and if the FPU feature flag (bit 0) is
// also set, supports the FCMOVCC and FCOMI instructions.
bool cpu_info_t::cmov() const {
	return _eax1.edx[15];
}

// Page Attribute Table
// Indicates whether the processor supports the Page Attribute Table.
// This feature augments the Memory Type Range Registers (MTRRs),
// allowing an operating system to specify attributes of memory on 4K
// granularity through a linear address.
bool cpu_info_t::pat() const {
	return _eax1.edx[16];
}

// 36-bit Page Size Extension
// Indicates whether the processor supports 4-MB pages that are capable
// of addressing physical memory beyond 4-GB. This feature indicates
// that the upper four bits of the physical address of the 4-MB page is
// encoded by bits 13-16 of the page directory entry.
bool cpu_info_t::pse36() const {
	return _eax1.edx[17];
}

// Processor serial number is present and enabled
// The processor supports the 96-bit processor serial number feature,
// and the feature is enabled.Note: The Pentium 4 and subsequent
// processor families do not support this feature.
bool cpu_info_t::psn() const {
	return _eax1.edx[18];
}

// CLFLUSH Instruction
// Indicates that the processor supports the CLFLUSH instruction.
bool cpu_info_t::clfsh() const {
	return _eax1.edx[19];
}

// 20 - reserved

// Debug Store
// Indicates that the processor supports the ability to write debug
// information into a memory resident buffer. This feature is used by
// the branch trace store (BTS) and precise event-based sampling (PEBS)
// facilities.
bool cpu_info_t::ds() const {
	return _eax1.edx[21];
}

// Thermal Monitor and Software Controlled Clock Facilities
// The processor implements internal MSRs that allow processor
// temperature to be monitored and processor performance to be modulated
// in predefined duty cycles under software control.
bool cpu_info_t::acpi() const {
	return _eax1.edx[22];
}

// MMX technology
// The processor supports the MMX technology instruction set extensions
// to Intel Architecture.
bool cpu_info_t::mmx() const {
	return _eax1.edx[23];
}

// FXSAVE and FXSTOR Instructions
// The FXSAVE and FXRSTOR instructions are supported for fast save and
// restore of the floating point context. Presence of this bit also
// indicates that CR4.OSFXSR is available for an operating system to
// indicate that it supports the FXSAVE and FXRSTOR instructions.
bool cpu_info_t::fxsr() const {
	return _eax1.edx[24];
}

// Streaming SIMD Extensions
// The processor supports the Streaming SIMD Extensions to the Intel
// Architecture.
bool cpu_info_t::sse() const {
	return _eax1.edx[25];
}

// Streaming SIMD Extensions 2
// Indicates the processor supports the Streaming SIMD Extensions 2
// Instructions.
bool cpu_info_t::sse2() const {
	return _eax1.edx[26];
}

// Self-Snoop
// The processor supports the management of conflicting memory types by
// performing a snoop of its own cache structure for transactions issued
// to the bus.
bool cpu_info_t::ss() const {
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
bool cpu_info_t::htt() const {
	return _eax1.edx[28];
}

// Thermal Monitor
// The processor implements the Thermal Monitor automatic thermal
// control circuitry (TCC).
bool cpu_info_t::tm() const {
	return _eax1.edx[29];
}

// IA64 processor emulating x86
bool cpu_info_t::ia64() const {
	return _eax1.edx[30];
}

// Pending Break Enable
// The processor supports the use of the FERR#/PBE# pin when the
// processor is in the stop-clock state (STPCLK# is asserted) to signal
// the processor that an interrupt is pending and that the processor
// should return to normal operation to handle the interrupt. Bit 10
// (PBE enable) in the IA32_MISC_ENABLE MSR enables this capability.
bool cpu_info_t::pbe() const {
	return _eax1.edx[31];
}
FEA_REGION(#pragma endregion)


/**
 * EAX=7 feature bits
 */
FEA_REGION(#pragma region EAX7)

// EBX

bool cpu_info_t::fsgsbase() const {
	return _eax7_ecx0.ebx[0];
}
bool cpu_info_t::ia32_tsc_adjust() const {
	return _eax7_ecx0.ebx[1];
}
bool cpu_info_t::sgx() const {
	return _eax7_ecx0.ebx[2];
}
bool cpu_info_t::bmi1() const {
	return _eax7_ecx0.ebx[3];
}
bool cpu_info_t::hle() const {
	return _eax7_ecx0.ebx[4];
}
bool cpu_info_t::avx2() const {
	return _eax7_ecx0.ebx[5];
}
bool cpu_info_t::fdp_excptn_only() const {
	return _eax7_ecx0.ebx[6];
}
bool cpu_info_t::smep() const {
	return _eax7_ecx0.ebx[7];
}
bool cpu_info_t::bmi2() const {
	return _eax7_ecx0.ebx[8];
}
bool cpu_info_t::erms() const {
	return _eax7_ecx0.ebx[9];
}
bool cpu_info_t::invpcid() const {
	return _eax7_ecx0.ebx[10];
}
bool cpu_info_t::rtm() const {
	return _eax7_ecx0.ebx[11];
}
bool cpu_info_t::pqm() const {
	return _eax7_ecx0.ebx[12];
}
bool cpu_info_t::fpu_csds() const {
	return _eax7_ecx0.ebx[13];
}
bool cpu_info_t::mpx() const {
	return _eax7_ecx0.ebx[14];
}
bool cpu_info_t::pqe() const {
	return _eax7_ecx0.ebx[15];
}
bool cpu_info_t::avx512_f() const {
	return _eax7_ecx0.ebx[16];
}
bool cpu_info_t::avx512_dq() const {
	return _eax7_ecx0.ebx[17];
}
bool cpu_info_t::rdseed() const {
	return _eax7_ecx0.ebx[18];
}
bool cpu_info_t::adx() const {
	return _eax7_ecx0.ebx[19];
}
bool cpu_info_t::smap() const {
	return _eax7_ecx0.ebx[20];
}
bool cpu_info_t::avx512_ifma() const {
	return _eax7_ecx0.ebx[21];
}
bool cpu_info_t::pcommit() const {
	return _eax7_ecx0.ebx[22];
}
bool cpu_info_t::clflushopt() const {
	return _eax7_ecx0.ebx[23];
}
bool cpu_info_t::clwb() const {
	return _eax7_ecx0.ebx[24];
}
bool cpu_info_t::intel_pt() const {
	return _eax7_ecx0.ebx[25];
}
bool cpu_info_t::avx512_pf() const {
	return _eax7_ecx0.ebx[26];
}
bool cpu_info_t::avx512_er() const {
	return _eax7_ecx0.ebx[27];
}
bool cpu_info_t::avx512_cd() const {
	return _eax7_ecx0.ebx[28];
}
bool cpu_info_t::sha() const {
	return _eax7_ecx0.ebx[29];
}
bool cpu_info_t::avx512_bw() const {
	return _eax7_ecx0.ebx[30];
}
bool cpu_info_t::avx512_vl() const {
	return _eax7_ecx0.ebx[31];
}

// ECX
bool cpu_info_t::prefetchwt1() const {
	return _eax7_ecx0.ecx[0];
}
bool cpu_info_t::avx512_vbmi() const {
	return _eax7_ecx0.ecx[1];
}
bool cpu_info_t::umip() const {
	return _eax7_ecx0.ecx[2];
}
bool cpu_info_t::pku() const {
	return _eax7_ecx0.ecx[3];
}
bool cpu_info_t::ospke() const {
	return _eax7_ecx0.ecx[4];
}
bool cpu_info_t::waitpkg() const {
	return _eax7_ecx0.ecx[5];
}
bool cpu_info_t::avx512_vbmi2() const {
	return _eax7_ecx0.ecx[6];
}
bool cpu_info_t::cet_ss() const {
	return _eax7_ecx0.ecx[7];
}
bool cpu_info_t::gfni() const {
	return _eax7_ecx0.ecx[8];
}
bool cpu_info_t::vaes() const {
	return _eax7_ecx0.ecx[9];
}
bool cpu_info_t::vpclmulqdq() const {
	return _eax7_ecx0.ecx[10];
}
bool cpu_info_t::avx512_vnni() const {
	return _eax7_ecx0.ecx[11];
}
bool cpu_info_t::avx512_bitalg() const {
	return _eax7_ecx0.ecx[12];
}
// 13
bool cpu_info_t::avx512_vpopcntdq() const {
	return _eax7_ecx0.ecx[14];
}
// 15
bool cpu_info_t::five_level_paging() const {
	return _eax7_ecx0.ecx[16];
}
uint8_t cpu_info_t::mawau() const {
	constexpr uint32_t mask = 0b0000'0000'0011'1110'0000'0000;
	uint32_t ret = _eax7_ecx0.ecx.to_ulong();
	ret &= mask;
	ret >>= 9;
	return uint8_t(ret);
}
bool cpu_info_t::rdpid() const {
	return _eax7_ecx0.ecx[22];
}
// 23 - 24
bool cpu_info_t::cldemote() const {
	return _eax7_ecx0.ecx[25];
}
// 26
bool cpu_info_t::movdiri() const {
	return _eax7_ecx0.ecx[27];
}
bool cpu_info_t::movdir64b() const {
	return _eax7_ecx0.ecx[28];
}
bool cpu_info_t::enqcmd() const {
	return _eax7_ecx0.ecx[29];
}
bool cpu_info_t::sgx_lc() const {
	return _eax7_ecx0.ecx[30];
}
bool cpu_info_t::pks() const {
	return _eax7_ecx0.ecx[31];
}

// EDX
// 0 - 1
bool cpu_info_t::avx512_4vnniw() const {
	return _eax7_ecx0.edx[2];
}
bool cpu_info_t::avx512_4fmaps() const {
	return _eax7_ecx0.edx[3];
}
bool cpu_info_t::fsrm() const {
	return _eax7_ecx0.edx[4];
}
// 5 - 7
bool cpu_info_t::avx512_vp2intersect() const {
	return _eax7_ecx0.edx[8];
}
bool cpu_info_t::srbds_ctrl() const {
	return _eax7_ecx0.edx[9];
}
bool cpu_info_t::md_clear() const {
	return _eax7_ecx0.edx[10];
}
// 11 - 12
bool cpu_info_t::tsx_force_abort() const {
	return _eax7_ecx0.edx[13];
}
bool cpu_info_t::serialize() const {
	return _eax7_ecx0.edx[14];
}
bool cpu_info_t::hybrid() const {
	return _eax7_ecx0.edx[15];
}
bool cpu_info_t::tsxldtrk() const {
	return _eax7_ecx0.edx[16];
}
// 17
bool cpu_info_t::pconfig() const {
	return _eax7_ecx0.edx[18];
}
bool cpu_info_t::lbr() const {
	return _eax7_ecx0.edx[19];
}
bool cpu_info_t::cet_ibt() const {
	return _eax7_ecx0.edx[20];
}
// 21
bool cpu_info_t::amx_bf16() const {
	return _eax7_ecx0.edx[22];
}
// 23
bool cpu_info_t::amx_tile() const {
	return _eax7_ecx0.edx[24];
}
bool cpu_info_t::amx_int8() const {
	return _eax7_ecx0.edx[25];
}
bool cpu_info_t::spec_ctrl() const {
	return _eax7_ecx0.edx[26];
}
bool cpu_info_t::stibp() const {
	return _eax7_ecx0.edx[27];
}
bool cpu_info_t::l1d_flush() const {
	return _eax7_ecx0.edx[28];
}
bool cpu_info_t::ia32_arch_capabilities() const {
	return _eax7_ecx0.edx[29];
}
bool cpu_info_t::ia32_core_capabilities() const {
	return _eax7_ecx0.edx[30];
}
bool cpu_info_t::ssbd() const {
	return _eax7_ecx0.edx[31];
}


// EAX=7, ECX=1
bool cpu_info_t::avx512_bf16() const {
	return _eax7_ecx1.eax[5];
}
FEA_REGION(#pragma endregion)


/**
 * EAX=80000001h feature bits
 */
FEA_REGION(#pragma region EAX80000001)

// ECX

bool cpu_info_t::lahf_lm() const {
	return _eax80000001.ecx[0];
}
bool cpu_info_t::cmp_legacy() const {
	return _eax80000001.ecx[1];
}
bool cpu_info_t::svm() const {
	return _eax80000001.ecx[2];
}
bool cpu_info_t::extapic() const {
	return _eax80000001.ecx[3];
}
bool cpu_info_t::cr8_legacy() const {
	return _eax80000001.ecx[4];
}
bool cpu_info_t::abm() const {
	return _eax80000001.ecx[5];
}
bool cpu_info_t::sse4a() const {
	return _eax80000001.ecx[6];
}
bool cpu_info_t::misalignsse() const {
	return _eax80000001.ecx[7];
}
bool cpu_info_t::_3dnowprefetch() const {
	return _eax80000001.ecx[8];
}
bool cpu_info_t::osvw() const {
	return _eax80000001.ecx[9];
}
bool cpu_info_t::ibs() const {
	return _eax80000001.ecx[10];
}
bool cpu_info_t::xop() const {
	return _eax80000001.ecx[11];
}
bool cpu_info_t::skinit() const {
	return _eax80000001.ecx[12];
}
bool cpu_info_t::wdt() const {
	return _eax80000001.ecx[13];
}
// 14
bool cpu_info_t::lwp() const {
	return _eax80000001.ecx[15];
}
bool cpu_info_t::fma4() const {
	return _eax80000001.ecx[16];
}
bool cpu_info_t::tce() const {
	return _eax80000001.ecx[17];
}
// 18
bool cpu_info_t::nodeid_msr() const {
	return _eax80000001.ecx[19];
}
// 20
bool cpu_info_t::tbm() const {
	return _eax80000001.ecx[21];
}
bool cpu_info_t::topoext() const {
	return _eax80000001.ecx[22];
}
bool cpu_info_t::perfctr_core() const {
	return _eax80000001.ecx[23];
}
bool cpu_info_t::perfctr_nb() const {
	return _eax80000001.ecx[24];
}
// 25
bool cpu_info_t::dbx() const {
	return _eax80000001.ecx[26];
}
bool cpu_info_t::perftsc() const {
	return _eax80000001.ecx[27];
}
bool cpu_info_t::pcx_l2i() const {
	return _eax80000001.ecx[28];
}
// 29 - 31


// EDX

bool cpu_info_t::fpu_ext() const {
	return _eax80000001.edx[0];
}
bool cpu_info_t::vme_ext() const {
	return _eax80000001.edx[1];
}
bool cpu_info_t::de_ext() const {
	return _eax80000001.edx[2];
}
bool cpu_info_t::pse_ext() const {
	return _eax80000001.edx[3];
}
bool cpu_info_t::tsc_ext() const {
	return _eax80000001.edx[4];
}
bool cpu_info_t::msr_ext() const {
	return _eax80000001.edx[5];
}
bool cpu_info_t::pae_ext() const {
	return _eax80000001.edx[6];
}
bool cpu_info_t::mce_ext() const {
	return _eax80000001.edx[7];
}
bool cpu_info_t::cx8_ext() const {
	return _eax80000001.edx[8];
}
bool cpu_info_t::apic_ext() const {
	return _eax80000001.edx[9];
}
// 10
bool cpu_info_t::syscall() const {
	return _eax80000001.edx[11];
}
bool cpu_info_t::mtrr_ext() const {
	return _eax80000001.edx[12];
}
bool cpu_info_t::pge_ext() const {
	return _eax80000001.edx[13];
}
bool cpu_info_t::mca_ext() const {
	return _eax80000001.edx[14];
}
bool cpu_info_t::cmov_ext() const {
	return _eax80000001.edx[15];
}
bool cpu_info_t::pat_ext() const {
	return _eax80000001.edx[16];
}
bool cpu_info_t::pse36_ext() const {
	return _eax80000001.edx[17];
}
// 18
bool cpu_info_t::mp() const {
	return _eax80000001.edx[19];
}
bool cpu_info_t::nx() const {
	return _eax80000001.edx[20];
}
// 21
bool cpu_info_t::mmxext() const {
	return _eax80000001.edx[22];
}
bool cpu_info_t::mmx_ext() const {
	return _eax80000001.edx[23];
}
bool cpu_info_t::fxsr_ext() const {
	return _eax80000001.edx[24];
}
bool cpu_info_t::fxsr_opt() const {
	return _eax80000001.edx[25];
}
bool cpu_info_t::pdpe1gb() const {
	return _eax80000001.edx[26];
}
bool cpu_info_t::rdtscp() const {
	return _eax80000001.edx[27];
}
// 28
bool cpu_info_t::lm() const {
	return _eax80000001.edx[29];
}
bool cpu_info_t::_3dnowext() const {
	return _eax80000001.edx[30];
}
bool cpu_info_t::_3dnow() const {
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
const cpu_id& cpu_info_t::eax1() const {
	return _eax1;
}

// INPUT EAX = 02H: TLB/Cache/Prefetch Information Returned in EAX, EBX,
// ECX, EDX
const cpu_id& cpu_info_t::eax2() const {
	return _eax2;
}

// INPUT EAX = 03H: Processor Serial Number (Only available on Pentium
// 3)
const cpu_id& cpu_info_t::eax3() const {
	return _eax3;
}

// INPUT EAX = 04H: Returns Deterministic Cache Parameters for Each
// Level
const cpu_id& cpu_info_t::eax4() const {
	return _eax4;
}

// INPUT EAX = 06H: Returns Thermal and Power Management Features
const cpu_id& cpu_info_t::eax6() const {
	return _eax6;
}

// INPUT EAX = 07H: Returns Structured Extended Feature Enumeration
// Information
const cpu_id& cpu_info_t::eax7_ecx0() const {
	return _eax7_ecx0;
}
const cpu_id& cpu_info_t::eax7_ecx1() const {
	return _eax7_ecx1;
}

// INPUT EAX = 0BH: Returns Extended Topology Information
const cpu_id& cpu_info_t::eax0b() const {
	return _eax0b;
}

// INPUT EAX=80000001H: Extended Processor Info and Feature Bits
const cpu_id& cpu_info_t::eax80000001() const {
	return _eax80000001;
}

// 80000002H, 80000003H, 80000004H are decoded in brand string.

// INPUT EAX=80000005H: L1 Cache and TLB Identifiers
const cpu_id& cpu_info_t::eax80000005() const {
	return _eax80000005;
}

// INPUT EAX=80000006H: Extended L2 Cache Features
const cpu_id& cpu_info_t::eax80000006() const {
	return _eax80000006;
}

// INPUT EAX=80000007H: Advanced Power Management Information
const cpu_id& cpu_info_t::eax80000007() const {
	return _eax80000007;
}

// INPUT EAX=80000008H: Virtual and Physical address Sizes
const cpu_id& cpu_info_t::eax80000008() const {
	return _eax80000008;
}
FEA_REGION(#pragma endregion)

// Print all supported feature bits to console.
void cpu_info_t::print_all() const {
	auto print_bool = [](const char* str, bool b) {
		printf("%-18s%s\n", str, b ? "true" : "false");
	};

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
	printf("%-18s%hhu\n", "num_addressable_ids", num_addressable_logical_ids());
	printf("%-18s%hhu\n", "local_apic_id", local_apic_id());
	printf("\n");

	printf("eax1 - edx\n");
	print_bool("fpu", fpu());
	print_bool("vme", vme());
	print_bool("de", de());
	print_bool("pse", pse());
	print_bool("tsc", tsc());
	print_bool("msr", msr());
	print_bool("pae", pae());
	print_bool("mce", mce());
	print_bool("cx8", cx8());
	print_bool("apic", apic());
	print_bool("sep", sep());
	print_bool("mtrr", mtrr());
	print_bool("pge", pge());
	print_bool("mca", mca());
	print_bool("cmov", cmov());
	print_bool("pat", pat());
	print_bool("pse36", pse36());
	print_bool("psn", psn());
	print_bool("clfsh", clfsh());
	print_bool("ds", ds());
	print_bool("acpi", acpi());
	print_bool("mmx", mmx());
	print_bool("fxsr", fxsr());
	print_bool("sse", sse());
	print_bool("sse2", sse2());
	print_bool("ss", ss());
	print_bool("htt", htt());
	print_bool("tm", tm());
	print_bool("ia64", ia64());
	print_bool("pbe", pbe());
	printf("\n");

	printf("eax1 - ecx\n");
	print_bool("sse3", sse3());
	print_bool("pclmulqdq", pclmulqdq());
	print_bool("dtes64", dtes64());
	print_bool("monitor", monitor());
	print_bool("ds_cpl", ds_cpl());
	print_bool("vmx", vmx());
	print_bool("smx", smx());
	print_bool("est", est());
	print_bool("tm2", tm2());
	print_bool("ssse3", ssse3());
	print_bool("cnxt_id", cnxt_id());
	print_bool("sdbg", sdbg());
	print_bool("fma", fma());
	print_bool("cx16", cx16());
	print_bool("xtpr", xtpr());
	print_bool("pdcm", pdcm());
	print_bool("pcid", pcid());
	print_bool("dca", dca());
	print_bool("sse41", sse41());
	print_bool("sse42", sse42());
	print_bool("x2apic", x2apic());
	print_bool("movbe", movbe());
	print_bool("popcnt", popcnt());
	print_bool("tsc_deadline", tsc_deadline());
	print_bool("aes", aes());
	print_bool("xsave", xsave());
	print_bool("osxsave", osxsave());
	print_bool("avx", avx());
	print_bool("f16c", f16c());
	print_bool("rdrnd", rdrnd());
	print_bool("hypervisor", hypervisor());
	printf("\n");

	printf("eax7_ecx0 - ebx\n");
	print_bool("fsgsbase", fsgsbase());
	print_bool("ia32_tsc_adjust", ia32_tsc_adjust());
	print_bool("sgx", sgx());
	print_bool("bmi1", bmi1());
	print_bool("hle", hle());
	print_bool("avx2", avx2());
	print_bool("fdp_excptn_only", fdp_excptn_only());
	print_bool("smep", smep());
	print_bool("bmi2", bmi2());
	print_bool("erms", erms());
	print_bool("invpcid", invpcid());
	print_bool("rtm", rtm());
	print_bool("pqm", pqm());
	print_bool("fpu_csds", fpu_csds());
	print_bool("mpx", mpx());
	print_bool("pqe", pqe());
	print_bool("avx512f", avx512_f());
	print_bool("avx512dq", avx512_dq());
	print_bool("rdseed", rdseed());
	print_bool("adx", adx());
	print_bool("smap", smap());
	print_bool("avx512ifma", avx512_ifma());
	print_bool("pcommit", pcommit());
	print_bool("clflushopt", clflushopt());
	print_bool("clwb", clwb());
	print_bool("intel_pt", intel_pt());
	print_bool("avx512pf", avx512_pf());
	print_bool("avx512er", avx512_er());
	print_bool("avx512cd", avx512_cd());
	print_bool("sha", sha());
	print_bool("avx512bw", avx512_bw());
	print_bool("avx512vl", avx512_vl());
	printf("\n");


	printf("eax7_ecx0 - ecx\n");
	print_bool("prefetchwt1", prefetchwt1());
	print_bool("avx512_vbmi", avx512_vbmi());
	print_bool("umip", umip());
	print_bool("pku", pku());
	print_bool("ospke", ospke());
	print_bool("waitpkg", waitpkg());
	print_bool("avx512_vbmi2", avx512_vbmi2());
	print_bool("cet_ss", cet_ss());
	print_bool("gfni", gfni());
	print_bool("vaes", vaes());
	print_bool("vpclmulqdq", vpclmulqdq());
	print_bool("avx512_vnni", avx512_vnni());
	print_bool("avx512_bitalg", avx512_bitalg());
	print_bool("avx512_vpopcntdq", avx512_vpopcntdq());
	print_bool("five_level_paging", five_level_paging());
	printf("%-18s%hhu\n", "mawau", mawau());
	print_bool("rdpid", rdpid());
	print_bool("cldemote", cldemote());
	print_bool("movdiri", movdiri());
	print_bool("movdir64b", movdir64b());
	print_bool("enqcmd", enqcmd());
	print_bool("sgx_lc", sgx_lc());
	print_bool("pks", pks());
	printf("\n");

	printf("eax7_ecx0 - edx\n");
	print_bool("avx512_4vnniw", avx512_4vnniw());
	print_bool("avx512_4fmaps", avx512_4fmaps());
	print_bool("fsrm", fsrm());
	print_bool("avx512_vp2intersect", avx512_vp2intersect());
	print_bool("srbds_ctrl", srbds_ctrl());
	print_bool("md_clear", md_clear());
	print_bool("tsx_force_abort", tsx_force_abort());
	print_bool("serialize", serialize());
	print_bool("hybrid", hybrid());
	print_bool("tsxldtrk", tsxldtrk());
	print_bool("pconfig", pconfig());
	print_bool("lbr", lbr());
	print_bool("cet_ibt", cet_ibt());
	print_bool("amx_bf16", amx_bf16());
	print_bool("amx_tile", amx_tile());
	print_bool("amx_int8", amx_int8());
	print_bool("spec_ctrl", spec_ctrl());
	print_bool("stibp", stibp());
	print_bool("l1d_flush", l1d_flush());
	print_bool("ia32_arch_capabilities", ia32_arch_capabilities());
	print_bool("ia32_core_capabilities", ia32_core_capabilities());
	print_bool("ssbd", ssbd());
	printf("\n");

	printf("eax7_ecx1 - eax\n");
	print_bool("avx512_bf16", avx512_bf16());
	printf("\n");

	print_bool("fpu_ext", fpu_ext());
	print_bool("vme_ext", vme_ext());
	print_bool("de_ext", de_ext());
	print_bool("pse_ext", pse_ext());
	print_bool("tsc_ext", tsc_ext());
	print_bool("msr_ext", msr_ext());
	print_bool("pae_ext", pae_ext());
	print_bool("mce_ext", mce_ext());
	print_bool("cx8_ext", cx8_ext());
	print_bool("apic_ext", apic_ext());
	print_bool("syscall", syscall());
	print_bool("mtrr_ext", mtrr_ext());
	print_bool("pge_ext", pge_ext());
	print_bool("mca_ext", mca_ext());
	print_bool("cmov_ext", cmov_ext());
	print_bool("pat_ext", pat_ext());
	print_bool("pse36_ext", pse36_ext());
	print_bool("mp", mp());
	print_bool("nx", nx());
	print_bool("mmxext", mmxext());
	print_bool("mmx_ext", mmx_ext());
	print_bool("fxsr_ext", fxsr_ext());
	print_bool("fxsr_opt", fxsr_opt());
	print_bool("pdpe1gb", pdpe1gb());
	print_bool("rdtscp", rdtscp());
	print_bool("lm", lm());
	print_bool("3dnowext", _3dnowext());
	print_bool("3dnow", _3dnow());
	print_bool("lahf_lm", lahf_lm());
	print_bool("cmp_legacy", cmp_legacy());
	print_bool("svm", svm());
	print_bool("extapic", extapic());
	print_bool("cr8_legacy", cr8_legacy());
	print_bool("abm", abm());
	print_bool("sse4a", sse4a());
	print_bool("misalignsse", misalignsse());
	print_bool("3dnowprefetch", _3dnowprefetch());
	print_bool("osvw", osvw());
	print_bool("ibs", ibs());
	print_bool("xop", xop());
	print_bool("skinit", skinit());
	print_bool("wdt", wdt());
	print_bool("lwp", lwp());
	print_bool("fma4", fma4());
	print_bool("tce", tce());
	print_bool("nodeid_msr", nodeid_msr());
	print_bool("tbm", tbm());
	print_bool("topoext", topoext());
	print_bool("perfctr_core", perfctr_core());
	print_bool("perfctr_nb", perfctr_nb());
	print_bool("dbx", dbx());
	print_bool("perftsc", perftsc());
	print_bool("pcx_l2i", pcx_l2i());
	printf("\n");
}

// [[nodiscard]] inline const cpu_info_t cpu_info;
} // namespace fea

#endif // !FEA_ARM
