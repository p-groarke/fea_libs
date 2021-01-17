#include <fea/meta/pack.hpp>
#include <fea/utils/serialize.hpp>
#include <gtest/gtest.h>

namespace {
TEST(serialize, basics) {

	// std::vector<std::vector<int>> v;
	// static_assert(fea::serializer<std::vector<std::vector<int>>>::depth > 1);
	// static_assert(fea::serializer<fea::pack<fea::pack<int>>>::depth > 1);
	// static_assert(!fea::serialize_more_v<std::vector<int>>);
}
} // namespace
