/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include "utils.h"
#include "popt.h"

/* Settings processing utility functions ************************************
// **************************************************************************
*/

namespace {

enum EscapeFormat { ESCAPE_SYSTEMD, ESCAPE_OLD_ALT, ESCAPE_ALT, ESCAPE_HASH, ESCAPE_ACCOUNT };

struct escape_format_definition : public popt::integral_definition {
public:
	escape_format_definition(char s, const char * l, const char * d) : integral_definition(s, l, a, d), v(ESCAPE_SYSTEMD) {}
	virtual ~escape_format_definition();
	const EscapeFormat & query_value() const { return v; }
protected:
	static const char a[];
	virtual void action(popt::processor &, const char *);
	EscapeFormat v;
};

const char escape_format_definition::a[] = "escfmt";
escape_format_definition::~escape_format_definition() {}
void escape_format_definition::action(popt::processor & /*proc*/, const char * text)
{
	if (0 == std::strcmp(text, "systemd")) {
		v = ESCAPE_SYSTEMD;
		set = true;
	} else
	if (0 == std::strcmp(text, "old-alt")) {
		v = ESCAPE_OLD_ALT;
		set = true;
	} else
	if (0 == std::strcmp(text, "alt")) {
		v = ESCAPE_ALT;
		set = true;
	} else
	if (0 == std::strcmp(text, "hash")) {
		v = ESCAPE_HASH;
		set = true;
	} else
	if (0 == std::strcmp(text, "account")) {
		v = ESCAPE_ACCOUNT;
		set = true;
	} else
		throw popt::error(text, "escape format specification is not {systemd|old-alt|account}");
}

std::string
escape (
	EscapeFormat format,
	const char * v
) {
	switch (format) {
		default:
		case ESCAPE_SYSTEMD:	return systemd_name_escape(v);
		case ESCAPE_ACCOUNT:	return account_name_escape(v);
		case ESCAPE_OLD_ALT:	return old_alt_name_escape(v);
		case ESCAPE_ALT:	return alt_name_escape(v);
		case ESCAPE_HASH:	return hashed_account_name(v);
	}
}

}

/* System control subcommands ***********************************************
// **************************************************************************
*/

void
escape [[gnu::noreturn]] (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	const char * prefix("");
	EscapeFormat escape_format(ESCAPE_SYSTEMD);
	try {
		popt::string_definition prefix_option('p', "prefix", "string", "Prefix each name with this (template) name.", prefix);
		escape_format_definition escape_format_option('\0', "format", "Choose the escape algorithm.");
		popt::definition * main_table[] = {
			&escape_format_option,
			&prefix_option
		};
		popt::top_table_definition main_option(sizeof main_table/sizeof *main_table, main_table, "Main options", "{service(s)...}");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
		if (escape_format_option.is_set()) escape_format= escape_format_option.query_value();
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}

	if (args.empty()) {
		die_missing_argument(prog, envs, "name(s)");
	}

	for (std::vector<const char *>::const_iterator b(args.begin()), e(args.end()), i(b); e != i; ++i) {
		const std::string escaped(escape(escape_format, *i));
		if (b != i) std::cout.put(' ');
		std::cout << prefix << escaped;
	}
	std::cout.put('\n');

	throw EXIT_SUCCESS;
}

void
systemd_escape [[gnu::noreturn]] (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	std::string prefix;
	const char * suffix("");
	try {
		const char * t(nullptr);
		popt::string_definition template_option('t', "template", "string", "Instantiate this template with each name.", t);
		popt::definition * main_table[] = {
			&template_option
		};
		popt::top_table_definition main_option(sizeof main_table/sizeof *main_table, main_table, "Main options", "{service(s)...}");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
		if (t) {
			if (const char * at = std::strchr(t, '@')) {
				prefix = std::string(t, at + 1);
				suffix = at + 1;
			} else
				prefix = t;
		}
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}

	if (args.empty()) {
		die_missing_argument(prog, envs, "name(s)");
	}

	for (std::vector<const char *>::const_iterator b(args.begin()), e(args.end()), i(b); e != i; ++i) {
		const std::string escaped(systemd_name_escape(*i));
		if (b != i) std::cout.put(' ');
		std::cout << prefix << escaped << suffix;
	}
	std::cout.put('\n');

	throw EXIT_SUCCESS;
}
