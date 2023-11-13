/* See LICENSE file for copyright and license details. */

#ifndef JSTRP_SELECTOR_STRING_MISC_H
#define JSTRP_SELECTOR_STRING_MISC_H 1
#include "jstr-macros.h"

JSTRP_BEGIN_DECLS
#include <endian.h>
#include <limits.h>
JSTRP_END_DECLS

#include "_glibc-generic-string-misc.h"

#if defined __GNUC__ || defined __clang__
#	pragma GCC diagnostic ignored "-Wshift-count-overflow"
#	pragma GCC diagnostic push
#endif

JSTRP_BEGIN_DECLS

/* P must be aligned to sizeof(jstr_word_ty). */
JSTR_INLINE
JSTR_FUNC_PURE
static jstr_word_ty
jstr_word_toword(const void *JSTR_RESTRICT p)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_ATTR_MAY_ALIAS
	return *(jstr_word_ty *)p;
#else
	JSTR_ASSERT(sizeof(jstr_word_ty) >= 2 && sizeof(jstr_word_ty) <= 64, "");
#	define SH(idx) \
		(JSTR_ENDIAN_LITTLE ? ((jstr_word_ty) * ((unsigned char *)p + (idx)) << ((idx)*CHAR_BIT)) : ((jstr_word_ty) * ((unsigned char *)p + (idx)) >> ((idx)*CHAR_BIT)))
	/* Generated with ../bin/generate-ctoword 2 64. */
	switch (sizeof(jstr_word_ty)) {
	case 2: return SH(1) | SH(0);
	case 3: return SH(2) | SH(1) | SH(0);
	case 4: return SH(3) | SH(2) | SH(1) | SH(0);
	case 5: return SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 6: return SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 7: return SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 8: return SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 9: return SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 10: return SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 11: return SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 12: return SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 13: return SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 14: return SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 15: return SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 16: return SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 17: return SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 18: return SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 19: return SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 20: return SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 21: return SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 22: return SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 23: return SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 24: return SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 25: return SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 26: return SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 27: return SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 28: return SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 29: return SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 30: return SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 31: return SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 32: return SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 33: return SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 34: return SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 35: return SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 36: return SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 37: return SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 38: return SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 39: return SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 40: return SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 41: return SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 42: return SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 43: return SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 44: return SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 45: return SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 46: return SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 47: return SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 48: return SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 49: return SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 50: return SH(49) | SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 51: return SH(50) | SH(49) | SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 52: return SH(51) | SH(50) | SH(49) | SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 53: return SH(52) | SH(51) | SH(50) | SH(49) | SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 54: return SH(53) | SH(52) | SH(51) | SH(50) | SH(49) | SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 55: return SH(54) | SH(53) | SH(52) | SH(51) | SH(50) | SH(49) | SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 56: return SH(55) | SH(54) | SH(53) | SH(52) | SH(51) | SH(50) | SH(49) | SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 57: return SH(56) | SH(55) | SH(54) | SH(53) | SH(52) | SH(51) | SH(50) | SH(49) | SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 58: return SH(57) | SH(56) | SH(55) | SH(54) | SH(53) | SH(52) | SH(51) | SH(50) | SH(49) | SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 59: return SH(58) | SH(57) | SH(56) | SH(55) | SH(54) | SH(53) | SH(52) | SH(51) | SH(50) | SH(49) | SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 60: return SH(59) | SH(58) | SH(57) | SH(56) | SH(55) | SH(54) | SH(53) | SH(52) | SH(51) | SH(50) | SH(49) | SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 61: return SH(60) | SH(59) | SH(58) | SH(57) | SH(56) | SH(55) | SH(54) | SH(53) | SH(52) | SH(51) | SH(50) | SH(49) | SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 62: return SH(61) | SH(60) | SH(59) | SH(58) | SH(57) | SH(56) | SH(55) | SH(54) | SH(53) | SH(52) | SH(51) | SH(50) | SH(49) | SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 63: return SH(62) | SH(61) | SH(60) | SH(59) | SH(58) | SH(57) | SH(56) | SH(55) | SH(54) | SH(53) | SH(52) | SH(51) | SH(50) | SH(49) | SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	case 64: return SH(63) | SH(62) | SH(61) | SH(60) | SH(59) | SH(58) | SH(57) | SH(56) | SH(55) | SH(54) | SH(53) | SH(52) | SH(51) | SH(50) | SH(49) | SH(48) | SH(47) | SH(46) | SH(45) | SH(44) | SH(43) | SH(42) | SH(41) | SH(40) | SH(39) | SH(38) | SH(37) | SH(36) | SH(35) | SH(34) | SH(33) | SH(32) | SH(31) | SH(30) | SH(29) | SH(28) | SH(27) | SH(26) | SH(25) | SH(24) | SH(23) | SH(22) | SH(21) | SH(20) | SH(19) | SH(18) | SH(17) | SH(16) | SH(15) | SH(14) | SH(13) | SH(12) | SH(11) | SH(10) | SH(9) | SH(8) | SH(7) | SH(6) | SH(5) | SH(4) | SH(3) | SH(2) | SH(1) | SH(0);
	}
#	undef SH
#endif
}

JSTRP_END_DECLS

#if defined __GNUC__ || defined __clang__
#	pragma GCC diagnostic pop
#endif

#endif /* JSTRP_STRING_MISC_H */
