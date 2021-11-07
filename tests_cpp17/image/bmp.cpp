#include <fea/image/bmp.hpp>
#include <gtest/gtest.h>

namespace {
TEST(image, bmp) {
	{
		std::vector<uint8_t> test_data{ 255, 0, 0 };
		const std::vector<uint8_t> expect_data{ 0, 0, 255, 0 };
		const size_t expect_size = expect_data.size();

		fea::bmp img(1, 1, 3, test_data);

		EXPECT_EQ(img.header.size, sizeof(fea::bmp_header) + expect_size);
		EXPECT_EQ(img.header.width, 1);
		EXPECT_EQ(img.header.height, -1);
		EXPECT_EQ(img.header.image_size, expect_size);
		EXPECT_EQ(img.data, expect_data);
	}
	{
		std::vector<uint8_t> test_data{ 255, 0, 0, 255, 0, 0 };
		const std::vector<uint8_t> expect_data{ 0, 0, 255, 0, 0, 255, 0, 0 };
		const size_t expect_size = expect_data.size();

		fea::bmp img(2, 1, 3, test_data);

		EXPECT_EQ(img.header.size, sizeof(fea::bmp_header) + expect_size);
		EXPECT_EQ(img.header.width, 2);
		EXPECT_EQ(img.header.height, -1);
		EXPECT_EQ(img.header.image_size, expect_size);
		EXPECT_EQ(img.data, expect_data);
	}
	{
		std::vector<uint8_t> test_data{ 255, 0, 0, 255, 0, 0, 255, 0, 0 };
		const std::vector<uint8_t> expect_data{ 0, 0, 255, 0, 0, 255, 0, 0, 255,
			0, 0, 0 };
		const size_t expect_size = expect_data.size();

		fea::bmp img(3, 1, 3, test_data);

		EXPECT_EQ(img.header.size, sizeof(fea::bmp_header) + expect_size);
		EXPECT_EQ(img.header.width, 3);
		EXPECT_EQ(img.header.height, -1);
		EXPECT_EQ(img.header.image_size, expect_size);
		EXPECT_EQ(img.data, expect_data);
	}
	{
		std::vector<uint8_t> test_data{ 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0,
			0 };
		const std::vector<uint8_t> expect_data{ 0, 0, 255, 0, 0, 255, 0, 0, 255,
			0, 0, 255 };
		const size_t expect_size = expect_data.size();

		fea::bmp img(4, 1, 3, test_data);

		EXPECT_EQ(img.header.size, sizeof(fea::bmp_header) + expect_size);
		EXPECT_EQ(img.header.width, 4);
		EXPECT_EQ(img.header.height, -1);
		EXPECT_EQ(img.header.image_size, expect_size);
		EXPECT_EQ(img.data, expect_data);
	}
}
} // namespace
