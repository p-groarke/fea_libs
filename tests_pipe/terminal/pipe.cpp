#include <fea/terminal/pipe.hpp>
#include <gtest/gtest.h>
#include <string>

extern int test_num;

namespace {
const std::string expected = "l1 🙂\nl2\n<>\né\n";
const std::wstring wexpected = L"l1 🙂\nl2\n<>\né\n";

// TEST(testing, test) {
//	HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
//	if (stdin_handle == INVALID_HANDLE_VALUE) {
//		fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
//	}
//
//	switch (GetFileType(stdin_handle)) {
//	case FILE_TYPE_CHAR: {
//		// Unsupported. Please send me a use-case / example.
//	} break;
//	case FILE_TYPE_DISK: {
//		LARGE_INTEGER byte_size;
//		byte_size.QuadPart = 0;
//		if (GetFileSizeEx(stdin_handle, &byte_size) == 0) {
//			fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
//		}
//
//		std::cout << "byte_size : " << byte_size.QuadPart << std::endl;
//
//		unsigned long read = 0;
//		std::vector<std::byte> buffer(size_t(byte_size.QuadPart));
//		// std::wstring str(size_t(byte_size.QuadPart) + 1, L'\0');
//		std::string str(size_t(byte_size.QuadPart) + 1, L'\0');
//
//		if (ReadFile(stdin_handle, str.data(), uint32_t(buffer.size()), &read,
//					nullptr)
//				== 0) {
//			fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
//		}
//		assert(read == buffer.size());
//
//		std::cout << str << std::endl;
//		std::wstring wstr = fea::utf8_to_utf16_w(str);
//		std::wcout << wstr << std::endl;
//
//	} break;
//	case FILE_TYPE_PIPE: {
//		unsigned long avail = 0;
//		if (PeekNamedPipe(stdin_handle, nullptr, 0, nullptr, &avail, nullptr)
//				== 0) {
//			fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
//		}
//		std::cout << "avail : " << avail << std::endl;
//
//	} break;
//	case FILE_TYPE_REMOTE: {
//		// Unsupported. Please send me a use-case / example.
//	} break;
//	default: {
//		// GetLastError returns NO_ERROR on valid unknown.
//		fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
//	} break;
//	}
//
//
//	// DWORD unread = 0;
//	// if (GetNumberOfConsoleInputEvents(stdin_handle, &unread) == 0) {
//	//	fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
//	// }
//	// std::cout << "unread : " << unread << std::endl;
//
//	// DWORD read = 0;
//	// std::vector<INPUT_RECORD> buf(unread);
//	// if (ReadConsoleInput(stdin_handle, buf.data(), uint32_t(buf.size()),
//	// &read)
//	//		== 0) {
//	//	fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
//	// }
//
//	// for (const INPUT_RECORD& r : buf) {
//	//	std::cout << r.EventType << std::endl;
//	// }
//
//	// std::cout << t << std::endl;
// }

TEST(pipe, basics) {
	switch (test_num) {
	case 0: {
		std::string str = fea::read_pipe_text();
		EXPECT_EQ(str, expected);
	} break;
	case 1: {
		std::wstring str = fea::wread_pipe_text();
		EXPECT_EQ(str, wexpected);
	} break;
	default: {
		assert(false);
	} break;
	}

	std::wcout << std::endl;
}
} // namespace
