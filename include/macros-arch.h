
#ifndef JSTR_MACROS_ARCH_H
/** @brief JSTR_MACROS_ARCH_H macro. */
#define JSTR_MACROS_ARCH_H 1

#if (defined __x86_64 || defined __x86_64__) && (defined _ILP32 || defined __ILP32__)
/** @brief JSTR_ARCH_X86_32 macro. */
#	define JSTR_ARCH_X86_32 1
#elif defined __amd64__ || defined __amd64 \
|| defined _M_AMD64 || defined __x86_64__  \
|| defined __x86_64 || defined _M_X64      \
|| defined _M_X64
/** @brief JSTR_ARCH_X86_64 macro. */
#	define JSTR_ARCH_X86_64 1
#elif defined __aarch64__ || defined _M_ARM64
/** @brief JSTR_ARCH_ARM64 macro. */
#	define JSTR_ARCH_ARM64 1
#elif defined __arm__ || defined __arm \
|| defined __thumb__ || defined _ARM   \
|| defined _M_ARM || defined _M_ARM_T
/** @brief JSTR_ARCH_ARM macro. */
#	define JSTR_ARCH_ARM 1
#	if defined __ARM_ARCH_2__
/** @brief JSTR_ARCH_ARM2 macro. */
#		define JSTR_ARCH_ARM2 1
#	elif defined __ARM_ARCH_3__ || defined __ARM_ARCH_3M__
/** @brief JSTR_ARCH_ARM3 macro. */
#		define JSTR_ARCH_ARM3 1
#	elif defined __ARM_ARCH_4T__ || defined __TARGET_ARM_4T
/** @brief JSTR_ARCH_ARM4T macro. */
#		define JSTR_ARCH_ARM4T 1
#	elif defined __ARM_ARCH_5__ || defined __ARM_ARCH_5E__ \
	|| defined __ARM_ARCH_5T__ || defined __ARM_ARCH_5TE__  \
	|| defined __ARM_ARCH_5TEJ__
/** @brief JSTR_ARCH_ARM5 macro. */
#		define JSTR_ARCH_ARM5 1
#	elif defined __ARM_ARCH_6T2__
/** @brief JSTR_ARCH_ARM6T2 macro. */
#		define JSTR_ARCH_ARM6T2 1
#	elif defined __ARM_ARCH_6__ || defined __ARM_ARCH_6J__ \
	|| defined __ARM_ARCH_6Z__ || defined __ARM_ARCH_6ZK__  \
	|| defined __ARM_ARCH_6K__
/** @brief JSTR_ARCH_ARM6 macro. */
#		define JSTR_ARCH_ARM6 1
#	elif defined __ARM_ARCH_7__ || defined __ARM_ARCH_7A__ \
	|| defined __ARM_ARCH_7R__ || defined __ARM_ARCH_7M__   \
	|| defined __ARM_ARCH_7EM__ || defined __ARM_ARCH_7S__
/** @brief JSTR_ARCH_ARM7 macro. */
#		define JSTR_ARCH_ARM7 1
#	endif
#elif defined mips || defined __mips__ \
|| defined __mips || defined __MIPS__
/** @brief JSTR_ARCH_MIPS macro. */
#	define JSTR_ARCH_MIPS 1
#elif defined __sh__
/** @brief JSTR_ARCH_SH macro. */
#	define JSTR_ARCH_SH 1
#	if defined __sh1__
/** @brief JSTR_ARCH_SH1 macro. */
#		define JSTR_ARCH_SH1 1
#	elif defined __sh2__
/** @brief JSTR_ARCH_SH2 macro. */
#		define JSTR_ARCH_SH2 1
#	elif defined __sh3__
/** @brief JSTR_ARCH_SH3 macro. */
#		define JSTR_ARCH_SH3 1
#	elif defined __sh4__
/** @brief JSTR_ARCH_SH4 macro. */
#		define JSTR_ARCH_SH4 1
#	elif defined __sh5__
/** @brief JSTR_ARCH_SH5 macro. */
#		define JSTR_ARCH_SH5 1
#	endif
#elif defined __powerpc || defined __powerpc__      \
|| defined __POWERPC__ || defined __ppc__ || _M_PPC \
|| defined __PPC__ || defined _ARCH_PPC
/** @brief JSTR_ARCH_POWERPC macro. */
#	define JSTR_ARCH_POWERPC 1
#elif defined __PPC64__ || defined __ppc64__ \
|| defined _ARCH_PPC64 || defined __powerpc64__
/** @brief JSTR_ARCH_POWERPC64 macro. */
#	define JSTR_ARCH_POWERPC64 1
#elif defined _ARCH_PWR4
/** @brief JSTR_ARCH_POWERPC4 macro. */
#	define JSTR_ARCH_POWERPC4 1
#elif defined _ARCH_PWR5X
/** @brief JSTR_ARCH_POWERPC5X macro. */
#	define JSTR_ARCH_POWERPC5X 1
#elif defined _ARCH_PWR6
/** @brief JSTR_ARCH_POWERPC6 macro. */
#	define JSTR_ARCH_POWERPC6 1
#elif defined _ARCH_PWR7
/** @brief JSTR_ARCH_POWERPC7 macro. */
#	define JSTR_ARCH_POWERPC7 1
#elif defined _ARCH_PWR8
/** @brief JSTR_ARCH_POWERPC8 macro. */
#	define JSTR_ARCH_POWERPC8 1
#elif defined _ARCH_PWR9
/** @brief JSTR_ARCH_POWERPC9 macro. */
#	define JSTR_ARCH_POWERPC9 1
#elif defined __sparc__ || defined __sparc
/** @brief JSTR_ARCH_SPARC macro. */
#	define JSTR_ARCH_SPARC 1
#	if defined __sparc_v8__ || defined __sparcv8
/** @brief JSTR_ARCH_SPARCV8 macro. */
#		define JSTR_ARCH_SPARCV8 1
#	elif defined __sparc_v9__ || defined __sparcv9
/** @brief JSTR_ARCH_SPARCV9 macro. */
#		define JSTR_ARCH_SPARCV9 1
#	endif
#elif defined __m68k__ || defined M68000 || defined __MC68K__
/** @brief JSTR_ARCH_M68K macro. */
#	define JSTR_ARCH_M68K 1
#elif defined __alpha__ || defined __alpha || defined _M_ALPHA
/** @brief JSTR_ARCH_ALPHA macro. */
#	define JSTR_ARCH_ALPHA 1
#	if defined __alpha_ev4__
/** @brief JSTR_ARCH_ALPHA macro. */
#		define JSTR_ARCH_ALPHA 1
#	elif defined __alpha_ev5__
#	elif defined __alpha_ev6__
#	endif
#elif defined __hppa__ || defined __hppa || defined __HPPA__
/** @brief JSTR_ARCH_HPPA macro. */
#	define JSTR_ARCH_HPPA 1
#	if defined _PA_RISC1_0
/** @brief JSTR_ARCH_HPPA_RISC1_0 macro. */
#		define JSTR_ARCH_HPPA_RISC1_0 1
#	elif defined _PA_RISC1_1 || defined __HPPA11__ || defined __PA7100__
/** @brief JSTR_ARCH_HPPA_RISC1_1 macro. */
#		define JSTR_ARCH_HPPA_RISC1_1 1
#	elif defined _PA_RISC2_0 || defined __RISC2_0__ \
	|| defined __HPPA20__ || defined __PA8000__
/** @brief JSTR_ARCH_HPPA_RISC2_0 macro. */
#		define JSTR_ARCH_HPPA_RISC2_0 1
#	endif
#elif defined __riscv_zbb || defined __riscv_xtheadbb
/** @brief JSTR_ARCH_RISCV macro. */
#	define JSTR_ARCH_RISCV 1
#elif defined __s390x__ || defined __s390__
/** @brief JSTR_ARCH_S390 macro. */
#	define JSTR_ARCH_S390 1
#elif defined __ia64__ || defined _IA64 \
|| defined __IA64__ || defined __ia64   \
|| defined _M_IA64 || defined __itanium__
/** @brief JSTR_ARCH_IA64 macro. */
#	define JSTR_ARCH_IA64 1
#elif defined i386 || defined __i386__ \
|| defined __i386 || defined _M_IX86
/** @brief JSTR_ARCH_I386 macro. */
#	define JSTR_ARCH_I386 1
#elif defined __loongarch64
/** @brief JSTR_ARCH_LOONGARCH64 macro. */
#	define JSTR_ARCH_LOONGARCH64 1
#elif defined __loongarch__
/** @brief JSTR_ARCH_LOONGARCH macro. */
#	define JSTR_ARCH_LOONGARCH 1
#elif defined __CSKY__
/** @brief JSTR_ARCH_CSKY macro. */
#	define JSTR_ARCH_CSKY 1
#endif /* arch */

#ifndef JSTR_ARCH_X86_32
/** @brief JSTR_ARCH_X86_32 macro. */
#	define JSTR_ARCH_X86_32 0
#endif
#ifndef JSTR_ARCH_X86_64
/** @brief JSTR_ARCH_X86_64 macro. */
#	define JSTR_ARCH_X86_64 0
#endif
#ifndef JSTR_ARCH_ARM64
/** @brief JSTR_ARCH_ARM64 macro. */
#	define JSTR_ARCH_ARM64 0
#endif
#ifndef JSTR_ARCH_ARM
/** @brief JSTR_ARCH_ARM macro. */
#	define JSTR_ARCH_ARM 0
#endif
#ifndef JSTR_ARCH_ARM2
/** @brief JSTR_ARCH_ARM2 macro. */
#	define JSTR_ARCH_ARM2 0
#endif
#ifndef JSTR_ARCH_ARM3
/** @brief JSTR_ARCH_ARM3 macro. */
#	define JSTR_ARCH_ARM3 0
#endif
#ifndef JSTR_ARCH_ARM4T
/** @brief JSTR_ARCH_ARM4T macro. */
#	define JSTR_ARCH_ARM4T 0
#endif
#ifndef JSTR_ARCH_ARM5
/** @brief JSTR_ARCH_ARM5 macro. */
#	define JSTR_ARCH_ARM5 0
#endif
#ifndef JSTR_ARCH_ARM6T2
/** @brief JSTR_ARCH_ARM6T2 macro. */
#	define JSTR_ARCH_ARM6T2 0
#endif
#ifndef JSTR_ARCH_ARM6
/** @brief JSTR_ARCH_ARM6 macro. */
#	define JSTR_ARCH_ARM6 0
#endif
#ifndef JSTR_ARCH_ARM7
/** @brief JSTR_ARCH_ARM7 macro. */
#	define JSTR_ARCH_ARM7 0
#endif
#ifndef JSTR_ARCH_MIPS
/** @brief JSTR_ARCH_MIPS macro. */
#	define JSTR_ARCH_MIPS 0
#endif
#ifndef JSTR_ARCH_SH
/** @brief JSTR_ARCH_SH macro. */
#	define JSTR_ARCH_SH 0
#endif
#ifndef JSTR_ARCH_SH1
/** @brief JSTR_ARCH_SH1 macro. */
#	define JSTR_ARCH_SH1 0
#endif
#ifndef JSTR_ARCH_SH2
/** @brief JSTR_ARCH_SH2 macro. */
#	define JSTR_ARCH_SH2 0
#endif
#ifndef JSTR_ARCH_SH3
/** @brief JSTR_ARCH_SH3 macro. */
#	define JSTR_ARCH_SH3 0
#endif
#ifndef JSTR_ARCH_SH4
/** @brief JSTR_ARCH_SH4 macro. */
#	define JSTR_ARCH_SH4 0
#endif
#ifndef JSTR_ARCH_SH5
/** @brief JSTR_ARCH_SH5 macro. */
#	define JSTR_ARCH_SH5 0
#endif
#ifndef JSTR_ARCH_POWERPC
/** @brief JSTR_ARCH_POWERPC macro. */
#	define JSTR_ARCH_POWERPC 0
#endif
#ifndef JSTR_ARCH_POWERPC64
/** @brief JSTR_ARCH_POWERPC64 macro. */
#	define JSTR_ARCH_POWERPC64 0
#endif
#ifndef JSTR_ARCH_POWERPC4
/** @brief JSTR_ARCH_POWERPC4 macro. */
#	define JSTR_ARCH_POWERPC4 0
#endif
#ifndef JSTR_ARCH_POWERPC5X
/** @brief JSTR_ARCH_POWERPC5X macro. */
#	define JSTR_ARCH_POWERPC5X 0
#endif
#ifndef JSTR_ARCH_POWERPC6
/** @brief JSTR_ARCH_POWERPC6 macro. */
#	define JSTR_ARCH_POWERPC6 0
#endif
#ifndef JSTR_ARCH_POWERPC7
/** @brief JSTR_ARCH_POWERPC7 macro. */
#	define JSTR_ARCH_POWERPC7 0
#endif
#ifndef JSTR_ARCH_POWERPC8
/** @brief JSTR_ARCH_POWERPC8 macro. */
#	define JSTR_ARCH_POWERPC8 0
#endif
#ifndef JSTR_ARCH_POWERPC9
/** @brief JSTR_ARCH_POWERPC9 macro. */
#	define JSTR_ARCH_POWERPC9 0
#endif
#ifndef JSTR_ARCH_SPARC
/** @brief JSTR_ARCH_SPARC macro. */
#	define JSTR_ARCH_SPARC 0
#endif
#ifndef JSTR_ARCH_SPARCV8
/** @brief JSTR_ARCH_SPARCV8 macro. */
#	define JSTR_ARCH_SPARCV8 0
#endif
#ifndef JSTR_ARCH_SPARCV9
/** @brief JSTR_ARCH_SPARCV9 macro. */
#	define JSTR_ARCH_SPARCV9 0
#endif
#ifndef JSTR_ARCH_M68K
/** @brief JSTR_ARCH_M68K macro. */
#	define JSTR_ARCH_M68K 0
#endif
#ifndef JSTR_ARCH_ALPHA
/** @brief JSTR_ARCH_ALPHA macro. */
#	define JSTR_ARCH_ALPHA 0
#endif
#ifndef JSTR_ARCH_ALPHA
/** @brief JSTR_ARCH_ALPHA macro. */
#	define JSTR_ARCH_ALPHA 0
#endif
#ifndef JSTR_ARCH_HPPA
/** @brief JSTR_ARCH_HPPA macro. */
#	define JSTR_ARCH_HPPA 0
#endif
#ifndef JSTR_ARCH_HPPA_RISC1_0
/** @brief JSTR_ARCH_HPPA_RISC1_0 macro. */
#	define JSTR_ARCH_HPPA_RISC1_0 0
#endif
#ifndef JSTR_ARCH_HPPA_RISC1_1
/** @brief JSTR_ARCH_HPPA_RISC1_1 macro. */
#	define JSTR_ARCH_HPPA_RISC1_1 0
#endif
#ifndef JSTR_ARCH_HPPA_RISC2_0
/** @brief JSTR_ARCH_HPPA_RISC2_0 macro. */
#	define JSTR_ARCH_HPPA_RISC2_0 0
#endif
#ifndef JSTR_ARCH_RISCV
/** @brief JSTR_ARCH_RISCV macro. */
#	define JSTR_ARCH_RISCV 0
#endif
#ifndef JSTR_ARCH_S390
/** @brief JSTR_ARCH_S390 macro. */
#	define JSTR_ARCH_S390 0
#endif
#ifndef JSTR_ARCH_IA64
/** @brief JSTR_ARCH_IA64 macro. */
#	define JSTR_ARCH_IA64 0
#endif
#ifndef JSTR_ARCH_I386
/** @brief JSTR_ARCH_I386 macro. */
#	define JSTR_ARCH_I386 0
#endif
#ifndef JSTR_ARCH_LOONGARCH64
/** @brief JSTR_ARCH_LOONGARCH64 macro. */
#	define JSTR_ARCH_LOONGARCH64 0
#endif
#ifndef JSTR_ARCH_LOONGARCH
/** @brief JSTR_ARCH_LOONGARCH macro. */
#	define JSTR_ARCH_LOONGARCH 0
#endif
#ifndef JSTR_ARCH_CSKY
/** @brief JSTR_ARCH_CSKY macro. */
#	define JSTR_ARCH_CSKY 0
#endif

#endif /* JSTR_MACROS_ARCH_H */
