#ifndef TEST_HAYSTACK_NEEDLE_H
#define TEST_HAYSTACK_NEEDLE_H

/* clang-format off */

static struct test_ty {
	const char *hs;
	const char *ne;
} haystacks_and_needles[] = {
	{"yxxyyyyxyxxxxy","yyyyy"},
	{",yyy,xxxyxxxxy",",yyy,"},
	{"yxyyyxxxyxxxxyyyy","yyyyy"},
	{"yxxyyyyxyxxxxy","xxxxx"},
	{"yxxyyyyxyxxxxy","xyyyx"},
	{",xxyyyyxyxxxxy","xyy,y"},
	{"yxheL,yyyyxyxxxxy","yyheL,"},
	{"yxxyyyyxyxxxxy","yyyyy"},
	{"yyyyyxxxyxxxxy","yyyyy"},
	{"yxxx,xxxxy","xxx,"},
	{"yxxxyxxxxy","xxxx"},
	{"yxxxyxxxxy","xxx"},
	{"yxxxyxxy","xxx"},
	{"xxx","xxx"},
	{"xxx","x"},
	{"xxx","yyy"},
	{",","x,x"},
	{"x","xxx"},
	{"xxx",""},
	{"","xxx"},
	{"",""},
	{" he11o wor1d","he11o wor1d"},
	{"he11o wor1","he11o wor1d"},
	{"he11o wor1d","he11o wor1d"},
	{"he11o wor1d","he11o wor1"},
	{"he11o wor1d","he11o wor"},
	{"he11o wor1d","he11o wo"},
	{"he11o wor1d","he11o w"},
	{"he11o wor1d","he11o "},
	{"he11o wor1d","he11o"},
	{"he11o wor1d","wor1d"},
	{"he11o wor1d","he11o "},
	{"he11o wor1d"," he11o"},
	{"he11o wor1d","  he11o"},
	{"he11o wor1d","o w"},
	{"he11o wor1d","he1"},
	{"he11o","he11o"},
	{" he11o","he11o "},
	{" he11o","he11o"},
	{"he11o ","he11o"},
	{" he11o ","he11o"},
	{"  he11o ","he11o"},
	{"he11o  ","he11o"},
	{"he11o  ","he11o     "},
	{"yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy","yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"},
	{"xxyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy","yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"},
	{"xxyyyy,yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy","yyyyyyyyyyyyyyyyy,yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"},
	{"xxyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyxx","yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"},

	/* Some of these haystacks and needles were taken from musl's libc-test.

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#define y256 "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"
#define NE(hs, ne) \
	{hs, ne}, \
	{y256 hs, y256 ne}, \
	{hs y256, ne y256}, \

	NE("","a")
	NE("a","a")
	NE("a","b")
	NE("aa","b")
	NE("aa","a")
	NE("aba","b")
	NE("abba","b")
	NE("abba","ba")
	NE("abc abc","d")
	NE("0-1-2-3-a-5-6-7-8-9","")
	NE("0-1-2-3-a-5-6-7-8-9","")
	NE("_ _ _\xff_ _ _","\x7f_")
	NE("_ _ _\x7f_ _ _","\xff_")
	NE("","")
	NE("abcd","")
	NE("abcd","a")
	NE("abcd","b")
	NE("abcd","c")
	NE("abcd","d")
	NE("abcd","ab")
	NE("abcd","bc")
	NE("abcd","cd")
	NE("ababa","baba")
	NE("ababab","babab")
	NE("abababa","bababa")
	NE("abababab","bababab")
	NE("ababababa","babababa")
	NE("abbababab","bababa")
	NE("abbababab","ababab")
	NE("abacabcabcab","abcabcab")
	NE("nanabanabanana","aba")
	NE("nanabanabanana","ban")
	NE("nanabanabanana","anab")
	NE("nanabanabanana","banana")
	NE("_ _\xff_ _","_\xff_")

	NE("","4")
	NE("4","4")
	NE("4","b")
	NE("44","b")
	NE("44","4")
	NE("4b4","b")
	NE("4bb4","b")
	NE("4bb4","b4")
	NE("4bc 4bc","d")
	NE("0-1-2-3-4-5-6-7-8-9","")
	NE("0-1-2-3-4-5-6-7-8-9","")
	NE("_ _ _\xff_ _ _","\x7f_")
	NE("_ _ _\x7f_ _ _","\xff_")
	NE("","")
	NE("4bcd","")
	NE("4bcd","4")
	NE("4bcd","b")
	NE("4bcd","c")
	NE("4bcd","d")
	NE("4bcd","4b")
	NE("4bcd","bc")
	NE("4bcd","cd")
	NE("4b4b4","b4b4")
	NE("4b4b4b","b4b4b")
	NE("4b4b4b4","b4b4b4")
	NE("4b4b4b4b","b4b4b4b")
	NE("4b4b4b4b4","b4b4b4b4")
	NE("4bb4b4b4b","b4b4b4")
	NE("4bb4b4b4b","4b4b4b")
	NE("4b4c4bc4bc4b","4bc4bc4b")
	NE("n4n4b4n4b4n4n4","4b4")
	NE("n4n4b4n4b4n4n4","b4n")
	NE("n4n4b4n4b4n4n4","4n4b")
	NE("n4n4b4n4b4n4n4","b4n4n4")
	NE("_ _\xff_ _","_\xff_")

	{"end", "EnD"}
	};

#endif /* TEST_HAYSTACK_NEEDLE_H */
