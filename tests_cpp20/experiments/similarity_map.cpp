#include <array>
#include <cmath>
#include <fea/language/language.hpp>
#include <fea/macros/literals.hpp>
#include <fea/math/statistics.hpp>
#include <fea/string/string.hpp>
#include <format>
#include <gtest/gtest.h>
#include <iostream>
#include <limits>
#include <numbers>
#include <numeric>
#include <string>
#include <string_view>

namespace {

/*
English letter frequencies (wiki)
E	12.7%
T	9.1%
A	8.2%
O	7.5%
I	7.0%
N	6.7%
S	6.3%
H	6.1%
R	6.0%
D	4.3%
L	4.0%
C	2.8%
U	2.8%
M	2.4%
W	2.4%
F	2.2%
G	2.0%
Y	2.0%
P	1.9%
B	1.5%
V	0.98%
K	0.77%
J	0.15%
X	0.15%
Q	0.095%
Z	0.074%
*/
template <class CharT>
constexpr auto make_lookup() {
	// Non-printable and special chars are not weighed (0).
	std::array<uint8_t, (std::numeric_limits<uint8_t>::max)()> ret{};

	// ret[size_t(' ')] = ; Space matters?
	ret[size_t('E')] = 1;
	ret[size_t('e')] = 1;
	ret[size_t('T')] = 2;
	ret[size_t('t')] = 2;
	ret[size_t('A')] = 3;
	ret[size_t('a')] = 3;
	ret[size_t('O')] = 4;
	ret[size_t('o')] = 4;
	ret[size_t('I')] = 5;
	ret[size_t('i')] = 5;
	ret[size_t('N')] = 6;
	ret[size_t('n')] = 6;
	ret[size_t('S')] = 7;
	ret[size_t('s')] = 7;
	ret[size_t('H')] = 8;
	ret[size_t('h')] = 8;
	ret[size_t('R')] = 9;
	ret[size_t('r')] = 9;
	ret[size_t('D')] = 10;
	ret[size_t('d')] = 10;
	ret[size_t('L')] = 11;
	ret[size_t('l')] = 11;
	ret[size_t('C')] = 12;
	ret[size_t('c')] = 12;
	ret[size_t('U')] = 13;
	ret[size_t('u')] = 13;
	ret[size_t('M')] = 14;
	ret[size_t('m')] = 14;
	ret[size_t('W')] = 15;
	ret[size_t('w')] = 15;
	ret[size_t('F')] = 16;
	ret[size_t('f')] = 16;
	ret[size_t('G')] = 17;
	ret[size_t('g')] = 17;
	ret[size_t('Y')] = 18;
	ret[size_t('y')] = 18;
	ret[size_t('P')] = 19;
	ret[size_t('p')] = 19;
	ret[size_t('B')] = 20;
	ret[size_t('b')] = 20;
	ret[size_t('V')] = 21;
	ret[size_t('v')] = 21;
	ret[size_t('K')] = 22;
	ret[size_t('k')] = 22;
	ret[size_t('J')] = 23;
	ret[size_t('j')] = 23;
	ret[size_t('X')] = 24;
	ret[size_t('x')] = 24;
	ret[size_t('Q')] = 25;
	ret[size_t('q')] = 25;
	ret[size_t('Z')] = 26;
	ret[size_t('z')] = 26;

	ret[size_t('0')] = 27;
	ret[size_t('1')] = 28;
	ret[size_t('2')] = 29;
	ret[size_t('3')] = 30;
	ret[size_t('4')] = 31;
	ret[size_t('5')] = 32;
	ret[size_t('6')] = 33;
	ret[size_t('7')] = 34;
	ret[size_t('8')] = 35;
	ret[size_t('9')] = 36;

	// Q: Weigh low or not at all?
	// ret[size_t('(')] = ;
	// ret[size_t(')')] = ;
	// ret[size_t('!')] = ;
	// ret[size_t('?')] = ;
	// ret[size_t(',')] = ;
	// ret[size_t('-')] = ;
	// ret[size_t('.')] = ;
	// ret[size_t(':')] = ;
	// ret[size_t(';')] = ;
	// ret[size_t('"')] = ;
	// ret[size_t('\'')] = ;

	// ret[size_t('#')] = ;
	// ret[size_t('@')] = ;
	// ret[size_t('$')] = ;
	// ret[size_t('%')] = ;
	// ret[size_t('&')] = ;
	// ret[size_t('*')] = ;
	// ret[size_t('+')] = ;
	// ret[size_t('/')] = ;
	// ret[size_t('=')] = ;
	// ret[size_t('<')] = ;
	// ret[size_t('>')] = ;
	// ret[size_t('[')] = ;
	// ret[size_t('\\')] = ;
	// ret[size_t(']')] = ;
	// ret[size_t('^')] = ;
	// ret[size_t('_')] = ;
	// ret[size_t('`')] = ;
	// ret[size_t('{')] = ;
	// ret[size_t('|')] = ;
	// ret[size_t('}')] = ;
	// ret[size_t('~')] = ;

	return ret;
}

constexpr std::array<uint8_t, (std::numeric_limits<uint8_t>::max)()> char_lookup
		= make_lookup<char>();

std::pair<float, float> linear_regression(std::string_view sv) {
	std::vector<std::pair<float, float>> v;
	for (size_t i = 0; i < sv.size(); ++i) {
		float x = float(i);
		float y = float(char_lookup[sv[i]]);
		v.push_back({ x, y });
	}
	return fea::simple_linear_regression(v.begin(), v.end());
}

std::vector<double> dct2(std::string_view sv) {
	auto rectify = [](char c) {
		c = fea::to_lower_ascii(c);
		c -= 'a';
		constexpr double n = double('z' - 'a') * 0.5;
		return (double(c) - n) / n;
	};

	std::vector<double> ret(sv.size());
	double N = double(sv.size());

	for (size_t k = 0; k < sv.size(); ++k) {
		double sum = 0.0;
		for (size_t n = 0; n < sv.size(); ++n) {
			double xn = rectify(sv[n]);
			double c = (std::numbers::pi / N) * (double(n) + 0.5) * double(k);
			sum += xn * std::cos(c);
		}
		ret[k] = sum;
	}

	// Do ortho?
	if constexpr (true) {
		double x0_mul = 1.0 / std::sqrt(N);
		ret[0] *= x0_mul;

		double xn_mul = std::sqrt(2.0 / N);
		for (size_t i = 1; i < ret.size(); ++i) {
			ret[i] *= xn_mul;
		}
	}
	return ret;
}


TEST(similarity_map, experiments) {
	using namespace std::string_view_literals;
	// Print out the "most likely" english word, navigating bigrams
	// probabilities.
	{
		std::string result;

		// auto pop_front
		//		= [](std::vector<std::string_view>& s) -> std::string_view {
		//	if (s.size() == 1) {
		//		s = {};
		//	}
		//	s = { s.data() + 1u, s.size() - 1u };
		// };


		std::vector<std::string_view> bigrams(
				fea::en::bigrams().begin(), fea::en::bigrams().end());

		std::string_view current = bigrams.front();
		result += current;
		bigrams.erase(bigrams.begin());

		while (!bigrams.empty()) {
			// Find the first bigram that starts with last letter of current.
			// Bigrams are ordered by frequency, so that should be the most
			// likely.
			// if (fea::en::bigram_frequency(current) == 0.0) {
			//	break;
			//}

			auto it = std::find_if(
					bigrams.begin(), bigrams.end(), [&](std::string_view b) {
						return fea::en::bigram_frequency(b) != 0.0
							&& current.back() == b.front();
					});

			if (it != bigrams.end()) {
				current = *it;
				result += current.back();
				bigrams.erase(it);
			} else {
				// Get first.
				current = bigrams.front();
				result += " ";
				result += current;
				bigrams.erase(bigrams.begin());
			}
		}

		// std::cout << std::format("Most likely word :\n  {}\n", result);
	}

#if 0
	{
		std::vector<std::string_view> words{
			"zzz"sv,
			"kitten"sv,
			"kittens"sv,
			"sitten"sv,
			"sitting"sv,
			"It"sv,
			"it"sv,
			"its"sv,
			"it's"sv,
			"there"sv,
			"their"sv,
			"potato"sv,
			"potatos"sv,
			"tomato"sv,
			"tomatos"sv,
			"tomatoss"sv,
		};

		std::cout << "\nLinear Regression\n";
		for (std::string_view word : words) {
			auto [a, b] = linear_regression(word);
			std::cout << std::format("{} : a = {}, b = {}\n", word, a, b);
		}

		std::cout << "\nDHT\n";
		for (std::string_view word : words) {
			std::vector<double> dht = dct2(word);
			std::cout << word << " : ";
			double tot = 0.0;
			for (double d : dht) {
				std::cout << std::format("{:.6f},", d);
				tot += d;
			}
			std::cout << std::format("\n  total : {:.6f}\n", tot);
			// std::cout << sv << ": a = " << a << ", b = " << b << std::endl;
		}
	}
#endif
}

} // namespace