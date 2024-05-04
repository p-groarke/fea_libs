#include <fea/utils/platform.hpp>

#if FEA_MACOS
#pragma once
#include <algorithm>
#include <cstdio>
#include <fea/performance/cpu_info.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

std::vector<std::string> get_macos_cpu_features() {
	std::vector<std::string> ret;
	std::string result;
	std::array<char, 128> buffer;

	{
		std::shared_ptr<FILE> pipe(
				popen("sysctl machdep.cpu.features", "r"), pclose);
		if (!pipe)
			throw std::runtime_error("popen() failed!");
		while (!feof(pipe.get())) {
			if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
				result += buffer.data();
		}
		result.replace(result.find("machdep.cpu.features: "), 22, "");
		result.replace(result.find("\n"), 1, " ");
	}
	{
		std::shared_ptr<FILE> pipe(
				popen("sysctl machdep.cpu.leaf7_features", "r"), pclose);
		if (!pipe)
			throw std::runtime_error("popen() failed!");
		while (!feof(pipe.get())) {
			if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
				result += buffer.data();
		}
		result.replace(result.find("machdep.cpu.leaf7_features: "), 28, "");
		result.replace(result.find("\n"), 1, " ");
	}
	{
		std::shared_ptr<FILE> pipe(
				popen("sysctl machdep.cpu.extfeatures", "r"), pclose);
		if (!pipe)
			throw std::runtime_error("popen() failed!");
		while (!feof(pipe.get())) {
			if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
				result += buffer.data();
		}
		result.replace(result.find("machdep.cpu.extfeatures: "), 25, "");
		result.replace(result.find("\n"), 1, "");
	}

	std::stringstream ss{ result };
	std::string s;
	while (getline(ss, s, ' ')) {
		ret.push_back(s);
	}
	return ret;
}

#endif
