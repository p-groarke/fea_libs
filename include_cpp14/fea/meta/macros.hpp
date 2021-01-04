/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2020, Philippe Groarke
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

/*
A collection of useful macros, and a few "don't show your mom" ones.

See : https://codecraft.co/2014/11/25/variadic-macros-tricks/
*/

#define FEA_DETAIL_PASTE(x, y) x##y
#define FEA_PASTE(x, y) FEA_DETAIL_PASTE(x, y)

#define FEA_STRINGIFY(x) #x
#define FEA_WSTRINGIFY(x) L#x
// clang-format off
#define FEA_U16STRINGIFY(x) u#x
#define FEA_U32STRINGIFY(x) U#x
// clang-format on

#define FEA_STRINGIFY_COMMA(x) FEA_STRINGIFY(x),
#define FEA_WSTRINGIFY_COMMA(x) FEA_WSTRINGIFY(x),
#define FEA_U16STRINGIFY_COMMA(x) FEA_U16STRINGIFY(x),
#define FEA_U32STRINGIFY_COMMA(x) FEA_U32STRINGIFY(x),


// For MSVC, works on other platforms.
#define FEA_DETAIL_EXPAND(x) x

// Accept any number of args >= N, but expand to just the Nth one.
#define FEA_GET_NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, \
		_13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, \
		_27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
		_41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, \
		_55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, \
		_69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, \
		_83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, \
		_97, _98, _99, N, ...) \
	N

#define FEA_SIZEOF_VAARGS(...) \
	FEA_DETAIL_EXPAND(FEA_GET_NTH_ARG(__VA_ARGS__, 99, 98, 97, 96, 95, 94, 93, \
			92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, \
			76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, \
			60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, \
			44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, \
			28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, \
			12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

// Define some macros to help us create overrides based on the
// arity of a for-each-style macro.
#define FEA_DETAIL_FECALL_0(call, ...)
#define FEA_DETAIL_FECALL_1(call, f) call(f)
#define FEA_DETAIL_FECALL_2(call, f, ...) \
	call(f) FEA_DETAIL_FECALL_1(call, __VA_ARGS__)
#define FEA_DETAIL_FECALL_3(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_2(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_4(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_3(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_5(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_4(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_6(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_5(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_7(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_6(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_8(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_7(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_9(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_8(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_10(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_9(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_11(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_10(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_12(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_11(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_13(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_12(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_14(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_13(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_15(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_14(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_16(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_15(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_17(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_16(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_18(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_17(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_19(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_18(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_20(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_19(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_21(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_20(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_22(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_21(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_23(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_22(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_24(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_23(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_25(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_24(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_26(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_25(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_27(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_26(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_28(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_27(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_29(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_28(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_30(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_29(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_31(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_30(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_32(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_31(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_33(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_32(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_34(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_33(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_35(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_34(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_36(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_35(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_37(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_36(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_38(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_37(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_39(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_38(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_40(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_39(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_41(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_40(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_42(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_41(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_43(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_42(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_44(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_43(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_45(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_44(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_46(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_45(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_47(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_46(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_48(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_47(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_49(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_48(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_50(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_49(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_51(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_50(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_52(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_51(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_53(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_52(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_54(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_53(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_55(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_54(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_56(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_55(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_57(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_56(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_58(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_57(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_59(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_58(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_60(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_59(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_61(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_60(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_62(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_61(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_63(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_62(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_64(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_63(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_65(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_64(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_66(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_65(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_67(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_66(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_68(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_67(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_69(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_68(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_70(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_69(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_71(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_70(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_72(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_71(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_73(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_72(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_74(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_73(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_75(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_74(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_76(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_75(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_77(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_76(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_78(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_77(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_79(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_78(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_80(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_79(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_81(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_80(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_82(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_81(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_83(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_82(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_84(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_83(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_85(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_84(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_86(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_85(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_87(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_86(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_88(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_87(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_89(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_88(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_90(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_89(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_91(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_90(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_92(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_91(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_93(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_92(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_94(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_93(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_95(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_94(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_96(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_95(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_97(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_96(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_98(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_97(call, __VA_ARGS__))
#define FEA_DETAIL_FECALL_99(call, f, ...) \
	call(f) FEA_DETAIL_EXPAND(FEA_DETAIL_FECALL_98(call, __VA_ARGS__))


/**
 * Provide a for-each construct for variadic macros. Supports up
 * to 4 args.
 *
 * Example usage1:
 *     #define FWD_DECLARE_CLASS(cls) class cls;
 *     CALL_MACRO_X_FOR_EACH(FWD_DECLARE_CLASS, Foo, Bar)
 *
 * Example usage 2:
 *     #define START_NS(ns) namespace ns {
 *     #define END_NS(ns) }
 *     #define MY_NAMESPACES System, Net, Http
 *     CALL_MACRO_X_FOR_EACH(START_NS, MY_NAMESPACES)
 *     typedef foo int;
 *     CALL_MACRO_X_FOR_EACH(END_NS, MY_NAMESPACES)
 */
#define FEA_FOR_EACH(macro, ...) \
	FEA_DETAIL_EXPAND(FEA_GET_NTH_ARG(__VA_ARGS__, FEA_DETAIL_FECALL_99, \
			FEA_DETAIL_FECALL_98, FEA_DETAIL_FECALL_97, FEA_DETAIL_FECALL_96, \
			FEA_DETAIL_FECALL_95, FEA_DETAIL_FECALL_94, FEA_DETAIL_FECALL_93, \
			FEA_DETAIL_FECALL_92, FEA_DETAIL_FECALL_91, FEA_DETAIL_FECALL_90, \
			FEA_DETAIL_FECALL_89, FEA_DETAIL_FECALL_88, FEA_DETAIL_FECALL_87, \
			FEA_DETAIL_FECALL_86, FEA_DETAIL_FECALL_85, FEA_DETAIL_FECALL_84, \
			FEA_DETAIL_FECALL_83, FEA_DETAIL_FECALL_82, FEA_DETAIL_FECALL_81, \
			FEA_DETAIL_FECALL_80, FEA_DETAIL_FECALL_79, FEA_DETAIL_FECALL_78, \
			FEA_DETAIL_FECALL_77, FEA_DETAIL_FECALL_76, FEA_DETAIL_FECALL_75, \
			FEA_DETAIL_FECALL_74, FEA_DETAIL_FECALL_73, FEA_DETAIL_FECALL_72, \
			FEA_DETAIL_FECALL_71, FEA_DETAIL_FECALL_70, FEA_DETAIL_FECALL_69, \
			FEA_DETAIL_FECALL_68, FEA_DETAIL_FECALL_67, FEA_DETAIL_FECALL_66, \
			FEA_DETAIL_FECALL_65, FEA_DETAIL_FECALL_64, FEA_DETAIL_FECALL_63, \
			FEA_DETAIL_FECALL_62, FEA_DETAIL_FECALL_61, FEA_DETAIL_FECALL_60, \
			FEA_DETAIL_FECALL_59, FEA_DETAIL_FECALL_58, FEA_DETAIL_FECALL_57, \
			FEA_DETAIL_FECALL_56, FEA_DETAIL_FECALL_55, FEA_DETAIL_FECALL_54, \
			FEA_DETAIL_FECALL_53, FEA_DETAIL_FECALL_52, FEA_DETAIL_FECALL_51, \
			FEA_DETAIL_FECALL_50, FEA_DETAIL_FECALL_49, FEA_DETAIL_FECALL_48, \
			FEA_DETAIL_FECALL_47, FEA_DETAIL_FECALL_46, FEA_DETAIL_FECALL_45, \
			FEA_DETAIL_FECALL_44, FEA_DETAIL_FECALL_43, FEA_DETAIL_FECALL_42, \
			FEA_DETAIL_FECALL_41, FEA_DETAIL_FECALL_40, FEA_DETAIL_FECALL_39, \
			FEA_DETAIL_FECALL_38, FEA_DETAIL_FECALL_37, FEA_DETAIL_FECALL_36, \
			FEA_DETAIL_FECALL_35, FEA_DETAIL_FECALL_34, FEA_DETAIL_FECALL_33, \
			FEA_DETAIL_FECALL_32, FEA_DETAIL_FECALL_31, FEA_DETAIL_FECALL_30, \
			FEA_DETAIL_FECALL_29, FEA_DETAIL_FECALL_28, FEA_DETAIL_FECALL_27, \
			FEA_DETAIL_FECALL_26, FEA_DETAIL_FECALL_25, FEA_DETAIL_FECALL_24, \
			FEA_DETAIL_FECALL_23, FEA_DETAIL_FECALL_22, FEA_DETAIL_FECALL_21, \
			FEA_DETAIL_FECALL_20, FEA_DETAIL_FECALL_19, FEA_DETAIL_FECALL_18, \
			FEA_DETAIL_FECALL_17, FEA_DETAIL_FECALL_16, FEA_DETAIL_FECALL_15, \
			FEA_DETAIL_FECALL_14, FEA_DETAIL_FECALL_13, FEA_DETAIL_FECALL_12, \
			FEA_DETAIL_FECALL_11, FEA_DETAIL_FECALL_10, FEA_DETAIL_FECALL_9, \
			FEA_DETAIL_FECALL_8, FEA_DETAIL_FECALL_7, FEA_DETAIL_FECALL_6, \
			FEA_DETAIL_FECALL_5, FEA_DETAIL_FECALL_4, FEA_DETAIL_FECALL_3, \
			FEA_DETAIL_FECALL_2, FEA_DETAIL_FECALL_1, \
			FEA_DETAIL_FECALL_0)(macro, __VA_ARGS__))
