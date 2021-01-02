

namespace fea {


/*
FEA_STRING_ENUM creates an accompanying vector of enum strings and some accessors.
*/
// clang-format off
#define FEA_STRING_ENUM(ename, utype, ...) \
	template <class> \
	const std::vector<std::string>& enum_strings(); \
	template <class> \
	const std::vector<std::u32string>& enum_u32strings(); \
	\
	enum class ename : utype { __VA_ARGS__ }; \
	\
	namespace ename##_detail { \
		FEA_INLINE_VAR const std::string ename##_string = #__VA_ARGS__; \
		FEA_INLINE_VAR const std::u32string ename##_u32string = U#__VA_ARGS__; \
	} \
	\
	FEA_INLINE_VAR const std::vector<std::string> ename##_strings \
			= fea::split(ename##_detail::ename##_string, ", "); \
	\
	FEA_INLINE_VAR const std::vector<std::u32string> ename##_u32strings \
			= fea::split(ename##_detail::ename##_u32string, U", "); \
	\
	template <> \
	const std::vector<std::string>& enum_strings<ename>() { \
		return ename##_strings; \
	} \
	\
	template <> \
	const std::vector<std::u32string>& enum_u32strings<ename>() { \
		return ename##_u32strings; \
	} \
	\
	inline const std::string& to_string(ename e) { \
		return ename##_strings[size_t(e)]; \
	}\
	inline const std::u32string& to_u32string(ename e) { \
		return ename##_u32strings[size_t(e)]; \
	}
// clang-format on

}