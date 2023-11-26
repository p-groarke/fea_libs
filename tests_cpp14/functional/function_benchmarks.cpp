#include <fea/utils/platform.hpp>
#if FEA_RELEASE && FEA_CPP17 && defined(FEA_BENCHMARKS)

#include <fea/benchmark/benchmark.hpp>
#include <fea/functional/callback.hpp>
#include <fea/functional/function.hpp>
#include <fea/meta/pack.hpp>
#include <fea/meta/traits.hpp>
#include <gtest/gtest.h>
#include <random>
#include <vector>

namespace {
template <class>
struct raw_function_with_erasure;

template <class FuncRet, class... FuncArgs>
struct raw_function_with_erasure<FuncRet(FuncArgs...)> {
	using c_function_t = FuncRet (*)(FuncArgs...);
	using member_function_t = fea::member_func_ptr_t<FuncRet, FuncArgs...>;
	using erased_t = FuncRet (raw_function_with_erasure::*)(FuncArgs...) const;

	// Can be default constructed.
	constexpr raw_function_with_erasure() = default;

	// Accept raw function pointers and captureless lambdas.
	constexpr raw_function_with_erasure(c_function_t func)
			: _c_func(func)
			, _erased(&raw_function_with_erasure::call_c_func) {
	}

	// Accept member pointers.
	template <class T, class... OtherArgs>
	constexpr raw_function_with_erasure(FuncRet (T::*func)(OtherArgs...))
			: _member_func(func)
			, _erased(&raw_function_with_erasure::call_member_func) {

		static_assert(std::is_same<fea::front_t<FuncArgs...>, T*>::value,
				"fea::raw_function : first argument of member function pointer "
				"must be pointer to the class");

		static_assert(std::is_same<fea::pack<OtherArgs...>,
							  fea::idx_splice_after_t<0, FuncArgs...>>::value,
				"fea::raw_function : signature mismatch between provided "
				"function and expected function");
	}

	constexpr FuncRet operator()(FuncArgs... func_args) const {
		return (this->*_erased)(std::forward<FuncArgs>(func_args)...);
	}

private:
	template <class T, class... Rest>
	constexpr FuncRet call_member_impl(T* obj, Rest&&... rest_args) const {
		return (obj->*_member_func)(std::forward<Rest>(rest_args)...);
	}

	constexpr FuncRet call_member_func(FuncArgs... func_args) const {
		assert(_member_func != nullptr);
		return call_member_impl(std::forward<FuncArgs>(func_args)...);
	}

	constexpr FuncRet call_c_func(FuncArgs... func_args) const {
		assert(_c_func != nullptr);
		return _c_func(std::forward<FuncArgs>(func_args)...);
	}

	c_function_t _c_func = nullptr;
	member_function_t _member_func = nullptr;
	erased_t _erased = nullptr;
};

template <class>
struct raw_function_with_if;

template <class FuncRet, class... FuncArgs>
struct raw_function_with_if<FuncRet(FuncArgs...)> {
	using c_function_t = FuncRet (*)(FuncArgs...);
	using member_function_t = fea::member_func_ptr_t<FuncRet, FuncArgs...>;

	// Can be default constructed.
	constexpr raw_function_with_if() = default;

	// Accept raw function pointers and captureless lambdas.
	constexpr raw_function_with_if(c_function_t func)
			: _c_func(func) {
	}

	// Accept member pointers.
	template <class T, class... OtherArgs>
	constexpr raw_function_with_if(FuncRet (T::*func)(OtherArgs...))
			: _member_func(func) {

		static_assert(std::is_same<fea::front_t<FuncArgs...>, T*>::value,
				"fea::raw_function : first argument of member function pointer "
				"must be pointer to the class");

		static_assert(std::is_same<fea::pack<OtherArgs...>,
							  fea::idx_splice_after_t<0, FuncArgs...>>::value,
				"fea::raw_function : signature mismatch between provided "
				"function and expected function");
	}

	constexpr FuncRet operator()(FuncArgs... func_args) const {
		if (_c_func) {
			return _c_func(std::forward<FuncArgs>(func_args)...);
		}

		return do_member(std::forward<FuncArgs>(func_args)...);
	}

private:
	template <class T, class... Rest>
	constexpr FuncRet do_member(T* obj, Rest&&... rest_args) const {
		return (obj->*_member_func)(std::forward<Rest>(rest_args)...);
	}

	c_function_t _c_func = nullptr;
	member_function_t _member_func = nullptr;
};


template <class>
struct raw_function_with_union;

template <class FuncRet, class... FuncArgs>
struct raw_function_with_union<FuncRet(FuncArgs...)> {
	using c_function_t = FuncRet (*)(FuncArgs...);
	using member_function_t = fea::member_func_ptr_t<FuncRet, FuncArgs...>;

	// Can be default constructed.
	constexpr raw_function_with_union() = default;

	// Accept raw function pointers and captureless lambdas.
	constexpr raw_function_with_union(c_function_t func) {
		_func_union.c_func = func;
	}

	// Accept member pointers.
	template <class T, class... OtherArgs>
	constexpr raw_function_with_union(FuncRet (T::*func)(OtherArgs...))
			: _is_member(true) {
		_func_union.mem_func = func;

		static_assert(std::is_same<fea::front_t<FuncArgs...>, T*>::value,
				"fea::raw_function : first argument of member function "
				"pointer "
				"must be pointer to the class");

		static_assert(std::is_same<fea::pack<OtherArgs...>,
							  fea::idx_splice_after_t<0, FuncArgs...>>::value,
				"fea::raw_function : signature mismatch between provided "
				"function and expected function");
	}

	constexpr FuncRet operator()(FuncArgs... func_args) const {
		if (!_is_member) {
			return _func_union.c_func(std::forward<FuncArgs>(func_args)...);
		}

		return do_member(std::forward<FuncArgs>(func_args)...);
	}

private:
	// Function must be generated for compiler. Should never be called.
	template <class... Ignored>
	constexpr FuncRet do_member(Ignored&&...) const {
		assert(false);
	}

	template <class T, class... Rest>
	constexpr FuncRet do_member(T* obj, Rest&&... rest_args) const {
		return (obj->*_func_union.mem_func)(std::forward<Rest>(rest_args)...);
	}

	union func_u {
		c_function_t c_func;
		member_function_t mem_func;
		// const_member_function_t const_mem_func;
	};

	func_u _func_union = { nullptr };
	bool _is_member = false;
};

struct bench_obj {
	void func(size_t& i) {
		++i;
	}
};


TEST(function_cl, benchmarks) {
	constexpr size_t bench_count = 100'000'000;

	std::random_device rnd_device;
	std::mt19937_64 engine{ rnd_device() };
	std::uniform_int_distribution<size_t> dist{ 1, 5 };

	bench_obj obj;
	size_t answer = 0;
	fea::bench::suite suite;
	suite.average(5);
	suite.sleep_between(std::chrono::milliseconds{ 250 });
	using raw_ptr_t = void (bench_obj::*)(size_t&);

	{
		raw_ptr_t raw_ptr = &bench_obj::func;
		std::function<void(bench_obj*, size_t&)> std_function
				= &bench_obj::func;
		raw_function_with_erasure<void(bench_obj*, size_t&)>
				fea_raw_function_with_erasure = &bench_obj::func;
		raw_function_with_if<void(bench_obj*, size_t&)> fea_raw_function_with_if
				= &bench_obj::func;
		raw_function_with_union<void(bench_obj*, size_t&)>
				fea_raw_function_with_union = &bench_obj::func;
		fea::function_cl<void(bench_obj*, size_t&)> fea_function_cl
				= &bench_obj::func;
		auto fea_callback = fea::make_callback([&](size_t& i) { obj.func(i); });

		std::string title = std::string{ "Calling Callable " }
				+ std::to_string(bench_count) + std::string{ " Times" };
		suite.title(title.c_str());
		suite.benchmark("Raw Member Pointer", [&]() {
			for (size_t i = 0; i < bench_count; ++i) {
				(obj.*raw_ptr)(answer);
			}
		});
		suite.benchmark("std::function", [&]() {
			for (size_t i = 0; i < bench_count; ++i) {
				std_function(&obj, answer);
			}
		});
		suite.benchmark("fea::raw_function with erasure", [&]() {
			for (size_t i = 0; i < bench_count; ++i) {
				fea_raw_function_with_erasure(&obj, answer);
			}
		});
		suite.benchmark("fea::raw_function with if", [&]() {
			for (size_t i = 0; i < bench_count; ++i) {
				fea_raw_function_with_if(&obj, answer);
			}
		});
		suite.benchmark("fea::raw_function with union", [&]() {
			for (size_t i = 0; i < bench_count; ++i) {
				fea_raw_function_with_union(&obj, answer);
			}
		});
		suite.benchmark("fea::function_cl", [&]() {
			for (size_t i = 0; i < bench_count; ++i) {
				fea_function_cl(&obj, answer);
			}
		});
		suite.benchmark("fea::callback", [&]() {
			for (size_t i = 0; i < bench_count; ++i) {
				fea_callback(answer);
			}
		});
		suite.print();
	}

	{
		constexpr size_t vec_size = 10'000'000;
		answer = 0;
		std::vector<raw_ptr_t> raw_vec(vec_size, &bench_obj::func);
		std::vector<std::function<void(bench_obj*, size_t&)>> std_function_vec(
				vec_size, &bench_obj::func);
		std::vector<raw_function_with_erasure<void(bench_obj*, size_t&)>>
				fea_raw_function_with_erasure_vec(vec_size, &bench_obj::func);
		std::vector<raw_function_with_if<void(bench_obj*, size_t&)>>
				fea_raw_function_with_if_vec(vec_size, &bench_obj::func);
		std::vector<raw_function_with_union<void(bench_obj*, size_t&)>>
				fea_raw_function_with_union_vec(vec_size, &bench_obj::func);
		std::vector<fea::function_cl<void(bench_obj*, size_t&)>>
				fea_function_cl_vec(vec_size, &bench_obj::func);

		auto fea_callback = fea::make_callback([&](size_t& i) { obj.func(i); });
		using callback_t = decltype(fea_callback)::func_t;
		std::vector<fea::callback<callback_t, void(size_t&)>> fea_callback_vec(
				vec_size, fea_callback);

		std::string title = std::to_string(vec_size)
				+ " Callables Stored In A Vector, Iterated Linearly";
		suite.title(title.c_str());
		suite.benchmark("Raw Member Pointer", [&]() {
			for (size_t i = 0; i < raw_vec.size(); ++i) {
				(obj.*raw_vec[i])(answer);
			}
		});
		suite.benchmark("std::function", [&]() {
			for (size_t i = 0; i < raw_vec.size(); ++i) {
				std_function_vec[i](&obj, answer);
			}
		});
		suite.benchmark("fea::raw_function with erasure", [&]() {
			for (size_t i = 0; i < raw_vec.size(); ++i) {
				fea_raw_function_with_erasure_vec[i](&obj, answer);
			}
		});
		suite.benchmark("fea::raw_function with if", [&]() {
			for (size_t i = 0; i < raw_vec.size(); ++i) {
				fea_raw_function_with_if_vec[i](&obj, answer);
			}
		});
		suite.benchmark("fea::raw_function with union", [&]() {
			for (size_t i = 0; i < raw_vec.size(); ++i) {
				fea_raw_function_with_union_vec[i](&obj, answer);
			}
		});
		suite.benchmark("fea::function_cl", [&]() {
			for (size_t i = 0; i < raw_vec.size(); ++i) {
				fea_function_cl_vec[i](&obj, answer);
			}
		});
		suite.benchmark("fea::callback", [&]() {
			for (size_t i = 0; i < raw_vec.size(); ++i) {
				fea_callback_vec[i](answer);
			}
		});
		suite.print();


		// Generate random indexes, the same for all benchmarks.
		std::uniform_int_distribution<size_t> vec_dist{ 0, raw_vec.size() - 1 };
		std::vector<size_t> random_idxes(raw_vec.size());
		std::generate(random_idxes.begin(), random_idxes.end(),
				[&]() { return vec_dist(engine); });


		title = std::to_string(vec_size)
				+ " Callables Stored In A Vector, Iterated Randomly";
		suite.title(title.c_str());
		suite.benchmark("Raw Member Pointer", [&]() {
			for (size_t i = 0; i < raw_vec.size(); ++i) {
				size_t idx = random_idxes[i];
				(obj.*raw_vec[idx])(answer);
			}
		});
		suite.benchmark("std::function", [&]() {
			for (size_t i = 0; i < raw_vec.size(); ++i) {
				size_t idx = random_idxes[i];
				std_function_vec[idx](&obj, answer);
			}
		});
		suite.benchmark("fea::raw_function with erasure", [&]() {
			for (size_t i = 0; i < raw_vec.size(); ++i) {
				size_t idx = random_idxes[i];
				fea_raw_function_with_erasure_vec[idx](&obj, answer);
			}
		});
		suite.benchmark("fea::raw_function with if", [&]() {
			for (size_t i = 0; i < raw_vec.size(); ++i) {
				size_t idx = random_idxes[i];
				fea_raw_function_with_if_vec[idx](&obj, answer);
			}
		});
		suite.benchmark("fea::raw_function with union", [&]() {
			for (size_t i = 0; i < raw_vec.size(); ++i) {
				size_t idx = random_idxes[i];
				fea_raw_function_with_union_vec[idx](&obj, answer);
			}
		});
		suite.benchmark("fea::function_cl", [&]() {
			for (size_t i = 0; i < raw_vec.size(); ++i) {
				size_t idx = random_idxes[i];
				fea_function_cl_vec[idx](&obj, answer);
			}
		});
		suite.benchmark("fea::callback", [&]() {
			for (size_t i = 0; i < raw_vec.size(); ++i) {
				size_t idx = random_idxes[i];
				fea_callback_vec[idx](answer);
			}
		});
		suite.print();
	}

	// constexpr size_t s
	//		= sizeof(raw_function_with_union<void(bench_obj*, size_t&)>);

	printf("\n\n%zu\n", answer);
}
} // namespace

#endif
