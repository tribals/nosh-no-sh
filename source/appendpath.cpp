/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include "utils.h"
#include "ProcessEnvironment.h"
#include "popt.h"

/* Main function ************************************************************
// **************************************************************************
*/

void
appendpath (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::top_table_definition main_option(0, nullptr, "Main options", "{var} {dir} {prog}");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}

	if (args.empty()) die_missing_variable_name(prog, envs);
	const char * var(args.front());
	args.erase(args.begin());
	if (args.empty()) die_missing_argument(prog, envs, "variable value");
	const char * dir(args.front());
	args.erase(args.begin());
	next_prog = arg0_of(args);
	if (const char * val = envs.query(var)) {
		std::string s(val);
		s += ':';
		s += dir;
		if (!envs.set(var, s)) {
			die_errno(prog, envs, var);
		}
	} else {
		if (!envs.set(var, dir)) {
			die_errno(prog, envs, var);
		}
	}
}
