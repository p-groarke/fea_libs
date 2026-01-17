#include <fea/utility/error.hpp>

#include <cassert>
#include <fea/utility/platform.hpp>
#include <gtest/gtest.h>

namespace {
// How to test this?
TEST(error, basics) {
	// We just call the functions.
	std::error_code ec = fea::last_errno_error();
	EXPECT_TRUE(ec || !ec);

	ec = fea::last_os_error();
	EXPECT_TRUE(ec || !ec);


#if defined(FEA_NOTHROW) || !defined(NDEBUG)
	EXPECT_DEATH(fea::maybe_throw(__FUNCTION__, __LINE__, "msg"), "");
	EXPECT_DEATH(fea::maybe_throw(__FUNCTION__, __LINE__, L"msg"), "");

	EXPECT_DEATH(
			fea::maybe_throw<std::out_of_range>(__FUNCTION__, __LINE__, "msg"),
			"");
	EXPECT_DEATH(
			fea::maybe_throw<std::out_of_range>(__FUNCTION__, __LINE__, L"msg"),
			"");

	EXPECT_DEATH(fea::maybe_throw(__FUNCTION__, __LINE__,
						 std::error_code{ int(std::errc::file_exists),
								 std::system_category() }),
			"");
	EXPECT_DEATH(fea::maybe_throw_w(__FUNCTION__, __LINE__,
						 std::error_code{ int(std::errc::file_exists),
								 std::system_category() }),
			"");
#else
	EXPECT_THROW(fea::maybe_throw(__FUNCTION__, __LINE__, "msg"),
			std::runtime_error);
	EXPECT_THROW(fea::maybe_throw(__FUNCTION__, __LINE__, L"msg"),
			std::runtime_error);

	EXPECT_THROW(
			fea::maybe_throw<std::out_of_range>(__FUNCTION__, __LINE__, "msg"),
			std::runtime_error);
	EXPECT_THROW(
			fea::maybe_throw<std::out_of_range>(__FUNCTION__, __LINE__, L"msg"),
			std::runtime_error);

	EXPECT_THROW(fea::maybe_throw(__FUNCTION__, __LINE__,
						 std::error_code{ int(std::errc::file_exists),
								 std::system_category() }),
			std::system_error);
	EXPECT_THROW(fea::maybe_throw_w(__FUNCTION__, __LINE__,
						 std::error_code{ int(std::errc::file_exists),
								 std::system_category() }),
			std::system_error);
#endif

	EXPECT_DEATH(fea::error_exit(__FUNCTION__, __LINE__, "msg"), "");
	EXPECT_DEATH(fea::error_exit_w(__FUNCTION__, __LINE__, L"msg"), "");

	EXPECT_DEATH(fea::error_exit(__FUNCTION__, __LINE__,
						 std::error_code{ 42, std::system_category() }),
			"");
	EXPECT_DEATH(fea::error_exit_w(__FUNCTION__, __LINE__,
						 std::error_code{ 42, std::system_category() }),
			"");
}

} // namespace
