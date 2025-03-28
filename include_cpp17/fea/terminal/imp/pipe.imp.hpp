#pragma once

// The standard doesn't provide codecvt equivalents. Use the old
// functionality until they do.
#if FEA_MSVC
#pragma warning(push)
#pragma warning(disable : 4996)
#elif FEA_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace fea {
namespace detail {
template <class CinT, class StringT>
void read_pipe_text(CinT& mcin, StringT& out) {
	auto e = fea::make_on_exit([&]() {
		// Clear pipe bits.
		mcin.clear();
	});

	size_t cin_count = fea::available_pipe_bytes();
	if (cin_count == 0) {
		return;
	}

	out.reserve((cin_count / sizeof(typename StringT::value_type)) + 1);
	StringT line;
	while (std::getline(mcin, line)) {
		out.insert(out.end(), line.begin(), line.end());
		out += L'\n';
	}
}
} // namespace detail

void fast_iostreams() {
	std::cin.sync_with_stdio(false);
	std::cout.sync_with_stdio(false);
	std::cerr.sync_with_stdio(false);
	std::clog.sync_with_stdio(false);
	std::wcin.sync_with_stdio(false);
	std::wcout.sync_with_stdio(false);
	std::wcerr.sync_with_stdio(false);
	std::wclog.sync_with_stdio(false);
}

size_t available_pipe_bytes() {
	size_t ret = 0;

#if FEA_WINDOWS
	HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
	if (stdin_handle == INVALID_HANDLE_VALUE) {
		fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
	}

	switch (GetFileType(stdin_handle)) {
	case FILE_TYPE_CHAR: {
		// Unsupported. Please send me a use-case / example.
	} break;
	case FILE_TYPE_DISK: {
		LARGE_INTEGER byte_size;
#if FEA_32BIT
		byte_size.LowPart = 0;
#else
		byte_size.QuadPart = 0;
#endif
		if (GetFileSizeEx(stdin_handle, &byte_size) == 0) {
			fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
		}
#if FEA_32BIT
		ret = size_t(byte_size.LowPart);
#else
		ret = size_t(byte_size.QuadPart);
#endif
	} break;
	case FILE_TYPE_PIPE: {
		// For some obscure reason, we need to readfile first.
		char buf[] = { 0 };
		if (ReadFile(stdin_handle, &buf, 0, nullptr, nullptr) == 0) {
			SetLastError(0);
			return 0;
		}

		// Now peek size.
		unsigned long avail = 0;
		if (PeekNamedPipe(stdin_handle, nullptr, 0, nullptr, &avail, nullptr)
				== 0) {
			fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
		}
		ret = size_t(avail);
	} break;
	case FILE_TYPE_REMOTE: {
		// Unsupported. Please send me a use-case / example.
	} break;
	default: {
		// GetLastError returns NO_ERROR on valid unknown.
		fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
	} break;
	}
#else
	int n = 0;
	if (ioctl(fileno(stdin), FIONREAD, &n) != 0) {
		fea::maybe_throw_on_errno(__FUNCTION__, __LINE__);
	}
	ret = size_t(n);
#endif

	return ret;
}

std::wstring wread_pipe_text() {
	std::wstring ret;

#if FEA_WINDOWS
	// To fix pipe input, use U8TEXT (and not U16).
	fea::translation_resetter tr
			= fea::translate_io(fea::translation_mode::u8text);
	fea::unused(tr);

	detail::read_pipe_text(std::wcin, ret);
#else
	// wcin is borked, use cin and convert to wstring (utf32).
	std::string temp;
	detail::read_pipe_text(std::cin, temp);
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
	ret = convert.from_bytes(temp);
#endif
	return ret;
}

std::string read_pipe_text() {
	std::string ret;
	detail::read_pipe_text(std::cin, ret);
	return ret;
}
} // namespace fea

#if FEA_MSVC
#pragma warning(pop)
#elif FEA_CLANG
#pragma clang diagnostic pop
#endif