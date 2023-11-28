#!/usr/bin/awk -f

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
		gsub(/^\/\/<br>/, "```")
	}
	print
}
