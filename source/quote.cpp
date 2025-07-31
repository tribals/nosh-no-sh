/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include "utils.h"
#include <cstdio>

namespace {

enum QuoteState { UNQUOTED, DOUBLE, SINGLE } ;

inline
QuoteState
nosh_quoting_level_needed (
	const std::string & s
) {
	if (s.empty()) return SINGLE;
	for (std::string::const_iterator p(s.begin()), e(s.end()); e != p; ++p) {
		const char c(*p);
		if ('\0' == c || '\r' == c || '\n' == c) return SINGLE;
	}
	for (std::string::const_iterator p(s.begin()), e(s.end()); e != p; ++p) {
		const char c(*p);
		if ('\'' == c || std::isspace(c)) return DOUBLE;
	}
	return UNQUOTED;
}

inline
void
append_mark (
	std::string & r,
	QuoteState quote
) {
	switch (quote) {
		case UNQUOTED:	break;
		case SINGLE: r += '\''; break;
		case DOUBLE: r += '\"'; break;
	}
}

}

std::string
quote_for_nosh (
	const std::string & s
) {
	std::string r;
	QuoteState defquote(nosh_quoting_level_needed(s)), quote(defquote);
	append_mark(r, quote);
	for (std::string::const_iterator b(s.begin()), e(s.end()), p(b); e != p; ++p) {
		const char c(*p);
	again:
		switch (quote) {
			case UNQUOTED:
				// Unquoted makes use of escapes, until something really needs quoting.
				if ('\\' == c || '\'' == c || '\"' == c)
					r += '\\';
				else
				if ('\0' == c || '\"' == c || std::isspace(c)) {
					quote = SINGLE;
					append_mark(r, quote);
					goto again;
				} else
				if ('\'' == c || (p == b && '#' == c)) {	// EOL comment rule
					quote = DOUBLE;
					append_mark(r, quote);
					goto again;
				}
				r += c;
				break;
			case SINGLE:
				// Single quoting only lasts for a run of whitespace and double quotes, unless single quoting is the default.
				if (quote != defquote && '\0' != c && '\"' != c && !std::isspace(c)) {
					append_mark(r, quote);
					quote = defquote;
					append_mark(r, quote);
					goto again;
				}
				r += c;
				break;
			case DOUBLE:
				// Double quoting lasts until something truly needs single quoting.
				// This is unlike conf file quoting, which aims for the shortest double-quoted run lengths.
				// nosh does not have the wacky sh rules for escapes in double quotes that need to be avoided.
				// But escaped newlines are discarded, and so have to be single-quoted instead.
				if ('\\' == c)
					r += c;
				else
				if ('\"' == c)
					r += '\\';
				else
				if ('\0' == c || '\r' == c || '\n' == c) {
					append_mark(r, quote);
					quote = SINGLE;
					append_mark(r, quote);
					goto again;
				}
				r += c;
				break;
		}
	}
	append_mark(r, quote);
	return r;
}

std::string
quote_for_conf (
	const std::string & s
) {
	std::string r;
	QuoteState quote(s.empty() ? SINGLE : UNQUOTED);
	append_mark(r, quote);
	for (std::string::const_iterator b(s.begin()), e(s.end()), p(b); e != p; ++p) {
		const char c(*p);
	again:
		switch (quote) {
			case UNQUOTED:
				// Unquoted makes use of escapes, until somthing really needs quoting.
				if ('\\' == c || '\'' == c || '\"' == c)
					r += '\\';
				else
				if ('\0' == c || '\"' == c || std::isspace(c)) {
					r += '\'';
					quote = SINGLE;
					goto again;
				} else
				if ('\'' == c || (p == b && '#' == c)) {	// EOL comment rule
					r += '\"';
					quote = DOUBLE;
					goto again;
				}
				r += c;
				break;
			case SINGLE:
				// Single quoting only lasts for a run of whitespace and double quotes.
				if ('\0' != c && '\"' != c && !std::isspace(c)) {
					r += '\'';
					quote = UNQUOTED;
					goto again;
				}
				r += c;
				break;
			case DOUBLE:
				// Double quoting only lasts for a run of comment introducers, slashes, and single quotes.
				// This is unlike nosh file quoting, which aims for the longest double-quoted run lengths.
				// It avoids the wacky sh rules for escapes in double quotes, which conf files have to be compatible with even though they aren't scripts.
				if ('\\' == c)
					r += c;
				else
				if ('\'' != c && !(p == b && '#' == c)) {
					r += '\"';
					quote = UNQUOTED;
					goto again;
				}
				r += c;
				break;
		}
	}
	append_mark(r, quote);
	return r;
}

namespace {

inline
bool
is_posix_portable_filename_character (
	char c
) {
	return std::isalnum(c) || '/' == c || '-' == c || '_' == c || '.' == c;
}

inline
QuoteState
sh_quoting_level_needed (
	const std::string & s
) {
	if (s.empty()) return SINGLE;
	int r(0);
	for (std::string::const_iterator p(s.begin()), e(s.end()); e != p; ++p) {
		const char c(*p);
		if ('\0' == c) {
			// We use double-quoting for NULs.
			if (r < 2) r = 2;
		} else
		if ('\'' == c) {
			// If we are already single-quoting, we can deal with single quotes.
			// If we are not, we might as well go straight to double-quoting.
			if (r < 1) r = 2;
		} else
		if (!is_posix_portable_filename_character(c))
		{
			if (r < 1) r = 1;
		}
	}
	return r > 1 ? DOUBLE : r > 0 ? SINGLE : UNQUOTED;
}

}

std::string
quote_for_sh (
	const std::string & s
) {
	std::string r;
	switch (sh_quoting_level_needed(s)) {
		case UNQUOTED:
			r = s;
			break;
		case SINGLE:
			r += '\'';
			// In the sh rules \ is never an escape character and there is no way to quote ' .
			for (std::string::const_iterator p(s.begin()), e(s.end()); e != p; ++p) {
				const char c(*p);
				// So terminate and start a new single-quoted string.
				if ('\'' == c)
					r += "'\\'";
				r += c;
			}
			r += '\'';
			break;
		case DOUBLE:
			r += '\"';
			// sh has some quite wacky rules where \ is mostly not an escape character.
			for (std::string::const_iterator p(s.begin()), e(s.end()); e != p; ++p) {
				const char c(*p);
				if ('\"' == c || '\\' == c || '$' == c || '`' == c) {
					r += '\\';
					r += c;
				} else
				if ('\0' == c) {
					r += "\"$'\\0'\"";
				} else
				if ('\r' == c || '\n' == c) {
					r += "\"'";
					r += c;
					r += "'\"";
				} else
				{
					r += c;
				}
			}
			r += '\"';
			break;
	}
	return r;
}
