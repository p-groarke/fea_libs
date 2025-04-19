#include <fea/encoding/base64.hpp>
#include <fea/utility/platform.hpp>
#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>

namespace {
TEST(base64, basics) {
	// Basics.
	{
		const std::string str = "Sun";
		std::string enc;
		fea::to_base64(str.begin(), str.end(), std::back_inserter(enc));
		EXPECT_EQ(enc, "U3Vu");

		std::string dec;
		fea::from_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		EXPECT_EQ(dec, str);
	}

	// Double pad.
	{
		const std::string str = "S";
		std::string enc;
		fea::to_base64(str.begin(), str.end(), std::back_inserter(enc));
		EXPECT_EQ(enc, "Uw==");

		std::string dec;
		fea::from_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		EXPECT_EQ(dec, str);
	}

	// Single pad.
	{
		const std::string str = "Su";
		std::string enc;
		fea::to_base64(str.begin(), str.end(), std::back_inserter(enc));
		EXPECT_EQ(enc, "U3U=");

		std::string dec;
		fea::from_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		EXPECT_EQ(dec, str);
	}

	// just a sentence
	{
		const std::string str = "Many hands make light work.";
		std::string enc;
		fea::to_base64(str.begin(), str.end(), std::back_inserter(enc));
		EXPECT_EQ(enc, "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu");

		std::string dec;
		fea::from_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		EXPECT_EQ(dec, str);
	}

	// input output iterator
	{
		const std::string str = "Sun";
		std::string enc(4, '\0');
		fea::to_base64(str.begin(), str.end(), enc.begin());
		EXPECT_EQ(enc, "U3Vu");

		std::string dec(3, '\0');
		fea::from_base64(enc.begin(), enc.end(), dec.begin());
		EXPECT_EQ(dec, str);
	}

	// input output iterator
	{
		const std::string str = "S";
		std::string enc(4, '\0');
		fea::to_base64(str.begin(), str.end(), enc.begin());
		EXPECT_EQ(enc, "Uw==");

		std::string dec(1, '\0');
		fea::from_base64(enc.begin(), enc.end(), dec.begin());
		EXPECT_EQ(dec, str);
	}

	// input output iterator
	{
		const std::string str = "Su";
		std::string enc(4, '\0');
		fea::to_base64(str.begin(), str.end(), enc.begin());
		EXPECT_EQ(enc, "U3U=");

		std::string dec(2, '\0');
		fea::from_base64(enc.begin(), enc.end(), dec.begin());
		EXPECT_EQ(dec, str);
	}

	// vector of bytes
	{
		const std::vector<char> str{ 'S' };
		std::string enc;
		fea::to_base64(str.begin(), str.end(), std::back_inserter(enc));
		EXPECT_EQ(enc, "Uw==");

		std::vector<char> dec;
		fea::from_base64(enc.begin(), enc.end(), std::inserter(dec, dec.end()));
		EXPECT_EQ(dec, str);
	}

	// vector of bytes
	{
		const std::vector<uint8_t> str{ 'S', 'u' };
		std::string enc;
		fea::to_base64(str.begin(), str.end(), std::back_inserter(enc));
		EXPECT_EQ(enc, "U3U=");

		std::vector<uint8_t> dec;
		fea::from_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		EXPECT_EQ(dec, str);
	}

	// wstring
	{
		const std::string str = "Sun";
		std::wstring enc;
		fea::to_base64(str.begin(), str.end(), std::back_inserter(enc));
		EXPECT_EQ(enc, L"U3Vu");

		std::string dec;
		fea::from_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		EXPECT_EQ(dec, "Sun");
	}

	// wstring
	{
		const std::wstring str = L"Sun";
		std::wstring enc;
		fea::to_base64(str.begin(), str.end(), std::back_inserter(enc));
#if FEA_WINDOWS
		EXPECT_EQ(enc, L"UwB1AG4A"); // little-endian
#else
		EXPECT_EQ(enc, L"UwAAAHUAAABuAAAA"); // wstring 4 byte
#endif

		std::wstring dec;
		fea::from_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		EXPECT_EQ(dec, L"Sun");
	}

	// wstring
	{
		const std::wstring str = L"Sun";
		std::string enc;
		fea::to_base64(str.begin(), str.end(), std::back_inserter(enc));
#if FEA_WINDOWS
		EXPECT_EQ(enc, "UwB1AG4A"); // little-endian
#else
		EXPECT_EQ(enc, "UwAAAHUAAABuAAAA"); // wstring 4 byte
#endif

		// Works since we can always convert to bytes, but incorrect output.
		std::string dec;
		fea::from_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		std::string expected;
#if FEA_WINDOWS
		//"S\0u\0n\0";
		{
			expected.push_back('S');
			expected.push_back('\0');
			expected.push_back('u');
			expected.push_back('\0');
			expected.push_back('n');
			expected.push_back('\0');
		}
#else
		//"S\0\0\0u\0\0\0n\0\0\0";
		expected.push_back('\0');
		expected.push_back('\0');
		expected.push_back('\0');
		expected.push_back('u');
		expected.push_back('\0');
		expected.push_back('\0');
		expected.push_back('\0');
		expected.push_back('n');
		expected.push_back('\0');
		expected.push_back('\0');
		expected.push_back('\0');
#endif
		EXPECT_EQ(dec, expected);

		// Correct output, encode type should be the same as decode.
		std::wstring dec2;
		fea::from_base64(enc.begin(), enc.end(), std::back_inserter(dec2));
		EXPECT_EQ(dec2, L"Sun");
	}

	// Non char input iterators.
	{
		const std::string str = "Sun\0";
		std::array<uint32_t, 1> data{};
		std::memcpy(data.data(), str.data(), sizeof(uint32_t));

		std::string enc;
		fea::to_base64(data.begin(), data.end(), std::back_inserter(enc));
		EXPECT_EQ(enc, "U3VuAA==");

		std::vector<uint32_t> dec;
		fea::from_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		EXPECT_EQ(dec.front(), data.front());
	}

	// Image data iterators.
	{
		struct bmp {
			unsigned char hexData[70] = { 0x42, 0x4D, 0x46, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
				0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
				0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
				0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0xFF, 0x00, 0x00 };

			auto operator<=>(const bmp&) const noexcept = default;
		};

		std::vector<bmp> v(1);
		std::string enc;
		fea::to_base64(v.begin(), v.end(), std::back_inserter(enc));
		EXPECT_EQ(enc, "Qk1GAAAAAAAAADYAAAAoAAAAAgAAAAIAAAABABgAAAAAABAAAAAAAAA"
					   "AAAAAAAAAAAAAAAAAAP8A/wAAAAAAAAAAAP8AAA==");

		std::vector<bmp> dec;
		fea::from_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		EXPECT_EQ(dec.front(), v.front());
	}

	// Single object data.
	{
		uint32_t data = 0x006e7553;
		std::string enc;
		fea::to_base64(&data, &data + 1, std::back_inserter(enc));
		EXPECT_EQ(enc, "U3VuAA==");

		uint32_t dec;
		fea::from_base64(enc.begin(), enc.end(), &dec);
		EXPECT_EQ(dec, data);
	}

	// Single object image.
	{
		struct bmp {
			unsigned char hexData[70] = { 0x42, 0x4D, 0x46, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
				0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
				0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
				0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0xFF, 0x00, 0x00 };

			auto operator<=>(const bmp&) const noexcept = default;
		};

		bmp data;
		std::string enc;
		fea::to_base64(&data, &data + 1, std::inserter(enc, enc.end()));
		EXPECT_EQ(enc, "Qk1GAAAAAAAAADYAAAAoAAAAAgAAAAIAAAABABgAAAAAABAAAAAAAAA"
					   "AAAAAAAAAAAAAAAAAAP8A/wAAAAAAAAAAAP8AAA==");

		bmp dec;
		fea::from_base64(enc.begin(), enc.end(), &dec);
		EXPECT_EQ(dec, data);
	}
}

TEST(base64, rfc4648) {
	std::vector<std::pair<std::string, std::string>> rfc_tests{
		{ "", "" },
		{ "f", "Zg==" },
		{ "fo", "Zm8=" },
		{ "foo", "Zm9v" },
		{ "foob", "Zm9vYg==" },
		{ "fooba", "Zm9vYmE=" },
		{ "foobar", "Zm9vYmFy" },
	};

	for (const auto& [input, expected] : rfc_tests) {
		std::string enc;
		fea::to_base64(input.begin(), input.end(), std::back_inserter(enc));
		EXPECT_EQ(enc, expected);

		std::string dec;
		fea::from_base64(enc.begin(), enc.end(), std::back_inserter(dec));
		EXPECT_EQ(dec, input);
	}
}

TEST(base64, crypto_lib_tests) {
	// TODO
#if 0
	auto hex = ding::stob(
			"49276d206b696c6c696e6720796f757220627261696e206c696b65206"
			"120706f69736f6e6f7573206d757368726f6f6d");
	std::vector<uint8_t> bytes = ding::decode_hex(hex);

	EXPECT_EQ(ding::btos(bytes),
			"I'm killing your brain like a poisonous mushroom");
	EXPECT_EQ(ding::btos(ding::decode_hex(hex)),
			"I'm killing your brain like a poisonous mushroom");

	printf("%s\n", ding::btos(bytes).c_str());
	std::string base64 = ding::btos(ding::encode_base64(bytes));
	EXPECT_EQ(base64,
			"SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t");

	hex = ding::stob("0x034092ccfff0228f73b62a14");
	bytes = ding::decode_hex(hex);
	base64 = ding::btos(ding::encode_base64(bytes));
	EXPECT_EQ(base64, "A0CSzP/wIo9ztioU");

	hex = ding::stob("0x34092ccfff0228f73b62a14");
	bytes = ding::decode_hex(hex);
	base64 = ding::btos(ding::encode_base64(bytes));
	EXPECT_EQ(base64, "A0CSzP/wIo9ztioU");

	hex = ding::stob("0X34092ccfff0228f73b62a14");
	bytes = ding::decode_hex(hex);
	base64 = ding::btos(ding::encode_base64(bytes));
	EXPECT_EQ(base64, "A0CSzP/wIo9ztioU");

	hex = ding::stob("34092ccfff0228f73b62a14");
	bytes = ding::decode_hex(hex);
	base64 = ding::btos(ding::encode_base64(bytes));
	EXPECT_EQ(base64, "A0CSzP/wIo9ztioU");

	EXPECT_EQ(ding::btos(ding::encode_base64(ding::stob("te"))), "dGU=");
	EXPECT_EQ(ding::btos(ding::encode_base64(ding::stob("t"))), "dA==");
	EXPECT_EQ(
			ding::btos(ding::encode_base64(ding::stob("any carnal pleasure."))),
			"YW55IGNhcm5hbCBwbGVhc3VyZS4=");
	EXPECT_EQ(
			ding::btos(ding::encode_base64(ding::stob("any carnal pleasure"))),
			"YW55IGNhcm5hbCBwbGVhc3VyZQ==");
	EXPECT_EQ(ding::btos(ding::encode_base64(ding::stob("any carnal pleasur"))),
			"YW55IGNhcm5hbCBwbGVhc3Vy");
	EXPECT_EQ(ding::btos(ding::encode_base64(ding::stob("any carnal pleasu"))),
			"YW55IGNhcm5hbCBwbGVhc3U=");
	EXPECT_EQ(ding::btos(ding::encode_base64(ding::stob("any carnal pleas"))),
			"YW55IGNhcm5hbCBwbGVhcw==");

	EXPECT_EQ(ding::btos(ding::decode_base64(ding::stob("dGU="))), "te");
	EXPECT_EQ(ding::btos(ding::decode_base64(ding::stob("dA=="))), "t");
	EXPECT_EQ(ding::btos(ding::decode_base64(
					  ding::stob("YW55IGNhcm5hbCBwbGVhc3VyZS4="))),
			"any carnal pleasure.");
	EXPECT_EQ(ding::btos(ding::decode_base64(
					  ding::stob("YW55IGNhcm5hbCBwbGVhc3VyZQ=="))),
			"any carnal pleasure");
	EXPECT_EQ(ding::btos(ding::decode_base64(
					  ding::stob("YW55IGNhcm5hbCBwbGVhc3Vy"))),
			"any carnal pleasur");
	EXPECT_EQ(ding::btos(ding::decode_base64(
					  ding::stob("YW55IGNhcm5hbCBwbGVhc3U="))),
			"any carnal pleasu");
	EXPECT_EQ(ding::btos(ding::decode_base64(
					  ding::stob("YW55IGNhcm5hbCBwbGVhcw=="))),
			"any carnal pleas");

	EXPECT_EQ(ding::btos(ding::decode_base64(ding::stob("dGU"))), "te");
	EXPECT_EQ(ding::btos(ding::decode_base64(ding::stob("dA"))), "t");
	EXPECT_EQ(ding::btos(ding::decode_base64(
					  ding::stob("YW55IGNhcm5hbCBwbGVhc3VyZS4"))),
			"any carnal pleasure.");
	EXPECT_EQ(ding::btos(ding::decode_base64(
					  ding::stob("YW55IGNhcm5hbCBwbGVhc3VyZQ"))),
			"any carnal pleasure");
	EXPECT_EQ(ding::btos(ding::decode_base64(
					  ding::stob("YW55IGNhcm5hbCBwbGVhc3U"))),
			"any carnal pleasu");
	EXPECT_EQ(ding::btos(ding::decode_base64(
					  ding::stob("YW55IGNhcm5hbCBwbGVhcw"))),
			"any carnal pleas");
#endif
}
} // namespace