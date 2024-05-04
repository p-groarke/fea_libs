#pragma once

namespace fea {
template <class Enum>
constexpr std::underlying_type_t<Enum> to_underlying(Enum e) {
	static_assert(std::is_enum_v<Enum>,
			"fea::to_underlying : Only supports casting enums.");
	return static_cast<std::underlying_type_t<Enum>>(e);
}
} // namespace fea