#include <fea/encoding/base64.hpp>
#include <gtest/gtest.h>

#include <array>
#include <bit>
#include <cstring>
#include <fea/containers/stack_vector.hpp>
#include <iterator>
#include <type_traits>

namespace fea {

// Encodes the data pointed at by the input iterators into a base64 string.
// Outputs chars to the output iterator.
// template <std::forward_iterator FwdIt, class OutIt>
//	requires std::output_iterator<OutIt, char>
template <std::forward_iterator FwdIt, std::output_iterator<char> OutIt>
void encode_base64(FwdIt first, FwdIt last, OutIt out) {
	out;

	using value_t = typename std::iterator_traits<FwdIt>::value_type;
	static_assert(std::is_trivially_copyable_v<value_t>,
			"Iterated on value must be trivially copyable, it will be cast to "
			"bytes.");

	// constexpr size_t value_bytes = sizeof(value_t);

	//// Stage the input bytes, 3 of them.
	//// When the staging is full, encode it and flush to output staging.
	// fea::stack_vector<uint8_t, 3> input_staging{};

	// Stage the encoding in 4 bytes.
	// When the staging is full, flush to output and clear.
	fea::stack_vector<char, 4> output_staging{};

	auto flush = []() {};

	for (; first != last; ++first) {
		// Convert to bytes.
		auto value = *first;
		uint8_t bytes[sizeof(value)];
		std::memcpy(bytes, &value, sizeof(value));

		//= std::bit_cast<uint8_t[value_bytes]>(*first);

		for (size_t i = 0; i < sizeof(value); ++i) {
			// For each byte, mask the
			char c = detail::base64_lut[bytes[i] & 0b0011'1111];
			output_staging.push_back(c);

			if (output_staging.size() == 4) {
				// flush and clear
			}
		}
	}

	// std::vector<uint8_t> ret;
	// for (size_t i = 0; i < bytes.size(); i += 3) {
	//	uint8_t one = bytes[i];
	//	int shift_min = 0;

	//	uint8_t two, three;
	//	if (i + 1 >= bytes.size()) {
	//		two = 0;
	//		three = 0;
	//		shift_min = 12;
	//	} else if (i + 2 >= bytes.size()) {
	//		two = bytes[i + 1];
	//		three = 0;
	//		shift_min = 6;
	//	} else {
	//		two = bytes[i + 1];
	//		three = bytes[i + 2];
	//	}

	//	unsigned val24 = (one << 16) | (two << 8) | three;
	//	for (int shift = 18; shift >= 0; shift -= 6) {
	//		if (shift < shift_min) {
	//			*out = '=';
	//			++out;
	//		} else {
	//			unsigned ans = val24 >> shift;
	//			ans &= 0b0011'1111;
	//			*out = detail::base64_lut[ans];
	//			++out;
	//		}
	//	}
	//}
	// return ret;
}

template <std::forward_iterator FwdIt, std::output_iterator<char> OutIt>
void decode_base64(FwdIt first, FwdIt last, OutIt out) {
	first;
	last;
	out;
}
} // namespace fea

namespace {
TEST(base64, basics) {
	{
		static_assert(
				std::output_iterator<std::back_insert_iterator<std::string>,
						char>);

		const std::string str = "Many hands make light work.";
		std::string enc;
		fea::encode_base64(str.begin(), str.end(), std::back_inserter(enc));
		EXPECT_EQ(enc, "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu");

		std::string dec;
		fea::decode_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		EXPECT_EQ(dec, str);
	}

	{
		const std::string str = "Sun";
		std::string enc;
		fea::encode_base64(str.begin(), str.end(), std::back_inserter(enc));
		EXPECT_EQ(enc, "U3Vu");

		std::string dec;
		fea::decode_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		EXPECT_EQ(dec, str);
	}

	{
		const std::string str = "S";
		std::string enc;
		fea::encode_base64(str.begin(), str.end(), std::back_inserter(enc));
		EXPECT_EQ(enc, "U3Vu");

		std::string dec;
		fea::decode_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		EXPECT_EQ(dec, str);
	}

	{
		const std::vector<char> str{ 'S' };
		std::string enc;
		fea::encode_base64(str.begin(), str.end(), std::back_inserter(enc));
		EXPECT_EQ(enc, "Uw==");

		std::vector<char> dec;
		fea::decode_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		EXPECT_EQ(dec, str);
	}

	{
		const std::vector<uint8_t> str{ 'S', 'u' };
		std::string enc;
		fea::encode_base64(str.begin(), str.end(), std::back_inserter(enc));
		EXPECT_EQ(enc, "U3U=");

		std::vector<uint8_t> dec;
		fea::decode_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		EXPECT_EQ(dec, str);
	}
}

TEST(base64, crypto) {
	// auto hex = ding::stob(
	//			"49276d206b696c6c696e6720796f757220627261696e206c696b65206"
	//			"120706f69736f6e6f7573206d757368726f6f6d");
	//	std::vector<uint8_t> bytes = ding::decode_hex(hex);
	//
	//	EXPECT_EQ(ding::btos(bytes),
	//			"I'm killing your brain like a poisonous mushroom");
	//	EXPECT_EQ(ding::btos(ding::decode_hex(hex)),
	//			"I'm killing your brain like a poisonous mushroom");
	//
	//	printf("%s\n", ding::btos(bytes).c_str());
	//	std::string base64 = ding::btos(ding::encode_base64(bytes));
	//	EXPECT_EQ(base64,
	//			"SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t");
	//
	//	hex = ding::stob("0x034092ccfff0228f73b62a14");
	//	bytes = ding::decode_hex(hex);
	//	base64 = ding::btos(ding::encode_base64(bytes));
	//	EXPECT_EQ(base64, "A0CSzP/wIo9ztioU");
	//
	//	hex = ding::stob("0x34092ccfff0228f73b62a14");
	//	bytes = ding::decode_hex(hex);
	//	base64 = ding::btos(ding::encode_base64(bytes));
	//	EXPECT_EQ(base64, "A0CSzP/wIo9ztioU");
	//
	//	hex = ding::stob("0X34092ccfff0228f73b62a14");
	//	bytes = ding::decode_hex(hex);
	//	base64 = ding::btos(ding::encode_base64(bytes));
	//	EXPECT_EQ(base64, "A0CSzP/wIo9ztioU");
	//
	//	hex = ding::stob("34092ccfff0228f73b62a14");
	//	bytes = ding::decode_hex(hex);
	//	base64 = ding::btos(ding::encode_base64(bytes));
	//	EXPECT_EQ(base64, "A0CSzP/wIo9ztioU");
	//
	//	EXPECT_EQ(ding::btos(ding::encode_base64(ding::stob("te"))), "dGU=");
	//	EXPECT_EQ(ding::btos(ding::encode_base64(ding::stob("t"))), "dA==");
	//	EXPECT_EQ(
	//			ding::btos(ding::encode_base64(ding::stob("any carnal
	// pleasure."))), 			"YW55IGNhcm5hbCBwbGVhc3VyZS4="); 	EXPECT_EQ(
	//			ding::btos(ding::encode_base64(ding::stob("any carnal
	// pleasure"))), 			"YW55IGNhcm5hbCBwbGVhc3VyZQ==");
	//	EXPECT_EQ(ding::btos(ding::encode_base64(ding::stob("any carnal
	// pleasur"))), 			"YW55IGNhcm5hbCBwbGVhc3Vy");
	//	EXPECT_EQ(ding::btos(ding::encode_base64(ding::stob("any carnal
	// pleasu"))), 			"YW55IGNhcm5hbCBwbGVhc3U=");
	//	EXPECT_EQ(ding::btos(ding::encode_base64(ding::stob("any carnal
	// pleas"))), 			"YW55IGNhcm5hbCBwbGVhcw==");
	//
	//	EXPECT_EQ(ding::btos(ding::decode_base64(ding::stob("dGU="))), "te");
	//	EXPECT_EQ(ding::btos(ding::decode_base64(ding::stob("dA=="))), "t");
	//	EXPECT_EQ(ding::btos(ding::decode_base64(
	//					  ding::stob("YW55IGNhcm5hbCBwbGVhc3VyZS4="))),
	//			"any carnal pleasure.");
	//	EXPECT_EQ(ding::btos(ding::decode_base64(
	//					  ding::stob("YW55IGNhcm5hbCBwbGVhc3VyZQ=="))),
	//			"any carnal pleasure");
	//	EXPECT_EQ(ding::btos(ding::decode_base64(
	//					  ding::stob("YW55IGNhcm5hbCBwbGVhc3Vy"))),
	//			"any carnal pleasur");
	//	EXPECT_EQ(ding::btos(ding::decode_base64(
	//					  ding::stob("YW55IGNhcm5hbCBwbGVhc3U="))),
	//			"any carnal pleasu");
	//	EXPECT_EQ(ding::btos(ding::decode_base64(
	//					  ding::stob("YW55IGNhcm5hbCBwbGVhcw=="))),
	//			"any carnal pleas");
	//
	//	EXPECT_EQ(ding::btos(ding::decode_base64(ding::stob("dGU"))), "te");
	//	EXPECT_EQ(ding::btos(ding::decode_base64(ding::stob("dA"))), "t");
	//	EXPECT_EQ(ding::btos(ding::decode_base64(
	//					  ding::stob("YW55IGNhcm5hbCBwbGVhc3VyZS4"))),
	//			"any carnal pleasure.");
	//	EXPECT_EQ(ding::btos(ding::decode_base64(
	//					  ding::stob("YW55IGNhcm5hbCBwbGVhc3VyZQ"))),
	//			"any carnal pleasure");
	//	EXPECT_EQ(ding::btos(ding::decode_base64(
	//					  ding::stob("YW55IGNhcm5hbCBwbGVhc3U"))),
	//			"any carnal pleasu");
	//	EXPECT_EQ(ding::btos(ding::decode_base64(
	//					  ding::stob("YW55IGNhcm5hbCBwbGVhcw"))),
	//			"any carnal pleas");
}
} // namespace