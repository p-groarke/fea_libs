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
#include "fea/macros/macros.hpp"

/*
See : https://codecraft.co/2014/11/25/variadic-macros-tricks/
*/

// Define some macros to help us create overrides based on the
// arity of a for-each-style macro.
#define FEA_DETAIL_FECALL_0(call)
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
 * to 99 args.
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


// Helper macros for FEA_FOR_EACH_VA
#define FEA_DETAIL_FECALLVA_0(call)
#define FEA_DETAIL_FECALLVA_1(call, args, f) call(f, args)
#define FEA_DETAIL_FECALLVA_2(call, args, f, ...) \
	call(f, args) FEA_DETAIL_FECALLVA_1(call, FEA_VA_LIST(args), __VA_ARGS__)
#define FEA_DETAIL_FECALLVA_3(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_2(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_4(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_3(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_5(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_4(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_6(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_5(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_7(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_6(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_8(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_7(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_9(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_8(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_10(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_9(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_11(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_10(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_12(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_11(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_13(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_12(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_14(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_13(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_15(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_14(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_16(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_15(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_17(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_16(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_18(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_17(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_19(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_18(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_20(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_19(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_21(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_20(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_22(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_21(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_23(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_22(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_24(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_23(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_25(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_24(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_26(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_25(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_27(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_26(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_28(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_27(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_29(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_28(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_30(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_29(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_31(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_30(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_32(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_31(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_33(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_32(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_34(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_33(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_35(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_34(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_36(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_35(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_37(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_36(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_38(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_37(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_39(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_38(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_40(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_39(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_41(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_40(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_42(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_41(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_43(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_42(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_44(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_43(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_45(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_44(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_46(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_45(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_47(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_46(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_48(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_47(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_49(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_48(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_50(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_49(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_51(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_50(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_52(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_51(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_53(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_52(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_54(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_53(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_55(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_54(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_56(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_55(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_57(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_56(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_58(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_57(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_59(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_58(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_60(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_59(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_61(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_60(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_62(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_61(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_63(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_62(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_64(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_63(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_65(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_64(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_66(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_65(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_67(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_66(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_68(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_67(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_69(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_68(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_70(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_69(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_71(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_70(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_72(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_71(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_73(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_72(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_74(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_73(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_75(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_74(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_76(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_75(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_77(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_76(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_78(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_77(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_79(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_78(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_80(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_79(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_81(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_80(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_82(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_81(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_83(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_82(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_84(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_83(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_85(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_84(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_86(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_85(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_87(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_86(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_88(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_87(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_89(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_88(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_90(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_89(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_91(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_90(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_92(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_91(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_93(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_92(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_94(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_93(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_95(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_94(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_96(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_95(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_97(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_96(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_98(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_97(call, FEA_VA_LIST(args), __VA_ARGS__))
#define FEA_DETAIL_FECALLVA_99(call, args, f, ...) \
	call(f, args) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLVA_98(call, FEA_VA_LIST(args), __VA_ARGS__))


// Calls the provided macro on each parameter of the variadic macro.
// Also provides the original __VA_ARGS__ to the macro "lambda".
// ex.
// #define my_macro(val, ...)
// FEA_FOR_EACH_VA(my_macro, ...)
#define FEA_FOR_EACH_VA(macro, ...) \
	FEA_DETAIL_EXPAND(FEA_GET_NTH_ARG(__VA_ARGS__, FEA_DETAIL_FECALLVA_99, \
			FEA_DETAIL_FECALLVA_98, FEA_DETAIL_FECALLVA_97, \
			FEA_DETAIL_FECALLVA_96, FEA_DETAIL_FECALLVA_95, \
			FEA_DETAIL_FECALLVA_94, FEA_DETAIL_FECALLVA_93, \
			FEA_DETAIL_FECALLVA_92, FEA_DETAIL_FECALLVA_91, \
			FEA_DETAIL_FECALLVA_90, FEA_DETAIL_FECALLVA_89, \
			FEA_DETAIL_FECALLVA_88, FEA_DETAIL_FECALLVA_87, \
			FEA_DETAIL_FECALLVA_86, FEA_DETAIL_FECALLVA_85, \
			FEA_DETAIL_FECALLVA_84, FEA_DETAIL_FECALLVA_83, \
			FEA_DETAIL_FECALLVA_82, FEA_DETAIL_FECALLVA_81, \
			FEA_DETAIL_FECALLVA_80, FEA_DETAIL_FECALLVA_79, \
			FEA_DETAIL_FECALLVA_78, FEA_DETAIL_FECALLVA_77, \
			FEA_DETAIL_FECALLVA_76, FEA_DETAIL_FECALLVA_75, \
			FEA_DETAIL_FECALLVA_74, FEA_DETAIL_FECALLVA_73, \
			FEA_DETAIL_FECALLVA_72, FEA_DETAIL_FECALLVA_71, \
			FEA_DETAIL_FECALLVA_70, FEA_DETAIL_FECALLVA_69, \
			FEA_DETAIL_FECALLVA_68, FEA_DETAIL_FECALLVA_67, \
			FEA_DETAIL_FECALLVA_66, FEA_DETAIL_FECALLVA_65, \
			FEA_DETAIL_FECALLVA_64, FEA_DETAIL_FECALLVA_63, \
			FEA_DETAIL_FECALLVA_62, FEA_DETAIL_FECALLVA_61, \
			FEA_DETAIL_FECALLVA_60, FEA_DETAIL_FECALLVA_59, \
			FEA_DETAIL_FECALLVA_58, FEA_DETAIL_FECALLVA_57, \
			FEA_DETAIL_FECALLVA_56, FEA_DETAIL_FECALLVA_55, \
			FEA_DETAIL_FECALLVA_54, FEA_DETAIL_FECALLVA_53, \
			FEA_DETAIL_FECALLVA_52, FEA_DETAIL_FECALLVA_51, \
			FEA_DETAIL_FECALLVA_50, FEA_DETAIL_FECALLVA_49, \
			FEA_DETAIL_FECALLVA_48, FEA_DETAIL_FECALLVA_47, \
			FEA_DETAIL_FECALLVA_46, FEA_DETAIL_FECALLVA_45, \
			FEA_DETAIL_FECALLVA_44, FEA_DETAIL_FECALLVA_43, \
			FEA_DETAIL_FECALLVA_42, FEA_DETAIL_FECALLVA_41, \
			FEA_DETAIL_FECALLVA_40, FEA_DETAIL_FECALLVA_39, \
			FEA_DETAIL_FECALLVA_38, FEA_DETAIL_FECALLVA_37, \
			FEA_DETAIL_FECALLVA_36, FEA_DETAIL_FECALLVA_35, \
			FEA_DETAIL_FECALLVA_34, FEA_DETAIL_FECALLVA_33, \
			FEA_DETAIL_FECALLVA_32, FEA_DETAIL_FECALLVA_31, \
			FEA_DETAIL_FECALLVA_30, FEA_DETAIL_FECALLVA_29, \
			FEA_DETAIL_FECALLVA_28, FEA_DETAIL_FECALLVA_27, \
			FEA_DETAIL_FECALLVA_26, FEA_DETAIL_FECALLVA_25, \
			FEA_DETAIL_FECALLVA_24, FEA_DETAIL_FECALLVA_23, \
			FEA_DETAIL_FECALLVA_22, FEA_DETAIL_FECALLVA_21, \
			FEA_DETAIL_FECALLVA_20, FEA_DETAIL_FECALLVA_19, \
			FEA_DETAIL_FECALLVA_18, FEA_DETAIL_FECALLVA_17, \
			FEA_DETAIL_FECALLVA_16, FEA_DETAIL_FECALLVA_15, \
			FEA_DETAIL_FECALLVA_14, FEA_DETAIL_FECALLVA_13, \
			FEA_DETAIL_FECALLVA_12, FEA_DETAIL_FECALLVA_11, \
			FEA_DETAIL_FECALLVA_10, FEA_DETAIL_FECALLVA_9, \
			FEA_DETAIL_FECALLVA_8, FEA_DETAIL_FECALLVA_7, \
			FEA_DETAIL_FECALLVA_6, FEA_DETAIL_FECALLVA_5, \
			FEA_DETAIL_FECALLVA_4, FEA_DETAIL_FECALLVA_3, \
			FEA_DETAIL_FECALLVA_2, FEA_DETAIL_FECALLVA_1, \
			FEA_DETAIL_FECALLVA_0)( \
			macro, FEA_VA_LIST(__VA_ARGS__), __VA_ARGS__))

// Calls the provided macro on each parameter of the variadic macro.
// Passes on macro_args to your macro lambda.
// ex.
// #define my_macro(some, args, x)
// FEA_FOR_EACH_PARAM(my_macro, FEA_VA_LIST(1, 2), 1, 2);
#define FEA_FOR_EACH_PARAM(macro, macro_args, ...) \
	FEA_DETAIL_EXPAND(FEA_GET_NTH_ARG(__VA_ARGS__, FEA_DETAIL_FECALLVA_99, \
			FEA_DETAIL_FECALLVA_98, FEA_DETAIL_FECALLVA_97, \
			FEA_DETAIL_FECALLVA_96, FEA_DETAIL_FECALLVA_95, \
			FEA_DETAIL_FECALLVA_94, FEA_DETAIL_FECALLVA_93, \
			FEA_DETAIL_FECALLVA_92, FEA_DETAIL_FECALLVA_91, \
			FEA_DETAIL_FECALLVA_90, FEA_DETAIL_FECALLVA_89, \
			FEA_DETAIL_FECALLVA_88, FEA_DETAIL_FECALLVA_87, \
			FEA_DETAIL_FECALLVA_86, FEA_DETAIL_FECALLVA_85, \
			FEA_DETAIL_FECALLVA_84, FEA_DETAIL_FECALLVA_83, \
			FEA_DETAIL_FECALLVA_82, FEA_DETAIL_FECALLVA_81, \
			FEA_DETAIL_FECALLVA_80, FEA_DETAIL_FECALLVA_79, \
			FEA_DETAIL_FECALLVA_78, FEA_DETAIL_FECALLVA_77, \
			FEA_DETAIL_FECALLVA_76, FEA_DETAIL_FECALLVA_75, \
			FEA_DETAIL_FECALLVA_74, FEA_DETAIL_FECALLVA_73, \
			FEA_DETAIL_FECALLVA_72, FEA_DETAIL_FECALLVA_71, \
			FEA_DETAIL_FECALLVA_70, FEA_DETAIL_FECALLVA_69, \
			FEA_DETAIL_FECALLVA_68, FEA_DETAIL_FECALLVA_67, \
			FEA_DETAIL_FECALLVA_66, FEA_DETAIL_FECALLVA_65, \
			FEA_DETAIL_FECALLVA_64, FEA_DETAIL_FECALLVA_63, \
			FEA_DETAIL_FECALLVA_62, FEA_DETAIL_FECALLVA_61, \
			FEA_DETAIL_FECALLVA_60, FEA_DETAIL_FECALLVA_59, \
			FEA_DETAIL_FECALLVA_58, FEA_DETAIL_FECALLVA_57, \
			FEA_DETAIL_FECALLVA_56, FEA_DETAIL_FECALLVA_55, \
			FEA_DETAIL_FECALLVA_54, FEA_DETAIL_FECALLVA_53, \
			FEA_DETAIL_FECALLVA_52, FEA_DETAIL_FECALLVA_51, \
			FEA_DETAIL_FECALLVA_50, FEA_DETAIL_FECALLVA_49, \
			FEA_DETAIL_FECALLVA_48, FEA_DETAIL_FECALLVA_47, \
			FEA_DETAIL_FECALLVA_46, FEA_DETAIL_FECALLVA_45, \
			FEA_DETAIL_FECALLVA_44, FEA_DETAIL_FECALLVA_43, \
			FEA_DETAIL_FECALLVA_42, FEA_DETAIL_FECALLVA_41, \
			FEA_DETAIL_FECALLVA_40, FEA_DETAIL_FECALLVA_39, \
			FEA_DETAIL_FECALLVA_38, FEA_DETAIL_FECALLVA_37, \
			FEA_DETAIL_FECALLVA_36, FEA_DETAIL_FECALLVA_35, \
			FEA_DETAIL_FECALLVA_34, FEA_DETAIL_FECALLVA_33, \
			FEA_DETAIL_FECALLVA_32, FEA_DETAIL_FECALLVA_31, \
			FEA_DETAIL_FECALLVA_30, FEA_DETAIL_FECALLVA_29, \
			FEA_DETAIL_FECALLVA_28, FEA_DETAIL_FECALLVA_27, \
			FEA_DETAIL_FECALLVA_26, FEA_DETAIL_FECALLVA_25, \
			FEA_DETAIL_FECALLVA_24, FEA_DETAIL_FECALLVA_23, \
			FEA_DETAIL_FECALLVA_22, FEA_DETAIL_FECALLVA_21, \
			FEA_DETAIL_FECALLVA_20, FEA_DETAIL_FECALLVA_19, \
			FEA_DETAIL_FECALLVA_18, FEA_DETAIL_FECALLVA_17, \
			FEA_DETAIL_FECALLVA_16, FEA_DETAIL_FECALLVA_15, \
			FEA_DETAIL_FECALLVA_14, FEA_DETAIL_FECALLVA_13, \
			FEA_DETAIL_FECALLVA_12, FEA_DETAIL_FECALLVA_11, \
			FEA_DETAIL_FECALLVA_10, FEA_DETAIL_FECALLVA_9, \
			FEA_DETAIL_FECALLVA_8, FEA_DETAIL_FECALLVA_7, \
			FEA_DETAIL_FECALLVA_6, FEA_DETAIL_FECALLVA_5, \
			FEA_DETAIL_FECALLVA_4, FEA_DETAIL_FECALLVA_3, \
			FEA_DETAIL_FECALLVA_2, FEA_DETAIL_FECALLVA_1, \
			FEA_DETAIL_FECALLVA_0)( \
			macro, FEA_VA_LIST(macro_args), __VA_ARGS__))

#define FEA_DETAIL_FECALLLAST_0(call)
#define FEA_DETAIL_FECALLLAST_1(call, last_call, f) last_call(f)
#define FEA_DETAIL_FECALLLAST_2(call, last_call, f, ...) \
	call(f) FEA_DETAIL_FECALLLAST_1(call, last_call, __VA_ARGS__)
#define FEA_DETAIL_FECALLLAST_3(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_2(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_4(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_3(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_5(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_4(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_6(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_5(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_7(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_6(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_8(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_7(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_9(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_8(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_10(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_9(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_11(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_10(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_12(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_11(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_13(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_12(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_14(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_13(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_15(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_14(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_16(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_15(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_17(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_16(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_18(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_17(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_19(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_18(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_20(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_19(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_21(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_20(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_22(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_21(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_23(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_22(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_24(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_23(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_25(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_24(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_26(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_25(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_27(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_26(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_28(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_27(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_29(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_28(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_30(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_29(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_31(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_30(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_32(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_31(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_33(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_32(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_34(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_33(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_35(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_34(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_36(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_35(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_37(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_36(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_38(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_37(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_39(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_38(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_40(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_39(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_41(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_40(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_42(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_41(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_43(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_42(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_44(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_43(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_45(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_44(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_46(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_45(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_47(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_46(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_48(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_47(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_49(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_48(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_50(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_49(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_51(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_50(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_52(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_51(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_53(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_52(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_54(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_53(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_55(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_54(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_56(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_55(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_57(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_56(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_58(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_57(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_59(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_58(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_60(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_59(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_61(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_60(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_62(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_61(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_63(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_62(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_64(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_63(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_65(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_64(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_66(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_65(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_67(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_66(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_68(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_67(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_69(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_68(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_70(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_69(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_71(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_70(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_72(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_71(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_73(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_72(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_74(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_73(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_75(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_74(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_76(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_75(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_77(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_76(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_78(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_77(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_79(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_78(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_80(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_79(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_81(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_80(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_82(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_81(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_83(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_82(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_84(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_83(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_85(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_84(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_86(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_85(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_87(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_86(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_88(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_87(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_89(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_88(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_90(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_89(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_91(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_90(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_92(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_91(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_93(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_92(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_94(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_93(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_95(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_94(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_96(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_95(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_97(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_96(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_98(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_97(call, last_call, __VA_ARGS__))
#define FEA_DETAIL_FECALLLAST_99(call, last_call, f, ...) \
	call(f) FEA_DETAIL_EXPAND( \
			FEA_DETAIL_FECALLLAST_98(call, last_call, __VA_ARGS__))


// Calls your provided macro on each variadic parameter.
// Uses a second macro for the last element.
// ex.
// #define add_comma(x) x,
// #define last(x) x
// FEA_FOR_EACH_LAST(add_comma, last, ...)
#define FEA_FOR_EACH_LAST(macro, last_macro, ...) \
	FEA_DETAIL_EXPAND(FEA_GET_NTH_ARG(__VA_ARGS__, FEA_DETAIL_FECALLLAST_99, \
			FEA_DETAIL_FECALLLAST_98, FEA_DETAIL_FECALLLAST_97, \
			FEA_DETAIL_FECALLLAST_96, FEA_DETAIL_FECALLLAST_95, \
			FEA_DETAIL_FECALLLAST_94, FEA_DETAIL_FECALLLAST_93, \
			FEA_DETAIL_FECALLLAST_92, FEA_DETAIL_FECALLLAST_91, \
			FEA_DETAIL_FECALLLAST_90, FEA_DETAIL_FECALLLAST_89, \
			FEA_DETAIL_FECALLLAST_88, FEA_DETAIL_FECALLLAST_87, \
			FEA_DETAIL_FECALLLAST_86, FEA_DETAIL_FECALLLAST_85, \
			FEA_DETAIL_FECALLLAST_84, FEA_DETAIL_FECALLLAST_83, \
			FEA_DETAIL_FECALLLAST_82, FEA_DETAIL_FECALLLAST_81, \
			FEA_DETAIL_FECALLLAST_80, FEA_DETAIL_FECALLLAST_79, \
			FEA_DETAIL_FECALLLAST_78, FEA_DETAIL_FECALLLAST_77, \
			FEA_DETAIL_FECALLLAST_76, FEA_DETAIL_FECALLLAST_75, \
			FEA_DETAIL_FECALLLAST_74, FEA_DETAIL_FECALLLAST_73, \
			FEA_DETAIL_FECALLLAST_72, FEA_DETAIL_FECALLLAST_71, \
			FEA_DETAIL_FECALLLAST_70, FEA_DETAIL_FECALLLAST_69, \
			FEA_DETAIL_FECALLLAST_68, FEA_DETAIL_FECALLLAST_67, \
			FEA_DETAIL_FECALLLAST_66, FEA_DETAIL_FECALLLAST_65, \
			FEA_DETAIL_FECALLLAST_64, FEA_DETAIL_FECALLLAST_63, \
			FEA_DETAIL_FECALLLAST_62, FEA_DETAIL_FECALLLAST_61, \
			FEA_DETAIL_FECALLLAST_60, FEA_DETAIL_FECALLLAST_59, \
			FEA_DETAIL_FECALLLAST_58, FEA_DETAIL_FECALLLAST_57, \
			FEA_DETAIL_FECALLLAST_56, FEA_DETAIL_FECALLLAST_55, \
			FEA_DETAIL_FECALLLAST_54, FEA_DETAIL_FECALLLAST_53, \
			FEA_DETAIL_FECALLLAST_52, FEA_DETAIL_FECALLLAST_51, \
			FEA_DETAIL_FECALLLAST_50, FEA_DETAIL_FECALLLAST_49, \
			FEA_DETAIL_FECALLLAST_48, FEA_DETAIL_FECALLLAST_47, \
			FEA_DETAIL_FECALLLAST_46, FEA_DETAIL_FECALLLAST_45, \
			FEA_DETAIL_FECALLLAST_44, FEA_DETAIL_FECALLLAST_43, \
			FEA_DETAIL_FECALLLAST_42, FEA_DETAIL_FECALLLAST_41, \
			FEA_DETAIL_FECALLLAST_40, FEA_DETAIL_FECALLLAST_39, \
			FEA_DETAIL_FECALLLAST_38, FEA_DETAIL_FECALLLAST_37, \
			FEA_DETAIL_FECALLLAST_36, FEA_DETAIL_FECALLLAST_35, \
			FEA_DETAIL_FECALLLAST_34, FEA_DETAIL_FECALLLAST_33, \
			FEA_DETAIL_FECALLLAST_32, FEA_DETAIL_FECALLLAST_31, \
			FEA_DETAIL_FECALLLAST_30, FEA_DETAIL_FECALLLAST_29, \
			FEA_DETAIL_FECALLLAST_28, FEA_DETAIL_FECALLLAST_27, \
			FEA_DETAIL_FECALLLAST_26, FEA_DETAIL_FECALLLAST_25, \
			FEA_DETAIL_FECALLLAST_24, FEA_DETAIL_FECALLLAST_23, \
			FEA_DETAIL_FECALLLAST_22, FEA_DETAIL_FECALLLAST_21, \
			FEA_DETAIL_FECALLLAST_20, FEA_DETAIL_FECALLLAST_19, \
			FEA_DETAIL_FECALLLAST_18, FEA_DETAIL_FECALLLAST_17, \
			FEA_DETAIL_FECALLLAST_16, FEA_DETAIL_FECALLLAST_15, \
			FEA_DETAIL_FECALLLAST_14, FEA_DETAIL_FECALLLAST_13, \
			FEA_DETAIL_FECALLLAST_12, FEA_DETAIL_FECALLLAST_11, \
			FEA_DETAIL_FECALLLAST_10, FEA_DETAIL_FECALLLAST_9, \
			FEA_DETAIL_FECALLLAST_8, FEA_DETAIL_FECALLLAST_7, \
			FEA_DETAIL_FECALLLAST_6, FEA_DETAIL_FECALLLAST_5, \
			FEA_DETAIL_FECALLLAST_4, FEA_DETAIL_FECALLLAST_3, \
			FEA_DETAIL_FECALLLAST_2, FEA_DETAIL_FECALLLAST_1, \
			FEA_DETAIL_FECALLLAST_0)(macro, last_macro, __VA_ARGS__))
