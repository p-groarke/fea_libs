namespace fea {
namespace bench {
void title(const std::string& message, FILE* stream) {
	fea::unused(message);
	fprintf(stream, "%.*s\n", int(message.size()),
			"############################################################");
	fprintf(stream, "%s\n", message.c_str());
	fprintf(stream, "%.*s\n", int(message.size()),
			"############################################################");
}

time_point_t start(const std::string& message, FILE* stream) {
	if (!message.empty()) {
		fprintf(stream, "\n%s\n", message.c_str());
		fprintf(stream, "%.*s\n", int(message.size()),
				"--------------------------------------------------------");
	}

	return std::chrono::steady_clock::now();
}

double stop(time_point_t start_time, const std::string& message, FILE* stream) {
	fea::unused(message);
	time_point_t end_time = std::chrono::steady_clock::now();
	const std::chrono::duration<double> elapsed_time = end_time - start_time;

	fprintf(stream, "%s%*fs\n", message.c_str(), 70 - int(message.size()),
			elapsed_time.count());
	return elapsed_time.count();
}

void escape(void* p) {
#if FEA_WINDOWS
	fea::unused(p);
	_WriteBarrier();
#else
	asm volatile("" : : "g"(p) : "memory");
#endif
}

void clobber() {
#if FEA_WINDOWS
	_ReadBarrier();
#else
	asm volatile("" : : : "memory");
#endif
}

void suite::title(const std::string& message) {
	_title = message;
}

void suite::average(size_t num_runs) {
	if (num_runs == 0) {
		return;
	}
	_num_average = num_runs;
}

void suite::sleep_between(std::chrono::seconds seconds) {
	_sleep_between = std::chrono::milliseconds(seconds);
}

void suite::sleep_between(std::chrono::milliseconds milli_seconds) {
	_sleep_between = milli_seconds;
}

template <class Func, class InBetweenFunc>
void suite::benchmark(const std::string& message, Func&& func,
		InBetweenFunc&& inbetween_func) {

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

template <class Func>
void suite::benchmark(const std::string& message, Func&& func) {
	benchmark(message, std::forward<Func>(func), []() {});
}

void suite::print(FILE* stream) {
	std::this_thread::sleep_for(_sleep_between);

	if (!_title.empty()) {
		fprintf(stream, "%.*s\n", int(_title.size()),
				"##########################################################"
				"##");
		fprintf(stream, "%s\n", _title.c_str());
		fprintf(stream, "%.*s\n", int(_title.size()),
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
		fprintf(stream, "%s%*fs        %fx\n", p.message.c_str(),
				70 - int(p.message.size()), p.time, ratio);
	}
	fprintf(stream, "%s", "\n");

	_results.clear();
}

suite::pair::pair(const std::string& msg, double t)
		: message(msg)
		, time(t) {
}
} // namespace bench
} // namespace fea
