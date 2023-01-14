// #include "../../../build/bin/simd_api_mmx.hpp"
// #include "../../../build/bin/simd_api_sse.hpp"
// #include "../../../build/bin/simd_api_sse2.hpp"
// #include "../../../build/bin/simd_api_sse3.hpp"

#include "simd_gen_descriptions.hpp"
#include "simd_gen_header_template.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <fea/containers/stack_vector.hpp>
#include <fea/enum/enum_array.hpp>
#include <fea/maps/unsigned_map.hpp>
// #include <fea/performance/simd.hpp>
// #include <fea/performance/simd_register.hpp>
// #include <fea/performance/simd_register_traits.hpp>
#include <fea/simd/simd_version.hpp>
#include <fea/string/string.hpp>
#include <fea/utils/file.hpp>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <pugixml.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

extern const char* argv0;
namespace {

// Parses the intel intrinsics guide to generate the api header.
// Expects dataset (offline download) to be in bin folder, named :
// 'Intel Intrinsics Guide/'
#if 1
const std::unordered_map<std::string, size_t> intrin_name_to_desc_idx = []() {
	std::unordered_map<std::string, size_t> ret;
	for (size_t i = 0; i < intrin_descriptions.size(); ++i) {
		const intrin_desc& d = intrin_descriptions[i];

		assert(!ret.contains(d.intrin_name));
		ret.insert({ d.intrin_name, i });
	}
	return ret;
}();

const intrin_desc& get_intrin_desc(const std::string& intrin_name) {
	return intrin_descriptions[intrin_name_to_desc_idx.at(intrin_name)];
}

// Version to enum string
const fea::enum_array<std::string, fea::simd_ver> ver_to_fea_enum_str{
	"fea::simd_ver::mmx",
	"fea::simd_ver::sse",
	"fea::simd_ver::sse2",
	"fea::simd_ver::sse3",
	"fea::simd_ver::ssse3",
	"fea::simd_ver::sse41",
	"fea::simd_ver::sse42",
	"fea::simd_ver::avx",
	"fea::simd_ver::avx2",
	"fea::simd_ver::avx512f",
};

const fea::enum_array<std::string, fea::simd_ver> ver_to_filename{
	"simd_api_mmx.hpp",
	"simd_api_sse.hpp",
	"simd_api_sse2.hpp",
	"simd_api_sse3.hpp",
	"simd_api_ssse3.hpp",
	"simd_api_sse41.hpp",
	"simd_api_sse42.hpp",
	"simd_api_avx.hpp",
	"simd_api_avx2.hpp",
	"simd_api_avx512f.hpp",
};

const fea::enum_array<std::string, fea::simd_ver> ver_to_api_name{
	"mmx",
	"sse",
	"sse2",
	"sse3",
	"ssse3",
	"sse41",
	"sse42",
	"avx",
	"avx2",
	"avx512f",
};

const std::unordered_map<std::string, fea::simd_ver> supported_cpuids{
	{ "MMX", fea::simd_ver::mmx },
	{ "SSE", fea::simd_ver::sse },
	{ "SSE2", fea::simd_ver::sse2 },
	{ "SSE3", fea::simd_ver::sse3 },
};

// TODO
//{ "SSSE3", fea::simd_ver::ssse3 },
//{ "SSE4.1", fea::simd_ver::sse41 },
//{ "SSE4.2", fea::simd_ver::sse42 },
//{ "AVX", fea::simd_ver::avx },
//{ "AVX2", fea::simd_ver::avx2 },
//{ "AVX512F", fea::simd_ver::avx512f },

struct arg_info {
	arg_info() = default;
	arg_info(const pugi::xml_node& param)
			: type(param.attribute("type").value())
			, varname(param.attribute("varname").value()) {
	}

	std::string to_signature() const {
		return type + " " + varname;
	}

	std::string to_passed() const {
		std::string ret;
		if (take_address) {
			ret = cast_str + '&' + varname;
		} else {
			ret = cast_str + varname;
		}

		if (!fea::starts_with(type, 'm')) {
			return ret;
		}

		if (!fea::contains(type, "_t")
				|| std::count(type.begin(), type.end(), '_') != 2) {
			return ret;
		}

		assert(cast_str.empty());
		assert(!take_address);

		// if (fea::contains(type, '*')) {
		//	return varname + "->xmm";
		// }
		return varname + ".xmm";
	}

	std::string type;
	std::string varname;

	// Added by function generator.
	std::string cast_str;
	size_t original_idx = (std::numeric_limits<size_t>::max)();
	bool is_template = false;
	bool is_output = false;
	bool take_address = false;
};

struct intrin_info {
	intrin_info() = default;
	intrin_info(const pugi::xml_node& intrin)
			: cpuid(intrin.child("CPUID").text().get())
			, simd_version(supported_cpuids.at(cpuid))
			, intrin_name(intrin.attribute("name").value())
			, description(clean_description(
					  intrin.child("description").text().get()))
			, operation(clean_operation(intrin.child("operation").text().get()))
			, instruction(clean_instruction(intrin))
			, return_parameter(arg_info{ intrin.child("return") })
			, args(parse_params(intrin)) {
	}

	std::string cpuid;
	fea::simd_ver simd_version = fea::simd_ver::count;
	std::string intrin_name;
	std::string func_name;
	std::string description;
	std::string operation;
	std::string instruction;
	arg_info return_parameter;
	std::vector<arg_info> args;

	// Added by descriptions.
	bool m32bits = false;
	bool commented = false;

private:
	static std::string clean_description(const std::string& desc) {
		if (desc.empty()) {
			return desc;
		}

		std::string ret = desc;
		if (fea::contains(desc, "\n\t")) {
			fea::replace_all_inplace(ret, "\n\t", " ");
		}

		constexpr size_t column_limit = 79;
		if (ret.size() <= column_limit) {
			if (!fea::ends_with(ret, '\n')) {
				ret += '\n';
			}
			return ret;
		}

		size_t prev_space_idx = 0;
		size_t running_idx = 0;
		for (size_t i = 0; i < ret.size(); ++i) {
			if (running_idx != 0 && running_idx % column_limit == 0) {
				ret[prev_space_idx] = '\n';
				running_idx += i - prev_space_idx;
			}

			if (ret[i] == ' ') {
				prev_space_idx = i;
			}

			++running_idx;
		}

		if (!fea::ends_with(ret, '\n')) {
			ret += '\n';
		}
		return ret;
	}

	static std::string clean_operation(const std::string& op) {
		if (op.empty()) {
			return op;
		}

		std::string ret = op;
		if (fea::starts_with(ret, '\n')) {
			ret = ret.substr(1);
		}

		if (fea::ends_with(ret, "\n\t")) {
			ret = ret.substr(0, op.size() - 2);
		}

		if (!fea::ends_with(ret, '\n')) {
			ret += '\n';
		}

		return ret;
	}

	static std::string clean_instruction(const pugi::xml_node& intrin) {
		pugi::xml_attribute seq = intrin.attribute("sequence");
		if (seq && std::string_view{ seq.value() } == "TRUE") {
			return "SEQUENCE\n";
		}

		pugi::xml_node inst = intrin.child("instruction");
		if (inst) {
			std::string n = inst.attribute("name").value();
			std::string f = inst.attribute("form").value();
			fea::to_lower_ascii_inplace(n);
			fea::to_lower_ascii_inplace(f);
			return n + ' ' + f + '\n';
		}
		return {};
	}

	static std::vector<arg_info> parse_params(const pugi::xml_node& intrin) {
		std::vector<arg_info> ret;
		for (pugi::xml_node p : intrin.children("parameter")) {
			ret.push_back(arg_info{ p });
		}
		return ret;
	}
};

namespace detail {
std::string make_register_type(topt opt, const std::string& in_type) {
	static const std::unordered_map<std::string, std::string> to_prefix{
		{ "__m64", "m64_" },
		{ "__m128", "m128_" },
		{ "__m128i", "m128_" },
		{ "__m128d", "m128_" },
		{ "__m256", "m256_" },
		{ "__m256i", "m256_" },
		{ "__m256d", "m256_" },
		{ "__m512", "m512_" },
		{ "__m512i", "m512_" },
		{ "__m512d", "m512_" },
	};

	static const std::unordered_map<topt, std::string> to_suffix{
		{ topt::f32, "f32_t" },
		{ topt::f64, "f64_t" },
		{ topt::i8, "i8_t" },
		{ topt::u8, "u8_t" },
		{ topt::i16, "i16_t" },
		{ topt::u16, "u16_t" },
		{ topt::i32, "i32_t" },
		{ topt::u32, "u32_t" },
		{ topt::i64, "i64_t" },
		{ topt::u64, "u64_t" },
	};

	assert(to_prefix.contains(in_type));
	assert(to_suffix.contains(opt));

	switch (opt) {
	case topt::f32: {
		assert(in_type.find('i') == in_type.npos
				&& in_type.find('d') == in_type.npos);
	} break;
	case topt::f64: {
		assert(in_type.find('d') != in_type.npos);
	} break;
	case topt::i8:
		[[fallthrough]];
	case topt::u8:
		[[fallthrough]];
	case topt::i16:
		[[fallthrough]];
	case topt::u16:
		[[fallthrough]];
	case topt::i32:
		[[fallthrough]];
	case topt::u32:
		[[fallthrough]];
	case topt::i64:
		[[fallthrough]];
	case topt::u64: {
		assert(in_type.find('i') != in_type.npos || in_type == "__m64");
	} break;
	default: {
		assert(false);
	} break;
	}

	return to_prefix.at(in_type) + to_suffix.at(opt);
}

std::string make_cpp_type(topt opt, const std::string& in_type) {
	static const std::unordered_map<topt, std::string> to_type{
		{ topt::f32, "float" },
		{ topt::f64, "double" },
		{ topt::i8, "char" },
		{ topt::u8, "unsigned char" },
		{ topt::i16, "short" },
		{ topt::u16, "unsigned short" },
		{ topt::i32, "int" },
		{ topt::u32, "unsigned int" },
		{ topt::i64, "long long" },
		{ topt::u64, "unsigned long long" },
	};

	assert(in_type == "void" || in_type == "char"
			|| fea::starts_with(in_type, "__m"));
	assert(to_type.contains(opt));

	return to_type.at(opt);
}

std::string clean_type(const std::string& in_type) {
	std::string ret = in_type;
	fea::replace_all_inplace(ret, ' ', "");
	fea::replace_all_inplace(ret, '*', "");
	fea::replace_all_inplace(ret, '&', "");
	fea::replace_all_inplace(ret, "const", "");
	return ret;
}

std::string make_single_type(
		const std::vector<topt>& opts, const std::string& in_type) {
	std::string prefix;
	std::string type;
	std::string suffix;

	std::string cleaned_type = clean_type(in_type);
	bool cast_to_cpp = std::find(opts.begin(), opts.end(), topt::reg_to_cpp)
			!= opts.end();

	for (topt opt : opts) {
		assert(opt != topt::overloads);

		switch (opt) {
		case topt::keep: {
			type = in_type;
		} break;
		case topt::keep_type: {
			type = cleaned_type;
		} break;
		case topt::ptr: {
			suffix = '*';
		} break;
		case topt::ref: {
			suffix = '&';
		} break;
		case topt::const_ptr: {
			suffix = " const*";
		} break;
		case topt::const_ref: {
			suffix = " const&";
		} break;
		case topt::bool_: {
			type = "bool";
		} break;
		case topt::f32:
			[[fallthrough]];
		case topt::f64:
			[[fallthrough]];
		case topt::i8:
			[[fallthrough]];
		case topt::u8:
			[[fallthrough]];
		case topt::i16:
			[[fallthrough]];
		case topt::u16:
			[[fallthrough]];
		case topt::i32:
			[[fallthrough]];
		case topt::u32:
			[[fallthrough]];
		case topt::i64:
			[[fallthrough]];
		case topt::u64: {
			if (in_type.starts_with("__m") && !cast_to_cpp) {
				type = make_register_type(opt, cleaned_type);
			} else {
				assert(!cast_to_cpp || fea::starts_with(cleaned_type, "__m"));
				type = make_cpp_type(opt, cleaned_type);
			}
		} break;
		case topt::to_last_param: {
			// handled
		} break;
		case topt::template_: {
			// handled
		} break;
		case topt::reg_to_cpp: {
			// handled
		} break;
		case topt::take_address: {
			// handled
		} break;
		case topt::cast: {
			// handled
		} break;
		default: {
			assert(false);
		} break;
		}
	}

	assert(!type.starts_with("__m"));
	return prefix + type + suffix;
}

bool needs_overloads(const intrin_desc& d) {
	auto it = std::find(
			d.return_opts.begin(), d.return_opts.end(), topt::overloads);
	if (it != d.return_opts.end()) {
		return true;
	}

	for (const std::vector<topt>& opts : d.arg_opts) {
		it = std::find(opts.begin(), opts.end(), topt::overloads);
		if (it != opts.end()) {
			return true;
		}
	}
	return false;
}

std::vector<intrin_desc> generate_overload_descs(const intrin_desc& in_desc) {
	assert(needs_overloads(in_desc));

	static const std::vector<topt> overload_opts{
		topt::i8,
		topt::u8,
		topt::i16,
		topt::u16,
		topt::i32,
		topt::u32,
		topt::i64,
		topt::u64,
	};

	auto replace_opt = [](topt new_opt, std::vector<topt>& opts) {
		auto it = std::find(opts.begin(), opts.end(), topt::overloads);
		if (it == opts.end()) {
			return;
		}
		*it = new_opt;
	};

	size_t num_overloads = overload_opts.size();
	std::vector<intrin_desc> ret(num_overloads, in_desc);
	for (size_t i = 0; i < num_overloads; ++i) {
		intrin_desc& new_desc = ret[i];
		topt new_opt = overload_opts[i];

		for (std::vector<topt>& opts : new_desc.arg_opts) {
			replace_opt(new_opt, opts);
		}
		replace_opt(new_opt, new_desc.return_opts);

		assert(!needs_overloads(new_desc));
	}

	return ret;
}

void update_intrin(const intrin_desc& desc, intrin_info& new_info) {
	assert(new_info.args.size() == desc.arg_opts.size());

	auto convert_param = [](const std::vector<topt>& opts, arg_info& arg) {
		if (std::find(opts.begin(), opts.end(), topt::take_address)
				!= opts.end()) {
			arg.take_address = true;
		}

		if (std::find(opts.begin(), opts.end(), topt::reg_to_cpp)
				!= opts.end()) {
			arg.cast_str = '(' + arg.type + ')';
		}

		if (std::find(opts.begin(), opts.end(), topt::cast) != opts.end()) {
			assert(arg.cast_str.empty());
			arg.cast_str = '(' + arg.type + ')';
		}

		arg.type = make_single_type(opts, arg.type);
		arg.is_template = std::find(opts.begin(), opts.end(), topt::template_)
				!= opts.end();
	};

	for (size_t i = 0; i < desc.arg_opts.size(); ++i) {
		const std::vector<topt>& opts = desc.arg_opts[i];
		arg_info& new_arg = new_info.args[i];

		convert_param(opts, new_arg);
	}
	convert_param(desc.return_opts, new_info.return_parameter);

#if FEA_DEBUG
	size_t num_move = std::count(desc.return_opts.begin(),
			desc.return_opts.end(), topt::to_last_param);
	for (const std::vector<topt>& opts : desc.arg_opts) {
		num_move += std::count(opts.begin(), opts.end(), topt::to_last_param);
	}
	assert(num_move <= 1);
#endif

	// Move params to last position when required.
	if (std::find(desc.return_opts.begin(), desc.return_opts.end(),
				topt::to_last_param)
			!= desc.return_opts.end()) {

		// Undecided on output param location.
		if constexpr (true) {
			new_info.args.push_back(new_info.return_parameter);
			new_info.args.back().is_output = true;
		} else {
			new_info.args.insert(
					new_info.args.begin(), new_info.return_parameter);
			new_info.args.front().is_output = true;
		}

		new_info.return_parameter = {};
		new_info.return_parameter.type = "void";
		new_info.return_parameter.varname = "";
		return;
	}

	for (size_t i = 0; i < desc.arg_opts.size(); ++i) {
		const std::vector<topt>& opts = desc.arg_opts[i];

		if (std::find(opts.begin(), opts.end(), topt::to_last_param)
				!= opts.end()) {

			// Undecided on output param location.
			if constexpr (true) {
				if (i == desc.arg_opts.size() - 1) {
					return;
				}

				new_info.args.push_back(new_info.args[i]);
				new_info.args.back().original_idx = i;
				new_info.args.erase(new_info.args.begin() + i);
			} else {
				if (i == 0) {
					return;
				}

				arg_info temp = new_info.args[i];
				temp.original_idx = i;
				new_info.args.erase(new_info.args.begin() + i);
				new_info.args.insert(new_info.args.begin(), temp);
			}
			return;
		}
	}
}
} // namespace detail

void generate_functions(
		const intrin_info& in_info, std::vector<intrin_info>& out) {

	std::vector<intrin_desc> descs;
	{
		const intrin_desc& desc = get_intrin_desc(in_info.intrin_name);
		if (desc.func_name.empty()) {
			// ignored
			return;
		}

		out.clear();

		intrin_info new_info = in_info;
		new_info.func_name = desc.func_name;
		new_info.m32bits = desc.m32bits;
		new_info.commented = desc.commented;

		if (detail::needs_overloads(desc)) {
			descs = detail::generate_overload_descs(desc);
			out.assign(descs.size(), new_info);
		} else {
			descs.push_back(desc);
			out.push_back(new_info);
		}
	}

	assert(out.size() == descs.size());
	for (size_t i = 0; i < out.size(); ++i) {
		detail::update_intrin(descs[i], out[i]);
	}
}

std::string to_string(const intrin_info& in_info) {
	std::string ret = simd_function_template;

	if (in_info.m32bits) {
		ret.insert(1, "#if FEA_32BIT\n");
		ret += "#endif\n";
	}

	bool has_template = false;
	std::string template_str = "template <{arg}>\n";
	for (const arg_info& a : in_info.args) {
		if (a.is_template) {
			has_template = true;
			fea::replace_all_inplace(template_str, "{arg}", a.to_signature());
		}
	}

	if (has_template) {
		fea::replace_all_inplace(ret, "{template}", template_str);
	} else {
		fea::replace_all_inplace(ret, "{template}", "");
	}

	if (in_info.commented) {
		size_t idx = ret.find("FEA_FORCEINLINE");
		if (idx != std::string::npos) {
			ret.insert(idx, "/*");
			idx = ret.find_last_of("}") + 1;
			ret.insert(idx, "*/");
		}
	}

	fea::replace_all_inplace(ret, "{description}", in_info.description);
	fea::replace_all_inplace(ret, "{instruction}", in_info.instruction);
	fea::replace_all_inplace(ret, "{operation}", in_info.operation);
	fea::replace_all_inplace(
			ret, "{return_type}", in_info.return_parameter.type);
	fea::replace_all_inplace(ret, "{func_name}", in_info.func_name);

	std::string maybe_return_str;
	{
		std::string param_sig;
		for (const arg_info& a : in_info.args) {
			if (a.type == "void") {
				continue;
			}

			if (a.is_template) {
				continue;
			}

			if (a.is_output) {
				maybe_return_str = a.to_passed() + " = ";
			}
			param_sig += a.to_signature() + ", ";
		}

		if (!param_sig.empty()) {
			param_sig.pop_back();
			param_sig.pop_back();
		}
		fea::replace_all_inplace(ret, "{signature_params}", param_sig);
	}

	if (!maybe_return_str.empty()) {
		fea::replace_all_inplace(ret, "{maybe_return}", maybe_return_str);
	} else if (in_info.return_parameter.type != "void") {
		fea::replace_all_inplace(ret, "{maybe_return}", "return ");
	} else {
		fea::replace_all_inplace(ret, "{maybe_return}", "");
	}

	fea::replace_all_inplace(ret, "{intrin_name}", in_info.intrin_name);

	{
		std::string passed_params;
		std::vector<arg_info> args = in_info.args;
		auto it = std::find_if(
				args.begin(), args.end(), [](const arg_info& arg) {
					return arg.original_idx
							!= (std::numeric_limits<size_t>::max)();
				});

		if (it != args.end()) {
			arg_info temp = *it;
			args.erase(it);
			args.insert(args.begin() + temp.original_idx, temp);
		}

		for (const arg_info& a : args) {
			if (a.is_output) {
				continue;
			}

			std::string str;

			// TODO
			// if (p.ref_to_ptr) {
			//	str += '&';
			//}

			str += a.to_passed() + ", ";

			// if (p.first_passed) {
			//	passed_params.insert(0, str);
			// } else {
			//	passed_params += str;
			// }

			passed_params += str;
		}

		if (!passed_params.empty()) {
			passed_params.pop_back();
			passed_params.pop_back();
		}
		fea::replace_all_inplace(ret, "{passed_params}", passed_params);
	}

	return ret;
}


std::string load_xml_str(const std::filesystem::path& bin_dir) {
	std::string ret;
	const std::filesystem::path data_filepath
			= bin_dir / "tests_data/intelintrinsicsguide.js";

	std::string data_str;
	fea::open_text_file_raw(data_filepath, data_str);

	if (data_str.empty()) {
		fprintf(stderr, "Problem reading file : '%s'\n",
				data_filepath.string().c_str());
		return ret;
	}

	size_t start_idx = data_str.find_first_of('"') + 1;
	size_t end_idx = data_str.find_last_of('"');
	ret = std::string(data_str.begin() + start_idx, data_str.begin() + end_idx);

	fea::replace_all_inplace(ret, "\\n\\", "");
	fea::replace_all_inplace(ret, "\\\"", "\"");
	return ret;
}

// Returns true on success.
bool load_doc(const std::filesystem::path& bin_dir, pugi::xml_document& doc) {
	const std::filesystem::path xml_cache_filepath
			= bin_dir / "intelinstrinsicsguide_cache.xml";

	// Check if we have an xml cache of the cleaned data.
	if (!std::filesystem::exists(xml_cache_filepath)) {
		std::string xml_str = load_xml_str(bin_dir);
		pugi::xml_parse_result res = doc.load_string(xml_str.c_str());
		if (!res) {
			fprintf(stderr, "pugixml : Couldn't parse xml string.\n");
			return false;
		}

		if (!doc.save_file(xml_cache_filepath.string().c_str())) {
			fprintf(stderr, "pugixml : Couldn't save xml document.\n");
			return false;
		}
	} else {
		doc.load_file(xml_cache_filepath.string().c_str());
	}
	return true;
}

using map_key_t = std::underlying_type_t<fea::simd_ver>;
fea::unsigned_map<map_key_t, std::vector<intrin_info>> deserialize_doc(
		const pugi::xml_document& doc) {
	printf("XML loaded, finding intrinsics...\n");

	fea::unsigned_map<map_key_t, std::vector<intrin_info>> ret;
	pugi::xml_node root = doc.first_child();
	assert(std::string{ root.name() } == "intrinsics_list");

	for (pugi::xml_node intrin : root.children("intrinsic")) {
		// Don't port non-SIMD intrinsics.
		// Q : Maybe we should, with simd_api<simd_ver::count> ?
		if (!intrin.child("CPUID")) {
			continue;
		}

		// Some _ph leaks in avx512f, without types however.
		// Since we would require _ph types, these are not included.
		std::string intrin_name = intrin.attribute("name").value();
		if (fea::ends_with(intrin_name, "_ph")) {
			continue;
		}

		pugi::xml_attribute tech = intrin.attribute("tech");
		if (tech && !supported_cpuids.contains(tech.value())) {
			continue;
		}

		// Some intrinsics are redundant (synonyms that call the same
		// instruction). Skip those we don't want.
		// Ex, _mm_cvt_ps2pi and _mm_cvtps_pi32
		// if (ignored_intrins.contains(intrin_name)) {
		//	continue;
		//}

		// Some instructions have multiple cpuids. Make sure we support
		// all of them.
		bool unsupported = false;
		for (pugi::xml_node cpuid_node : intrin.children("CPUID")) {
			std::string cpuid_str = cpuid_node.text().get();
			if (!supported_cpuids.contains(cpuid_str)) {
				unsupported = true;
				break;
			}
		}
		if (unsupported) {
			continue;
		}

		std::string cpuid_str = intrin.child("CPUID").text().get();
		map_key_t ver = map_key_t(supported_cpuids.at(cpuid_str));
		if (!ret.contains(ver)) {
			ret.insert({ ver, {} });
		}

		std::vector<intrin_info>& vec = ret.at_unchecked(ver);
		vec.push_back(intrin_info{ intrin });

		printf("\tFound : %s ", intrin.first_attribute().value());
		printf("[%s]\n", intrin.child("CPUID").text().get());
	}

	return ret;
}

bool param_type_compare(const std::string& lhs, const std::string& rhs) {
	std::string mlhs = lhs;
	std::string mrhs = rhs;

	fea::replace_all_inplace(mlhs, "m64_", "");
	fea::replace_all_inplace(mlhs, "m128_", "");
	fea::replace_all_inplace(mlhs, "m256_", "");
	fea::replace_all_inplace(mlhs, "m512_", "");
	fea::replace_all_inplace(mrhs, "m64_", "");
	fea::replace_all_inplace(mrhs, "m128_", "");
	fea::replace_all_inplace(mrhs, "m256_", "");
	fea::replace_all_inplace(mrhs, "m512_", "");

	auto get_info = [](std::string& str, char& prefix, int& val) {
		if (fea::contains(str, "_t")) {
			prefix = str.front();
			assert(prefix == 'f' || prefix == 'i' || prefix == 'u');
			fea::replace_all_inplace(str, prefix, "");

			size_t idx = str.find("_t");
			str = str.substr(0, idx);
			val = std::stoi(str);
		}
	};

	char lhs_prefix = '\0';
	int lhs_size = 0;
	char rhs_prefix = '\0';
	int rhs_size = 0;
	get_info(mlhs, lhs_prefix, lhs_size);
	get_info(mrhs, rhs_prefix, rhs_size);

	if (lhs_prefix == '\0' || rhs_prefix == '\0') {
		return mlhs < mrhs;
	}

	if (lhs_prefix == rhs_prefix) {
		return lhs_size < rhs_size;
	}
	return lhs_prefix < rhs_prefix;
}

bool param_compare(const intrin_info& lhs, const intrin_info& rhs) {
	const arg_info& lhs_ret = lhs.return_parameter;
	const arg_info& rhs_ret = rhs.return_parameter;

	if (lhs_ret.type == rhs_ret.type) {
		size_t count = std::min(lhs.args.size(), rhs.args.size());
		for (size_t i = 0; i < count; ++i) {
			const arg_info& lhs_p = lhs.args[i];
			const arg_info& rhs_p = rhs.args[i];
			if (lhs_p.type == rhs_p.type) {
				continue;
			}

			return param_type_compare(lhs_p.type, rhs_p.type);
		}
	}

	return param_type_compare(lhs_ret.type, rhs_ret.type);
}

fea::unsigned_map<map_key_t, std::vector<intrin_info>> create_overloads(
		const fea::unsigned_map<map_key_t, std::vector<intrin_info>>& in) {

	fea::unsigned_map<map_key_t, std::vector<intrin_info>> ret;
	for (const std::pair<map_key_t, std::vector<intrin_info>>& p : in) {
		ret.insert({ p.first, {} });
		ret.at(p.first).reserve(p.second.size());
	}

	std::vector<intrin_info> new_intrins;
	for (const std::pair<map_key_t, std::vector<intrin_info>>& p : in) {
		std::vector<intrin_info>& ret_vec = ret.at(p.first);

		for (const intrin_info& info : p.second) {
			new_intrins.clear();
			generate_functions(info, new_intrins);

			// #if FEA_DEBUG
			//			for (const intrin_info& new_info : new_intrins) {
			//				if (new_info.commented) {
			//					continue;
			//				}
			//				assert(!register_to_fea_prefix.contains(
			//						new_info.return_parameter.type));
			//				for (const parameter_info& param :
			// new_info.parameters) {
			// assert(!register_to_fea_prefix.contains(param.type));
			//					assert(param.direct_conversion == true);
			//				}
			//			}
			// #endif

			ret_vec.insert(
					ret_vec.end(), new_intrins.begin(), new_intrins.end());
		}

		std::sort(ret_vec.begin(), ret_vec.end(),
				[](const intrin_info& lhs, const intrin_info& rhs) {
					if (lhs.intrin_name == rhs.intrin_name) {
						return param_compare(lhs, rhs);
					}
					return lhs.intrin_name < rhs.intrin_name;
				});
	}

	return ret;
}

void save_headers(const std::filesystem::path& bin_dir,
		const fea::unsigned_map<map_key_t, std::vector<intrin_info>>& db) {
	std::string out;

	for (map_key_t i = 0; i < map_key_t(fea::simd_ver::count); ++i) {
		if (!db.contains(i)) {
			continue;
		}

		fea::simd_ver ver = fea::simd_ver(i);
		const std::string& out_filename = ver_to_filename[ver];

		out = simd_api_header;

		// Include the previous api header.
		if (ver == fea::simd_ver::mmx) {
			fea::replace_all_inplace(out, "{prev_include}", "");
		} else {
			std::string prev_include = simd_prev_include;
			const std::string& prev_filename = ver_to_filename[ver - 1];
			fea::replace_all_inplace(
					prev_include, "{prev_filename}", prev_filename);
			fea::replace_all_inplace(out, "{prev_include}", prev_include);
		}

		// Add unspecialized class aliases, for autocomplete.
		if (ver == fea::simd_ver::mmx) {
			fea::replace_all_inplace(out, "{simd_api_root}", simd_api_root);
		} else {
			fea::replace_all_inplace(out, "{simd_api_root}", "");
		}

		// Add our version for our aliases.
		{
			const std::string& ver_str = ver_to_fea_enum_str[ver];
			fea::replace_all_inplace(out, "{simd_api_ver}", ver_str);
		}

		// Inherit the previous api.
		if (ver == fea::simd_ver::mmx) {
			fea::replace_all_inplace(out, "{prev_api_inherit}", "");
		} else {
			std::string inherit_str = prev_api_inherit;
			const std::string& prev_ver_str = ver_to_fea_enum_str[ver - 1];
			fea::replace_all_inplace(
					inherit_str, "{simd_api_prev_ver}", prev_ver_str);
			fea::replace_all_inplace(out, "{prev_api_inherit}", inherit_str);
		}

		// MMX is 32bit only
		if (ver == fea::simd_ver::mmx) {
			fea::replace_all_inplace(out, "{if_32bit}", "#if FEA_32BIT");
			fea::replace_all_inplace(out, "{endif_32bit}", "#endif");
		} else {
			fea::replace_all_inplace(out, "{if_32bit}", "");
			fea::replace_all_inplace(out, "{endif_32bit}", "");
		}

		// Final aliases.
		const std::string& api_name = ver_to_api_name[ver];
		fea::replace_all_inplace(out, "{api_name}", api_name);

		// Fill the actual functions.
		{
			std::string contents;

			const std::vector<intrin_info>& data = db.at(i);
			for (const intrin_info& intrin : data) {
				contents += to_string(intrin);
			}

			fea::replace_all_inplace(out, "{simd_api_contents}", contents);
		}

		const std::filesystem::path header_filepath = bin_dir / out_filename;
		std::ofstream ofs{ header_filepath };
		if (!ofs.is_open()) {
			fprintf(stderr, "Couldn't open file for writing : '%s'\n",
					header_filepath.string().c_str());
		}
		ofs << out;
	}
}

TEST(simd, generate_header) {
	const std::filesystem::path bin_dir = fea::executable_dir(argv0);

	pugi::xml_document doc;
	if (!load_doc(bin_dir, doc)) {
		return;
	}

	// All our intrsics data.
	fea::unsigned_map<map_key_t, std::vector<intrin_info>> db
			= deserialize_doc(doc);

	// To fix signature problems like multi-return types and collisions.
	db = create_overloads(db);

	// The header text.
	save_headers(bin_dir, db);
}

// template <fea::simd_ver Ver>
// void test(fea::simd_api<Ver> api) {
// }

#endif
} // namespace
