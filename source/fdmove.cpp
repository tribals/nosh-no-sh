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

/* Main function ************************************************************
// **************************************************************************
*/

void
fdmove (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	bool copy(false);
	try {
		popt::bool_definition copy_option('c', "copy", "Copy rather than move the file descriptor.", copy);
		popt::definition * top_table[] = {
			&copy_option
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "{to} {from} {prog}");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}

	if (args.empty()) {
		die_missing_argument(prog, envs, "destination file descriptor number");
	}
	const char * to(args.front());
	args.erase(args.begin());
	if (args.empty()) {
		die_missing_argument(prog, envs, "source file descriptor number");
	}
	const char * from(args.front());
	args.erase(args.begin());
	next_prog = arg0_of(args);

	const char * old(nullptr);
	old = to;
	const unsigned long d(std::strtoul(to, const_cast<char **>(&to), 0));
	if (to == old || *to)
		die_invalid(prog, envs, old, "Not a number.");
	old = from;
	const unsigned long s(std::strtoul(from, const_cast<char **>(&from), 0));
	if (from == old || *from)
		die_invalid(prog, envs, old, "Not a number.");

	const int rc(dup2(s, d));
	if (0 > rc) {
		die_errno(prog, envs, "dup2");
	}
	if (!copy) close(s);
}
