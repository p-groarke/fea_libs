#include <version>
#if defined(__cpp_lib_to_chars)

#include <array>
#include <charconv>
#include <cstdio>
#include <gtest/gtest.h>
#include <regex>
#include <sstream>
//#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

// Our scripting engine.
struct runtime;

// Our command parsers.
template <class T>
bool parse_let(std::string&&, T&&, runtime&);

template <class T, class U>
bool parse_add(T&&, U&&, runtime&);

template <class T>
bool parse_print(T&&);

// Helpers
using svmatch = std::match_results<std::string_view::const_iterator>;
using svsub_match = std::sub_match<std::string_view::const_iterator>;
using svregex_iterator = std::regex_iterator<std::string_view::const_iterator>;

inline std::string_view to_sv(const svsub_match& m) {
	return std::string_view(&(*m.first), m.length());
}

namespace detail {
// struct err_val {
//	~nonesuch() = delete;
//	nonesuch(nonesuch const&) = delete;
//	void operator=(nonesuch const&) = delete;
//};

template <class T, class Tuple>
struct tuple_idx;

template <class T, class... Types>
struct tuple_idx<T, std::tuple<T, Types...>> {
	static constexpr size_t value = 0;
};

template <class T, class U, class... Types>
struct tuple_idx<T, std::tuple<U, Types...>> {
	static constexpr size_t value
			= 1 + tuple_idx<T, std::tuple<Types...>>::value;
};

// Get the index of type T in Tuple.
template <class T, class Tuple>
inline constexpr size_t tuple_idx_v = detail::tuple_idx<T, Tuple>::value;

template <class>
struct is_tuple : std::false_type {};

template <class... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type {};

// Detects if type is tuple.
template <class T>
inline constexpr bool is_tuple_v = detail::is_tuple<T>::value;

template <class, class>
struct tuple_contains;

template <class T>
struct tuple_contains<T, std::tuple<>> : std::false_type {};

template <class T, class U, class... Ts>
struct tuple_contains<T, std::tuple<U, Ts...>>
		: tuple_contains<T, std::tuple<Ts...>> {};

template <class T, class... Ts>
struct tuple_contains<T, std::tuple<T, Ts...>> : std::true_type {};

// Does Tuple contain type T?
template <class T, class Tuple>
inline constexpr bool tuple_contains_v = tuple_contains<T, Tuple>::value;

template <template <class...> class, template <class...> class>
struct is_same_template : std::false_type {};

template <template <class...> class T>
struct is_same_template<T, T> : std::true_type {};

// Checks whether 2 types are the same, regardless of their template params.
template <template <class...> class T, template <class...> class U>
inline constexpr bool is_same_template_v = is_same_template<T, U>::value;

template <class...>
struct one_of : std::false_type {};

template <class...>
struct one_of_found : std::true_type {};

template <class Trait, class... Traits>
struct one_of_found<Trait, Traits...>
		: std::conditional_t<Trait::value, std::false_type,
				  one_of_found<Traits...>> {};

template <class Trait, class... Traits>
struct one_of<Trait, Traits...>
		: std::conditional_t<Trait::value, one_of_found<Traits...>,
				  one_of<Traits...>> {};

// Checks if one of the passed in traits is true.
template <class... Traits>
inline constexpr bool one_of_v = one_of<Traits...>::value;

template <class Func, size_t... Idxs>
constexpr auto static_for_imp(Func func, std::index_sequence<Idxs...>) {
	using return_t
			= std::invoke_result_t<Func, std::integral_constant<size_t, 0>>;
	if constexpr (std::is_same_v<return_t, void>) {
		(func(std::integral_constant<size_t, Idxs>{}), ...);
	} else {
		return std::make_tuple(func(std::integral_constant<size_t, Idxs>{})...);
	}
	//(func(std::integral_constant<size_t, Idxs>{}), ...);
}
// Compile-time loop.
template <size_t N, class Func>
constexpr auto static_for(Func func) {
	return static_for_imp(func, std::make_index_sequence<N>{});
}

template <class ArgConfig, class Descriptor, size_t I, size_t ArgCount,
		class... Args>
bool parse_arguments_impl(const std::vector<std::string_view>& args,
		runtime& rt, std::tuple<Args...>&& values) {
	if constexpr (I == ArgCount) {
		return std::apply(
				[&](auto&... args) {
					return Descriptor::on_parse(std::move(args)..., rt);
				},
				values);
	} else {
		// Try to parse all the args.
		// First come first served.
		using arg_enum_t = typename ArgConfig::arg_enum_t;
		using signature_t = std::decay_t<decltype(Descriptor::signature)>;
		static_assert(is_tuple_v<signature_t>,
				"Signature must be a tuple of arg_type arrays.");

		using param_arr_t = std::tuple_element_t<I, signature_t>;
		// static_assert(is_same_template_v<param_arr_t, std::array>,
		//		"Signature arguments must be arrays, even if you only accept 1 "
		//		"argument type.");

		constexpr size_t num_valid_args = std::tuple_size_v<param_arr_t>;

		bool found = false;
		// static_for<num_valid_args>([&](auto const_j) {
		static_for<num_valid_args>([&](auto const_j) {
			if (found) {
				return;
			}

			constexpr auto nested_tup = std::get<I>(Descriptor::signature);
			constexpr arg_enum_t arg_e = std::get<const_j()>(nested_tup);
			using arg_type = typename ArgConfig::template get_cpp_type<arg_e>;
			svmatch match;
			if (!std::regex_search(args[I].begin(), args[I].end(), match,
						ArgConfig::regexes[size_t(arg_e)])) {
				return;
			}

			std::string_view sv = to_sv(match[1]);
			found = true;
			auto val = std::get<size_t(arg_e)>(ArgConfig::from_strings)(sv);
			static_assert(std::is_same_v<decltype(val), arg_type>,
					"Value returned from 'from_string' doesn't match "
					"declared cpp_type.");
			parse_arguments_impl<ArgConfig, Descriptor, I + 1, ArgCount>(args,
					rt, std::tuple_cat(values, std::tuple{ std::move(val) }));
		});

		if (!found) {
			fprintf(stderr, "Couldn't parse '%.*s'.\n", int(args[I].size()),
					args[I].data());
			return false;
		}
		return true;
	}
}

// A helper to make the runtime code a little easier on the eyes.
template <class ArgConfig, class Descriptor>
bool parse_arguments(const std::vector<std::string_view>& args, runtime& rt) {
	using signature_t = decltype(Descriptor::signature);
	return parse_arguments_impl<ArgConfig, Descriptor, 0,
			std::tuple_size_v<signature_t>>(args, rt, std::tuple{});
}


// Our configurators.
// This map contains all descriptors and can be queried for various things.
template <class ArgEnum, class... Descriptors>
struct argument_config {
	// Here, we define helper aliases and variables, potentially helper
	// functions, etc.
	using arg_enum_t = ArgEnum;
	using descriptors = std::tuple<Descriptors...>;
	static constexpr size_t num_args = sizeof...(Descriptors);

	// We unroll values to make access easier.
	static constexpr auto names = std::array{ Descriptors::name... };
	using cpp_types = std::tuple<typename Descriptors::cpp_type...>;
	static const inline auto regexes = std::array{ Descriptors::parse_re... };

	// Used by our runtime.
	// A map of variable name to type & index.
	using variable_type_map_t
			= std::unordered_map<std::string, std::pair<ArgEnum, size_t>>;

	// The stored values of our arguments.
	// Indexed at their arg type index.
	using argument_map_t
			= std::tuple<std::vector<typename Descriptors::cpp_type>...>;

	// Used to do the final argument parse.
	static constexpr auto from_strings
			= std::tuple{ Descriptors::from_string... };

	// Helper to get the c++ type associated with the arg.
	template <arg_enum_t E>
	using get_cpp_type = std::tuple_element_t<size_t(E), cpp_types>;

	// Our asserts.
	static_assert(std::is_enum_v<ArgEnum>, "ArgEnum must be enum.");
	static_assert(sizeof...(Descriptors) == size_t(ArgEnum::count),
			"Missing descriptors.");

	// Just to make the implementation easier, we enforce the index of the
	// descriptor to match its enum value. This isn't necessary, but helps
	// readability in here.
	static_assert(((size_t(Descriptors::my_arg_type)
						  == tuple_idx_v<Descriptors, descriptors>)&&...),
			"Index of descriptor in variadic pack must match its enum index.");
};

template <class ArgConfig, class CommandEnum, class... Descriptors>
struct command_config {
	using arg_config = ArgConfig;
	using cmd_enum = CommandEnum;
	using descriptors = std::tuple<Descriptors...>;
	static constexpr size_t num_commands = sizeof...(Descriptors);

	static constexpr auto signatures = std::tuple{ Descriptors::signature... };
	static constexpr auto arg_counts = std::array{
		std::tuple_size_v<std::decay_t<decltype(Descriptors::signature)>>...
	};

	static constexpr auto names = std::array{ Descriptors::name... };

	// Used to parse the arguments and call the descriptor event.
	static constexpr auto arg_parsers
			= std::array{ &parse_arguments<arg_config, Descriptors>... };

	// Used to find the associated enum value of a command using a
	// string.
	static inline const std::unordered_map<std::string_view, CommandEnum>
			command_name_map{ { Descriptors::name,
					Descriptors::my_command }... };

	static_assert(std::is_enum_v<CommandEnum>, "CommandEnum must be enum.");
	static_assert(sizeof...(Descriptors) == size_t(CommandEnum::count),
			"Missing descriptors.");
	static_assert(((size_t(Descriptors::my_command)
						  == tuple_idx_v<Descriptors, descriptors>)&&...),
			"Index of descriptor in variadic pack must match its enum index.");
};
} // namespace detail

// Helper that gets an associated c++ type, given the argument enum value.
template <class ArgConfig, typename ArgConfig::arg_enum_t E>
using get_cpp_type
		= std::tuple_element_t<size_t(E), typename ArgConfig::cpp_types>;

// Options for our descriptors.
enum class arg_type : unsigned {
	variable, // a string without quotes, ex : var
	string, // a string with quotes, ex : "string"
	integer, // an int, ex : 42
	floating, // a float, ex : 42.f
	count,
};

// Our argument descriptors.
struct variable_descriptor {
	// The argument type.
	static constexpr arg_type my_arg_type = arg_type::variable;

	// The argument name.
	static constexpr std::string_view name = "variable";

	// The c++ type this argument represents.
	using cpp_type = std::string;

	// Used to parse the argument.
	static inline const std::regex parse_re{ "([[:alpha:]_]+[[:alnum:]_]+)" };

	// Converts your captured argument string to a c++ type.
	static constexpr auto from_string
			= [](std::string_view str) { return std::string(str); };
};

struct string_descriptor {
	static constexpr arg_type my_arg_type = arg_type::string;
	static constexpr std::string_view name = "string";
	using cpp_type = std::string;
	static inline const std::regex parse_re{ "['\"](.+?)['\"]" };
	static constexpr auto from_string
			= [](std::string_view str) { return std::string(str); };
};

struct integer_descriptor {
	static constexpr arg_type my_arg_type = arg_type::integer;
	static constexpr std::string_view name = "int";
	using cpp_type = int;
	static inline const std::regex parse_re{ "([[:digit:]]+)" };
	static constexpr auto from_string = [](std::string_view str) {
		int ret = 0;
		std::from_chars(str.data(), str.data() + str.size(), ret);
		return ret;
	};
};

struct floating_descriptor {
	static constexpr arg_type my_arg_type = arg_type::floating;
	static constexpr std::string_view name = "float";
	using cpp_type = float;
	static inline const std::regex parse_re{
		"([[:digit:]]+(\\.[[:digit:]]+))f"
	};
	static constexpr auto from_string = [](std::string_view str) {
		float ret = 0.f;
		std::from_chars(str.data(), str.data() + str.size(), ret);
		return ret;
	};
};

// clang-format off
// This is the final configuration object, used to query our descriptors.
using arg_config = detail::argument_config<
	arg_type, // The argument enum.
	variable_descriptor, // Your descriptors, ordered by enum index.
	string_descriptor,
	integer_descriptor,
	floating_descriptor
>;
// clang-format on


// The available commands.
enum class command {
	let,
	add,
	print,
	count,
};

// Our command descriptions.
struct let_descriptor {
	// Which command am I describing?
	static constexpr command my_command = command::let;

	// My command name, used to parse script & output debug messages.
	static constexpr std::string_view name = "let";

	// What is my signature?
	// Nested arrays are used for multiple valid arguments.
	static constexpr auto signature = std::tuple{
		std::array{ arg_type::variable },
		std::array{ arg_type::floating, arg_type::integer, arg_type::string },
	};

	// My parse function. Provided with the same quantity of arguments as
	// requested. Once we've reached this point, we are guaranteed arguments
	// conform to our requested types.
	static constexpr auto on_parse
			= [](std::string&& var_name, auto&& arg, runtime& rt) {
				  return parse_let(std::move(var_name), std::move(arg), rt);
			  };
};

struct add_descriptor {
	static constexpr command my_command = command::add;
	static constexpr std::string_view name = "add";
	static constexpr auto signature = std::tuple{
		std::array{ arg_type::floating, arg_type::integer },
		std::array{ arg_type::floating, arg_type::integer },
	};

	static constexpr auto on_parse = [](auto&& lhs, auto&& rhs, runtime& rt) {
		return parse_add(lhs, rhs, rt);
	};
};

struct print_descriptor {
	static constexpr command my_command = command::print;
	static constexpr std::string_view name = "print";
	static constexpr auto signature = std::tuple{
		std::array{ arg_type::variable, arg_type::string, arg_type::integer,
				arg_type::floating },
	};

	static constexpr auto on_parse
			= [](auto&& arg, runtime&) { return parse_print(arg); };
};

// clang-format off
using command_config = detail::command_config<
	arg_config, // The arguments configuration.
	command, // The enum of commands.
	let_descriptor, // Your descriptors.
	add_descriptor,
	print_descriptor
>;
// clang-format on

struct runtime {
	// Execute script.
	// Returns success.
	bool run(const std::string& script) {
		static const std::regex empty_re{ "^[[:blank:]]*$" };
		static const std::regex comment_re{ "^[[:blank:]]*\\/\\/.*$" };
		static const std::regex command_re{
			"[[:blank:]]*" // spaces + tabs
			"([[:alpha:]]+)[[:blank:]]*" // the command
			"\\([[:blank:]]*" // arguments start
			"(.+?)" // the args
			"[[:blank:]]*\\)[[:blank:]]*;" // arguments end
		};

		static const std::regex arg_re{
			"([^[:blank:],]+)[[:blank:]]*,?[[:blank:]]*"
		};

		std::istringstream iss(script);
		std::string line;
		size_t current_line = 0;

		while (std::getline(iss, line)) {
			++current_line;

			if (line.empty()) {
				continue;
			}
			if (std::regex_match(line, comment_re)) {
				continue;
			}
			if (std::regex_match(line, empty_re)) {
				continue;
			}
			if (*(--line.end()) != ';') {
				fprintf(stderr,
						"Missing trailing semicolon at line '%zu'. Outrageous, "
						"this isn't Python!\n",
						current_line);
				return false;
			}

			svmatch match;
			std::string_view line_view{ line };

			// Parse the command and capture the args.
			if (!std::regex_search(
						line_view.begin(), line_view.end(), match, command_re)
					|| match.size() < 2) {
				fprintf(stderr,
						"Couldn't parse line '%zu'.\n\tCommands use the "
						"following format : 'command(arg, ...);'\n",
						current_line);
				return false;
			}

			// Find our command.
			std::string_view cmd_string = to_sv(match[1]);
			if (command_config::command_name_map.count(cmd_string) == 0) {
				fprintf(stderr, "Unrecognized command '%.*s' at line '%zu'.\n",
						int(cmd_string.size()), cmd_string.data(),
						current_line);
				return false;
			}

			// Get our associated enum value.
			command cmd = command_config::command_name_map.at(cmd_string);
			std::string_view arg_string = to_sv(match[2]);
			size_t expected_num_args = command_config::arg_counts[size_t(cmd)];
			std::vector<std::string_view> args;

			// Parse the command arguments.
			for (auto it = svregex_iterator(
						 arg_string.begin(), arg_string.end(), arg_re);
					it != svregex_iterator{}; ++it) {
				const svmatch& m = *it;
				args.push_back(to_sv(m[1]));
			}

			if (args.size() != expected_num_args) {
				fprintf(stderr,
						"Incorrect amount of arguments for command '%.*s' at "
						"line '%zu'.\n\tExpects '%zu' arguments.\n",
						int(cmd_string.size()), cmd_string.data(), current_line,
						expected_num_args);
				return false;
			}

			// Now, using our descriptors, find what types the arguments are.
			if (!command_config::arg_parsers[size_t(cmd)](args, *this)) {
				fprintf(stderr, "Failed to parse arguments. Exiting.\n");
				return false;
			}
		}

		return true;
	}

	// Here, the caller provides the argument enum,
	// We enforce the type of value with a lookup in our descriptor "map",
	// arg_config.
	template <typename arg_config::arg_enum_t E>
	bool add_var(
			const std::string& name, const get_cpp_type<arg_config, E>& value) {

		// No shadowing in "C--".
		if (_arg_type_map.count(name) != 0) {
			size_t old_arg_t = size_t(_arg_type_map.at(name).first);
			fprintf(stderr,
					"error : Variable '%.*s' already declared. Previous type "
					"was "
					": '%.*s'\n",
					int(name.size()), name.data(),
					int(arg_config::names[old_arg_t].size()),
					arg_config::names[old_arg_t].data());
			return false;
		}

		auto& vec = std::get<size_t(E)>(_arg_value_map);
		size_t new_idx = vec.size();
		_arg_type_map.insert({ name, { E, new_idx } });
		vec.push_back(value);
		return true;
	}

private:
	size_t _current_line = 0;
	std::string _current_script;

	// A std::unordered_map<std::string, std::pair<arg_type, size_t>>
	typename arg_config::variable_type_map_t _arg_type_map;

	// A std::tuple<std::vector<values>...>
	typename arg_config::argument_map_t _arg_value_map;
};

template <class T>
bool parse_let(std::string&& var_name, T&& value, runtime& rt) {
	if constexpr (std::is_same_v<T, std::string>) {
		return rt.add_var<arg_type::string>(var_name, value);
	} else if constexpr (std::is_same_v<T, int>) {
		return rt.add_var<arg_type::integer>(var_name, value);
	} else if constexpr (std::is_same_v<T, float>) {
		return rt.add_var<arg_type::floating>(var_name, value);
	} else {
		return false;
	}
}

template <class T, class U>
bool parse_add(T&&, U&&, runtime&) {
	return false;
}

template <class T>
bool parse_print(T&&) {
	return false;
}


TEST(blog, mini_lang) {
	std::string script = { R"xx(
		let(potato, 42);
		let(tomato, 1.f);
		
		// Did I just write a lisp...?
		let(result, add(potato, tomato));
		
		print('The result : ');
		print(results);	
)xx" };

	runtime rt;
	rt.run(script);
}
#endif
