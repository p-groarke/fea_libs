#include "english_db.imp.hpp"
#include "english_dic.imp.hpp"
#include "iso_639_db.imp.hpp"

namespace fea {
namespace detail {
constexpr uint16_t compress_3char_code(std::string_view code) noexcept {
	assert(code.size() == 3);
	return uint16_t(code[2] - 'a') << 10 | uint16_t(code[1] - 'a') << 5
		 | uint16_t(code[0] - 'a');
}

constexpr uint16_t compress_2char_code(std::string_view code) noexcept {
	assert(code.size() == 2);
	return uint16_t(code[1] - 'a') << 5 | uint16_t(code[0] - 'a');
}
} // namespace detail
} // namespace fea

namespace fea {
bool iso_639_3_valid(std::string_view code) noexcept {
	if (code.size() != 3) {
		return false;
	}
	uint16_t idx
			= detail::iso_639_3_code_to_id[detail::compress_3char_code(code)];
	return (idx != (std::numeric_limits<uint16_t>::max)());
}

bool iso_639_2b_valid(std::string_view code) noexcept {
	if (code.size() != 3) {
		return false;
	}
	uint16_t idx
			= detail::iso_639_2b_code_to_id[detail::compress_3char_code(code)];
	return (idx != (std::numeric_limits<uint16_t>::max)());
}

bool iso_639_2t_valid(std::string_view code) noexcept {
	if (code.size() != 3) {
		return false;
	}
	uint16_t idx
			= detail::iso_639_2t_code_to_id[detail::compress_3char_code(code)];
	return (idx != (std::numeric_limits<uint16_t>::max)());
}

bool iso_639_1_valid(std::string_view code) noexcept {
	if (code.size() != 2) {
		return false;
	}
	uint16_t idx
			= detail::iso_639_1_code_to_id[detail::compress_2char_code(code)];
	return (idx != (std::numeric_limits<uint16_t>::max)());
}

const iso_639_lang& iso_639_3_lookup(std::string_view code) noexcept {
	assert(iso_639_3_valid(code));
	uint16_t idx
			= detail::iso_639_3_code_to_id[detail::compress_3char_code(code)];
	return detail::iso_639_languages[idx];
}

const iso_639_lang& iso_639_2b_lookup(std::string_view code) noexcept {
	assert(iso_639_2b_valid(code));
	uint16_t idx
			= detail::iso_639_2b_code_to_id[detail::compress_3char_code(code)];
	return detail::iso_639_languages[idx];
}

const iso_639_lang& iso_639_2t_lookup(std::string_view code) noexcept {
	assert(iso_639_2t_valid(code));
	uint16_t idx
			= detail::iso_639_2t_code_to_id[detail::compress_3char_code(code)];
	return detail::iso_639_languages[idx];
}

const iso_639_lang& iso_639_1_lookup(std::string_view code) noexcept {
	assert(iso_639_1_valid(code));
	uint16_t idx
			= detail::iso_639_1_code_to_id[detail::compress_2char_code(code)];
	return detail::iso_639_languages[idx];
}

namespace en {
constexpr std::span<const char> letters() noexcept {
	return std::span{ detail::letters.data(), detail::letters.size() };
}

constexpr double letter_frequency(char l) noexcept {
	return letter_frequency(uint8_t(l));
}

constexpr double letter_frequency(uint8_t l) noexcept {
	return detail::letter_frequencies[size_t(l)];
}

constexpr std::span<const std::string_view> dictionary() noexcept {
	return std::span{ detail::english_dic.data(), detail::english_dic.size() };
}

constexpr std::span<const std::string_view> bigrams() noexcept {
	return std::span{ detail::bigrams.data(), detail::bigrams.size() };
}

constexpr double bigram_frequency(std::string_view bigram) noexcept {
	assert(bigram.size() == 2u);
	assert(std::all_of(bigram.begin(), bigram.end(),
			[](char c) { return fea::is_lower_letter_ascii(c); }));

	size_t idx = detail::bigram_idx(bigram);
	assert(idx < detail::bigram_frequencies.size());
	return detail::bigram_frequencies[idx];
}
} // namespace en
} // namespace fea
