/*
BSD 3-Clause License

Copyright (c) 2024, Philippe Groarke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once
#include "fea/state_machines/fsm.hpp"
#include "fea/string/string.hpp"
#include "fea/utils/throw.hpp"

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>


namespace fea {
namespace detail {
inline int mprintf(const std::string& message) {
	return printf("%s", message.c_str());
}
inline int mwprintf(const std::wstring& message) {
	return wprintf(L"%s", message.c_str());
}
inline int u16printf(const std::u16string& message) {
	std::string out = utf16_to_utf8(message);
	return printf("%s", out.c_str());
}
inline int u32printf(const std::u32string& message) {
	std::string out = utf32_to_utf8(message);
	return printf("%s", out.c_str());
}

template <class CharT>
constexpr auto get_print() {
	if constexpr (std::is_same_v<CharT, char>) {
		return mprintf;
	} else if constexpr (std::is_same_v<CharT, wchar_t>) {
		return mwprintf;
	} else if constexpr (std::is_same_v<CharT, char16_t>) {
		// By default, we convert char16_t to utf8, so it uses printf.
		return u16printf;
	} else if constexpr (std::is_same_v<CharT, char32_t>) {
		// By default, we convert char32_t to utf8, so it uses printf.
		return u32printf;
	}
}


enum class user_option_e : std::uint8_t {
	flag,
	required_arg,
	optional_arg,
	default_arg,
	multi_arg,
	raw_arg,
	count,
};

template <class CharT = char>
struct user_option {
	using string = std::basic_string<CharT, std::char_traits<CharT>,
			std::allocator<CharT>>;

	user_option() = default;

	user_option(string&& longopt, CharT shortopt, user_option_e t,
			std::function<bool()> func, string&& help)
			: long_name(longopt)
			, short_name(shortopt)
			, opt_type(t)
			, flag_func(std::move(func))
			, description(std::move(help)) {
	}
	user_option(string&& longopt, CharT shortopt, user_option_e t,
			std::function<bool(string&&)> func, string&& help)
			: long_name(longopt)
			, short_name(shortopt)
			, opt_type(t)
			, one_arg_func(std::move(func))
			, description(std::move(help)) {
	}
	user_option(string&& longopt, CharT shortopt, user_option_e t,
			std::function<bool(string&&)> func, string&& help,
			string&& default_val, bool always_execute)
			: long_name(longopt)
			, short_name(shortopt)
			, opt_type(t)
			, always_execute(always_execute)
			, one_arg_func(std::move(func))
			, description(std::move(help))
			, default_val(std::move(default_val)) {
	}
	user_option(string&& longopt, CharT shortopt, user_option_e t,
			std::function<bool(std::vector<string>&&)> func, string&& help)
			: long_name(longopt)
			, short_name(shortopt)
			, opt_type(t)
			, multi_arg_func(std::move(func))
			, description(std::move(help)) {
	}


	string long_name;
	CharT short_name;
	user_option_e opt_type = user_option_e::count;
	bool always_execute = false;

	std::function<bool()> flag_func;
	std::function<bool(string&&)> one_arg_func;
	std::function<bool(std::vector<string>&&)> multi_arg_func;

	string description;
	string default_val;

	bool has_been_parsed = false;
};
} // namespace detail


// get_opt supports all char types.
// Uses printf if you provide char.
// Uses wprintf if you provide wchar_t.

// By default, will convert char16_t and char32_t into utf8 and will print
// with printf. You can customize the print function. If it is customized,
// it will be used as-is. Must be a compatible signature with printf.
template <class CharT = char,
		class PrintfT = decltype(detail::get_print<CharT>())>
struct get_opt {
	using string = std::basic_string<CharT, std::char_traits<CharT>,
			std::allocator<CharT>>;

	static constexpr CharT null_char = FEA_CH('\0');

	get_opt();

	// Construct using a custom print function. The function signature must
	// match printf.
	get_opt(PrintfT printf_func);

	// An option that uses "raw args". Raw args do not have '--' or '-' in
	// front of them. They are often file names or strings. These will be
	// parsed in the order of appearance. ex : 'my_tool a/raw/arg.txt'
	// Quotes will be added to the name.
	void add_raw_option(
			string&& name, std::function<bool(string&&)>&& func, string&& help);

	// An option that doesn't need any argument. AKA a flag.
	// ex : '--flag'
	void add_flag_option(string&& long_name, std::function<bool()>&& func,
			string&& help, CharT short_name = null_char);

	// An option that can accept a single argument or not.
	// If no user argument is provided, your callback is called with your
	// default argument.
	// ex : '--has_default arg' or '--has_default'
	void add_default_arg_option(string&& long_name,
			std::function<bool(string&&)>&& func, string&& help,
			string&& default_value, CharT short_name = null_char,
			bool always_execute = false);

	// An option that can accept a single argument or not.
	// ex : '--optional arg' or '--optional'
	void add_optional_arg_option(string&& long_name,
			std::function<bool(string&&)>&& func, string&& help,
			CharT short_name = null_char);

	// An option that requires a single argument to be set.
	// ex : '--required arg'
	void add_required_arg_option(string&& long_name,
			std::function<bool(string&&)>&& func, string&& help,
			CharT short_name = null_char);

	// An option that accepts multiple arguments.
	// Can be enclosed in quotes.
	// Requires at minimum one option.
	// ex : '--multi "a b c d"'
	void add_multi_arg_option(string&& long_name,
			std::function<bool(std::vector<string>&&)>&& func, string&& help,
			CharT short_name = null_char);

	// Add behavior that requires the first argument (argv[0]).
	// The first argument is always the execution path.
	void add_arg0_callback(std::function<bool(string&&)>&& func);

	// Add help callback, which will be called whenever the user passes in a
	// help option. It is called after help has been printed, to make sure we
	// never corrupt the help printing.
	void add_help_callback(std::function<void()>&& func);

	// Adds some text before printing the help.
	void add_help_intro(const string& message);

	// Adds some text after printing the help.
	void add_help_outro(const string& message);

	// By default, if a user provides no options, help will be printed and
	// success will be false. Use this to allow success on no arguments passed.
	void no_options_is_ok();

	// By default, help is printed if an error is encountered.
	// Disable this behavior and print a short message (suggesting --help).
	void print_full_help_on_error(bool enable);

	// By default, the text wrapping will use 120 characters width.
	// Use this to change the width of the console window.
	void console_width(size_t character_width);

	// Parse the arguments, execute your callbacks, returns success bool
	// (and prints help if there was an error).
	bool parse_options(size_t argc, CharT const* const* argv);

	// Generic print.
	void print(const string& message) const;

	// No need to call this. It is called every time you parse options (assuming
	// you need to parse them more than once).
	void reset();

private:
	static_assert(std::is_same_v<CharT, char> || std::is_same_v<CharT, wchar_t>
					|| std::is_same_v<CharT, char16_t>
					|| std::is_same_v<CharT, char32_t>,
			"getopt : unknown character type, getopt only supports char, "
			"wchar_t, char16_t and char32_t");

	void add_option(detail::user_option<CharT>&& o);

	enum class state {
		arg0,
		choose_parsing,
		parse_longarg,
		parse_shortarg,
		parse_concat,
		parse_raw,
		end,
		count
	};
	enum class transition {
		parse_next,
		exit,
		error,
		help,
		do_longarg,
		do_shortarg,
		do_concat,
		do_raw,
		count
	};

	using fsm_t = fsm<transition, state, void(get_opt*)>;
	using state_t = fsm_state<transition, state, void(get_opt*)>;

	std::unique_ptr<fsm_t> make_machine() const;

	void on_arg0_enter(fsm_t&);
	void on_parse_next_enter(fsm_t&);
	void on_parse_longopt(fsm_t&);
	void on_parse_shortopt(fsm_t&);
	void on_parse_concat(fsm_t&);
	void on_parse_raw(fsm_t&);
	void on_end(fsm_t&);
	void on_print_error(fsm_t&);
	void on_print_help(fsm_t&);

	std::unique_ptr<fsm_t> _machine = make_machine();

	std::unordered_map<CharT, string> _short_opt_to_long_opt;
	std::map<string, detail::user_option<CharT>> _long_opt_to_user_opt;
	std::vector<detail::user_option<CharT>> _raw_opts;

	std::function<bool(string&&)> _arg0_func;
	std::function<void()> _help_func;

	std::vector<string> _all_args;
	PrintfT _print_func;

	string _help_intro;
	string _help_outro;

	size_t _output_width = 120;
	bool _no_arg_is_help = true;
	bool _print_full_help_on_error = true;

	// State machine eval things :
	std::deque<string> _parser_args;
	bool _success = true;
};

template <class CharT, class PrintfT>
get_opt<CharT, PrintfT>::get_opt()
		: get_opt(detail::get_print<CharT>()) {
}

template <class CharT, class PrintfT>
get_opt<CharT, PrintfT>::get_opt(PrintfT printf_func)
		: _print_func(printf_func) {
}

template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::reset() {
	_machine->reset();

	_all_args.clear();
	_parser_args.clear();

	for (auto& r : _raw_opts) {
		r.has_been_parsed = false;
	}

	for (auto& p : _long_opt_to_user_opt) {
		p.second.has_been_parsed = false;
	}

	_success = true;
}


template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::add_raw_option(
		string&& name, std::function<bool(string&&)>&& func, string&& help) {
	using namespace detail;

	auto it = std::find_if(_raw_opts.begin(), _raw_opts.end(),
			[&](const user_option<CharT>& r) { return r.long_name == name; });

	if (it != _raw_opts.end()) {
		fea::maybe_throw<std::invalid_argument>(
				__FUNCTION__, __LINE__, "Raw option already exists.");
	}

	_raw_opts.push_back(user_option<CharT>{
			std::move(FEA_LIT("\"") + name + FEA_LIT("\"")),
			FEA_CH('\0'),
			user_option_e::raw_arg,
			std::move(func),
			std::move(help),
	});
}


template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::add_flag_option(string&& long_name,
		std::function<bool()>&& func, string&& help,
		CharT short_name /*= '\0'*/) {
	using namespace detail;

	add_option(user_option<CharT>{
			std::move(long_name),
			short_name,
			user_option_e::flag,
			std::move(func),
			std::move(help),
	});
}
template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::add_required_arg_option(string&& long_name,
		std::function<bool(string&&)>&& func, string&& help,
		CharT short_name /*= '\0'*/) {
	using namespace detail;

	add_option(user_option<CharT>{
			std::move(long_name),
			short_name,
			user_option_e::required_arg,
			std::move(func),
			std::move(help),
	});
}

template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::add_optional_arg_option(string&& long_name,
		std::function<bool(string&&)>&& func, string&& help,
		CharT short_name /*= '\0'*/) {
	using namespace detail;

	add_option(user_option<CharT>{
			std::move(long_name),
			short_name,
			user_option_e::optional_arg,
			std::move(func),
			std::move(help),
	});
}

template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::add_default_arg_option(string&& long_name,
		std::function<bool(string&&)>&& func, string&& help,
		string&& default_value, CharT short_name /*= '\0'*/,
		bool always_execute /*= false*/) {
	using namespace detail;

	add_option(user_option<CharT>{
			std::move(long_name),
			short_name,
			user_option_e::default_arg,
			std::move(func),
			std::move(help),
			std::move(default_value),
			always_execute,
	});
}

template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::add_multi_arg_option(string&& long_name,
		std::function<bool(std::vector<string>&&)>&& func, string&& help,
		CharT short_name /*= '\0'*/) {
	using namespace detail;

	add_option(user_option<CharT>{
			std::move(long_name),
			short_name,
			user_option_e::multi_arg,
			std::move(func),
			std::move(help),
	});
}


template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::add_option(detail::user_option<CharT>&& o) {
	using namespace detail;

	if (o.short_name != FEA_CH('\0')) {
		if (_short_opt_to_long_opt.count(o.short_name) > 0) {
			fea::maybe_throw<std::invalid_argument>(
					__FUNCTION__, __LINE__, "Short option already exists.");
		}
		_short_opt_to_long_opt.insert({ o.short_name, o.long_name });
	}

	if (_long_opt_to_user_opt.count(o.long_name) > 0) {
		fea::maybe_throw<std::invalid_argument>(
				__FUNCTION__, __LINE__, "Long option already exists.");
	}

	string name = o.long_name;
	_long_opt_to_user_opt.insert({ std::move(name), std::move(o) });
}


template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::add_arg0_callback(
		std::function<bool(string&&)>&& func) {
	_arg0_func = std::move(func);
}

template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::add_help_callback(std::function<void()>&& func) {
	_help_func = std::move(func);
}


template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::add_help_intro(const string& message) {
	_help_intro = message;
}


template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::add_help_outro(const string& message) {
	_help_outro = message;
}

template <class CharT, class PrintfT>
void fea::get_opt<CharT, PrintfT>::no_options_is_ok() {
	_no_arg_is_help = false;
}

template <class CharT, class PrintfT>
void fea::get_opt<CharT, PrintfT>::print_full_help_on_error(bool enable) {
	_print_full_help_on_error = enable;
}

template <class CharT, class PrintfT>
void fea::get_opt<CharT, PrintfT>::console_width(size_t output_width) {
	_output_width = output_width;
}


template <class CharT, class PrintfT>
bool get_opt<CharT, PrintfT>::parse_options(
		size_t argc, CharT const* const* argv) {
	reset();

	_all_args.reserve(argc);
	//_parsing_args.reserve(argc);

	for (size_t i = 0; i < argc; ++i) {
		_all_args.push_back({ argv[i] });
		_parser_args.push_back({ argv[i] });
	}

	while (!_machine->finished()) {
		_machine->update(this);
	}

	return _success;
}

template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::print(const string& message) const {
	_print_func(message.c_str());
}

template <class CharT, class PrintfT>
std::unique_ptr<typename get_opt<CharT, PrintfT>::fsm_t>
get_opt<CharT, PrintfT>::make_machine() const {
	std::unique_ptr<fsm_t> ret = std::make_unique<fsm_t>();

	// arg0
	{
		state_t arg0_state;
		arg0_state.template add_transition<transition::parse_next,
				state::choose_parsing>();
		arg0_state.template add_transition<transition::exit, state::end>();
		arg0_state.template add_transition<transition::error, state::end>();
		arg0_state.template add_transition<transition::help, state::end>();

		arg0_state.template add_event<fsm_event::on_enter>(
				&get_opt::on_arg0_enter);
		ret->template add_state<state::arg0>(std::move(arg0_state));
	}

	// choose_parsing
	{
		state_t choose_state;
		choose_state.template add_transition<transition::do_concat,
				state::parse_concat>();
		choose_state.template add_transition<transition::do_longarg,
				state::parse_longarg>();
		choose_state.template add_transition<transition::do_shortarg,
				state::parse_shortarg>();
		choose_state.template add_transition<transition::do_raw,
				state::parse_raw>();
		choose_state.template add_transition<transition::help, state::end>();
		choose_state.template add_transition<transition::exit, state::end>();
		choose_state.template add_transition<transition::error, state::end>();

		choose_state.template add_event<fsm_event::on_enter>(
				&get_opt::on_parse_next_enter);
		ret->template add_state<state::choose_parsing>(std::move(choose_state));
	}

	// raw
	{
		state_t raw_state;
		raw_state.template add_transition<transition::error, state::end>();
		raw_state.template add_transition<transition::parse_next,
				state::choose_parsing>();
		raw_state.template add_event<fsm_event::on_enter>(
				&get_opt::on_parse_raw);
		ret->template add_state<state::parse_raw>(std::move(raw_state));
	}

	// long
	{
		state_t long_state;
		long_state.template add_transition<transition::error, state::end>();
		long_state.template add_transition<transition::parse_next,
				state::choose_parsing>();
		long_state.template add_event<fsm_event::on_enter>(
				&get_opt::on_parse_longopt);
		ret->template add_state<state::parse_longarg>(std::move(long_state));
	}

	// short
	{
		state_t short_state;
		short_state.template add_transition<transition::error, state::end>();
		short_state.template add_transition<transition::do_longarg,
				state::parse_longarg>();
		short_state.template add_event<fsm_event::on_enter>(
				&get_opt::on_parse_shortopt);
		ret->template add_state<state::parse_shortarg>(std::move(short_state));
	}

	// concat
	{
		state_t concat_state;
		concat_state.template add_transition<transition::error, state::end>();
		concat_state.template add_transition<transition::do_longarg,
				state::parse_longarg>();
		concat_state.template add_event<fsm_event::on_enter>(
				&get_opt::on_parse_concat);
		ret->template add_state<state::parse_concat>(std::move(concat_state));
	}

	// end
	{
		state_t end_state;
		end_state.template add_transition<transition::help, state::end>();
		end_state.template add_transition<transition::error, state::end>();

		end_state.template add_event<fsm_event::on_enter_from,
				transition::error>(&get_opt::on_print_error);
		end_state
				.template add_event<fsm_event::on_enter_from, transition::help>(
						&get_opt::on_print_help);
		end_state
				.template add_event<fsm_event::on_enter_from, transition::exit>(
						&get_opt::on_end);

		ret->template add_state<state::end>(std::move(end_state));
		ret->template set_finish_state<state::end>();
	}

	return ret;
}

template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::on_arg0_enter(fsm_t& m) {
	if (_parser_args.empty()) {
		return m.template trigger<transition::error>(this);
	}

	bool success = true;
	if (_arg0_func) {
		success = std::invoke(_arg0_func, std::move(_parser_args.front()));
	}

	_parser_args.pop_front();

	if (!success) {
		return m.template trigger<transition::error>(this);
	}

	if (_parser_args.empty()) {
		if (_no_arg_is_help) {
			return m.template trigger<transition::help>(this);
		} else {
			return m.template trigger<transition::exit>(this);
		}
	}

	return m.template trigger<transition::parse_next>(this);
}

template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::on_parse_next_enter(fsm_t& m) {

	if (_parser_args.empty()) {
		return m.template trigger<transition::exit>(this);
	}

	string& first = _parser_args.front();

	// help
	if (first == FEA_LIT("-h") || first == FEA_LIT("--help")
			|| first == FEA_LIT("/?") || first == FEA_LIT("/help")
			|| first == FEA_LIT("/h")) {
		return m.template trigger<transition::help>(this);
	}

	// A single short arg, ex : '-d'
	if (fea::starts_with(first, FEA_LIT("-")) && first.size() == 2) {
		return m.template trigger<transition::do_shortarg>(this);
	}

	// A long arg, ex '--something'
	if (fea::starts_with(first, FEA_LIT("--"))) {
		return m.template trigger<transition::do_longarg>(this);
	}

	// Concatenated short args, ex '-abdsc'
	if (fea::starts_with(first, FEA_LIT("-"))) {
		return m.template trigger<transition::do_concat>(this);
	}

	// Everything else failed, check raw args. ex '"some arg"'
	return m.template trigger<transition::do_raw>(this);
}

template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::on_parse_longopt(fsm_t& m) {
	using namespace detail;
	string opt_str = _parser_args.front();
	_parser_args.pop_front();

	size_t new_beg = opt_str.find_first_not_of(FEA_LIT("-"));
	opt_str = opt_str.substr(new_beg);

	if (_long_opt_to_user_opt.count(opt_str) == 0) {
		print(FEA_LIT("Could not parse : '") + opt_str + FEA_LIT("'\n"));
		print(FEA_LIT("Option doesn't exist.\n"));
		return m.template trigger<transition::error>(this);
	}

	user_option<CharT>& user_opt = _long_opt_to_user_opt.at(opt_str);

	if (user_opt.has_been_parsed) {
		print(FEA_LIT("'") + opt_str + FEA_LIT("' already parsed.\n"));
		return m.template trigger<transition::error>(this);
	}
	user_opt.has_been_parsed = true;

	// Raw args are stored elsewhere.
	assert(user_opt.opt_type != user_option_e::raw_arg);

	bool success = false;
	// Set this now for later.
	string default_val = user_opt.default_val;

	switch (user_opt.opt_type) {
	case user_option_e::flag: {
		// A simple flag, call user func.
		success = user_opt.flag_func();
	} break;
	case user_option_e::required_arg: {
		// An option that requires one argument.

		if (_parser_args.empty()
				|| fea::starts_with(_parser_args.front(), FEA_LIT("-"))) {
			print(FEA_LIT("Could not parse : '") + opt_str + FEA_LIT("'\n"));
			print(FEA_LIT("Option requires an argument, none was provided.\n"));
			return m.template trigger<transition::error>(this);
		}

		string arg = _parser_args.front();
		_parser_args.pop_front();

		success = user_opt.one_arg_func(std::move(arg));
	} break;
	case user_option_e::optional_arg: {
		default_val = FEA_LIT(""); // Reset the default val to nothing.
		// Parsing is the same as default.
	}
		[[fallthrough]];
	case user_option_e::default_arg: {
		if (_parser_args.empty()
				|| fea::starts_with(_parser_args.front(), FEA_LIT("-"))) {
			success = user_opt.one_arg_func(std::move(default_val));
		} else {
			string arg = _parser_args.front();
			_parser_args.pop_front();

			success = user_opt.one_arg_func(std::move(arg));
		}
	} break;
	case user_option_e::multi_arg: {

		// Needs at least 1 arg.
		if (_parser_args.empty()
				|| fea::starts_with(_parser_args.front(), FEA_LIT("-"))) {
			print(FEA_LIT("Could not parse : '") + opt_str + FEA_LIT("'\n"));
			print(FEA_LIT("Option requires at minimum 1 argument, none was "
						  "provided.\n"));
			return m.template trigger<transition::error>(this);
		}

		std::vector<string> args;

		string arg = _parser_args.front();
		_parser_args.pop_front();

		// Were the args enclosed in quotes?
		if (arg.find(FEA_CH(' ')) != string::npos) {
			args = fea::split_to_str(arg, FEA_LIT(" "));
			success = user_opt.multi_arg_func(std::move(args));
		} else {
			// Gather everything up till the end or the next '-'
			args.push_back(std::move(arg));

			while (!_parser_args.empty()
					&& !fea::starts_with(_parser_args.front(), FEA_LIT("-"))) {
				args.push_back(std::move(_parser_args.front()));
				_parser_args.pop_front();
			}

			success = user_opt.multi_arg_func(std::move(args));
		}
	} break;
	default: {
		assert(false);
		print(FEA_LIT(
				"Something went horribly wrong, please report this bug <3\n"));
		return m.template trigger<transition::error>(this);
	} break;
	}

	if (!success) {
		if (!_parser_args.empty()) {
			print(FEA_LIT("Problem parsing argument '") + _parser_args.front()
					+ FEA_LIT("'.\n"));
		} else {
			print(FEA_LIT("Problem parsing argument '--") + user_opt.long_name
					+ FEA_LIT("'.\n"));
		}
		return m.template trigger<transition::error>(this);
	}

	return m.template trigger<transition::parse_next>(this);
}

template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::on_parse_shortopt(fsm_t& m) {
	assert(_parser_args.front().size() == 2);

	string arg = _parser_args.front();
	_parser_args.pop_front();

	size_t new_beg = arg.find_first_not_of(FEA_LIT("-"));
	arg = arg.substr(new_beg);

	assert(arg.size() == 1);

	CharT short_opt = arg[0];

	if (_short_opt_to_long_opt.count(short_opt) == 0) {
		print(FEA_LIT("Could not parse : '") + arg + FEA_LIT("'\n"));
		print(FEA_LIT("Option not recognized.\n"));
		return m.template trigger<transition::error>(this);
	}

	_parser_args.push_front(
			FEA_LIT("--") + _short_opt_to_long_opt.at(short_opt));
	return m.template trigger<transition::do_longarg>(this);
}

template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::on_parse_concat(fsm_t& m) {
	string arg = _parser_args.front();
	_parser_args.pop_front();

	size_t new_beg = arg.find_first_not_of(FEA_LIT("-"));
	arg = arg.substr(new_beg);

	std::vector<string> long_args;
	for (CharT short_opt : arg) {
		if (_short_opt_to_long_opt.count(short_opt) == 0) {
			print(FEA_LIT("Could not parse : '") + string{ short_opt }
					+ FEA_LIT("'\n"));
			print(FEA_LIT("Option not recognized.\n"));
			return m.template trigger<transition::error>(this);
		}

		long_args.push_back(
				FEA_LIT("--") + _short_opt_to_long_opt.at(short_opt));
	}

	_parser_args.insert(
			_parser_args.begin(), long_args.begin(), long_args.end());
	return m.template trigger<transition::do_longarg>(this);
}

template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::on_parse_raw(fsm_t& m) {
	using namespace detail;

	auto next_rawopt = std::find_if(_raw_opts.begin(), _raw_opts.end(),
			[](const user_option<CharT>& o) { return !o.has_been_parsed; });

	// We've parsed all raw options, user provided options are curropted.
	if (next_rawopt == _raw_opts.end()) {
		print(FEA_LIT("Could not parse : '") + _parser_args.front()
				+ FEA_LIT("'\n"));
		print(FEA_LIT("All arguments have previously been parsed.\n"));
		return m.template trigger<transition::error>(this);
	}

	bool success = next_rawopt->one_arg_func(std::move(_parser_args.front()));
	next_rawopt->has_been_parsed = true;

	if (!success) {
		print(FEA_LIT("'") + _parser_args.front()
				+ FEA_LIT("' problem parsing argument.\n"));
		return m.template trigger<transition::error>(this);
	}

	_parser_args.pop_front();

	return m.template trigger<transition::parse_next>(this);
}

template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::on_end(fsm_t& m) {
	bool success = true;

	// Parse default opts that always want to execute.
	for (const std::pair<const string, detail::user_option<CharT>>& p :
			_long_opt_to_user_opt) {
		const detail::user_option<CharT> user_opt = p.second;
		if (user_opt.opt_type != fea::detail::user_option_e::default_arg) {
			continue;
		}
		if (!user_opt.always_execute || user_opt.has_been_parsed) {
			continue;
		}

		string default_val = user_opt.default_val;
		success = user_opt.one_arg_func(std::move(default_val));
		if (!success) {
			break;
		}
	}

	if (!success) {
		m.template trigger<transition::error>(this);
	}
}

template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::on_print_error(fsm_t& m) {
	if (_print_full_help_on_error) {
		print(FEA_LIT("\n"));
		return m.template trigger<transition::help>(this);
	}
	_success = false;
	print(FEA_LIT("Use --help for extra help.\n"));
}

template <class CharT, class PrintfT>
void get_opt<CharT, PrintfT>::on_print_help(fsm_t&) {
	_success = false;

	using namespace detail;

	// The first string is printed as-is.
	// Tries to find '\n'. If it does, splits the incoming string and prints at
	// indentation.
	// If there is no '\n', simply prints str and returns.
	auto print_description = [this](const string& desc, size_t indendation) {
		if (desc.empty())
			return;

		// Split the string if it contains \n, so substrings start on a new line
		// with the appropriate indentation.
		std::vector<string> str_vec;
		if (desc.find(FEA_CH('\n')) == string::npos) {
			// There is no '\n'.
			str_vec.push_back(desc);
		} else {
			// There is '\n'
			str_vec = fea::split_to_str(desc, FEA_LIT("\n"));
		}

		// Now, make sure all strings are less than output_width in *real* size.

		// To do that, we must convert to utf32 to do our work. This includes
		// getting "language" size, and splitting the strings.

		// If some strings are wider than the max output, split them at the
		// first word possible, so it's pretty. Insert the split strings
		// in the final output vector.
		// If no splitting is required, simply add to the vector.
		std::vector<string> out_str_vec;
		out_str_vec.reserve(out_str_vec.size());

		for (const string& s : str_vec) {
			// Get a utf32 string to work without brain damage.
			std::u32string utf32 = any_to_utf32(s);
			// Gets the actual lexical size of the string. We include the
			// indentation width that comes before.
			size_t real_size = utf32.size() + indendation;

			// We must splitty split.
			if (real_size > _output_width) {
				size_t pos = 0;
				// Find substrings and split them.
				while (real_size - pos > _output_width) {
					std::u32string new_str
							= utf32.substr(pos, _output_width - indendation);
					// Find where we can split nicely.
					size_t space_pos = new_str.find_last_of(U' ');

					// Pushback the string, split nicely at the last word.
					out_str_vec.push_back(
							utf32_to_any<CharT>(new_str.substr(0, space_pos)));

					// Don't forget to ignore the space for the next sentence.
					pos += space_pos + 1;
				}
				// We still have 1 leftover split at the end.
				std::u32string new_str
						= utf32.substr(pos, _output_width - indendation);
				out_str_vec.push_back(utf32_to_any<CharT>(new_str));

			} else {
				out_str_vec.push_back(s);
			}
		}


		// Finally, print everything at the right indentation.
		for (size_t i = 0; i < out_str_vec.size(); ++i) {
			const string& substr = out_str_vec[i];
			print(substr + FEA_LIT("\n"));

			// Print the indentation for the next string if there is one.
			if (i + 1 < out_str_vec.size()) {
				print(string(indendation, FEA_CH(' ')));
			}
		}
	};

	constexpr size_t indent = 1;
	constexpr size_t shortopt_width = 4;
	constexpr size_t shortopt_total_width = indent + shortopt_width;
	constexpr size_t longopt_space = 2;
	constexpr size_t longopt_width_max = 30;
	constexpr size_t rawopt_help_indent = 4;
	const string opt_str = FEA_LIT(" <optional>");
	const string req_str = FEA_LIT(" <value>");
	const string multi_str = FEA_LIT(" <multiple>");
	const string default_beg = FEA_LIT(" <=");
	const string default_end = FEA_LIT(">");

	if (!_help_intro.empty()) {
		print(_help_intro + FEA_LIT("\n"));
	}

	// Usage
	{
		string out_str;
		for (const user_option<CharT>& raw_opt : _raw_opts) {
			out_str += FEA_LIT(" ");
			out_str += raw_opt.long_name;
		}

		print(FEA_LIT("\nUsage: ") + _all_args.front() + out_str
				+ FEA_LIT(" [options]\n\n"));
	}

	// Raw Options
	if (!_raw_opts.empty()) {
		// Find the biggest raw option name size.
		// The raw option's name is stored in its long_opt string.
		size_t max_name_width = 0;
		for (const user_option<CharT>& raw_opt : _raw_opts) {
			size_t name_width = raw_opt.long_name.size() + rawopt_help_indent;
			max_name_width = std::max(max_name_width, name_width);
		}

		// Print section header.
		print(FEA_LIT("Arguments:\n"));

		// Now, print the raw option help.
		for (const user_option<CharT>& raw_opt : _raw_opts) {
			// Print indentation.
			print(string(indent, FEA_CH(' ')));

			// Print the help, and use max_name_width so each help line is
			// properly aligned.
			string out = raw_opt.long_name;
			out.resize(max_name_width, FEA_CH(' '));
			print(out);

			// Print the help message. This will split the message if it is too
			// wide, or if the user used '\n' in his message.
			print_description(raw_opt.description, indent + max_name_width);
		}
		print(FEA_LIT("\n"));
	}

	// All Other Options
	{
		// First, compute the maximum width of long options.
		size_t longopt_width = 0;
		for (const std::pair<const string, user_option<CharT>>& opt_p :
				_long_opt_to_user_opt) {
			const string& long_opt_str = opt_p.first;
			const user_option<CharT>& opt = opt_p.second;

			size_t size = 2 + long_opt_str.size() + longopt_space;
			if (opt.opt_type == user_option_e::optional_arg) {
				size += opt_str.size();
			} else if (opt.opt_type == user_option_e::required_arg) {
				size += req_str.size();
			} else if (opt.opt_type == user_option_e::default_arg) {
				size += default_beg.size() + opt.default_val.size()
						+ default_end.size();
			} else if (opt.opt_type == user_option_e::multi_arg) {
				size += multi_str.size();
			}

			longopt_width = std::max(longopt_width, size);
		}

		// Cap it to longopt_width_max though. If it is bigger than this, we'll
		// print it on a new line.
		if (longopt_width > longopt_width_max) {
			longopt_width = longopt_width_max;
		}

		// Option printer.
		// We will loop twice. Once for options with short flags, considered
		// your "main options". A second time for options without short opts,
		// considered "extra options".
		auto option_help_printer
				= [&, this](const std::pair<const string, user_option<CharT>>&
								  opt_p) {
					  const string& long_opt_str = opt_p.first;
					  const user_option<CharT>& opt = opt_p.second;

					  // Print indentation.
					  print(string(indent, FEA_CH(' ')));

					  // If the option has a shortarg, print that.
					  if (opt.short_name != FEA_CH('\0')) {
						  string shortopt_str;
						  shortopt_str += FEA_LIT("-");
						  shortopt_str += opt.short_name;
						  shortopt_str += FEA_LIT(",");
						  string out = shortopt_str;
						  out.resize(shortopt_width, FEA_CH(' '));
						  print(out);
					  } else {
						  print(string(shortopt_width, FEA_CH(' ')));
					  }

					  // Build the longopt string.
					  string longopt_str;
					  longopt_str += FEA_LIT("--");
					  longopt_str += long_opt_str;

					  // Add the specific "instructions" for each type of arg.
					  if (opt.opt_type == user_option_e::optional_arg) {
						  longopt_str += opt_str;
					  } else if (opt.opt_type == user_option_e::required_arg) {
						  longopt_str += req_str;
					  } else if (opt.opt_type == user_option_e::default_arg) {
						  longopt_str += default_beg;
						  longopt_str += opt.default_val;
						  longopt_str += default_end;
					  } else if (opt.opt_type == user_option_e::multi_arg) {
						  longopt_str += multi_str;
					  }

					  // Print the longopt string.
					  string out = longopt_str;
					  out.resize(longopt_width, FEA_CH(' '));
					  print(out);

					  // If it was bigger than the max width, the description
					  // will be printed on the next line, indented up to the
					  // right position.
					  if (longopt_str.size() >= longopt_width) {
						  print(FEA_LIT("\n"));
						  print(string(longopt_width + shortopt_total_width,
								  FEA_CH(' ')));
					  }

					  // Print the help message, indents appropriately and
					  // splits into multiple strings if the message is too
					  // wide.
					  print_description(opt.description,
							  longopt_width + shortopt_total_width);
				  };

		// Print the main options (those that have short opt).
		print(FEA_LIT("Options:\n"));
		for (const std::pair<const string, user_option<CharT>>& opt_p :
				_long_opt_to_user_opt) {
			const user_option<CharT>& opt = opt_p.second;
			if (opt.short_name == FEA_CH('\0')) {
				// skip, print later.
				continue;
			}
			option_help_printer(opt_p);
		}

		// Print the help command help.
		string short_help = FEA_LIT("-h,");
		short_help.resize(shortopt_width, FEA_CH(' '));

		string long_help = FEA_LIT("--help");
		long_help.resize(longopt_width, FEA_CH(' '));

		print(string(indent, FEA_CH(' ')) + short_help + long_help
				+ FEA_LIT("Print this help\n"));

		// Print options that don't have short opt.
		print(FEA_LIT("\nExtra Options:\n"));
		for (const std::pair<const string, user_option<CharT>>& opt_p :
				_long_opt_to_user_opt) {
			const user_option<CharT>& opt = opt_p.second;
			if (opt.short_name != FEA_CH('\0')) {
				// skip, already printed.
				continue;
			}
			option_help_printer(opt_p);
		}

		if (longopt_width == 0) // No options, width is --help only.
			longopt_width = 2 + 4 + longopt_space;

		// Print user outro.
		if (!_help_outro.empty()) {
			print(FEA_LIT("\n") + _help_outro + FEA_LIT("\n"));
		}

		// Finally, if the user had passed in a callback to be notified when
		// help was called, call that.
		if (_help_func) {
			_help_func();
		}
	}
} // namespace fea


} // namespace fea
