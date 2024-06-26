﻿#include <fea/performance/cpu_info.hpp>
#if !FEA_ARM
#include <gtest/gtest.h>
#include <unordered_map>

#if FEA_MACOS
#include "cpu_info_macos.hpp"
#else
#endif

namespace {
std::unordered_map<std::string, bool> flag_map{
	{ "fpu", fea::cpu_info.fpu() },
	{ "vme", fea::cpu_info.vme() },
	{ "de", fea::cpu_info.de() },
	{ "pse", fea::cpu_info.pse() },
	{ "tsc", fea::cpu_info.tsc() },
	{ "msr", fea::cpu_info.msr() },
	{ "pae", fea::cpu_info.pae() },
	{ "mce", fea::cpu_info.mce() },
	{ "cx8", fea::cpu_info.cx8() },
	{ "apic", fea::cpu_info.apic() },
	{ "sep", fea::cpu_info.sep() },
	{ "mtrr", fea::cpu_info.mtrr() },
	{ "pge", fea::cpu_info.pge() },
	{ "mca", fea::cpu_info.mca() },
	{ "cmov", fea::cpu_info.cmov() },
	{ "pat", fea::cpu_info.pat() },
	{ "pse36", fea::cpu_info.pse36() },
	{ "psn", fea::cpu_info.psn() },
	{ "clfsh", fea::cpu_info.clfsh() },
	{ "ds", fea::cpu_info.ds() },
	{ "acpi", fea::cpu_info.acpi() },
	{ "mmx", fea::cpu_info.mmx() },
	{ "fxsr", fea::cpu_info.fxsr() },
	{ "sse", fea::cpu_info.sse() },
	{ "sse2", fea::cpu_info.sse2() },
	{ "ss", fea::cpu_info.ss() },
	{ "htt", fea::cpu_info.htt() },
	{ "tm", fea::cpu_info.tm() },
	{ "ia64", fea::cpu_info.ia64() },
	{ "pbe", fea::cpu_info.pbe() },
	{ "sse3", fea::cpu_info.sse3() },
	{ "pclmulqdq", fea::cpu_info.pclmulqdq() },
	{ "dtes64", fea::cpu_info.dtes64() },
	{ "monitor", fea::cpu_info.monitor() },
	{ "ds-cpl", fea::cpu_info.ds_cpl() },
	{ "vmx", fea::cpu_info.vmx() },
	{ "smx", fea::cpu_info.smx() },
	{ "est", fea::cpu_info.est() },
	{ "tm2", fea::cpu_info.tm2() },
	{ "ssse3", fea::cpu_info.ssse3() },
	{ "cnxt-id", fea::cpu_info.cnxt_id() },
	{ "fma", fea::cpu_info.fma() },
	{ "cx16", fea::cpu_info.cx16() },
	{ "xtpr", fea::cpu_info.xtpr() },
	{ "pdcm", fea::cpu_info.pdcm() },
	{ "pcid", fea::cpu_info.pcid() },
	{ "dca", fea::cpu_info.dca() },
	{ "sse41", fea::cpu_info.sse41() },
	{ "sse42", fea::cpu_info.sse42() },
	{ "x2apic", fea::cpu_info.x2apic() },
	{ "movbe", fea::cpu_info.movbe() },
	{ "popcnt", fea::cpu_info.popcnt() },
	{ "tsc-deadline", fea::cpu_info.tsc_deadline() },
	{ "aes", fea::cpu_info.aes() },
	{ "xsave", fea::cpu_info.xsave() },
	{ "osxsave", fea::cpu_info.osxsave() },
	{ "avx", fea::cpu_info.avx() },
	{ "f16c", fea::cpu_info.f16c() },
	{ "rdrnd", fea::cpu_info.rdrnd() },
	{ "hypervisor", fea::cpu_info.hypervisor() },
	//
	{ "fsgsbase", fea::cpu_info.fsgsbase() },
	{ "ia32_tsc_adjust", fea::cpu_info.ia32_tsc_adjust() },
	{ "sgx", fea::cpu_info.sgx() },
	{ "bmi1", fea::cpu_info.bmi1() },
	{ "hle", fea::cpu_info.hle() },
	{ "avx2", fea::cpu_info.avx2() },
	{ "smep", fea::cpu_info.smep() },
	{ "bmi2", fea::cpu_info.bmi2() },
	{ "erms", fea::cpu_info.erms() },
	{ "invpcid", fea::cpu_info.invpcid() },
	{ "rtm", fea::cpu_info.rtm() },
	{ "pqm", fea::cpu_info.pqm() },
	{ "fpu_csds", fea::cpu_info.fpu_csds() },
	{ "mpx", fea::cpu_info.mpx() },
	{ "pqe", fea::cpu_info.pqe() },
	{ "avx512f", fea::cpu_info.avx512_f() },
	{ "avx512dq", fea::cpu_info.avx512_dq() },
	{ "rdseed", fea::cpu_info.rdseed() },
	{ "adx", fea::cpu_info.adx() },
	{ "smap", fea::cpu_info.smap() },
	{ "avx512ifma", fea::cpu_info.avx512_ifma() },
	{ "pcommit", fea::cpu_info.pcommit() },
	{ "clflushopt", fea::cpu_info.clflushopt() },
	{ "clwb", fea::cpu_info.clwb() },
	{ "intel_pt", fea::cpu_info.intel_pt() },
	{ "avx512pf", fea::cpu_info.avx512_pf() },
	{ "avx512er", fea::cpu_info.avx512_er() },
	{ "avx512cd", fea::cpu_info.avx512_cd() },
	{ "sha", fea::cpu_info.sha() },
	{ "avx512bw", fea::cpu_info.avx512_bw() },
	{ "avx512vl", fea::cpu_info.avx512_vl() },
	//
	{ "prefetchwt1", fea::cpu_info.prefetchwt1() },
	{ "avx512vbmi", fea::cpu_info.avx512_vbmi() },
	{ "umip", fea::cpu_info.umip() },
	{ "pku", fea::cpu_info.pku() },
	{ "ospke", fea::cpu_info.ospke() },
	{ "avx512vbmi2", fea::cpu_info.avx512_vbmi2() },
	{ "gfni", fea::cpu_info.gfni() },
	{ "vaes", fea::cpu_info.vaes() },
	{ "vpclmulqdq", fea::cpu_info.vpclmulqdq() },
	{ "avx512vnni", fea::cpu_info.avx512_vnni() },
	{ "avx512bitalg", fea::cpu_info.avx512_bitalg() },
	{ "avx512vpopcntdq", fea::cpu_info.avx512_vpopcntdq() },
	{ "rdpid", fea::cpu_info.rdpid() },
	{ "sgx_lc", fea::cpu_info.sgx_lc() },
	//
	{ "avx512_4vnniw", fea::cpu_info.avx512_4vnniw() },
	{ "avx512_4fmaps", fea::cpu_info.avx512_4fmaps() },
	{ "fsrm", fea::cpu_info.fsrm() },
	{ "avx512_vp2intersect", fea::cpu_info.avx512_vp2intersect() },
	{ "srbds_ctrl", fea::cpu_info.srbds_ctrl() },
	{ "md_clear", fea::cpu_info.md_clear() },
	{ "tsx_force_abort", fea::cpu_info.tsx_force_abort() },
	{ "serialize", fea::cpu_info.serialize() },
	{ "hybrid", fea::cpu_info.hybrid() },
	{ "tsxldtrk", fea::cpu_info.tsxldtrk() },
	{ "pconfig", fea::cpu_info.pconfig() },
	{ "lbr", fea::cpu_info.lbr() },
	{ "cet_ibt", fea::cpu_info.cet_ibt() },
	{ "amx_bf16", fea::cpu_info.amx_bf16() },
	{ "amx_tile", fea::cpu_info.amx_tile() },
	{ "amx_int8", fea::cpu_info.amx_int8() },
	{ "spec_ctrl", fea::cpu_info.spec_ctrl() },
	{ "stibp", fea::cpu_info.stibp() },
	{ "l1d_flush", fea::cpu_info.l1d_flush() },
	{ "ia32_arch_capabilities", fea::cpu_info.ia32_arch_capabilities() },
	{ "ia32_core_capabilities", fea::cpu_info.ia32_core_capabilities() },
	{ "ssbd", fea::cpu_info.ssbd() },
	//
	{ "fpu_ext", fea::cpu_info.fpu_ext() },
	{ "vme_ext", fea::cpu_info.vme_ext() },
	{ "de_ext", fea::cpu_info.de_ext() },
	{ "pse_ext", fea::cpu_info.pse_ext() },
	{ "tsc_ext", fea::cpu_info.tsc_ext() },
	{ "msr_ext", fea::cpu_info.msr_ext() },
	{ "pae_ext", fea::cpu_info.pae_ext() },
	{ "mce_ext", fea::cpu_info.mce_ext() },
	{ "cx8_ext", fea::cpu_info.cx8_ext() },
	{ "apic_ext", fea::cpu_info.apic_ext() },
	{ "syscall", fea::cpu_info.syscall() },
	{ "mtrr_ext", fea::cpu_info.mtrr_ext() },
	{ "pge_ext", fea::cpu_info.pge_ext() },
	{ "mca_ext", fea::cpu_info.mca_ext() },
	{ "cmov_ext", fea::cpu_info.cmov_ext() },
	{ "pat_ext", fea::cpu_info.pat_ext() },
	{ "pse36_ext", fea::cpu_info.pse36_ext() },
	{ "mp", fea::cpu_info.mp() },
	{ "nx", fea::cpu_info.nx() },
	{ "mmxext", fea::cpu_info.mmxext() },
	{ "mmx_ext", fea::cpu_info.mmx_ext() },
	{ "fxsr_ext", fea::cpu_info.fxsr_ext() },
	{ "fxsr_opt", fea::cpu_info.fxsr_opt() },
	{ "pdpe1gb", fea::cpu_info.pdpe1gb() },
	{ "rdtscp", fea::cpu_info.rdtscp() },
	{ "lm", fea::cpu_info.lm() },
	{ "3dnowext", fea::cpu_info._3dnowext() },
	{ "3dnow", fea::cpu_info._3dnow() },
	{ "lahf_lm", fea::cpu_info.lahf_lm() },
	{ "cmp_legacy", fea::cpu_info.cmp_legacy() },
	{ "svm", fea::cpu_info.svm() },
	{ "extapic", fea::cpu_info.extapic() },
	{ "cr8_legacy", fea::cpu_info.cr8_legacy() },
	{ "abm", fea::cpu_info.abm() },
	{ "sse4a", fea::cpu_info.sse4a() },
	{ "misalignsse", fea::cpu_info.misalignsse() },
	{ "3dnowprefetch", fea::cpu_info._3dnowprefetch() },
	{ "osvw", fea::cpu_info.osvw() },
	{ "ibs", fea::cpu_info.ibs() },
	{ "xop", fea::cpu_info.xop() },
	{ "skinit", fea::cpu_info.skinit() },
	{ "wdt", fea::cpu_info.wdt() },
	{ "lwp", fea::cpu_info.lwp() },
	{ "fma4", fea::cpu_info.fma4() },
	{ "tce", fea::cpu_info.tce() },
	{ "nodeid_msr", fea::cpu_info.nodeid_msr() },
	{ "tbm", fea::cpu_info.tbm() },
	{ "topoext", fea::cpu_info.topoext() },
	{ "perfctr_core", fea::cpu_info.perfctr_core() },
	{ "perfctr_nb", fea::cpu_info.perfctr_nb() },
	{ "dbx", fea::cpu_info.dbx() },
	{ "perftsc", fea::cpu_info.perftsc() },
	{ "pcx_l2i", fea::cpu_info.pcx_l2i() },
};

std::unordered_map<std::string, uint32_t> int_map{
	{ "family_id", fea::cpu_info.family_id() },
	{ "ext_family_id", fea::cpu_info.extended_family_id() },
	{ "model", fea::cpu_info.model() },
	{ "ext_model_id", fea::cpu_info.extended_model_id() },
	{ "processor_type", fea::cpu_info.processor_type() },
	{ "stepping_id", fea::cpu_info.stepping_id() },
	{ "mawau", fea::cpu_info.mawau() },
	{ "brand_index", fea::cpu_info.brand_index() },
	{ "clflush_line_size", fea::cpu_info.clflush_line_size() },
	{ "num_addressable_ids", fea::cpu_info.num_addressable_logical_ids() },
	{ "local_apic_id", fea::cpu_info.local_apic_id() },
};

TEST(cpu_info, basics) {
#if FEA_MACOS
	std::vector<std::string> cpu_tokens = get_macos_cpu_features();
	printf("Gathered cpuid flags:\n\t");
	for (const std::string& str : cpu_tokens) {
		printf("%s, ", str.c_str());
	}
	printf("\n");

	for (auto& s : cpu_tokens) {
		std::transform(s.begin(), s.end(), s.begin(), ::tolower);

		if (s == "em64t")
			continue;
		if (s == "tsci")
			continue;

		if (s == "rdwrfsgs") {
			s = "fsgsbase";
		} else if (s == "xd") {
			s = "nx";
		} else if (s == "tsctmr") {
			s = "tsc-deadline";
		} else if (s == "dscpl") {
			s = "ds-cpl";
		} else if (s == "tpr") {
			s = "xtpr";
		} else if (s == "mon") {
			s = "monitor";
		} else if (s == "sse4.1") {
			s = "sse41";
		} else if (s == "sse4.2") {
			s = "sse42";
		} else if (s == "avx1.0") {
			s = "avx";
		} else if (s == "rdrand") {
			s = "rdrnd";
		} else if (s == "lahf") {
			s = "lahf_lm";
		} else if (s == "ibrs") {
			s = "spec_ctrl";
		} else if (s == "vmm") {
			s = "hypervisor";
		} else if (s == "tsc_thread_offset") {
			s = "ia32_tsc_adjust";
		} else if (s == "l1df") {
			s = "l1d_flush";
		} else if (s == "mdclear") {
			s = "md_clear";
		} else if (s == "acapmsr") {
			s = "ia32_arch_capabilities";
		} else if (s == "1gbpage") {
			s = "pdpe1gb";
		} else if (s == "clfsopt") {
			s = "clflushopt";
		} else if (s == "lzcnt") {
			s = "abm";
		} else if (s == "prefetchw") {
			s = "3dnowprefetch";
		} else if (s == "ipt") {
			s = "intel_pt";
		}

		if (flag_map.count(s) == 0) {
			printf("Missing cpu feature : %s\n", s.c_str());
			// Can't guarantee this is an error, since unixes may support
			// hypervisor 0x40000000.
			// https://lkml.org/lkml/2008/10/1/246

			// EXPECT_TRUE(false);
		} else {
			EXPECT_TRUE(flag_map.at(s));
			flag_map.erase(s);
		}
	}

	flag_map.erase("lm"); // Might be em64t or tsci.
	for (const auto& x : flag_map) {
		if (x.second) {
			printf("Uncaught cpu option : %s\n", x.first.c_str());
		}
		EXPECT_FALSE(x.second);
	}

#else
	// How do you test this on windows?
#endif

	EXPECT_NE(fea::cpu_info.amd(), fea::cpu_info.intel());

	if (fea::cpu_info.intel()) {
		EXPECT_EQ(std::string{ fea::cpu_info.vendor() }, "GenuineIntel");
	} else if (fea::cpu_info.amd()) {
		EXPECT_EQ(std::string{ fea::cpu_info.vendor() }, "AuthenticAMD");
	}

	// fea::cpu_info.print_all();
}
} // namespace
#endif
