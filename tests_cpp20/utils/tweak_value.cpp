#include <fea/utils/platform.hpp>
#if !defined(FEA_MACOS)

#include <chrono>
#include <fea/utils/scope.hpp>
#include <fea/utils/tweak_value.hpp>
#include <gtest/gtest.h>
#include <thread>

// Tweak values only work in debug.
#if FEA_DEBUG
namespace {
using namespace std::chrono_literals;

std::string open_current_file() {
	std::ifstream ifs{ __FILE__ };
	assert(ifs.is_open());
	return fea::any_to_utf8(fea::open_text_file_with_bom(ifs));
}

void save_current_file(const std::string& data) {

	std::ofstream ofs{ __FILE__ };
	assert(ofs.is_open());
	ofs << data;
}

struct restore_file {
	restore_file()
			: file_backup(open_current_file()) {
	}

	~restore_file() {
		save_current_file(file_backup);
	}

	std::string file_backup;
};

float get_tweak_val() {
	float u = FEA_TWEAK(0.f);
	float u2 = FEA_TWEAK(1.f);
	EXPECT_EQ(u, 0.f);
	EXPECT_EQ(u2, 1.f);

	return FEA_TWEAK(42.f);
}
int get_tweak_val2() {
	int u = FEA_TWEAK(0);
	int u2 = FEA_TWEAK(1);
	EXPECT_EQ(u, 0);
	EXPECT_EQ(u2, 1);

	return FEA_TWEAK(42);
}

TEST(tweak_value, basics) {
	restore_file file_restorer5000;

	float val = get_tweak_val();
	EXPECT_EQ(val, 42.f);
	int val2 = get_tweak_val2();
	EXPECT_EQ(val2, 42);

	std::this_thread::sleep_for(200ms);
	fea::tweak_update();

	val = get_tweak_val();
	EXPECT_EQ(val, 42.f);
	val2 = get_tweak_val2();
	EXPECT_EQ(val2, 42);

	// Modify this file.
	{
		std::ofstream ofs{ __FILE__, std::ios::app };
		ASSERT_TRUE(ofs.is_open());
		ofs << "\n";
	}

	std::this_thread::sleep_for(200ms);
	fea::tweak_update();

	val = get_tweak_val();
	EXPECT_EQ(val, 42.f);
	val2 = get_tweak_val2();
	EXPECT_EQ(val2, 42);

	// Modify tweak value.
	std::string file_data = open_current_file();
	fea::replace_all_inplace(file_data, "FEA_TWEAK(42.f)", "FEA_TWEAK(101.f)");
	save_current_file(file_data);

	std::this_thread::sleep_for(200ms);
	fea::tweak_update();

	val = get_tweak_val();
	EXPECT_EQ(val, 101.f);
	val2 = get_tweak_val2();
	EXPECT_EQ(val2, 42);

	fea::replace_all_inplace(file_data, "FEA_TWEAK(42)", "FEA_TWEAK(-1)");
	save_current_file(file_data);

	std::this_thread::sleep_for(200ms);
	fea::tweak_update();

	val = get_tweak_val();
	EXPECT_EQ(val, 101.f);
	val2 = get_tweak_val2();
	EXPECT_EQ(val2, -1);
}


// TEST(tweak_value, demo) {
// restore_file file_restorer5000;
//	size_t loop_cnt = 0;
//
//	float tval = 0.f;
//	while (tval != 69.f) {
//
//		tval = FEA_TWEAK(69.f);
//		printf("%f\n", tval);
//
//		fea::tweak_update();
//		std::this_thread::sleep_for(1s);
//
//		// Just in case.
//		++loop_cnt;
//		if (loop_cnt == 100) {
//			break;
//		}
//	}
//}


} // namespace
#endif // FEA_DEBUG
#endif // !FEA_MACOS
