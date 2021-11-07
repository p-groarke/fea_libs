#pragma once
#include "fea/utils/platform.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fea {
FEA_PACKED(struct bmp_header {
	uint16_t magic = 0x4D42;
	uint32_t size = 0; // must fill
	uint16_t reserved1 = 0;
	uint16_t reserved2 = 0;
	uint32_t pixel_addr = sizeof(bmp_header);

	// dib
	uint32_t dib_size = 40;
	int32_t width = 0; // must fill
	int32_t height = 0; // must fill
	uint16_t color_planes = 1;
	uint16_t bits_per_pixel = 0; // must fill
	uint32_t compression = 0;
	uint32_t image_size = 0; // must fill
	int32_t horiz_res = 2835; // 72 dpi
	int32_t vert_res = 2835; // 72 dpi
	uint32_t num_palette_colors = 0;
	uint32_t num_imporant_colors = 0;
});
static_assert(sizeof(bmp_header) == 54, "bmp_header : wrong size");


struct bmp {
	// Expects top to bottom, packed RGB pixels.
	// If alpha is present (RGBA), it will be dropped.
	bmp(int width, int height, int stride,
			const std::vector<uint8_t>& in_data) {
		// rgb or rgba
		assert(stride == 3 || stride == 4);

		data.reserve(in_data.size()); // at minimum
		size_t row_size = size_t(width) * stride;
		size_t padding_count = (width * 3) % 4;
		if (padding_count != 0) {
			padding_count = 4 - padding_count;
		}
		std::vector<uint8_t> padding(padding_count);

		// Copy data with appropriate padding, and invert rgb to bgr.
		for (size_t i = 0; i < in_data.size(); i += row_size) {
			for (size_t j = 0; j < row_size; j += stride) {
				size_t idx = i + j;
				data.push_back(in_data[idx + 2]);
				data.push_back(in_data[idx + 1]);
				data.push_back(in_data[idx]);
			}
			data.insert(data.end(), padding.begin(), padding.end());
		}

		header.size = uint32_t(sizeof(bmp_header) + data.size());
		header.width = width;
		header.height = -height; // top to bottom
		header.bits_per_pixel = 24;
		header.image_size = uint32_t(data.size());
	}

	void write(const std::filesystem::path& filepath) const {
		std::ofstream ofs(filepath, std::ios::binary);
		if (!ofs.is_open()) {
			return;
		}
		ofs.write(reinterpret_cast<const char*>(&header), sizeof(bmp_header));
		ofs.write(reinterpret_cast<const char*>(data.data()), data.size());
	}

	bmp_header header;
	std::vector<uint8_t> data;
};

} // namespace fea
