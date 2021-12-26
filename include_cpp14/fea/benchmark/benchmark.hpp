/**
 * Copyright (c) 2022, Philippe Groarke <philippe.groarke@gmail.com>.
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
#include "fea/utils/unused.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>
#include <vector>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace fea {
namespace bench {
static std::chrono::time_point<std::chrono::steady_clock> start_time, end_time;

static inline void title(const char* message, FILE* stream = stdout) {
	fea::unused(message);
	fprintf(stream, "%.*s\n", int(strlen(message)),
			"############################################################");
	fprintf(stream, "%s\n", message);
	fprintf(stream, "%.*s\n", int(strlen(message)),
			"############################################################");
}

static inline void start(const char* message = "", FILE* stream = stdout) {
	if (strlen(message) != 0) {
		fprintf(stream, "\n%s\n", message);
		fprintf(stream, "%.*s\n", int(strlen(message)),
				"--------------------------------------------------------");
	}

	start_time = std::chrono::steady_clock::now();
}

static inline double stop(const char* message = "", FILE* stream = stdout) {
	fea::unused(message);
	end_time = std::chrono::steady_clock::now();
	const std::chrono::duration<double> elapsed_time = end_time - start_time;

	fprintf(stream, "%s%*fs\n", message, 70 - int(strlen(message)),
			elapsed_time.count());
	return elapsed_time.count();
}

/**
 * This deactivates compiler optimizations for the passed pointer.
 * It indicates that the pointer memory "could" have been modified.
 *
 * Usage: Pass the pointer to an allocated object you want to benchmark.
 * https://www.youtube.com/watch?v=nXaxk27zwlk
 */
static inline void escape(void* p) {
#ifdef _MSC_VER
	fea::unused(p);
	_WriteBarrier();
#else
	asm volatile("" : : "g"(p) : "memory");
#endif
}

/**
 * This method deactivates compiler optimizations by indicating all memory
 * could have been modified.
 *
 * Usage: Use after a call, to make sure the compiler doesn't remove the call.
 * https://www.youtube.com/watch?v=nXaxk27zwlk
 */
static inline void clobber() {
#ifdef _MSC_VER
	_ReadBarrier();
#else
	asm volatile("" : : : "memory");
#endif
}

struct suite {
	// Set the title for the benchmark run. Optional.
	void title(const char* message) {
		_title = message;
	}

	// Run each benchmark num_runs times and average results.
	void average(size_t num_runs) {
		if (num_runs == 0) {
			return;
		}
		_num_average = num_runs;
	}

	// Useful when profiling. Sleeps in between runs of the benchmarks.
	void sleep_between(std::chrono::seconds seconds) {
		_sleep_between = std::chrono::milliseconds(seconds);
	}
	void sleep_between(std::chrono::milliseconds milli_seconds) {
		_sleep_between = milli_seconds;
	}

	// Run a benchmark on func.
	// If averaging was set, will average the times.
	// Pass in message (name of the benchmark).
	// Pass in callable benchmark function or lambda.
	// Pass in a function that will be executed in between runs
	// (useful when averaging to reset things). This function isn't measured.
	// It is executed after each call to func.
	template <class Func, class InBetweenFunc>
	void benchmark(
			const char* message, Func&& func, InBetweenFunc&& inbetween_func) {

		clock_duration_t elapsed_time = clock_duration_t(0);
		std::this_thread::sleep_for(_sleep_between);

		for (size_t i = 0; i < _num_average; ++i) {
			std::chrono::time_point<std::chrono::steady_clock> _start_time,
					_end_time;

			_start_time = std::chrono::steady_clock::now();
			func();
			_end_time = std::chrono::steady_clock::now();

			elapsed_time += _end_time - _start_time;

			inbetween_func();
		}

		fea::dseconds elapsed_d(elapsed_time);
		_results.push_back(
				pair{ message, elapsed_d.count() / double(_num_average) });
	}

	// Run a benchmark on func.
	// If averaging was set, will average the times.
	// Pass in message (name of the benchmark).
	template <class Func>
	void benchmark(const char* message, Func&& func) {
		benchmark(message, std::forward<Func>(func), []() {});
	}

	// Print the results of the benchmark run to selected stream output
	// (defaults to stdout).
	// Resets the suite to accept new benchmarks.
	void print(FILE* stream = stdout) {
		std::this_thread::sleep_for(_sleep_between);

		if (_title != nullptr) {
			fprintf(stream, "%.*s\n", int(strlen(_title)),
					"##########################################################"
					"##");
			fprintf(stream, "%s\n", _title);
			fprintf(stream, "%.*s\n", int(strlen(_title)),
					"##########################################################"
					"##");
		}

		if (_results.empty())
			return;

		if (_results.size() > 1) {
			std::sort(_results.begin(), _results.end(),
					[](const pair& lhs, const pair& rhs) {
						return lhs.time < rhs.time;
					});
		}

		for (const pair& p : _results) {
			double ratio = _results.back().time / p.time;
			fprintf(stream, "%s%*fs        %fx\n", p.message,
					70 - int(strlen(p.message)), p.time, ratio);
		}
		fprintf(stream, "%s", "\n");

		_results.clear();
	}

private:
	using clock_duration_t = std::chrono::steady_clock::duration;

	struct pair {
		pair(const char* msg, double t)
				: message(msg)
				, time(t) {
		}
		pair() = default;

		const char* message{ nullptr };
		double time{ 0.0 };
	};

	const char* _title{ nullptr };
	size_t _num_average = 1;
	std::chrono::milliseconds _sleep_between{ 0 };
	std::vector<pair> _results;
};
} // namespace bench
} // namespace fea
