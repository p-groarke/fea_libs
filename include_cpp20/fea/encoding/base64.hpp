/*
BSD 3-Clause License

Copyright (c) 2025, Philippe Groarke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include <array>
#include <string>
#include <string_view>
#include <vector>

// namespace fea {
//// Encode data into a base64 string.
// inline std::vector<uint8_t> encode_base64(const std::vector<uint8_t>& bytes);
// inline std::vector<uint8_t> decode_base64(const std::vector<uint8_t>&
// base64); } // namespace fea

namespace fea {
namespace detail {
// constexpr inline std::string_view base64_lut
//		= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

constexpr inline std::array<char, 64> base64_lut{ 'A', 'B', 'C', 'D', 'E', 'F',
	'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
	'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
	'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };

// constexpr inline std::array<uint8_t, 64> base64_lut{ uint8_t('A'),
// uint8_t('B'), 	uint8_t('C'), uint8_t('D'), uint8_t('E'), uint8_t('F'),
// uint8_t('G'), 	uint8_t('H'), uint8_t('I'), uint8_t('J'), uint8_t('K'),
// uint8_t('L'), 	uint8_t('M'), uint8_t('N'), uint8_t('O'), uint8_t('P'),
// uint8_t('Q'), 	uint8_t('R'), uint8_t('S'), uint8_t('T'), uint8_t('U'),
// uint8_t('V'), 	uint8_t('W'), uint8_t('X'), uint8_t('Y'), uint8_t('Z'),
// uint8_t('a'), 	uint8_t('b'), uint8_t('c'), uint8_t('d'), uint8_t('e'),
// uint8_t('f'), 	uint8_t('g'), uint8_t('h'), uint8_t('i'), uint8_t('j'),
// uint8_t('k'), 	uint8_t('l'), uint8_t('m'), uint8_t('n'), uint8_t('o'),
// uint8_t('p'), 	uint8_t('q'), uint8_t('r'), uint8_t('s'), uint8_t('t'),
// uint8_t('u'), 	uint8_t('v'), uint8_t('w'), uint8_t('x'), uint8_t('y'),
// uint8_t('z'), 	uint8_t('0'), uint8_t('1'), uint8_t('2'), uint8_t('3'),
// uint8_t('4'), 	uint8_t('5'), uint8_t('6'), uint8_t('7'), uint8_t('8'),
// uint8_t('9'), 	uint8_t('+'), uint8_t('/') };

constexpr inline std::array<uint8_t, 256> base64_rlut{ 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63, 52, 53, 54, 55, 56, 57, 58,
	59, 60, 61, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
	13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0, 0, 26,
	27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
	46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// const std::vector<unsigned>& base64_inverse_lut() {
//	static std::vector<unsigned> inverse_lut;
//	if (inverse_lut.size() == 0) {
//		inverse_lut.resize(256);
//		unsigned i = 0;
//		for (char c : base64_lut) {
//			inverse_lut[c] = i++;
//		}
//	}
//	return inverse_lut;
// }
} // namespace detail


// std::vector<uint8_t> encode_base64(const std::vector<uint8_t>& bytes) {
//	std::vector<uint8_t> ret;
//
//	for (size_t i = 0; i < bytes.size(); i += 3) {
//		uint8_t one = bytes[i];
//		int shift_min = 0;
//
//		uint8_t two, three;
//		if (i + 1 >= bytes.size()) {
//			two = 0;
//			three = 0;
//			shift_min = 12;
//		} else if (i + 2 >= bytes.size()) {
//			two = bytes[i + 1];
//			three = 0;
//			shift_min = 6;
//		} else {
//			two = bytes[i + 1];
//			three = bytes[i + 2];
//		}
//
//		unsigned val24 = (one << 16) | (two << 8) | three;
//		for (int shift = 18; shift >= 0; shift -= 6) {
//			if (shift < shift_min) {
//				ret.push_back('=');
//			} else {
//				unsigned ans = val24 >> shift;
//				ans &= 0b0011'1111;
//				ret.push_back(detail::base64_lut[ans]);
//			}
//		}
//	}
//	return ret;
// }
//
// std::vector<uint8_t> decode_base64(const std::vector<uint8_t>& base64) {
//	std::vector<uint8_t> data = base64;
//	if (base64.size() % 4 != 0) {
//		for (size_t i = 0; i < 4 - (base64.size() % 4); ++i) {
//			data.push_back('=');
//		}
//	}
//
//	std::vector<uint8_t> ret;
//	for (size_t i = 0; i < data.size(); i += 4) {
//		unsigned one = detail::base64_rlut[data[i]] << 18;
//		unsigned two = detail::base64_rlut[data[i + 1]] << 12;
//		unsigned three = detail::base64_rlut[data[i + 2]] << 6;
//		unsigned four = detail::base64_rlut[data[i + 3]];
//
//		if (data[i + 2] == '=') {
//			three = 0;
//		}
//
//		if (data[i + 3] == '=') {
//			four = 0;
//		}
//
//		unsigned val24 = one | two | three | four;
//
//		unsigned ans = val24 >> 16;
//		ret.push_back(static_cast<uint8_t>(ans));
//
//		if (data[i + 2] != '=') {
//			ans = val24 >> 8;
//			ans &= 0b1111'1111;
//			ret.push_back(static_cast<uint8_t>(ans));
//		}
//
//		if (data[i + 3] != '=') {
//			ans = val24;
//			ans &= 0b1111'1111;
//			ret.push_back(static_cast<uint8_t>(ans));
//		}
//	}
//	return ret;
// }
} // namespace fea
