#!/usr/bin/awk -f

# Copyright (c) 2023 James Tirta Halim <tirtajames45 at gmail dot com>
# This file is part of the jstring library.
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# 
# MIT License (Expat)

BEGIN {
	in_code = 0;
	curr = 0;
}
{
	gsub(/^\/\//, "```")
	curr = ($0 ~ /^```/)
	if (curr)
		in_code = (in_code) ? 0 : 1;
	if (!in_code) {
		gsub(/\*/, "\\*")
		gsub(/_/, "\\_")
		gsub(/\(/, "\\(")
		gsub(/\)/,  "\\)")
		gsub(/\\/, "\\")
		gsub(/^-- /, "### ")
		gsub(/^--- /, "## ")
		gsub(/^---- /, "# ")
		if ($0 != "" && $0 !~ /^```/ && $0 !~ /^#{0,3} /)
			gsub(/$/, "<br>")
		gsub(/^\/\/<br>/, "```")
	}
	print
}
