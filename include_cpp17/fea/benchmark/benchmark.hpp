/**
 * Copyright (c) 2024, Philippe Groarke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include "fea/time/time.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/unused.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

#if FEA_WINDOWS
#include <intrin.h>
#endif

namespace fea {
namespace bench {
using time_point_t = std::chrono::time_point<std::chrono::steady_clock>;

// Prints a fancy formatted title for benchmark.
inline void title(const std::string& message, FILE* stream = stdout);

// Start the benchmark.
inline time_point_t start(
		const std::string& message = {}, FILE* stream = stdout);

// Stop the benchmark. Returns time in seconds.
inline double stop(time_point_t start_time, const std::string& message = {},
		FILE* stream = stdout);

/**
 * This deactivates compiler optimizations for the passed pointer.
 * It indicates that the pointer memory "could" have been modified.
 *
 * Usage: Pass the pointer to an allocated object you want to benchmark.
 * https://www.youtube.com/watch?v=nXaxk27zwlk
 */
inline void escape(void* p);

/**
 * This method deactivates compiler optimizations by indicating all memory
 * could have been modified.
 *
 * Usage: Use after a call, to make sure the compiler doesn't remove the call.
 * https://www.youtube.com/watch?v=nXaxk27zwlk
 */
inline void clobber();

struct suite {
	// Set the title for the benchmark run. Optional.
	inline void title(const std::string& message);

	// Run each benchmark num_runs times and average results.
	inline void average(size_t num_runs);

	// Useful when profiling. Sleeps in between runs of the benchmarks.
	inline void sleep_between(std::chrono::seconds seconds);

	// Useful when profiling. Sleeps in between runs of the benchmarks.
	inline void sleep_between(std::chrono::milliseconds milli_seconds);

	// Run a benchmark on func.
	// If averaging was set, will average the times.
	// Pass in message (name of the benchmark).
	// Pass in callable benchmark function or lambda.
	// Pass in a function that will be executed in between runs
	// (useful when averaging to reset things). This function isn't measured.
	// It is executed after each call to func.
	template <class Func, class InBetweenFunc>
	void benchmark(const std::string& message, Func&& func,
			InBetweenFunc&& inbetween_func);

	// Run a benchmark on func.
	// If averaging was set, will average the times.
	// Pass in message (name of the benchmark).
	template <class Func>
	void benchmark(const std::string& message, Func&& func);

	// Print the results of the benchmark run to selected stream output
	// (defaults to stdout).
	// Resets the suite to accept new benchmarks.
	inline void print(FILE* stream = stdout);

private:
	using clock_duration_t = std::chrono::steady_clock::duration;

	struct pair {
		inline pair(const std::string& msg, double t);
		pair() = default;

		std::string message;
		double time{ 0.0 };
	};

	std::string _title;
	size_t _num_average = 1;
	std::chrono::milliseconds _sleep_between{ 0 };
	std::vector<pair> _results;
};
} // namespace bench
} // namespace fea

#include "imp/benchmark.imp.hpp"
