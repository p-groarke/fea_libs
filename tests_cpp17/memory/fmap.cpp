#include <fea/memory/fmap.hpp>
#include <fea/utils/file.hpp>
#include <gtest/gtest.h>

extern const char* argv0;

namespace {
TEST(fmap, basics) {
	std::filesystem::path exe_path = fea::executable_dir(argv0);
	std::filesystem::path testfiles_dir = exe_path / "tests_data/";

	fea::ifmap ifmap(testfiles_dir / L"fmap_test.txt");
	std::span<const char> s{ reinterpret_cast<const char*>(ifmap.data()),
		ifmap.size() };
}
} // namespace
