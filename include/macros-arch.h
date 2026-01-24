/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 * This file is part of the jstring library.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#ifndef JSTR_MACROS_ARCH_H
#define JSTR_MACROS_ARCH_H 1

#if (defined __x86_64 || defined __x86_64__) && (defined _ILP32 || defined __ILP32__)
#	define JSTR_ARCH_X86_32 1
#elif defined __amd64__ || defined __amd64 \
|| defined _M_AMD64 || defined __x86_64__  \
|| defined __x86_64 || defined _M_X64      \
|| defined _M_X64
#	define JSTR_ARCH_X86_64 1
#elif defined __aarch64__ || defined _M_ARM64
#	define JSTR_ARCH_ARM64 1
#elif defined __arm__ || defined __arm \
|| defined __thumb__ || defined _ARM   \
|| defined _M_ARM || defined _M_ARM_T
#	define JSTR_ARCH_ARM 1
#	if defined __ARM_ARCH_2__
#		define JSTR_ARCH_ARM2 1
#	elif defined __ARM_ARCH_3__ || defined __ARM_ARCH_3M__
#		define JSTR_ARCH_ARM3 1
#	elif defined __ARM_ARCH_4T__ || defined __TARGET_ARM_4T
#		define JSTR_ARCH_ARM4T 1
#	elif defined __ARM_ARCH_5__ || defined __ARM_ARCH_5E__ \
	|| defined __ARM_ARCH_5T__ || defined __ARM_ARCH_5TE__  \
	|| defined __ARM_ARCH_5TEJ__
#		define JSTR_ARCH_ARM5 1
#	elif defined __ARM_ARCH_6T2__
#		define JSTR_ARCH_ARM6T2 1
#	elif defined __ARM_ARCH_6__ || defined __ARM_ARCH_6J__ \
	|| defined __ARM_ARCH_6Z__ || defined __ARM_ARCH_6ZK__  \
	|| defined __ARM_ARCH_6K__
#		define JSTR_ARCH_ARM6 1
#	elif defined __ARM_ARCH_7__ || defined __ARM_ARCH_7A__ \
	|| defined __ARM_ARCH_7R__ || defined __ARM_ARCH_7M__   \
	|| defined __ARM_ARCH_7EM__ || defined __ARM_ARCH_7S__
#		define JSTR_ARCH_ARM7 1
#	endif
#elif defined mips || defined __mips__ \
|| defined __mips || defined __MIPS__
#	define JSTR_ARCH_MIPS 1
#elif defined __sh__
#	define JSTR_ARCH_SH 1
#	if defined __sh1__
#		define JSTR_ARCH_SH1 1
#	elif defined __sh2__
#		define JSTR_ARCH_SH2 1
#	elif defined __sh3__
#		define JSTR_ARCH_SH3 1
#	elif defined __sh4__
#		define JSTR_ARCH_SH4 1
#	elif defined __sh5__
#		define JSTR_ARCH_SH5 1
#	endif
#elif defined __powerpc || defined __powerpc__      \
|| defined __POWERPC__ || defined __ppc__ || _M_PPC \
|| defined __PPC__ || defined _ARCH_PPC
#	define JSTR_ARCH_POWERPC 1
#elif defined __PPC64__ || defined __ppc64__ \
|| defined _ARCH_PPC64 || defined __powerpc64__
#	define JSTR_ARCH_POWERPC64 1
#elif defined _ARCH_PWR4
#	define JSTR_ARCH_POWERPC4 1
#elif defined _ARCH_PWR5X
#	define JSTR_ARCH_POWERPC5X 1
#elif defined _ARCH_PWR6
#	define JSTR_ARCH_POWERPC6 1
#elif defined _ARCH_PWR7
#	define JSTR_ARCH_POWERPC7 1
#elif defined _ARCH_PWR8
#	define JSTR_ARCH_POWERPC8 1
#elif defined _ARCH_PWR9
#	define JSTR_ARCH_POWERPC9 1
#elif defined __sparc__ || defined __sparc
#	define JSTR_ARCH_SPARC 1
#	if defined __sparc_v8__ || defined __sparcv8
#		define JSTR_ARCH_SPARCV8 1
#	elif defined __sparc_v9__ || defined __sparcv9
#		define JSTR_ARCH_SPARCV9 1
#	endif
#elif defined __m68k__ || defined M68000 || defined __MC68K__
#	define JSTR_ARCH_M68K 1
#elif defined __alpha__ || defined __alpha || defined _M_ALPHA
#	define JSTR_ARCH_ALPHA 1
#	if defined __alpha_ev4__
#		define JSTR_ARCH_ALPHA 1
#	elif defined __alpha_ev5__
#	elif defined __alpha_ev6__
#	endif
#elif defined __hppa__ || defined __hppa || defined __HPPA__
#	define JSTR_ARCH_HPPA 1
#	if defined _PA_RISC1_0
#		define JSTR_ARCH_HPPA_RISC1_0 1
#	elif defined _PA_RISC1_1 || defined __HPPA11__ || defined __PA7100__
#		define JSTR_ARCH_HPPA_RISC1_1 1
#	elif defined _PA_RISC2_0 || defined __RISC2_0__ \
	|| defined __HPPA20__ || defined __PA8000__
#		define JSTR_ARCH_HPPA_RISC2_0 1
#	endif
#elif defined __riscv_zbb || defined __riscv_xtheadbb
#	define JSTR_ARCH_RISCV 1
#elif defined __s390x__ || defined __s390__
#	define JSTR_ARCH_S390 1
#elif defined __ia64__ || defined _IA64 \
|| defined __IA64__ || defined __ia64   \
|| defined _M_IA64 || defined __itanium__
#	define JSTR_ARCH_IA64 1
#elif defined i386 || defined __i386__ \
|| defined __i386 || defined _M_IX86
#	define JSTR_ARCH_I386 1
#elif defined __loongarch64
#	define JSTR_ARCH_LOONGARCH64 1
#elif defined __loongarch__
#	define JSTR_ARCH_LOONGARCH 1
#elif defined __CSKY__
#	define JSTR_ARCH_CSKY 1
#endif /* arch */

#ifndef JSTR_ARCH_X86_32
#	define JSTR_ARCH_X86_32 0
#endif
#ifndef JSTR_ARCH_X86_64
#	define JSTR_ARCH_X86_64 0
#endif
#ifndef JSTR_ARCH_ARM64
#	define JSTR_ARCH_ARM64 0
#endif
#ifndef JSTR_ARCH_ARM
#	define JSTR_ARCH_ARM 0
#endif
#ifndef JSTR_ARCH_ARM2
#	define JSTR_ARCH_ARM2 0
#endif
#ifndef JSTR_ARCH_ARM3
#	define JSTR_ARCH_ARM3 0
#endif
#ifndef JSTR_ARCH_ARM4T
#	define JSTR_ARCH_ARM4T 0
#endif
#ifndef JSTR_ARCH_ARM5
#	define JSTR_ARCH_ARM5 0
#endif
#ifndef JSTR_ARCH_ARM6T2
#	define JSTR_ARCH_ARM6T2 0
#endif
#ifndef JSTR_ARCH_ARM6
#	define JSTR_ARCH_ARM6 0
#endif
#ifndef JSTR_ARCH_ARM7
#	define JSTR_ARCH_ARM7 0
#endif
#ifndef JSTR_ARCH_MIPS
#	define JSTR_ARCH_MIPS 0
#endif
#ifndef JSTR_ARCH_SH
#	define JSTR_ARCH_SH 0
#endif
#ifndef JSTR_ARCH_SH1
#	define JSTR_ARCH_SH1 0
#endif
#ifndef JSTR_ARCH_SH2
#	define JSTR_ARCH_SH2 0
#endif
#ifndef JSTR_ARCH_SH3
#	define JSTR_ARCH_SH3 0
#endif
#ifndef JSTR_ARCH_SH4
#	define JSTR_ARCH_SH4 0
#endif
#ifndef JSTR_ARCH_SH5
#	define JSTR_ARCH_SH5 0
#endif
#ifndef JSTR_ARCH_POWERPC
#	define JSTR_ARCH_POWERPC 0
#endif
#ifndef JSTR_ARCH_POWERPC64
#	define JSTR_ARCH_POWERPC64 0
#endif
#ifndef JSTR_ARCH_POWERPC4
#	define JSTR_ARCH_POWERPC4 0
#endif
#ifndef JSTR_ARCH_POWERPC5X
#	define JSTR_ARCH_POWERPC5X 0
#endif
#ifndef JSTR_ARCH_POWERPC6
#	define JSTR_ARCH_POWERPC6 0
#endif
#ifndef JSTR_ARCH_POWERPC7
#	define JSTR_ARCH_POWERPC7 0
#endif
#ifndef JSTR_ARCH_POWERPC8
#	define JSTR_ARCH_POWERPC8 0
#endif
#ifndef JSTR_ARCH_POWERPC9
#	define JSTR_ARCH_POWERPC9 0
#endif
#ifndef JSTR_ARCH_SPARC
#	define JSTR_ARCH_SPARC 0
#endif
#ifndef JSTR_ARCH_SPARCV8
#	define JSTR_ARCH_SPARCV8 0
#endif
#ifndef JSTR_ARCH_SPARCV9
#	define JSTR_ARCH_SPARCV9 0
#endif
#ifndef JSTR_ARCH_M68K
#	define JSTR_ARCH_M68K 0
#endif
#ifndef JSTR_ARCH_ALPHA
#	define JSTR_ARCH_ALPHA 0
#endif
#ifndef JSTR_ARCH_ALPHA
#	define JSTR_ARCH_ALPHA 0
#endif
#ifndef JSTR_ARCH_HPPA
#	define JSTR_ARCH_HPPA 0
#endif
#ifndef JSTR_ARCH_HPPA_RISC1_0
#	define JSTR_ARCH_HPPA_RISC1_0 0
#endif
#ifndef JSTR_ARCH_HPPA_RISC1_1
#	define JSTR_ARCH_HPPA_RISC1_1 0
#endif
#ifndef JSTR_ARCH_HPPA_RISC2_0
#	define JSTR_ARCH_HPPA_RISC2_0 0
#endif
#ifndef JSTR_ARCH_RISCV
#	define JSTR_ARCH_RISCV 0
#endif
#ifndef JSTR_ARCH_S390
#	define JSTR_ARCH_S390 0
#endif
#ifndef JSTR_ARCH_IA64
#	define JSTR_ARCH_IA64 0
#endif
#ifndef JSTR_ARCH_I386
#	define JSTR_ARCH_I386 0
#endif
#ifndef JSTR_ARCH_LOONGARCH64
#	define JSTR_ARCH_LOONGARCH64 0
#endif
#ifndef JSTR_ARCH_LOONGARCH
#	define JSTR_ARCH_LOONGARCH 0
#endif
#ifndef JSTR_ARCH_CSKY
#	define JSTR_ARCH_CSKY 0
#endif

#endif /* JSTR_MACROS_ARCH_H */
