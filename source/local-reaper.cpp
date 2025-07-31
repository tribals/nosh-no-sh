/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#if defined(__FreeBSD__) || defined(__DragonFly__)
#include <sys/procctl.h>
#endif
#include <unistd.h>
#include "utils.h"
#include "popt.h"
#include "FileStar.h"

/* Main function ************************************************************
// **************************************************************************
*/

void
local_reaper (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::definition * top_table[] = {
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "{enable} {prog}");

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
		die_missing_argument(prog, envs, "protection level");
	}
	const char * arg(args.front());
	args.erase(args.begin());

	const std::string r(tolower(arg));
	bool on;
	if (is_bool_true(r))
		on = true;
	else
	if (is_bool_false(r))
		on = false;
	else
		die_invalid(prog, envs, arg, "Bad enable setting");

	if (0 > subreaper(on))
		die_errno(prog, envs, "procctl");

	next_prog = arg0_of(args);
}
