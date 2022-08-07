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

	std::string exp_str;
	fea::open_text_file(in_filepath, exp_str);

	// Invalid files
	{
		EXPECT_TRUE(fea::ifmap{ testfiles_dir / "asldfkj.txt" }.empty());
		EXPECT_FALSE(fea::ifmap{ testfiles_dir / "asldfkj.txt" }.is_open());

		fea::ifmap ifm;
		ifm.open(testfiles_dir / "asldfkj.txt");
		EXPECT_TRUE(ifm.empty());
		EXPECT_FALSE(ifm.is_open());
	}

	// Read
	{
		fea::ifmap ifm(in_filepath);
		EXPECT_NE(ifm.begin(), ifm.end());
		EXPECT_NE(ifm.rbegin(), ifm.rend());
		EXPECT_NE(ifm.data(), nullptr);
		EXPECT_TRUE(ifm.is_open());
		EXPECT_EQ(ifm.size(), exp_str.size());
		EXPECT_FALSE(ifm.empty());

		std::string_view s{ reinterpret_cast<const char*>(ifm.data()),
			ifm.size() };
		EXPECT_EQ(s, exp_str);

		{
			fea::span<const char> sp = fea::to_span<char>(ifm);
			EXPECT_EQ(sp.size(), s.size());
			std::string cmp(sp.data(), sp.size());
			EXPECT_EQ(exp_str, cmp);

			fea::to_span(ifm, sp);
			EXPECT_EQ(sp.size(), s.size());
			cmp = { sp.data(), sp.size() };
			EXPECT_EQ(exp_str, cmp);

			EXPECT_NO_THROW(fea::to_sv(ifm));

#if FEA_DEBUG || defined(FEA_NOTHROW)
			EXPECT_DEATH(fea::to_span<short>(ifm), "");
			EXPECT_DEATH(fea::to_span<int>(ifm), "");
			EXPECT_DEATH(fea::to_span<int64_t>(ifm), "");
			EXPECT_DEATH(fea::to_wsv(ifm), "");
#else
			EXPECT_THROW(fea::to_span<short>(ifm), std::invalid_argument);
			EXPECT_THROW(fea::to_span<int>(ifm), std::invalid_argument);
			EXPECT_THROW(fea::to_span<int64_t>(ifm), std::invalid_argument);
			EXPECT_THROW(fea::to_wsv(ifm), std::invalid_argument);
#endif
		}

		{
			fea::ifmap ifm2(std::move(ifm));

			// closed
			EXPECT_EQ(ifm.begin(), ifm.end());
			EXPECT_EQ(ifm.rbegin(), ifm.rend());
			EXPECT_EQ(ifm.data(), nullptr);
			EXPECT_FALSE(ifm.is_open());
			EXPECT_NE(ifm.size(), exp_str.size());
			EXPECT_TRUE(ifm.empty());

			// open
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

			ifm = std::move(ifm3);
		}

		ifm.close();
		EXPECT_EQ(ifm.begin(), ifm.end());
		EXPECT_EQ(ifm.rbegin(), ifm.rend());
		EXPECT_EQ(ifm.data(), nullptr);
		EXPECT_FALSE(ifm.is_open());
		EXPECT_NE(ifm.size(), exp_str.size());
		EXPECT_TRUE(ifm.empty());

		ifm.open(in_filepath);
		EXPECT_NE(ifm.begin(), ifm.end());
		EXPECT_NE(ifm.rbegin(), ifm.rend());
		EXPECT_NE(ifm.data(), nullptr);
		EXPECT_TRUE(ifm.is_open());
		EXPECT_EQ(ifm.size(), exp_str.size());
		EXPECT_FALSE(ifm.empty());
	}


	// Write
	{
		fea::ofmap ofm(in_filepath);
		EXPECT_NE(ofm.begin(), ofm.end());
		EXPECT_NE(ofm.rbegin(), ofm.rend());
		EXPECT_NE(ofm.data(), nullptr);
		EXPECT_TRUE(ofm.is_open());
		EXPECT_EQ(ofm.size(), exp_str.size());
		EXPECT_FALSE(ofm.empty());

		std::string_view s{ reinterpret_cast<const char*>(ofm.data()),
			ofm.size() };
		EXPECT_EQ(s, exp_str);

		{
			fea::span<const char> sp = fea::to_span<char>(ofm);
			EXPECT_EQ(sp.size(), s.size());
			std::string cmp(sp.data(), sp.size());
			EXPECT_EQ(exp_str, cmp);

			fea::to_span(ofm, sp);
			EXPECT_EQ(sp.size(), s.size());
			cmp = { sp.data(), sp.size() };
			EXPECT_EQ(exp_str, cmp);

			EXPECT_NO_THROW(fea::to_sv(ofm));

#if FEA_DEBUG || defined(FEA_NOTHROW)
			EXPECT_DEATH(fea::to_span<short>(ofm), "");
			EXPECT_DEATH(fea::to_span<int>(ofm), "");
			EXPECT_DEATH(fea::to_span<int64_t>(ofm), "");
			EXPECT_DEATH(fea::to_wsv(ofm), "");
#else
			EXPECT_THROW(fea::to_span<short>(ofm), std::invalid_argument);
			EXPECT_THROW(fea::to_span<int>(ofm), std::invalid_argument);
			EXPECT_THROW(fea::to_span<int64_t>(ofm), std::invalid_argument);
			EXPECT_THROW(fea::to_wsv(ofm), std::invalid_argument);
#endif
		}

		{
			fea::ofmap ofm2(std::move(ofm));

			// closed
			EXPECT_EQ(ofm.begin(), ofm.end());
			EXPECT_EQ(ofm.rbegin(), ofm.rend());
			EXPECT_EQ(ofm.data(), nullptr);
			EXPECT_FALSE(ofm.is_open());
			EXPECT_NE(ofm.size(), exp_str.size());
			EXPECT_TRUE(ofm.empty());

			// open
			EXPECT_NE(ofm2.begin(), ofm2.end());
			EXPECT_NE(ofm2.rbegin(), ofm2.rend());
			EXPECT_NE(ofm2.data(), nullptr);
			EXPECT_TRUE(ofm2.is_open());
			EXPECT_EQ(ofm2.size(), exp_str.size());
			EXPECT_FALSE(ofm2.empty());

			fea::ofmap ofm3;
			ofm3 = std::move(ofm2);

			EXPECT_EQ(ofm2.begin(), ofm2.end());
			EXPECT_EQ(ofm2.rbegin(), ofm2.rend());
			EXPECT_EQ(ofm2.data(), nullptr);
			EXPECT_FALSE(ofm2.is_open());
			EXPECT_NE(ofm2.size(), exp_str.size());
			EXPECT_TRUE(ofm2.empty());

			EXPECT_NE(ofm3.begin(), ofm3.end());
			EXPECT_NE(ofm3.rbegin(), ofm3.rend());
			EXPECT_NE(ofm3.data(), nullptr);
			EXPECT_TRUE(ofm3.is_open());
			EXPECT_EQ(ofm3.size(), exp_str.size());
			EXPECT_FALSE(ofm3.empty());

			ofm = std::move(ofm3);
		}

		ofm.close();
		EXPECT_EQ(ofm.begin(), ofm.end());
		EXPECT_EQ(ofm.rbegin(), ofm.rend());
		EXPECT_EQ(ofm.data(), nullptr);
		EXPECT_FALSE(ofm.is_open());
		EXPECT_NE(ofm.size(), exp_str.size());
		EXPECT_TRUE(ofm.empty());

		ofm.open(in_filepath);
		EXPECT_NE(ofm.begin(), ofm.end());
		EXPECT_NE(ofm.rbegin(), ofm.rend());
		EXPECT_NE(ofm.data(), nullptr);
		EXPECT_TRUE(ofm.is_open());
		EXPECT_EQ(ofm.size(), exp_str.size());
		EXPECT_FALSE(ofm.empty());
	}
}

TEST(fmap, read_write) {
	const std::filesystem::path exe_path = fea::executable_dir(argv0);
	const std::filesystem::path testfiles_dir = exe_path / "tests_data/";
	const std::filesystem::path in_filepath = testfiles_dir / L"fmap_test.txt";

	std::string exp_str;
	fea::open_text_file(in_filepath, exp_str);

	{
		fea::ofmap ofm{ in_filepath };
		ASSERT_EQ(ofm.size(), exp_str.size());
		for (size_t i = 0; i < exp_str.size(); ++i) {
			char cmp = exp_str[i];
			++cmp;

			char& c = *reinterpret_cast<char*>(&ofm[i]);
			++c;
			EXPECT_EQ(c, cmp);
			EXPECT_EQ(char(ofm[i]), cmp);
		}
	}
	{
		fea::ifmap ifm{ in_filepath };
		std::string_view sv = fea::to_sv(ifm);
		for (size_t i = 0; i < exp_str.size(); ++i) {
			char cmp = exp_str[i];
			++cmp;
			EXPECT_EQ(char(ifm[i]), cmp);
			EXPECT_EQ(sv[i], cmp);
		}
	}
	{
		std::ifstream ifs{ in_filepath };
		size_t size = std::filesystem::file_size(in_filepath);
		std::string data(size, '\0');
		ifs.read(data.data(), size);
		for (size_t i = 0; i < exp_str.size(); ++i) {
			char cmp = exp_str[i];
			++cmp;
			EXPECT_EQ(data[i], cmp);
		}
	}
	{
		// writing through span
		fea::ofmap ofm{ in_filepath };
		fea::span<char> s = fea::to_span<char>(ofm);
		for (char& c : s) {
			--c;
		}

		for (size_t i = 0; i < exp_str.size(); ++i) {
			char cmp = exp_str[i];
			EXPECT_EQ(s[i], cmp);
		}
	}
}
} // namespace
