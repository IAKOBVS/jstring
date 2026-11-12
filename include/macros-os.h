
#ifndef JSTR_MACROS_OS_H
/** @brief JSTR_MACROS_OS_H macro. */
#define JSTR_MACROS_OS_H 1

#if defined __linux__ || defined linux || defined __linux
/** @brief JSTR_OS_LINUX macro. */
#	define JSTR_OS_LINUX 1
#endif
#if defined __gnu_linux__
/** @brief JSTR_OS_GNULINUX macro. */
#	define JSTR_OS_GNULINUX 1
#endif
#if defined __ANDROID__
/** @brief JSTR_OS_ANDROID macro. */
#	define JSTR_OS_ANDROID 1
#endif
#if defined __FreeBSD__
/** @brief JSTR_OS_FREEBSD macro. */
#	define JSTR_OS_FREEBSD 1
#endif
#if defined __NetBSD__
/** @brief JSTR_OS_NETBSD macro. */
#	define JSTR_OS_NETBSD 1
#endif
#if defined __OpenBSD__
/** @brief JSTR_OS_OPENBSD macro. */
#	define JSTR_OS_OPENBSD 1
#endif
#if defined __bsdi__
/** @brief JSTR_OS_BSDI macro. */
#	define JSTR_OS_BSDI 1
#endif
#if defined __DragonFly__
/** @brief JSTR_OS_DRAGONFLYBSD macro. */
#	define JSTR_OS_DRAGONFLYBSD 1
#endif
#if defined _AIX || defined __TOS_AIX__
/** @brief JSTR_OS_AIX macro. */
#	define JSTR_OS_AIX 1
#endif
#if defined UTS
/** @brief JSTR_OS_UTS macro. */
#	define JSTR_OS_UTS 1
#endif
#if defined AMIGA || defined __amigaos__
/** @brief JSTR_OS_AMIGA macro. */
#	define JSTR_OS_AMIGA 1
#endif
#if defined aegis
/** @brief JSTR_OS_APOLLOAEGIS macro. */
#	define JSTR_OS_APOLLOAEGIS 1
#endif
#if defined apollo
/** @brief JSTR_OS_APOLLODOMAIN macro. */
#	define JSTR_OS_APOLLODOMAIN 1
#endif
#if defined __BEOS__
/** @brief JSTR_OS_BE macro. */
#	define JSTR_OS_BE 1
#endif
#if defined __bg__ || defined __THW_BLUEGEN__
/** @brief JSTR_OS_BLUEGENE macro. */
#	define JSTR_OS_BLUEGENE 1
#endif
#if defined __convex__
/** @brief JSTR_OS_CONVEX macro. */
#	define JSTR_OS_CONVEX 1
#endif
#if defined DGUX || defined __DGUX__ || defined __dgux__
/** @brief JSTR_OS_DGUX macro. */
#	define JSTR_OS_DGUX 1
#endif
#if defined _SEQUENT_ || defined sequent
/** @brief JSTR_OS_DYNIXPTX macro. */
#	define JSTR_OS_DYNIXPTX 1
#endif
#if defined __EMX__
/** @brief JSTR_OS_EMX macro. */
#	define JSTR_OS_EMX 1
#endif
#if defined __GNU__ || defined __gnu_hurd__
/** @brief JSTR_OS_GNUHURD macro. */
#	define JSTR_OS_GNUHURD 1
#endif
#if defined __FreeBSD_kernel__ && defined __GLIBC__
/** @brief JSTR_OS_GNUKFREEBSD macro. */
#	define JSTR_OS_GNUKFREEBSD 1
#endif
#if defined __hiuxmpp
/** @brief JSTR_OS_HIUXMPP macro. */
#	define JSTR_OS_HIUXMPP 1
#endif
#if defined _hpux || defined hpux || defined __hpux
/** @brief JSTR_OS_HPUX macro. */
#	define JSTR_OS_HPUX 1
#endif
#if defined __OS400__
/** @brief JSTR_OS_IBMOS400 macro. */
#	define JSTR_OS_IBMOS400 1
#endif
#if defined sgi || defined __sgi
/** @brief JSTR_OS_IRIX macro. */
#	define JSTR_OS_IRIX 1
#endif
#if defined __Lynx__
/** @brief JSTR_OS_LYNX macro. */
#	define JSTR_OS_LYNX 1
#endif
#if defined macintosh || defined Macintosh || (defined __APPLE__ && defined __MACH__)
/** @brief JSTR_OS_MAC macro. */
#	define JSTR_OS_MAC 1
#endif
#if defined __minix
/** @brief JSTR_OS_MINIX macro. */
#	define JSTR_OS_MINIX 1
#endif
#if defined __MORPHOS__
/** @brief JSTR_OS_MORPH macro. */
#	define JSTR_OS_MORPH 1
#endif
#if defined mpeix || defined __mpexl
/** @brief JSTR_OS_MPEIX macro. */
#	define JSTR_OS_MPEIX 1
#endif
#if defined MSDOS || defined __MSDOS__ || defined _MSDOS || defined __DOS__
/** @brief JSTR_OS_MSDOS macro. */
#	define JSTR_OS_MSDOS 1
#endif
#if defined __TANDEM
/** @brief JSTR_OS_NONSTOP macro. */
#	define JSTR_OS_NONSTOP 1
#endif
#if defined OS2 || defined _OS2 || defined __OS2__ || defined __TOS_OS2__
/** @brief JSTR_OS_OS2 macro. */
#	define JSTR_OS_OS2 1
#endif
#if defined __palmos__
/** @brief JSTR_OS_PALM macro. */
#	define JSTR_OS_PALM 1
#endif
#if defined EPLAN9
/** @brief JSTR_OS_PLAN9 macro. */
#	define JSTR_OS_PLAN9 1
#endif
#if defined pyr
/** @brief JSTR_OS_PYRAMIDDCOSX macro. */
#	define JSTR_OS_PYRAMIDDCOSX 1
#endif
#if defined __QNX__ || defined __QNXNTO__
/** @brief JSTR_OS_QNSX macro. */
#	define JSTR_OS_QNSX 1
#endif
#if defined sinux
/** @brief JSTR_OS_RELIANTUNIX macro. */
#	define JSTR_OS_RELIANTUNIX 1
#endif
#if defined M_I386 || defined M_XENIX || defined _SCO_DS
/** @brief JSTR_OS_SCOOPENSERVER macro. */
#	define JSTR_OS_SCOOPENSERVER 1
#endif
#if (defined sun || defined __sun) && (defined __SVR4 || defined __svr4__)
/** @brief JSTR_OS_SOLARIS macro. */
#	define JSTR_OS_SOLARIS 1
#endif
#if (defined sun || defined __sun) && (!defined __SVR4 && !defined __svr4__)
/** @brief JSTR_OS_SUNOS macro. */
#	define JSTR_OS_SUNOS   1
#endif
#if defined __VOS__
/** @brief JSTR_OS_STRATUSVOX macro. */
#	define JSTR_OS_STRATUSVOX 1
#endif
#if defined __SYLLABLE__
/** @brief JSTR_OS_SYLLABLE macro. */
#	define JSTR_OS_SYLLABLE 1
#endif
#if defined __SYMBIAN32__
/** @brief JSTR_OS_SYMBIAN macro. */
#	define JSTR_OS_SYMBIAN 1
#endif
#if defined __osf__ || defined __osf
/** @brief JSTR_OS_TRU64OSF1 macro. */
#	define JSTR_OS_TRU64OSF1 1
#endif
#if defined ultrix || defined __ultrix || defined __ultrix__ || (defined unix && defined vax)
/** @brief JSTR_OS_ULTRIX macro. */
#	define JSTR_OS_ULTRIX 1
#endif
#if defined _UNICOS
/** @brief JSTR_OS_UNIC macro. */
#	define JSTR_OS_UNIC 1
#endif
#if defined sco || defined _UNIXWARE7
/** @brief JSTR_OS_UNIXWARE macro. */
#	define JSTR_OS_UNIXWARE 1
#endif
#if defined VMS || defined __VMS
/** @brief JSTR_OS_VMS macro. */
#	define JSTR_OS_VMS 1
#endif
#if defined __VXWORKS__ || defined __vxworks
/** @brief JSTR_OS_VXWORKS macro. */
#	define JSTR_OS_VXWORKS 1
#endif
#if defined _WIN16 || defined _WIN32 || defined _WIN64 || defined __WIN32__ || defined __TOS_WIN__ || defined __WINDOWS__
/** @brief JSTR_OS_WINDOWS macro. */
#	define JSTR_OS_WINDOWS 1
#endif
#if defined _WIN32_WCE
/** @brief JSTR_OS_WINDOWSCE macro. */
#	define JSTR_OS_WINDOWSCE 1
#endif
#if defined _WINDU_SOURCE
/** @brief JSTR_OS_WINDU macro. */
#	define JSTR_OS_WINDU 1
#endif
#if defined __MVS__ || defined __HOS_MVS__ || defined __TOS_MVS__
/** @brief JSTR_OS_Z macro. */
#	define JSTR_OS_Z 1
#endif
#if defined __unix__ || defined __unix
/** @brief JSTR_ENV_UNIX macro. */
#	define JSTR_ENV_UNIX 1
#endif
#if defined __FreeBSD__ || defined __NetBSD__ || defined __OpenBSD__ || defined __bsdi__ || defined __DragonFly__ || defined _SYSTYPE_BSD
/** @brief JSTR_ENV_BSD macro. */
#	define JSTR_ENV_BSD 1
#endif
#ifdef __CYGWIN__
/** @brief JSTR_ENV_CYGWIN macro. */
#	define JSTR_ENV_CYGWIN 1
#endif
#ifdef __INTERIX
/** @brief JSTR_ENV_INTERIX macro. */
#	define JSTR_ENV_INTERIX 1
#endif
#if defined __sysv__ || defined __SVR4 || defined __svr4__ || defined _SYSTYPE_SRV4
/** @brief JSTR_ENV_SVR4 macro. */
#	define JSTR_ENV_SVR4 1
#endif
#if defined _UWIN
/** @brief JSTR_ENV_UWIN macro. */
#	define JSTR_ENV_UWIN 1
#endif /* env */

#ifndef JSTR_OS_LINUX
/** @brief JSTR_OS_LINUX macro. */
#	define JSTR_OS_LINUX 0
#endif
#ifndef JSTR_OS_GNULINUX
/** @brief JSTR_OS_GNULINUX macro. */
#	define JSTR_OS_GNULINUX 0
#endif
#ifndef JSTR_OS_ANDROID
/** @brief JSTR_OS_ANDROID macro. */
#	define JSTR_OS_ANDROID 0
#endif
#ifndef JSTR_OS_FREEBSD
/** @brief JSTR_OS_FREEBSD macro. */
#	define JSTR_OS_FREEBSD 0
#endif
#ifndef JSTR_OS_NETBSD
/** @brief JSTR_OS_NETBSD macro. */
#	define JSTR_OS_NETBSD 0
#endif
#ifndef JSTR_OS_OPENBSD
/** @brief JSTR_OS_OPENBSD macro. */
#	define JSTR_OS_OPENBSD 0
#endif
#ifndef JSTR_OS_BSDI
/** @brief JSTR_OS_BSDI macro. */
#	define JSTR_OS_BSDI 0
#endif
#ifndef JSTR_OS_DRAGONFLYBSD
/** @brief JSTR_OS_DRAGONFLYBSD macro. */
#	define JSTR_OS_DRAGONFLYBSD 0
#endif
#ifndef JSTR_OS_AIX
/** @brief JSTR_OS_AIX macro. */
#	define JSTR_OS_AIX 0
#endif
#ifndef JSTR_OS_UTS
/** @brief JSTR_OS_UTS macro. */
#	define JSTR_OS_UTS 0
#endif
#ifndef JSTR_OS_AMIGA
/** @brief JSTR_OS_AMIGA macro. */
#	define JSTR_OS_AMIGA 0
#endif
#ifndef JSTR_OS_APOLLOAEGIS
/** @brief JSTR_OS_APOLLOAEGIS macro. */
#	define JSTR_OS_APOLLOAEGIS 0
#endif
#ifndef JSTR_OS_APOLLODOMAIN
/** @brief JSTR_OS_APOLLODOMAIN macro. */
#	define JSTR_OS_APOLLODOMAIN 0
#endif
#ifndef JSTR_OS_BE
/** @brief JSTR_OS_BE macro. */
#	define JSTR_OS_BE 0
#endif
#ifndef JSTR_OS_BLUEGENE
/** @brief JSTR_OS_BLUEGENE macro. */
#	define JSTR_OS_BLUEGENE 0
#endif
#ifndef JSTR_OS_CONVEX
/** @brief JSTR_OS_CONVEX macro. */
#	define JSTR_OS_CONVEX 0
#endif
#ifndef JSTR_OS_DGUX
/** @brief JSTR_OS_DGUX macro. */
#	define JSTR_OS_DGUX 0
#endif
#ifndef JSTR_OS_DYNIXPTX
/** @brief JSTR_OS_DYNIXPTX macro. */
#	define JSTR_OS_DYNIXPTX 0
#endif
#ifndef JSTR_OS_EMX
/** @brief JSTR_OS_EMX macro. */
#	define JSTR_OS_EMX 0
#endif
#ifndef JSTR_OS_GNUHURD
/** @brief JSTR_OS_GNUHURD macro. */
#	define JSTR_OS_GNUHURD 0
#endif
#ifndef JSTR_OS_GNUKFREEBSD
/** @brief JSTR_OS_GNUKFREEBSD macro. */
#	define JSTR_OS_GNUKFREEBSD 0
#endif
#ifndef JSTR_OS_HIUXMPP
/** @brief JSTR_OS_HIUXMPP macro. */
#	define JSTR_OS_HIUXMPP 0
#endif
#ifndef JSTR_OS_HPUX
/** @brief JSTR_OS_HPUX macro. */
#	define JSTR_OS_HPUX 0
#endif
#ifndef JSTR_OS_IBMOS400
/** @brief JSTR_OS_IBMOS400 macro. */
#	define JSTR_OS_IBMOS400 0
#endif
#ifndef JSTR_OS_IRIX
/** @brief JSTR_OS_IRIX macro. */
#	define JSTR_OS_IRIX 0
#endif
#ifndef JSTR_OS_LYNX
/** @brief JSTR_OS_LYNX macro. */
#	define JSTR_OS_LYNX 0
#endif
#ifndef JSTR_OS_MAC
/** @brief JSTR_OS_MAC macro. */
#	define JSTR_OS_MAC 0
#endif
#ifndef JSTR_OS_MINIX
/** @brief JSTR_OS_MINIX macro. */
#	define JSTR_OS_MINIX 0
#endif
#ifndef JSTR_OS_MORPH
/** @brief JSTR_OS_MORPH macro. */
#	define JSTR_OS_MORPH 0
#endif
#ifndef JSTR_OS_MPEIX
/** @brief JSTR_OS_MPEIX macro. */
#	define JSTR_OS_MPEIX 0
#endif
#ifndef JSTR_OS_MSDOS
/** @brief JSTR_OS_MSDOS macro. */
#	define JSTR_OS_MSDOS 0
#endif
#ifndef JSTR_OS_NONSTOP
/** @brief JSTR_OS_NONSTOP macro. */
#	define JSTR_OS_NONSTOP 0
#endif
#ifndef JSTR_OS_OS2
/** @brief JSTR_OS_OS2 macro. */
#	define JSTR_OS_OS2 0
#endif
#ifndef JSTR_OS_PALM
/** @brief JSTR_OS_PALM macro. */
#	define JSTR_OS_PALM 0
#endif
#ifndef JSTR_OS_PLAN9
/** @brief JSTR_OS_PLAN9 macro. */
#	define JSTR_OS_PLAN9 0
#endif
#ifndef JSTR_OS_PYRAMIDDCOSX
/** @brief JSTR_OS_PYRAMIDDCOSX macro. */
#	define JSTR_OS_PYRAMIDDCOSX 0
#endif
#ifndef JSTR_OS_QNSX
/** @brief JSTR_OS_QNSX macro. */
#	define JSTR_OS_QNSX 0
#endif
#ifndef JSTR_OS_RELIANTUNIX
/** @brief JSTR_OS_RELIANTUNIX macro. */
#	define JSTR_OS_RELIANTUNIX 0
#endif
#ifndef JSTR_OS_SCOOPENSERVER
/** @brief JSTR_OS_SCOOPENSERVER macro. */
#	define JSTR_OS_SCOOPENSERVER 0
#endif
#ifndef JSTR_OS_STRATUSVOX
/** @brief JSTR_OS_STRATUSVOX macro. */
#	define JSTR_OS_STRATUSVOX 0
#endif
#ifndef JSTR_OS_SYLLABLE
/** @brief JSTR_OS_SYLLABLE macro. */
#	define JSTR_OS_SYLLABLE 0
#endif
#ifndef JSTR_OS_SYMBIAN
/** @brief JSTR_OS_SYMBIAN macro. */
#	define JSTR_OS_SYMBIAN 0
#endif
#ifndef JSTR_OS_TRU64OSF1
/** @brief JSTR_OS_TRU64OSF1 macro. */
#	define JSTR_OS_TRU64OSF1 0
#endif
#ifndef JSTR_OS_ULTRIX
/** @brief JSTR_OS_ULTRIX macro. */
#	define JSTR_OS_ULTRIX 0
#endif
#ifndef JSTR_OS_UNIC
/** @brief JSTR_OS_UNIC macro. */
#	define JSTR_OS_UNIC 0
#endif
#ifndef JSTR_OS_UNIXWARE
/** @brief JSTR_OS_UNIXWARE macro. */
#	define JSTR_OS_UNIXWARE 0
#endif
#ifndef JSTR_OS_VMS
/** @brief JSTR_OS_VMS macro. */
#	define JSTR_OS_VMS 0
#endif
#ifndef JSTR_OS_VXWORKS
/** @brief JSTR_OS_VXWORKS macro. */
#	define JSTR_OS_VXWORKS 0
#endif
#ifndef JSTR_OS_WINDOWS
/** @brief JSTR_OS_WINDOWS macro. */
#	define JSTR_OS_WINDOWS 0
#endif
#ifndef JSTR_OS_WINDOWSCE
/** @brief JSTR_OS_WINDOWSCE macro. */
#	define JSTR_OS_WINDOWSCE 0
#endif
#ifndef JSTR_OS_WINDU
/** @brief JSTR_OS_WINDU macro. */
#	define JSTR_OS_WINDU 0
#endif
#ifndef JSTR_OS_Z
/** @brief JSTR_OS_Z macro. */
#	define JSTR_OS_Z 0
#endif
#ifndef JSTR_OS_SOLARIS
/** @brief JSTR_OS_SOLARIS macro. */
#	define JSTR_OS_SOLARIS 0
#endif
#ifndef JSTR_OS_SUNOS
/** @brief JSTR_OS_SUNOS macro. */
#	define JSTR_OS_SUNOS 0
#endif

#ifndef JSTR_ENV_UNIX
/** @brief JSTR_ENV_UNIX macro. */
#	define JSTR_ENV_UNIX 0
#endif
#ifndef JSTR_ENV_BSD
/** @brief JSTR_ENV_BSD macro. */
#	define JSTR_ENV_BSD 0
#endif
#ifndef JSTR_ENV_CYGWIN
/** @brief JSTR_ENV_CYGWIN macro. */
#	define JSTR_ENV_CYGWIN 0
#endif
#ifndef JSTR_ENV_INTERIX
/** @brief JSTR_ENV_INTERIX macro. */
#	define JSTR_ENV_INTERIX 0
#endif
#ifndef JSTR_ENV_SVR4
/** @brief JSTR_ENV_SVR4 macro. */
#	define JSTR_ENV_SVR4 0
#endif
#ifndef JSTR_ENV_UWIN
/** @brief JSTR_ENV_UWIN macro. */
#	define JSTR_ENV_UWIN 0
#endif

#endif /* JSTR_MACROS_OS_H */
