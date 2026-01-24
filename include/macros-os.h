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

#ifndef JSTR_MACROS_OS_H
#define JSTR_MACROS_OS_H 1

#if defined __linux__ || defined linux || defined __linux
#	define JSTR_OS_LINUX 1
#endif
#if defined __gnu_linux__
#	define JSTR_OS_GNULINUX 1
#endif
#if defined __ANDROID__
#	define JSTR_OS_ANDROID 1
#endif
#if defined __FreeBSD__
#	define JSTR_OS_FREEBSD 1
#endif
#if defined __NetBSD__
#	define JSTR_OS_NETBSD 1
#endif
#if defined __OpenBSD__
#	define JSTR_OS_OPENBSD 1
#endif
#if defined __bsdi__
#	define JSTR_OS_BSDI 1
#endif
#if defined __DragonFly__
#	define JSTR_OS_DRAGONFLYBSD 1
#endif
#if defined _AIX || defined __TOS_AIX__
#	define JSTR_OS_AIX 1
#endif
#if defined UTS
#	define JSTR_OS_UTS 1
#endif
#if defined AMIGA || defined __amigaos__
#	define JSTR_OS_AMIGA 1
#endif
#if defined aegis
#	define JSTR_OS_APOLLOAEGIS 1
#endif
#if defined apollo
#	define JSTR_OS_APOLLODOMAIN 1
#endif
#if defined __BEOS__
#	define JSTR_OS_BE 1
#endif
#if defined __bg__ || defined __THW_BLUEGEN__
#	define JSTR_OS_BLUEGENE 1
#endif
#if defined __convex__
#	define JSTR_OS_CONVEX 1
#endif
#if defined DGUX || defined __DGUX__ || defined __dgux__
#	define JSTR_OS_DGUX 1
#endif
#if defined _SEQUENT_ || defined sequent
#	define JSTR_OS_DYNIXPTX 1
#endif
#if defined __EMX__
#	define JSTR_OS_EMX 1
#endif
#if defined __GNU__ || defined __gnu_hurd__
#	define JSTR_OS_GNUHURD 1
#endif
#if defined __FreeBSD_kernel__ && defined __GLIBC__
#	define JSTR_OS_GNUKFREEBSD 1
#endif
#if defined __hiuxmpp
#	define JSTR_OS_HIUXMPP 1
#endif
#if defined _hpux || defined hpux || defined __hpux
#	define JSTR_OS_HPUX 1
#endif
#if defined __OS400__
#	define JSTR_OS_IBMOS400 1
#endif
#if defined sgi || defined __sgi
#	define JSTR_OS_IRIX 1
#endif
#if defined __Lynx__
#	define JSTR_OS_LYNX 1
#endif
#if defined macintosh || defined Macintosh || (defined __APPLE__ && defined __MACH__)
#	define JSTR_OS_MAC 1
#endif
#if defined __minix
#	define JSTR_OS_MINIX 1
#endif
#if defined __MORPHOS__
#	define JSTR_OS_MORPH 1
#endif
#if defined mpeix || defined __mpexl
#	define JSTR_OS_MPEIX 1
#endif
#if defined MSDOS || defined __MSDOS__ || defined _MSDOS || defined __DOS__
#	define JSTR_OS_MSDOS 1
#endif
#if defined __TANDEM
#	define JSTR_OS_NONSTOP 1
#endif
#if defined OS2 || defined _OS2 || defined __OS2__ || defined __TOS_OS2__
#	define JSTR_OS_OS2 1
#endif
#if defined __palmos__
#	define JSTR_OS_PALM 1
#endif
#if defined EPLAN9
#	define JSTR_OS_PLAN9 1
#endif
#if defined pyr
#	define JSTR_OS_PYRAMIDDCOSX 1
#endif
#if defined __QNX__ || defined __QNXNTO__
#	define JSTR_OS_QNSX 1
#endif
#if defined sinux
#	define JSTR_OS_RELIANTUNIX 1
#endif
#if defined M_I386 || defined M_XENIX || defined _SCO_DS
#	define JSTR_OS_SCOOPENSERVER 1
#endif
#if (defined sun || defined __sun) && (defined __SVR4 || defined __svr4__)
#	define JSTR_OS_SOLARIS 1
#endif
#if (defined sun || defined __sun) && (!defined __SVR4 && !defined __svr4__)
#	define JSTR_OS_SUNOS   1
#endif
#if defined __VOS__
#	define JSTR_OS_STRATUSVOX 1
#endif
#if defined __SYLLABLE__
#	define JSTR_OS_SYLLABLE 1
#endif
#if defined __SYMBIAN32__
#	define JSTR_OS_SYMBIAN 1
#endif
#if defined __osf__ || defined __osf
#	define JSTR_OS_TRU64OSF1 1
#endif
#if defined ultrix || defined __ultrix || defined __ultrix__ || (defined unix && defined vax)
#	define JSTR_OS_ULTRIX 1
#endif
#if defined _UNICOS
#	define JSTR_OS_UNIC 1
#endif
#if defined sco || defined _UNIXWARE7
#	define JSTR_OS_UNIXWARE 1
#endif
#if defined VMS || defined __VMS
#	define JSTR_OS_VMS 1
#endif
#if defined __VXWORKS__ || defined __vxworks
#	define JSTR_OS_VXWORKS 1
#endif
#if defined _WIN16 || defined _WIN32 || defined _WIN64 || defined __WIN32__ || defined __TOS_WIN__ || defined __WINDOWS__
#	define JSTR_OS_WINDOWS 1
#endif
#if defined _WIN32_WCE
#	define JSTR_OS_WINDOWSCE 1
#endif
#if defined _WINDU_SOURCE
#	define JSTR_OS_WINDU 1
#endif
#if defined __MVS__ || defined __HOS_MVS__ || defined __TOS_MVS__
#	define JSTR_OS_Z 1
#endif
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

#ifndef JSTR_OS_LINUX
#	define JSTR_OS_LINUX 0
#endif
#ifndef JSTR_OS_GNULINUX
#	define JSTR_OS_GNULINUX 0
#endif
#ifndef JSTR_OS_ANDROID
#	define JSTR_OS_ANDROID 0
#endif
#ifndef JSTR_OS_FREEBSD
#	define JSTR_OS_FREEBSD 0
#endif
#ifndef JSTR_OS_NETBSD
#	define JSTR_OS_NETBSD 0
#endif
#ifndef JSTR_OS_OPENBSD
#	define JSTR_OS_OPENBSD 0
#endif
#ifndef JSTR_OS_BSDI
#	define JSTR_OS_BSDI 0
#endif
#ifndef JSTR_OS_DRAGONFLYBSD
#	define JSTR_OS_DRAGONFLYBSD 0
#endif
#ifndef JSTR_OS_AIX
#	define JSTR_OS_AIX 0
#endif
#ifndef JSTR_OS_UTS
#	define JSTR_OS_UTS 0
#endif
#ifndef JSTR_OS_AMIGA
#	define JSTR_OS_AMIGA 0
#endif
#ifndef JSTR_OS_APOLLOAEGIS
#	define JSTR_OS_APOLLOAEGIS 0
#endif
#ifndef JSTR_OS_APOLLODOMAIN
#	define JSTR_OS_APOLLODOMAIN 0
#endif
#ifndef JSTR_OS_BE
#	define JSTR_OS_BE 0
#endif
#ifndef JSTR_OS_BLUEGENE
#	define JSTR_OS_BLUEGENE 0
#endif
#ifndef JSTR_OS_CONVEX
#	define JSTR_OS_CONVEX 0
#endif
#ifndef JSTR_OS_DGUX
#	define JSTR_OS_DGUX 0
#endif
#ifndef JSTR_OS_DYNIXPTX
#	define JSTR_OS_DYNIXPTX 0
#endif
#ifndef JSTR_OS_EMX
#	define JSTR_OS_EMX 0
#endif
#ifndef JSTR_OS_GNUHURD
#	define JSTR_OS_GNUHURD 0
#endif
#ifndef JSTR_OS_GNUKFREEBSD
#	define JSTR_OS_GNUKFREEBSD 0
#endif
#ifndef JSTR_OS_HIUXMPP
#	define JSTR_OS_HIUXMPP 0
#endif
#ifndef JSTR_OS_HPUX
#	define JSTR_OS_HPUX 0
#endif
#ifndef JSTR_OS_IBMOS400
#	define JSTR_OS_IBMOS400 0
#endif
#ifndef JSTR_OS_IRIX
#	define JSTR_OS_IRIX 0
#endif
#ifndef JSTR_OS_LYNX
#	define JSTR_OS_LYNX 0
#endif
#ifndef JSTR_OS_MAC
#	define JSTR_OS_MAC 0
#endif
#ifndef JSTR_OS_MINIX
#	define JSTR_OS_MINIX 0
#endif
#ifndef JSTR_OS_MORPH
#	define JSTR_OS_MORPH 0
#endif
#ifndef JSTR_OS_MPEIX
#	define JSTR_OS_MPEIX 0
#endif
#ifndef JSTR_OS_MSDOS
#	define JSTR_OS_MSDOS 0
#endif
#ifndef JSTR_OS_NONSTOP
#	define JSTR_OS_NONSTOP 0
#endif
#ifndef JSTR_OS_OS2
#	define JSTR_OS_OS2 0
#endif
#ifndef JSTR_OS_PALM
#	define JSTR_OS_PALM 0
#endif
#ifndef JSTR_OS_PLAN9
#	define JSTR_OS_PLAN9 0
#endif
#ifndef JSTR_OS_PYRAMIDDCOSX
#	define JSTR_OS_PYRAMIDDCOSX 0
#endif
#ifndef JSTR_OS_QNSX
#	define JSTR_OS_QNSX 0
#endif
#ifndef JSTR_OS_RELIANTUNIX
#	define JSTR_OS_RELIANTUNIX 0
#endif
#ifndef JSTR_OS_SCOOPENSERVER
#	define JSTR_OS_SCOOPENSERVER 0
#endif
#ifndef JSTR_OS_STRATUSVOX
#	define JSTR_OS_STRATUSVOX 0
#endif
#ifndef JSTR_OS_SYLLABLE
#	define JSTR_OS_SYLLABLE 0
#endif
#ifndef JSTR_OS_SYMBIAN
#	define JSTR_OS_SYMBIAN 0
#endif
#ifndef JSTR_OS_TRU64OSF1
#	define JSTR_OS_TRU64OSF1 0
#endif
#ifndef JSTR_OS_ULTRIX
#	define JSTR_OS_ULTRIX 0
#endif
#ifndef JSTR_OS_UNIC
#	define JSTR_OS_UNIC 0
#endif
#ifndef JSTR_OS_UNIXWARE
#	define JSTR_OS_UNIXWARE 0
#endif
#ifndef JSTR_OS_VMS
#	define JSTR_OS_VMS 0
#endif
#ifndef JSTR_OS_VXWORKS
#	define JSTR_OS_VXWORKS 0
#endif
#ifndef JSTR_OS_WINDOWS
#	define JSTR_OS_WINDOWS 0
#endif
#ifndef JSTR_OS_WINDOWSCE
#	define JSTR_OS_WINDOWSCE 0
#endif
#ifndef JSTR_OS_WINDU
#	define JSTR_OS_WINDU 0
#endif
#ifndef JSTR_OS_Z
#	define JSTR_OS_Z 0
#endif
#ifndef JSTR_OS_SOLARIS
#	define JSTR_OS_SOLARIS 0
#endif
#ifndef JSTR_OS_SUNOS
#	define JSTR_OS_SUNOS 0
#endif

#ifndef JSTR_ENV_UNIX
#	define JSTR_ENV_UNIX 0
#endif
#ifndef JSTR_ENV_BSD
#	define JSTR_ENV_BSD 0
#endif
#ifndef JSTR_ENV_CYGWIN
#	define JSTR_ENV_CYGWIN 0
#endif
#ifndef JSTR_ENV_INTERIX
#	define JSTR_ENV_INTERIX 0
#endif
#ifndef JSTR_ENV_SVR4
#	define JSTR_ENV_SVR4 0
#endif
#ifndef JSTR_ENV_UWIN
#	define JSTR_ENV_UWIN 0
#endif

#endif /* JSTR_MACROS_OS_H */
