#include <fea/utils/string_conversions.hpp>
#include <gtest/gtest.h>

namespace {
TEST(string_conversions, basics) {
}

#if defined(FEA_WINDOWS) && defined(FEA_CODEPAGE_CONVERSIONS)
TEST(string_conversions, windows_codepage) {
}
#endif
} // namespace
