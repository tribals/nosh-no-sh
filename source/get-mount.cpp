/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <cerrno>
#include <unistd.h>
#include <fstab.h>
#include "utils.h"
#include "popt.h"

/* Main function ************************************************************
// **************************************************************************
*/

void
get_mount_where [[gnu::noreturn]] (
	const char * & /*next_prog*/,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::definition * top_table[] = {
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "{what}");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}
	if (args.empty()) {
		die_missing_argument(prog, envs, "\"what\"");
	}
	const char * what(args.front());
	args.erase(args.begin());
	if (!args.empty()) {
		die_unexpected_argument(prog, args, envs);
	}
	if (1 > setfsent()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Unable to open fstab database.");
		throw static_cast<int>(EXIT_TEMPORARY_FAILURE);
	}
	struct fstab * entry(getfsspec(what));
	if (!entry)
		die_invalid(prog, envs, what, "No such mounted device in the fstab database.");
	const char * type(entry->fs_type);
	if ((0 == std::strcmp(type, "xx"))
	||  (0 == std::strcmp(type, "sw"))
	) {
		die_invalid(prog, envs, type, "This type of fstab database entry does not represent a mount point.");
	}
	const char * where(entry->fs_file);
	std::cout << where << '\n';
	endfsent();

	throw EXIT_SUCCESS;
}

void
get_mount_what [[gnu::noreturn]] (
	const char * & /*next_prog*/,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::definition * top_table[] = {
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "{where}");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}
	if (args.empty()) {
		die_missing_argument(prog, envs, "\"where\"");
	}
	const char * where(args.front());
	args.erase(args.begin());
	if (!args.empty()) {
		die_unexpected_argument(prog, args, envs);
	}
	if (1 > setfsent()) {
		std::fprintf(stderr, "%s: FATAL: %s\n", prog, "Unable to open fstab database.");
		throw static_cast<int>(EXIT_TEMPORARY_FAILURE);
	}
	struct fstab * entry(getfsfile(where));
	if (!entry)
		die_invalid(prog, envs, where, "No such mount point in the fstab database.");
	const char * type(entry->fs_type);
	if ((0 == std::strcmp(type, "xx"))
	||  (0 == std::strcmp(type, "sw"))
	) {
		die_invalid(prog, envs, type, "This type of fstab database entry does not represent a mount point.");
	}
	const char * what(entry->fs_spec);
	std::cout << what << '\n';
	endfsent();

	throw EXIT_SUCCESS;
}
