#include <fea/utils/cpu_info.hpp>
#include <gtest/gtest.h>

#if FEA_MACOS
#include "cpu_info_macos_tests.hpp"
#else
#endif

namespace {
TEST(cpu_info, basics) {
#if FEA_MACOS
	std::vector<std::string> cpu_tokens = get_macos_cpu_features();
	for (auto& s : cpu_tokens) {
		std::transform(s.begin(), s.end(), s.begin(), ::tolower);

		if (s == "em64t")
			continue;
		if (s == "tsci")
			continue;

		if (s == "rdwrfsgs")
			s = "fsgsbase";
		if (s == "xd")
			s = "nx";
		if (s == "tsctmr")
			s = "tsc-deadline";
		if (s == "dscpl")
			s = "ds-cpl";
		if (s == "tpr")
			s = "xtpr";
		if (s == "mon")
			s = "monitor";
		if (s == "sse4.1")
			s = "sse41";
		if (s == "sse4.2")
			s = "sse42";
		if (s == "avx1.0")
			s = "avx";
		if (s == "rdrand")
			s = "rdrnd";
		if (s == "lahf")
			s = "lahf_lm";
		if (s == "lahf")
			s = "lahf_lm";
		if (s == "ibrs")
			s = "spec_ctrl";
		if (s == "vmm")
			s = "hypervisor";
		if (s == "tsc_thread_offset")
			s = "ia32_tsc_adjust";

		if (macos_cpu_info_map.count(s) == 0) {
			printf("Missing cpu feature : %s\n", s.c_str());
			// EXPECT_TRUE(false);
		} else {
			EXPECT_TRUE(macos_cpu_info_map.at(s));
			macos_cpu_info_map.erase(s);
		}
	}

	macos_cpu_info_map.erase("lm"); // Might be em64t or tsci.
	for (const auto& x : macos_cpu_info_map) {
		if (x.second) {
			printf("Uncaught cpu option : %s\n", x.first.c_str());
		}
		EXPECT_FALSE(x.second);
	}
#else
	// How do you test this on windows?
#endif

	EXPECT_NE(fea::cpu_info.amd(), fea::cpu_info.intel());
	// fea::cpu_info.print_all();
}
} // namespace
