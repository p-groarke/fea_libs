/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2021, Philippe Groarke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/

#pragma once
#include "fea/utils/platform.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <vector>


/*
The bmp class is a helper for debugging images.
It is FAR from a complete bmp implementation. It cannot even load
bmps. It is simply here when you need to quickly and simply output
an image.

For single header image loading/saving, check out stb.
https://github.com/nothings/stb
*/

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
	bmp(int width, int height, int pixel_stride,
			const std::vector<uint8_t>& in_data) {
		// rgb or rgba
		assert(pixel_stride == 3 || pixel_stride == 4);

		data.reserve(in_data.size()); // at minimum
		size_t row_size = size_t(width) * pixel_stride;
		size_t padding_count = (width * 3) % 4;
		if (padding_count != 0) {
			padding_count = 4 - padding_count;
		}
		std::vector<uint8_t> padding(padding_count);

		// Copy data with appropriate padding, and invert rgb to bgr.
		for (size_t i = 0; i < in_data.size(); i += row_size) {
			for (size_t j = 0; j < row_size; j += pixel_stride) {
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
	std::vector<uint8_t> data; // Padded bgr, top to bottom.
};

} // namespace fea
