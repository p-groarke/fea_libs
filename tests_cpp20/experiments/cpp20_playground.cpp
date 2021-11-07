#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <fea/meta/static_for.hpp>
#include <fea/meta/tuple.hpp>
#include <fea/utils/string.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <type_traits>

namespace {
using ssize_t = intmax_t;

template <class T>
struct someclass {
	T test = 42;
};

// struct cexpr_str {
//	template <size_t N>
//	constexpr cexpr_str(const char (&Cs)[N]) {
//	}
//};
//

// namespace detail {
//// Scott Schurr
// struct cexpr_str_lit {
//	template <size_t N>
//	consteval cexpr_str_lit(const char (&str)[N]) noexcept
//			: data(str)
//			, size(N - 1) {
//	}
//	consteval cexpr_str_lit() = default;
//
//	friend consteval std::strong_ordering operator<=>(
//			const cexpr_str_lit& lhs, const cexpr_str_lit& rhs) {
//		return fea::lexicographical_compare(
//				lhs.data, lhs.data + lhs.size, rhs.data, rhs.data + rhs.size);
//	}
//
//	friend consteval bool operator==(
//			const cexpr_str_lit& lhs, const cexpr_str_lit& rhs) {
//		if (lhs.size != rhs.size) {
//			return false;
//		}
//		return true;
//		// return std::equal(lhs.data, lhs.data + lhs.size, rhs.data);
//	}
//
//	const char* const data = nullptr;
//	const size_t size = 0;
//};
//
//} // namespace detail
//
// template <detail::cexpr_str_lit Str>
// struct cexpr_str {
//	consteval char operator[](size_t idx) const {
//		return Str.data[idx];
//	}
//
//	consteval size_t size() noexcept {
//		return Str.size;
//	}
//
//	// friend auto operator<=>(const cexpr_str&, const cexpr_str&) = default;
//
//	template <detail::cexpr_str_lit T1, detail::cexpr_str_lit T2>
//	friend consteval std::strong_ordering operator<=>(
//			const cexpr_str<T1>&, const cexpr_str<T2>&) {
//		return T1 <=> T2;
//	}
//
//	// template <detail::cexpr_str_lit T1, detail::cexpr_str_lit T2>
//	// friend constexpr bool operator==(
//	//		const cexpr_str<T1>&, const cexpr_str<T2>&) {
//	//	return T1 == T2;
//	//}
//
//	// consteval bool operator==(const cexpr_str&) const {
//	//	return true;
//	//}
//
//	// template <detail::cexpr_str_lit T1, detail::cexpr_str_lit T2>
//	// friend consteval bool operator==(
//	//		const cexpr_str<T1>& lhs, const cexpr_str<T2>& rhs) {
//	//	return T1 == T2;
//	//}
//};
//
// template <const char* const T>
// struct test_str {};
//
//// template <detail::cexpr_str_lit T1, detail::cexpr_str_lit T2>
//// consteval bool operator==(const cexpr_str<T1>& lhs, const cexpr_str<T2>&
/// rhs) / { 	return T1 == T2; /	// using c1 = std::decay_t<decltype(lhs)>;
////	// using c1 = std::decay_t<decltype(rhs)>;
////
////	// if constexpr (lhs.size() != rhs.size()) {
////	//	return false;
////	//}
////
////	// bool ret = true;
////	// fea::static_for<lhs.size()>([&](auto idx) { ret &= lhs[idx] ==
/// rhs[idx]; /	// }); return ret;
////}

struct test {
	friend constexpr std::strong_ordering operator<=>(
			const test& lhs, const test& rhs) {
		return lhs.v <=> rhs.v;
	}

	friend constexpr bool operator==(const test& lhs, const test& rhs) {
		return lhs.v == rhs.v;
	}

	const int* ptr = nullptr;
	int v = 42;
};

template <test T>
struct tpo_class {
	// How to do this?
	template <test T1, test T2>
	friend consteval bool operator==(
			const tpo_class<T1>&, const tpo_class<T2>&) {
		return T1 == T2;
	}
};

template <class CharT, size_t N>
struct string_literal {
	using array_t = std::array<CharT, N>;
	using value_type = array_t::value_type;
	using size_type = array_t::size_type;
	using difference_type = array_t::difference_type;
	using reference = array_t::reference;
	using const_reference = array_t::const_reference;
	using pointer = array_t::pointer;
	using const_pointer = array_t::const_pointer;
	using iterator = array_t::iterator;
	using const_iterator = array_t::const_iterator;
	using reverse_iterator = array_t::reverse_iterator;
	using const_reverse_iterator = array_t::const_reverse_iterator;

	// Should it be default constructible?
	// constexpr string_literal() = default;

	constexpr string_literal(const CharT (&str)[N])
			: _data{} {
		std::copy(str, str + N, _data.begin());
	}
	// constexpr string_literal(
	//		const std::basic_string<CharT, std::char_traits<CharT>>& str)
	//		: _data{} {
	//	std::copy(str, str + N, _data.begin());
	//}

	friend constexpr auto operator<=>(
			const string_literal&, const string_literal&)
			= default;

	friend constexpr bool operator==(
			const string_literal&, const string_literal&)
			= default;

	// Element access
	// constexpr reference at(size_type pos) {
	//	return _data.at(pos);
	//}
	constexpr const_reference at(size_type pos) const {
		return _data.at(pos);
	}

	// constexpr reference operator[](size_type pos) {
	//	return _data[pos];
	//}
	constexpr const_reference operator[](size_type pos) const {
		return _data[pos];
	}

	// reference front() {
	//	return _data[0];
	//}
	constexpr const_reference front() const {
		return _data[0];
	}

	// reference back() {
	//	return _data[size() - 1];
	//}
	constexpr const_reference back() const {
		return _data[size() - 1];
	}

	constexpr const_pointer data() const noexcept {
		return _data.data();
	}
	constexpr const_pointer c_str() const noexcept {
		return data();
	}

	// Iterators
	constexpr const_iterator begin() const noexcept {
		return _data.begin();
	}
	constexpr const_iterator cbegin() const noexcept {
		return _data.cbegin();
	}

	constexpr const_iterator end() const noexcept {
		return _data.end() - 1;
	}
	constexpr const_iterator cend() const noexcept {
		return _data.cend() - 1;
	}

	constexpr const_iterator rbegin() const noexcept {
		return ++_data.rbegin();
	}
	constexpr const_iterator crbegin() const noexcept {
		return ++_data.crbegin();
	}

	constexpr const_iterator rend() const noexcept {
		return _data.rend();
	}
	constexpr const_iterator crend() const noexcept {
		return _data.crend();
	}

	// Capacity
	constexpr bool empty() const noexcept {
		return _data.empty();
	}

	constexpr size_type size() const noexcept {
		return _data.size() - 1;
	}
	constexpr size_type length() const noexcept {
		return size();
	}

	constexpr size_type max_size() const noexcept {
		return _data.max_size();
	}

	array_t _data{};
};

template <size_t N>
struct lit {
	using CharT = char;

	consteval lit() = default;
	consteval lit(const CharT (&str)[N])
			: _data(str)
			, _size(N - 1) {
	}

	// template <class CharT, size_t N, class CharT2, size_t N2>
	// friend constexpr auto operator<=>(
	//		const lit<CharT, N>&, const lit<CharT2, N2>&) noexcept = default;

	// template <class C1, size_t N1, class C2, size_t N2>
	// constexpr bool operator==(const lit&) const = default;

	template <size_t N1, size_t N2>
	friend consteval bool operator==(const lit<N1>& lhs, const lit<N2>& rhs) {
		return lhs._data == rhs._data;
	}

	// template <class C1, size_t N1, class C2, size_t N2>
	// friend constexpr bool operator==(const lit<C1, N1>&, const lit<C2, N2>&)
	//		= default;

	// template <class CharT2, size_t N2>
	// friend constexpr auto operator==(
	//		const lit&, const lit<CharT2, N2>&) noexcept = default;

	const CharT* _data = nullptr;
	const size_t _size = 0;
};

template <const char* T>
struct wrapper {
	// consteval wrapper() = default;
	// template <string_literal T1, string_literal T2>
	// friend constexpr auto operator<=>(const wrapper<T1>&, const wrapper<T2>&)
	//		= default;

	// template <lit T1, lit T2>
	// friend consteval bool operator==(const wrapper<T1>&, const wrapper<T2>&)
	// { 	return T1 == T2;
	//}

	// template <string_literal T2>
	// friend constexpr auto operator<=>(const wrapper<T>&, const wrapper<T2>&)
	//		= default;
	// template <auto T2>
	// friend constexpr bool operator==(const wrapper&, const wrapper<T2>&)
	//		= default;

	// friend constexpr auto operator<=>(const wrapper&, const wrapper&) =
	// default; friend constexpr bool operator==(const wrapper&, const wrapper&)
	// = default;

	// static constexpr auto str = T;
};

// template <lit T>
// constexpr auto operator""_cexpr() {
//	return wrapper<T._data>{};
//}

TEST(playground, cpp20) {
	// static_assert(numWords("hello world abc xyz") == 4);

	constexpr lit str1{ "abc" };
	constexpr lit str2{ "abc" };
	static_assert(str1 == str2);

	// constexpr std::tuple tup
	//		= { "tomato"_cexpr, "potato"_cexpr, "onion"_cexpr };
	// constexpr auto sorted_tup = test_sort(tup);


	// constexpr auto str3 = "testing"_cexpr;
	// constexpr auto str4 = "testing2"_cexpr;
	// static_assert(str3 == str4);
	// static_assert(str3.back() == 'g');

	// constexpr char c1 = str3[4];

	// printf("%s\n", str3.c_str());


	// constexpr auto str = "test";
	// constexpr std::string_view str_view = str;
	// static constexpr std::string str; // { "test" };

	// constexpr someclass<int> s;
	// constexpr std::array arr{ "test" };

	// constexpr cexpr_str cs("test");
	// static_assert(cs.size() == 4, "bla");

	//{
	//	// cexpr_str<"a compile string"> t;
	//	// cexpr_str<"a compile string"> t2;
	//	cexpr_str<str1> t;
	//	cexpr_str<str2> t2;
	//	static_assert(t == t2);
	//	// static_assert(std::is_same_v<decltype(t), decltype(t2)>);
	//}

	//{ test_str<"a test"> t; }

	// constexpr auto v = t.testit();

	// std::cout << v._ptr << std::endl;
	// printf("%d\n", v);
	// std::cout << std::format("{}\n", v);

	{
		constexpr test t1;
		constexpr test t2;
		tpo_class<t1> tpo1;
		tpo_class<t1> tpo2;
		static_assert(tpo1 == tpo2);
	}
}
} // namespace
