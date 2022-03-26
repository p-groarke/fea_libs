#include <fea/utils/tweak_value.hpp>
#include <gtest/gtest.h>

// Tweak values only work in debug.
#if FEA_DEBUG

namespace {
float get_tweak_val() {
	return FEA_TWEAK(42.f);
}

TEST(tweak_value, basics) {
	float val = get_tweak_val();
	EXPECT_EQ(val, 42.f);

	fea::tweak_update();
	val = get_tweak_val();
	EXPECT_EQ(val, 42.f);

	// Modify this file.
	{
		std::ofstream ofs{ __FILE__ };
		ASSERT_TRUE(ofs.is_open());
		ofs << " ";
	}

	fea::tweak_update();
	val = get_tweak_val();
	EXPECT_EQ(val, 42.f);

	// Modify tweak value.
	{
		std::ifstream ifs{ __FILE__ };
		ASSERT_TRUE(ifs.is_open());
		std::string file_data
				= fea::any_to_utf8(fea::open_text_file_with_bom(ifs));
		ifs.close();

		std::ofstream ofs{ __FILE__ };
		ASSERT_TRUE(ofs.is_open());
		ofs << file_data;
	}
}

} // namespace
#endif
