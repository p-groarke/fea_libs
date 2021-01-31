#include <fea/utils/file.hpp>
#include <gtest/gtest.h>

extern const char* argv0;

namespace {
TEST(file, basics) {
	std::filesystem::path exe_path = fea::executable_dir(argv0);
	std::filesystem::path testfiles_dir = exe_path / "tests_data/";

	for (const std::filesystem::path& filepath :
			std::filesystem::directory_iterator(testfiles_dir)) {
		// printf("testing : %s\n", filepath.string().c_str());

		if (filepath.filename() != "text_file_crlf.txt"
				&& filepath.filename() != "text_file_lf.txt") {
			continue;
		}

		{
			size_t counter = 0;
			fea::read_text_file(filepath, [&](std::string&& line) {
				switch (counter) {
				case 0: {
					EXPECT_EQ(line, "Line1");
				} break;
				case 1: {
					EXPECT_EQ(line, "Line2");
				} break;
				case 2: {
					EXPECT_TRUE(line.empty());
					EXPECT_EQ(line, "");
				} break;
				case 3: {
					EXPECT_EQ(line, "Line4");
				} break;
				default: {
				} break;
				}

				++counter;
			});

			EXPECT_EQ(counter, 4u);
		}

		{
			size_t counter = 0;
			fea::wread_text_file(filepath, [&](std::wstring&& line) {
				switch (counter) {
				case 0: {
					EXPECT_EQ(line, L"Line1");
				} break;
				case 1: {
					EXPECT_EQ(line, L"Line2");
				} break;
				case 2: {
					EXPECT_TRUE(line.empty());
					EXPECT_EQ(line, L"");
				} break;
				case 3: {
					EXPECT_EQ(line, L"Line4");
				} break;
				default: {
				} break;
				}

				++counter;
			});

			EXPECT_EQ(counter, 4u);
		}

		{
			std::vector<uint8_t> text;
			fea::open_text_file(filepath, text);
			EXPECT_FALSE(text.empty());

			const char* testagainst = "Line1Line2Line4";
			std::vector<uint8_t> tester{ testagainst,
				testagainst + strlen(testagainst) };
			EXPECT_EQ(text.size(), tester.size());
			EXPECT_EQ(text, tester);
		}

		{
			std::vector<uint16_t> text;
			fea::wopen_text_file(filepath, text);
			EXPECT_FALSE(text.empty());

			const wchar_t* testagainst = L"Line1Line2Line4";
			std::vector<uint16_t> tester{ testagainst,
				testagainst + std::wcslen(testagainst) };
			EXPECT_EQ(text.size(), tester.size());
			EXPECT_EQ(text, tester);
		}

		{
			std::vector<std::vector<uint8_t>> lines;
			fea::open_text_file(filepath, lines);
			EXPECT_FALSE(lines.empty());

			std::vector<std::vector<uint8_t>> tester;

			const char* line1 = "Line1";
			const char* line2 = "Line2";
			const char* line4 = "Line4";
			tester.push_back({ line1, line1 + strlen(line1) });
			tester.push_back({ line2, line2 + strlen(line2) });
			tester.push_back({});
			tester.push_back({ line4, line4 + strlen(line4) });

			ASSERT_EQ(lines.size(), tester.size());
			for (size_t i = 0; i < lines.size(); ++i) {
				EXPECT_EQ(lines[i].size(), tester[i].size());
				EXPECT_EQ(lines[i], tester[i]);
			}
		}

		{
			std::vector<std::vector<uint16_t>> lines;
			fea::wopen_text_file(filepath, lines);
			EXPECT_FALSE(lines.empty());

			std::vector<std::vector<uint16_t>> tester;

			const wchar_t* line1 = L"Line1";
			const wchar_t* line2 = L"Line2";
			const wchar_t* line4 = L"Line4";
			tester.push_back({ line1, line1 + std::wcslen(line1) });
			tester.push_back({ line2, line2 + std::wcslen(line2) });
			tester.push_back({});
			tester.push_back({ line4, line4 + std::wcslen(line4) });

			ASSERT_EQ(lines.size(), tester.size());
			for (size_t i = 0; i < lines.size(); ++i) {
				EXPECT_EQ(lines[i].size(), tester[i].size());
				EXPECT_EQ(lines[i], tester[i]);
			}
		}

		{
			std::string text;
			fea::open_text_file(filepath, text);
			EXPECT_FALSE(text.empty());

			std::string tester = "Line1Line2Line4";
			EXPECT_EQ(text.size(), tester.size());
			EXPECT_EQ(text, tester);
		}
		{
			std::wstring text;
			fea::wopen_text_file(filepath, text);
			EXPECT_FALSE(text.empty());

			std::wstring tester = L"Line1Line2Line4";
			EXPECT_EQ(text.size(), tester.size());
			EXPECT_EQ(text, tester);
		}

		{
			std::vector<std::string> lines;
			fea::open_text_file(filepath, lines);
			EXPECT_FALSE(lines.empty());

			std::vector<std::string> tester;

			tester.push_back("Line1");
			tester.push_back("Line2");
			tester.push_back("");
			tester.push_back("Line4");

			ASSERT_EQ(lines.size(), tester.size());
			for (size_t i = 0; i < lines.size(); ++i) {
				EXPECT_EQ(lines[i].size(), tester[i].size());
				EXPECT_EQ(lines[i], tester[i]);
			}
		}
		{
			std::vector<std::wstring> lines;
			fea::wopen_text_file(filepath, lines);
			EXPECT_FALSE(lines.empty());

			std::vector<std::wstring> tester;

			tester.push_back(L"Line1");
			tester.push_back(L"Line2");
			tester.push_back(L"");
			tester.push_back(L"Line4");

			ASSERT_EQ(lines.size(), tester.size());
			for (size_t i = 0; i < lines.size(); ++i) {
				EXPECT_EQ(lines[i].size(), tester[i].size());
				EXPECT_EQ(lines[i], tester[i]);
			}
		}

		{
			std::string text;
			fea::open_text_file_raw(filepath, text);
			EXPECT_FALSE(text.empty());

			std::string tester = "Line1\nLine2\n\nLine4";
			if (filepath.string().find("crlf") != std::string::npos) {
				tester = "Line1\r\nLine2\r\n\r\nLine4";
			}
			EXPECT_EQ(text.size(), tester.size());
			EXPECT_EQ(text, tester);
		}
		{
			std::wstring text;
			fea::wopen_text_file_raw(filepath, text);
			EXPECT_FALSE(text.empty());

			std::wstring tester = L"Line1\nLine2\n\nLine4";
			if (filepath.string().find("crlf") != std::string::npos) {
				tester = L"Line1\r\nLine2\r\n\r\nLine4";
			}
			EXPECT_EQ(text.size(), tester.size());
			EXPECT_EQ(text, tester);
		}

		{
			const char* testagainst = "Line1\nLine2\n\nLine4";
			if (filepath.string().find("crlf") != std::string::npos) {
				testagainst = "Line1\r\nLine2\r\n\r\nLine4";
			}

			std::vector<uint8_t> bytes;
			fea::open_binary_file(filepath, bytes);
			EXPECT_FALSE(bytes.empty());

			std::vector<uint8_t> tester{ testagainst,
				testagainst + strlen(testagainst) };
			EXPECT_EQ(bytes.size(), tester.size());
			EXPECT_EQ(bytes, tester);
		}
	}
}
} // namespace
