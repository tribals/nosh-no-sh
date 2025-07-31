/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <string>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include "utils.h"
#include "FileStar.h"

std::vector<std::string>
read_file (
	FILE * f,
	unsigned long & line
) {
	line = 0UL;
	std::vector<std::string> a;
	std::string * current = nullptr;
	enum { UNQUOTED, DOUBLE, SINGLE } quote(UNQUOTED);
	bool slash(false), comment(false);
	for (;;) {
		const int c(std::fgetc(f));
		if (std::feof(f)) break;
		if (UNQUOTED == quote && !slash) {
			if (comment) {
				if ('\n' == c) {
					comment = false;
					++line;
				}
				continue;
			} else if ('#' == c && !current) {
				comment = true;
				continue;
			} else if (std::isspace(c)) {
				current = nullptr;
				continue;
			}
		}
		if (slash && '\n' == c) {
			++line;
			slash = false;
			continue;
		}
		if (!current) {
			a.push_back(std::string());
			current = &a.back();
		}
		if (slash) {
			*current += char(c);
			slash = false;
		} else {
			switch (quote) {
				case SINGLE:
					if ('\'' == c)
						quote = UNQUOTED;
					else
						*current += char(c);
					break;
				case DOUBLE:
					if ('\\' == c)
						slash = true;
					else if ('\"' == c)
						quote = UNQUOTED;
					else
						*current += char(c);
					break;
				case UNQUOTED:
					if ('\\' == c)
						slash = true;
					else if ('\"' == c)
						quote = DOUBLE;
					else if ('\'' == c)
						quote = SINGLE;
					else
						*current += char(c);
					break;
			}
			if ('\n' == c) ++line;
		}
	}
	if (slash)
		throw "Slash before end of file.";
	if (UNQUOTED != quote)
		throw "Unterminated quote at end of file.";
	return a;
}

std::vector<std::string>
read_file (
	const char * prog,
	const char * filename,
	FILE * f
) {
	unsigned long line;
	try {
		std::vector<std::string> v(read_file(f, line));
		if (std::ferror(f)) {
			const int error(errno);
			std::fclose(f);
			std::fprintf(stderr, "%s: ERROR: %s(%lu): %s\n", prog, filename, line, std::strerror(error));
			throw EXIT_FAILURE;
		}
		std::fclose(f);
		return v;
	} catch (const char * r) {
		std::fclose(f);
		std::fprintf(stderr, "%s: FATAL: %s(%lu): %s\n", prog, filename, line, r);
		throw EXIT_FAILURE;
	} catch (...) {
		std::fclose(f);
		throw;
	}
}

std::vector<std::string>
read_file (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * filename,
	const FileStar & f
) {
	unsigned long line;
	try {
		std::vector<std::string> v(read_file(f.operator FILE *(), line));
		if (std::ferror(f.operator FILE *()))
			die_errno(prog, envs, filename);
		return v;
	} catch (const char * r) {
		die_parser_error(prog, envs, filename, line, r);
	} catch (...) {
		throw;
	}
}

std::vector<std::string>
read_file (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * filename
) {
	FILE * f(std::fopen(filename, "r"));
	if (!f)
		die_errno(prog, envs, filename);
	return read_file(prog, filename, f);
}
