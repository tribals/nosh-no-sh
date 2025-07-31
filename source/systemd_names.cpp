/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include "utils.h"
#include <cstdio>
#include <unistd.h>

#include "systemd_names_escape_char.h"

namespace {

// Secretly allow for appending "-log" to most escaped names.
// Also allow for the maximum incorporating an extra NUL that the C++ string does not have.
inline
bool
too_long_for_account(
	const std::string & s
) {
	static const long login_name_max(sysconf(_SC_LOGIN_NAME_MAX));
	if (0 > login_name_max) return false;	// There is no realistic fallback maximum.
	return s.length() + 4U >= login_name_max;
}

}

std::string
systemd_name_unescape (
	const std::string & s
) {
	std::string r;
	for (std::string::const_iterator e(s.end()), q(s.begin()); e != q; ) {
		char c(*q++);
		if ('-' == c) {
			c = '/';
		} else
		if (ESCAPE_CHAR == c && e != q) {
			c = *q++;
			if ('X' == c || 'x' == c) {
				unsigned v(0U);
				for (unsigned n(0U);n < 2U; ++n) {
					if (e == q) break;
					c = *q;
					if (!std::isxdigit(c)) break;
					++q;
					const unsigned char d(std::isdigit(c) ? (c - '0') : (std::tolower(c) - 'a' + 10));
					v = (v << 4) | d;
				}
				c = char(v);
			}
		}
		r += c;
	}
	return r;
}

std::string
old_account_name_unescape (
	const std::string & s
) {
	std::string r;
	for (std::string::const_iterator e(s.end()), q(s.begin()); e != q; ) {
		char c(*q++);
		if (ESCAPE_CHAR == c && e != q) {
			c = *q++;
			if ('X' == c || 'x' == c) {
				unsigned v(0U);
				for (unsigned n(0U);n < 2U; ++n) {
					if (e == q) break;
					c = *q;
					if (!std::isxdigit(c)) break;
					++q;
					const unsigned char d(std::isdigit(c) ? (c - '0') : (std::tolower(c) - 'a' + 10));
					v = (v << 4) | d;
				}
				c = char(v);
			}
		}
		r += c;
	}
	return r;
}

std::string
account_name_unescape (
	const std::string & s
) {
	std::string r;
	for (std::string::const_iterator e(s.end()), q(s.begin()); e != q; ) {
		char c(*q++);
		if (ESCAPE_CHAR == c && e != q) {
			c = *q++;
			if ('X' == c || 'x' == c) {
				unsigned v(0U);
				for (unsigned n(0U);n < 2U; ++n) {
					if (e == q) break;
					c = *q;
					if (!std::isxdigit(c)) break;
					++q;
					const unsigned char d(std::isdigit(c) ? (c - '0') : (std::tolower(c) - 'a' + 10));
					v = (v << 4) | d;
				}
				c = char(v);
			} else
			switch (c) {
				case ESCAPE_CHAR:	break;
				case 'a':	c = '@'; break;
				case 'c':	c = ':'; break;
				case 'd':	c = '.'; break;
				case 'h':	c = ';'; break;
				case 'm':	c = '-'; break;
				case 'p':	c = '+'; break;
				case 'u':
					if (e == q) {
						r += ESCAPE_CHAR;
						break;
					}
					c = *q++;
					if (std::isalpha(c))
						c = std::toupper(c);
					break;
				case 'v':	c = ','; break;
				default:
					break;
			}
		}
		r += c;
	}
	return r;
}

std::string
systemd_name_escape (
	const std::string & s
) {
	std::string r;
	for (std::string::const_iterator e(s.end()), q(s.begin()); e != q; ) {
		const char c(*q++);
		if ('/' == c) {
			r += '-';
		} else
		if (ESCAPE_CHAR == c || '-' == c) {
			char buf[6];
			snprintf(buf, sizeof buf, "%cx%02x", ESCAPE_CHAR, unsigned(c));
			r += std::string(buf);
		} else
			r += c;
	}
	return r;
}

std::string
old_alt_name_escape (
	const std::string & s
) {
	std::string r;
	for (std::string::const_iterator e(s.end()), q(s.begin()); e != q; ) {
		const char c(*q++);
		if (ESCAPE_CHAR == c || prohibited_in_account(c)) {
			char buf[6];
			buf[0] = ESCAPE_CHAR;
			snprintf(buf + 1, sizeof buf - 1, "x%02x", unsigned(c));
			r += std::string(buf);
		} else
			r += c;
	}
	return r;
}

std::string
alt_name_escape (
	const std::string & s
) {
	std::string r;
	for (std::string::const_iterator e(s.end()), q(s.begin()); e != q; ) {
		const char c(*q++);
		if (ESCAPE_CHAR == c || prohibited_in_account(c)) {
			char buf[6];
			buf[0] = ESCAPE_CHAR;
			if (ESCAPE_CHAR == c) {
				buf[1] = ESCAPE_CHAR;
				buf[2] = '\0';
			} else
			if (':' == c) {
				buf[1] = 'c';
				buf[2] = '\0';
			} else
			if (';' == c) {
				buf[1] = 'h';
				buf[2] = '\0';
			} else
			if ('.' == c) {
				buf[1] = 'd';
				buf[2] = '\0';
			} else
			if (',' == c) {
				buf[1] = 'v';
				buf[2] = '\0';
			} else
			if ('-' == c) {
				buf[1] = 'm';
				buf[2] = '\0';
			} else
			if ('+' == c) {
				buf[1] = 'p';
				buf[2] = '\0';
			} else
			if ('@' == c) {
				buf[1] = 'a';
				buf[2] = '\0';
			} else
			if (std::isupper(c)) {
				buf[1] = 'u';
				buf[2] = std::tolower(c);
				buf[3] = '\0';
			} else
				snprintf(buf + 1, sizeof buf - 1, "x%02x", unsigned(c));
			r += std::string(buf);
		} else
			r += c;
	}
	return r;
}

std::string
hashed_account_name (
	const std::string & s
) {
	// This is the DJBT33A (Daniel J. Bernstein Times 33 and Add) hash.
	// The initial value is a prime number that worked well, as reported by DJB on comp.lang.c in the 1990s.
	// It ensures that the hash values spread out well through the value space, even when the input character values are small.
	// We don't need cryptographic anything; we need low collision probability and a small enough result to be encodable in base 36 in fewer than (on NetBSD) 16 characters.
	uint32_t hash(5381U);

	std::string r;
	bool begin_word(true);
	for (std::string::const_iterator e(s.end()), q(s.begin()); e != q; ) {
		const char c(*q++);
		if (std::isalnum(c)) {
			if (begin_word) {
				// For a minimal form of human legibility, include the first letter of each word.
				r += c;
				begin_word = false;
			}
			hash = hash * 33U + static_cast<unsigned char>(c);
		} else
			begin_word = true;
	}
	if (!begin_word) r += '-';
	std::string::iterator p(r.end());
	do {
		unsigned int v(hash % 36U);
		hash /= 36U;
		p = r.insert(p, "0123456789abcdefghijklmnopqrstuvwxyz"[v]);
	} while (hash > 0U);
	return r;
}

std::string
account_name_escape (
	const std::string & s
) {
	const std::string r(alt_name_escape(s));
	return too_long_for_account(r) ? hashed_account_name(s) : r;
}
