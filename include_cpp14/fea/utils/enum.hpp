#pragma once
#include "fea/utils/string.hpp"

#include <string>
#include <vector>

/*
FEA_STRING_ENUM creates an accompanying vector of enum strings and some
accessors. It only works on enums that start at 0 to N!

Generates :
enum class ename : type {...};
const std::vector<std::string> ename_strings;
const std::vector<std::string>& enum_strings<ename>();
const std::string& to_string(ename e);

Plus, it generates all of these for all string types :
string, wstring, u16string, u32string.

Prepend "string" with your desired string type. For ex, u32.
const std::vector<std::u32string> ename_u32strings;
const std::vector<std::u32string>& enum_u32strings<ename>();
const std::u32string& to_u32string(ename e);
*/
// clang-format off
#define FEA_STRING_ENUM(ename, utype, ...) \
	template <class> \
	const std::vector<std::string>& enum_strings(); \
	template <class> \
	const std::vector<std::wstring>& enum_wstrings(); \
	template <class> \
	const std::vector<std::u16string>& enum_u16strings(); \
	template <class> \
	const std::vector<std::u32string>& enum_u32strings(); \
	\
	enum class ename : utype { __VA_ARGS__ }; \
	\
	namespace ename##_detail { \
		FEA_INLINE_VAR const std::string ename##_string = #__VA_ARGS__; \
		FEA_INLINE_VAR const std::wstring ename##_wstring = L#__VA_ARGS__; \
		FEA_INLINE_VAR const std::u16string ename##_u16string = u#__VA_ARGS__; \
		FEA_INLINE_VAR const std::u32string ename##_u32string = U#__VA_ARGS__; \
	} \
	\
	FEA_INLINE_VAR const std::vector<std::string> ename##_strings \
			= fea::split(ename##_detail::ename##_string, ", "); \
	FEA_INLINE_VAR const std::vector<std::wstring> ename##_wstrings \
			= fea::split(ename##_detail::ename##_wstring, L", "); \
	FEA_INLINE_VAR const std::vector<std::u16string> ename##_u16strings \
			= fea::split(ename##_detail::ename##_u16string, u", "); \
	FEA_INLINE_VAR const std::vector<std::u32string> ename##_u32strings \
			= fea::split(ename##_detail::ename##_u32string, U", "); \
	\
	template <> \
	const std::vector<std::string>& enum_strings<ename>() { \
		return ename##_strings; \
	} \
	template <> \
	const std::vector<std::wstring>& enum_wstrings<ename>() { \
		return ename##_wstrings; \
	} \
	template <> \
	const std::vector<std::u16string>& enum_u16strings<ename>() { \
		return ename##_u16strings; \
	} \
	template <> \
	const std::vector<std::u32string>& enum_u32strings<ename>() { \
		return ename##_u32strings; \
	} \
	\
	inline const std::string& to_string(ename e) { \
		return ename##_strings[size_t(e)]; \
	}\
	inline const std::wstring& to_wstring(ename e) { \
		return ename##_wstrings[size_t(e)]; \
	}\
	inline const std::u16string& to_u16string(ename e) { \
		return ename##_u16strings[size_t(e)]; \
	}\
	inline const std::u32string& to_u32string(ename e) { \
		return ename##_u32strings[size_t(e)]; \
	}
// clang-format on
