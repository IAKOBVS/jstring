/* Copyright (c) 2023 James Tirta Halim <tirtajames45 at gmail dot com>
   This file is part of the jstring library.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

   MIT License (Expat) */

#ifndef JSTR_MACROS_OS_H
#define JSTR_MACROS_OS_H 1

#if defined __linux__ || defined linux || defined __linux
#	define JSTR_OS_LINUX 1
#	if defined __gnu_linux__
#		define JSTR_OS_GNULINUX 1
#	elif defined __ANDROID__
#		define JSTR_OS_ANDROID 1
#	endif
#elif defined __FreeBSD__
#	define JSTR_OS_FREEBSD 1
#elif defined __NetBSD__
#	define JSTR_OS_NETBSD 1
#elif defined __OpenBSD__
#	define JSTR_OS_OPENBSD 1
#elif defined __bsdi__
#	define JSTR_OS_BSDI 1
#elif defined __DragonFly__
#	define JSTR_OS_DRAGONFLYBSD 1
#elif defined _AIX || defined __TOS_AIX__
#	define JSTR_OS_AIX 1
#elif defined UTS
#	define JSTR_OS_UTS 1
#elif defined AMIGA || defined __amigaos__
#	define JSTR_OS_AMIGA 1
#elif defined aegis
#	define JSTR_OS_APOLLOAEGIS 1
#elif defined apollo
#	define JSTR_OS_APOLLODOMAIN 1
#elif defined __BEOS__
#	define JSTR_OS_BE 1
#elif defined __bg__ || defined __THW_BLUEGEN__
#	define JSTR_OS_BLUEGENE 1
#elif defined __convex__
#	define JSTR_OS_CONVEX 1
#elif defined DGUX || defined __DGUX__ || defined __dgux__
#	define JSTR_OS_DGUX 1
#elif defined _SEQUENT_ || defined sequent
#	define JSTR_OS_DYNIXPTX 1
#elif defined __EMX__
#	define JSTR_OS_EMX 1
#elif defined __GNU__ || defined __gnu_hurd__
#	define JSTR_OS_GNUHURD 1
#elif defined __FreeBSD_kernel__ && defined __GLIBC__
#	define JSTR_OS_GNUKFREEBSD 1
#elif defined __hiuxmpp
#	define JSTR_OS_HIUXMPP 1
#elif defined _hpux || defined hpux || defined __hpux
#	define JSTR_OS_HPUX 1
#elif defined __OS400__
#	define JSTR_OS_IBMOS400 1
#elif defined sgi || defined __sgi
#	define JSTR_OS_IRIX 1
#elif defined __Lynx__
#	define JSTR_OS_LYNX 1
#elif defined macintosh || defined Macintosh || (defined __APPLE__ && defined __MACH__)
#	define JSTR_OS_MAC 1
#elif defined __minix
#	define JSTR_OS_MINIX 1
#elif defined __MORPHOS__
#	define JSTR_OS_MORPH 1
#elif defined mpeix || defined __mpexl
#	define JSTR_OS_MPEIX 1
#elif defined MSDOS || defined __MSDOS__ || defined _MSDOS || defined __DOS__
#	define JSTR_OS_MSDOS 1
#elif defined __TANDEM
#	define JSTR_OS_NONSTOP 1
#elif defined OS2 || defined _OS2 || defined __OS2__ || defined __TOS_OS2__
#	define JSTR_OS_OS2 1
#elif defined __palmos__
#	define JSTR_OS_PALM 1
#elif defined EPLAN9
#	define JSTR_OS_PLAN9 1
#elif defined pyr
#	define JSTR_OS_PYRAMIDDCOSX 1
#elif defined __QNX__ || defined __QNXNTO__
#	define JSTR_OS_QNSX 1
#elif defined sinux
#	define JSTR_OS_RELIANTUNIX 1
#elif defined M_I386 || defined M_XENIX || defined _SCO_DS
#	define JSTR_OS_SCOOPENSERVER 1
#elif defined sun || defined __sun
#	if defined __SVR4 || defined __svr4__
#		define JSTR_OS_SOLARIS 1
#	else
#		define JSTR_OS_SUNOS 1
#	endif
#elif defined __VOS__
#	define JSTR_OS_STRATUSVOX 1
#elif defined __SYLLABLE__
#	define JSTR_OS_SYLLABLE 1
#elif defined __SYMBIAN32__
#	define JSTR_OS_SYMBIAN 1
#elif defined __osf__ || defined __osf
#	define JSTR_OS_TRU64OSF1 1
#elif defined ultrix || defined __ultrix || defined __ultrix__ || (defined unix && defined vax)
#	define JSTR_OS_ULTRIX 1
#elif defined _UNICOS
#	define JSTR_OS_UNIC 1
#elif defined sco || defined _UNIXWARE7
#	define JSTR_OS_UNIXWARE 1
#elif defined VMS || defined __VMS
#	define JSTR_OS_VMS 1
#elif defined __VXWORKS__ || defined __vxworks
#	define JSTR_OS_VXWORKS 1
#elif defined _WIN16 || defined _WIN32 || defined _WIN64 || defined __WIN32__ || defined __TOS_WIN__ || defined __WINDOWS__
#	define JSTR_OS_WINDOWS 1
#elif defined _WIN32_WCE
#	define JSTR_OS_WINDOWSCE 1
#elif defined _WINDU_SOURCE
#	define JSTR_OS_WINDU 1
#elif defined __MVS__ || defined __HOS_MVS__ || defined __TOS_MVS__
#	define JSTR_OS_Z 1
#else
#	define JSTR_OS_GENERIC 1
#endif /* os */

#if defined __unix__ || defined __unix
#	define JSTR_ENV_UNIX 1
#endif
#if defined __FreeBSD__ || defined __NetBSD__ || defined __OpenBSD__ || defined __bsdi__ || defined __DragonFly__ || defined _SYSTYPE_BSD
#	define JSTR_ENV_BSD 1
#endif
#ifdef __CYGWIN__
#	define JSTR_ENV_CYGWIN 1
#endif
#ifdef __INTERIX
#	define JSTR_ENV_INTERIX 1
#endif
#if defined __sysv__ || defined __SVR4 || defined __svr4__ || defined _SYSTYPE_SRV4
#	define JSTR_ENV_SVR4 1
#endif
#if defined _UWIN
#	define JSTR_ENV_UWIN 1
#endif /* env */

#endif /* JSTR_MACROS_OS_H */
