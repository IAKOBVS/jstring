#!/usr/bin/awk -f

# See LICENSE file for copyright and license details.

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
		gsub(/^-- /, "## ")
		gsub(/--- /, "# ")
		if ($0 !~ "")
			gsub(/$/, "<br>")
		gsub(/^\/\/<br>/, "```")
	}
	print
}
