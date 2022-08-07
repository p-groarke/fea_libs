#include <fea/memory/fmap.hpp>
#include <fea/utils/file.hpp>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <string_view>

extern const char* argv0;

namespace {
TEST(fmap, basics) {
	const std::filesystem::path exe_path = fea::executable_dir(argv0);
	const std::filesystem::path testfiles_dir = exe_path / "tests_data/";
	const std::filesystem::path in_filepath = testfiles_dir / L"fmap_test.txt";

	std::ifstream ifs(in_filepath);
	ASSERT_TRUE(ifs.is_open());
	size_t exp_fsize = fea::file_size(ifs);
	std::string exp_str(exp_fsize, '\0');
	ifs.read(exp_str.data(), exp_str.size());

	fea::ifmap ifm(in_filepath);
	EXPECT_NE(ifm.begin(), ifm.end());
	EXPECT_NE(ifm.rbegin(), ifm.rend());
	EXPECT_NE(ifm.data(), nullptr);
	EXPECT_TRUE(ifm.is_open());
	EXPECT_EQ(ifm.size(), exp_str.size());
	EXPECT_FALSE(ifm.empty());

	std::string_view s{ reinterpret_cast<const char*>(ifm.data()), ifm.size() };
	EXPECT_EQ(s, exp_str);

	{
		fea::ifmap ifm2(std::move(ifm));

		EXPECT_EQ(ifm.begin(), ifm.end());
		EXPECT_EQ(ifm.rbegin(), ifm.rend());
		EXPECT_EQ(ifm.data(), nullptr);
		EXPECT_FALSE(ifm.is_open());
		EXPECT_NE(ifm.size(), exp_str.size());
		EXPECT_TRUE(ifm.empty());

		EXPECT_NE(ifm2.begin(), ifm2.end());
		EXPECT_NE(ifm2.rbegin(), ifm2.rend());
		EXPECT_NE(ifm2.data(), nullptr);
		EXPECT_TRUE(ifm2.is_open());
		EXPECT_EQ(ifm2.size(), exp_str.size());
		EXPECT_FALSE(ifm2.empty());

		fea::ifmap ifm3;
		ifm3 = std::move(ifm2);

		EXPECT_EQ(ifm2.begin(), ifm2.end());
		EXPECT_EQ(ifm2.rbegin(), ifm2.rend());
		EXPECT_EQ(ifm2.data(), nullptr);
		EXPECT_FALSE(ifm2.is_open());
		EXPECT_NE(ifm2.size(), exp_str.size());
		EXPECT_TRUE(ifm2.empty());

		EXPECT_NE(ifm3.begin(), ifm3.end());
		EXPECT_NE(ifm3.rbegin(), ifm3.rend());
		EXPECT_NE(ifm3.data(), nullptr);
		EXPECT_TRUE(ifm3.is_open());
		EXPECT_EQ(ifm3.size(), exp_str.size());
		EXPECT_FALSE(ifm3.empty());
	}
}
} // namespace
