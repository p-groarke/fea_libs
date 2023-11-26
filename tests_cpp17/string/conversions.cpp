#include <fea/string/conversions.hpp>
#include <gtest/gtest.h>

namespace {
TEST(string_conversions, basics) {
}

#if FEA_WINDOWS && defined(FEA_CODEPAGE_CONVERSIONS)
TEST(string_conversions, windows_codepage) {
}
#endif
} // namespace
