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
		if (s == "EM64T")
			continue;
		if (s == "TSCI")
			continue;

		if (s == "RDWRFSGS")
			s = "fsgsbase";
		if (s == "XD")
			s = "nx";
		if (s == "TSCTMR")
			s = "tsc-deadline";
		if (s == "DSCPL")
			s = "ds-cpl";
		if (s == "TPR")
			s = "xtpr";
		if (s == "MON")
			s = "monitor";
		if (s == "SSE4.1")
			s = "sse41";
		if (s == "SSE4.2")
			s = "sse42";
		if (s == "AVX1.0")
			s = "avx";
		if (s == "RDRAND")
			s = "rdrnd";
		if (s == "LAHF")
			s = "lahf_lm";

		std::transform(s.begin(), s.end(), s.begin(), ::tolower);
		//			printf("%s\n", s.c_str());
		EXPECT_TRUE(cpu_info_map.at(s));
		cpu_info_map.erase(s);
	}

	cpu_info_map.erase("lm"); // Might be em64t or tsci.
	for (const auto& x : cpu_info_map) {
		//			printf("%s\n", x.first.c_str());
		EXPECT_FALSE(x.second);
	}
#else
	// How do you test this on windows?
#endif

	EXPECT_NE(fea::cpu_info.amd(), fea::cpu_info.intel());
	// fea::cpu_info.print_all();
}
} // namespace
