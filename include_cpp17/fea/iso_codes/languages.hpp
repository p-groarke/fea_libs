#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace fea {
struct iso_language {
	std::string family;
	std::string name;
	std::string native_name;
	std::string iso_639_1;
	std::string iso_639_2t;
	std::string iso_639_2b;
	std::string iso_639_3;
};

namespace detail {
inline const std::vector<iso_language>& languages() {
	static const std::vector<iso_language> langs = {};
	return langs;
}
} // namespace detail
} // namespace fea
