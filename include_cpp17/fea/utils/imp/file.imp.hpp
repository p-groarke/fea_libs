﻿namespace fea {
std::filesystem::path executable_dir(const char* argv0) {
#if FEA_WINDOWS
	return std::filesystem::absolute(argv0).remove_filename();
#else
	std::filesystem::path c_path = std::filesystem::current_path();
	std::string arg{ argv0 };
	if (starts_with(arg, ".")) {
		arg.erase(0, 1);
	}
	c_path += std::filesystem::path{ arg };
	if (c_path.string().find(".") != std::string::npos) {
		return c_path.remove_filename();
	}

	return c_path.parent_path();
#endif
}

std::filesystem::path executable_dir(const wchar_t* argv0) {
#if FEA_WINDOWS
	return std::filesystem::absolute(argv0).remove_filename();
#else
	std::filesystem::path c_path = std::filesystem::current_path();
	std::wstring arg{ argv0 };
	if (starts_with(arg, L".")) {
		arg.erase(0, 1);
	}
	c_path += std::filesystem::path{ arg };
	if (c_path.wstring().find(L".") != std::wstring::npos) {
		return c_path.remove_filename();
	}

	return c_path.parent_path();
#endif
}

std::FILE* fopen(const std::filesystem::path& path, const char* mode) {
	std::FILE* ret = nullptr;

#if FEA_WINDOWS
	if (fopen_s(&ret, path.string().c_str(), mode) != 0) {
		return nullptr;
	}
#else
	// nullptr if failed
	ret = std::fopen(path.string().c_str(), mode);
#endif

	return ret;
}

size_t file_size(std::FILE* ifs) {
	if (ifs == nullptr) {
		return 0;
	}
	std::fseek(ifs, 0, SEEK_END);
	size_t ret = size_t(std::ftell(ifs));
	std::rewind(ifs);
	return ret;
}

template <class IFStream>
void rewind(IFStream& fs) {
	fs.clear();
	fs.seekg(0, fs.beg);
}

template <class IFStream>
size_t file_size(IFStream& ifs) {
	if (!ifs.is_open()) {
		return 0;
	}

	ifs.seekg(0, ifs.end);
	size_t ret = size_t(ifs.tellg());
	fea::rewind(ifs);
	return ret;
}

template <class IFStream, class String, class Func>
bool basic_read_text_file(const std::filesystem::path& fpath, Func&& func) {
	IFStream ifs(fpath);
	if (!ifs.is_open()) {
		fprintf(stderr, "Couldn't open file : %s\n", fpath.string().c_str());
		return false;
	}

	String line{};
	while (std::getline(ifs, line)) {
		if (line.size() > 0 && line.back() == '\r') {
			line.pop_back();
		}
		func(std::move(line));
	}
	return true;
}

template <class Func>
bool read_text_file(const std::filesystem::path& fpath, Func&& func) {
	return basic_read_text_file<std::ifstream, std::string>(
			fpath, std::forward<Func>(func));
}

template <class Func>
bool wread_text_file(const std::filesystem::path& fpath, Func&& func) {
	return basic_read_text_file<std::wifstream, std::wstring>(
			fpath, std::forward<Func>(func));
}

template <class IFStream, class String, class UInt>
bool basic_open_text_file(
		const std::filesystem::path& fpath, std::vector<UInt>& out) {
	IFStream ifs(fpath, std::ios::ate);
	if (!ifs.is_open()) {
		fprintf(stderr, "Couldn't open file : %s\n", fpath.string().c_str());
		return false;
	}

	out = {};
	out.reserve(file_size(ifs));

	String line{};
	while (std::getline(ifs, line)) {
		if (line.size() > 0 && line.back() == '\r') {
			line.pop_back();
		}

		for (auto c : line) {
			out.push_back(static_cast<UInt>(c));
		}
	}
	return true;
}

bool open_text_file(
		const std::filesystem::path& fpath, std::vector<uint8_t>& out) {
	return basic_open_text_file<std::ifstream, std::string>(fpath, out);
}

bool wopen_text_file(
		const std::filesystem::path& fpath, std::vector<uint16_t>& out) {
	return basic_open_text_file<std::wifstream, std::wstring>(fpath, out);
}

template <class IFStream, class String, class UInt>
bool basic_open_text_file(const std::filesystem::path& fpath,
		std::vector<std::vector<UInt>>& out) {
	out = {};

	return basic_read_text_file<IFStream, String>(fpath, [&](String&& line) {
		out.push_back({});
		for (auto c : line) {
			out.back().push_back(static_cast<UInt>(c));
		}
	});
}

bool open_text_file(const std::filesystem::path& fpath,
		std::vector<std::vector<uint8_t>>& out) {
	return basic_open_text_file<std::ifstream, std::string>(fpath, out);
}

bool wopen_text_file(const std::filesystem::path& fpath,
		std::vector<std::vector<uint16_t>>& out) {
	return basic_open_text_file<std::wifstream, std::wstring>(fpath, out);
}

template <class IFStream, class UInt, class String>
bool basic_open_text_file(const std::filesystem::path& fpath, String& out) {
	std::vector<UInt> data;
	if (!basic_open_text_file<IFStream, String>(fpath, data)) {
		return false;
	}

	out = {};
	out.reserve(data.size());
	for (UInt u : data) {
		out.push_back(static_cast<typename String::value_type>(u));
	}
	return true;
}

bool open_text_file(const std::filesystem::path& fpath, std::string& out) {
	return basic_open_text_file<std::ifstream, uint8_t>(fpath, out);
}

bool wopen_text_file(const std::filesystem::path& fpath, std::wstring& out) {
	return basic_open_text_file<std::wifstream, uint16_t>(fpath, out);
}

template <class IFStream, class String, class Alloc,
		template <class, class> class Vec>
bool basic_open_text_file(
		const std::filesystem::path& fpath, Vec<String, Alloc>& out) {
	out = {};
	return basic_read_text_file<IFStream, String>(
			fpath, [&](String&& line) { out.push_back(line); });
}

bool open_text_file(
		const std::filesystem::path& fpath, std::vector<std::string>& out) {
	return basic_open_text_file<std::ifstream>(fpath, out);
}

bool wopen_text_file(
		const std::filesystem::path& fpath, std::vector<std::wstring>& out) {
	return basic_open_text_file<std::wifstream>(fpath, out);
}

template <class IFStream, class String>
bool open_text_file_raw(const std::filesystem::path& fpath, String& out) {
	IFStream ifs(fpath, std::ios::ate | std::ios::binary);
	if (!ifs.is_open()) {
		fprintf(stderr, "Couldn't open file : %s\n", fpath.string().c_str());
		return false;
	}

	out = String(file_size(ifs), '\0');
	ifs.read(out.data(), out.size());

	using c_t = typename String::value_type;
	size_t pos = out.find(c_t{}); // The real end is always screwed up.

	if (pos != String::npos) {
		out.resize(pos);
	}
	return true;
}

bool open_text_file_raw(const std::filesystem::path& fpath, std::string& out) {
	return open_text_file_raw<std::ifstream>(fpath, out);
}

bool wopen_text_file_raw(
		const std::filesystem::path& fpath, std::wstring& out) {
	return open_text_file_raw<std::wifstream>(fpath, out);
}

bool open_binary_file(
		const std::filesystem::path& f, std::vector<uint8_t>& out) {
	std::ifstream ifs{ f, std::ios::binary | std::ios::ate };
	if (!ifs.is_open()) {
		fprintf(stderr, "Couldn't open file '%s'\n", f.string().c_str());
		return false;
	}

	out = std::vector<uint8_t>(file_size(ifs));
	ifs.read(reinterpret_cast<char*>(out.data()), out.size());
	return true;
}

text_encoding detect_encoding(const std::string& str) {
	// 1. If a string doesn't contain nulls, its UTF-8
	if (str.find('\0') == std::string::npos) {
		return text_encoding::utf8;
	}

	// else
	// 2. If a string doesn't contain double nulls, it's UTF-16
	std::string double_null("\0\0", 2);
	if (str.find(double_null) == std::string::npos) {
		// 3. If the nulls are on odd numbered indices, it's UTF-16LE
		for (size_t i = 0; i < str.size(); ++i) {
			if (str[i] == '\0' && (i % 2) != 0) {
				return text_encoding::utf16le;
			}
		}

		// else
		// 4. The string defaults to UTF-16BE
		return text_encoding::utf16be;
	}

	// Files are not stored as utf32, so don't support that detection. At least
	// for now.
	return text_encoding::count;

	// else
	// 5. If the index modulo 4 is 0 and the character is greater than
	// 0x7F, the string is UTF-32LE. This is because the range of
	// UTF-32 only goes up to 0x7FFFFFFF, meaning approximately 22%
	// of the characters that can be represented will validate that
	// the string is not big endian; including a BOM.

	// else
	// 6. The string defaults to UTF-32BE
}

bool file_string_to_utf32(const std::string& input_str, text_encoding encoding,
		std::u32string& output_str) {

	switch (encoding) {
	case text_encoding::utf32be: {
		if (input_str.size() % 4 != 0) {
			return false;
		}

		size_t count = input_str.size() / 4;
		std::u32string temp(count, 0);

		for (size_t i = 0; i < count; ++i) {
			temp[i] = char32_t(
					input_str[i * 4 + 3] << 0 | input_str[i * 4 + 2] << 8
					| input_str[i * 4 + 1] << 16 | input_str[i * 4 + 0] << 24);
		}

		// Do this "dummy" conversion to test conversion was ok.
		try {
			std::string utf8 = utf32_to_utf8(temp);
			output_str = utf8_to_utf32(utf8);
		} catch (const std::range_error&) {
			output_str.clear();
			return false;
		}
	} break;
	case text_encoding::utf32le: {
		if (input_str.size() % 4 != 0) {
			return false;
		}

		size_t count = input_str.size() / 4;
		std::u32string temp(count, 0);
		for (size_t i = 0; i < count; ++i) {
			temp[i] = char32_t(
					input_str[i * 4 + 0] << 0 | input_str[i * 4 + 1] << 8
					| input_str[i * 4 + 2] << 16 | input_str[i * 4 + 3] << 24);
		}

		try {
			std::string utf8 = utf32_to_utf8(temp);
			output_str = utf8_to_utf32(utf8);
		} catch (const std::range_error&) {
			output_str.clear();
			return false;
		}
	} break;
	case text_encoding::utf16be: {
		if (input_str.size() % 2 != 0) {
			return false;
		}

		size_t count = input_str.size() / 2;
		std::u16string temp(count, 0);
		for (size_t i = 0; i < count; ++i) {
			temp[i] = char16_t(
					input_str[i * 2 + 1] << 0 | input_str[i * 2 + 0] << 8);
		}

		try {
			output_str = utf16_to_utf32(temp);
		} catch (const std::range_error&) {
			output_str.clear();
			return false;
		}
	} break;
	case text_encoding::utf16le: {
		if (input_str.size() % 2 != 0) {
			return false;
		}

		size_t count = input_str.size() / 2;
		std::u16string temp(count, 0);
		for (size_t i = 0; i < count; ++i) {
			temp[i] = char16_t(
					input_str[i * 2 + 0] << 0 | input_str[i * 2 + 1] << 8);
		}

		try {
			output_str = utf16_to_utf32(temp);
		} catch (const std::range_error&) {
			output_str.clear();
			return false;
		}
	} break;
	case text_encoding::utf8: {
		try {
			output_str = utf8_to_utf32(input_str);
		} catch (const std::range_error&) {
			output_str.clear();
			return false;
		}
	} break;
	default: {
		return false;
	} break;
	}

	return true;
}

std::u32string open_text_file_with_bom(std::ifstream& src) {
	if (!src.is_open()) {
		assert(false);
		return {};
	}

	// File BOMs
	const std::vector<std::string> boms = {
		std::string("\x00\x00\xFE\xFF", 4), // utf32be
		std::string("\xFF\xFE\x00\x00", 4), // utf32le
		std::string("\xFE\xFF", 2), // utf16be
		std::string("\xFF\xFE", 2), // utf16le
		std::string("\xEF\xBB\xBF", 3), // utf8
	};

	// Read file as chars.
	std::string buffer((std::istreambuf_iterator<char>(src)),
			std::istreambuf_iterator<char>());

	std::u32string ret;

	// Test the boms. If one is found, try to convert the file to utf32.
	for (size_t i = 0; i < boms.size(); ++i) {
		const std::string& bom = boms[i];
		if (buffer.compare(0, bom.size(), bom) != 0) {
			continue;
		}

		text_encoding enc = text_encoding(i);
		buffer = buffer.substr(bom.size());
		if (file_string_to_utf32(buffer, enc, ret)) {
			// Found bom, conversion was good, all gucci.
			return ret;
		}
		break;
	}
	// Oh no, BOM no found, try to figure it out.

	// First, try to detect utf8, utf16le and utf16be.
	{
		text_encoding enc = detect_encoding(buffer);

		if (enc != text_encoding::count) {
			if (file_string_to_utf32(buffer, enc, ret)) {
				// Yay.
				return ret;
			}
		}
	}

	// Brute force it
	for (size_t i = 0; i < size_t(text_encoding::count); ++i) {
		text_encoding enc = text_encoding(i);
		if (file_string_to_utf32(buffer, enc, ret)) {
			// There is a good chance the encoding is incorrect in a lexical
			// sense here, but programatically it is ok.
			return ret;
		}
	}

	// Evewything failed.
	fea::maybe_throw(__FUNCTION__, __LINE__,
			"Unsupported file encoding. Please use utf8, utf16 or utf32.");

	return ret;
}
} // namespace fea
