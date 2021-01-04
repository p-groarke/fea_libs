#include <array>
#include <gtest/gtest.h>
#include <fea/maps/type_map.hpp>

namespace {
TEST(tuple, basics) {
constexpr fea::type_pack<int, double> k;
constexpr std::tuple<short, size_t> v{ short(0), size_t(42) };

constexpr fea::type_map m = fea::make_type_map(k, v);

static_assert(m.contains<int>(), "");
static_assert(!m.contains<short>(), "");
static_assert(m.find<double>() == 42, "");
static_assert(
		std::is_same<std::decay_t<decltype(m.find<double>())>, size_t>::value,
		"");

enum class testit { one, two, three };

constexpr fea::non_type_type_pack<testit, testit::one, testit::two> k2;
constexpr std::tuple<short, size_t> v2{ short(0), size_t(42) };

constexpr fea::type_map m_nontype = fea::make_type_map(k2, v2);
static_assert(m_nontype.contains<testit::one>(), "");
static_assert(!m_nontype.contains<testit::three>(), "");
static_assert(m_nontype.find<testit::two>() == 42, "");
static_assert(
		std::is_same<std::decay_t<decltype(m_nontype.find<testit::two>())>,
				size_t>::value,
		"");

static_assert(!fea::is_same_nt_v<testit, testit::one, testit::two>, "");
static_assert(fea::is_same_nt_v<testit, testit::one, testit::one>, "");
}
}