#pragma once
#include "fea/time/time.hpp"

namespace fea {

struct delta_time {
	dseconds compute_dt() {
		auto last_frame_t = _new_frame_time;
		_new_frame_time = std::chrono::steady_clock::now();
		_dt = _new_frame_time - last_frame_t;
		return _dt;
	}

	dseconds dt() const {
		return _dt;
	}

	fseconds fdt() const {
		return _dt;
	}

private:
	std::chrono::steady_clock::time_point _new_frame_time
			= std::chrono::steady_clock::now();
	std::chrono::steady_clock::duration _dt;
};

} // namespace fea
