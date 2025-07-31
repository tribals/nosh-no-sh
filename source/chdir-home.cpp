/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "utils.h"
#include "popt.h"
#include "ProcessEnvironment.h"

/* Main function ************************************************************
// **************************************************************************
*/

void
chdir_home (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	bool oknodir(false);
	try {
		popt::bool_definition oknodir_option('\0', "oknodir", "Do not fail if the directory cannot be changed to.", oknodir);
		popt::definition * top_table[] = {
			&oknodir_option
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "{prog}");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}

	if (args.empty()) die_missing_next_program(prog, envs);
	const char * dir(envs.query("HOME"));
	if (!dir) {
		die_invalid_argument(prog, envs, args.front(), "No HOME environment variable.");
	}

	if (0 > chdir(dir)) {
		if (!oknodir || 0 > chdir(dir = "/")) {
			die_errno(prog, envs, dir);
		}
	}
	if ('/' == dir[0]) envs.set("PWD", dir); else envs.unset("PWD");
}
