/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstdlib>
#include <unistd.h>
#include "utils.h"
#include "builtins.h"
#include "ProcessEnvironment.h"
#include "popt.h"

/* List built-in commands ***************************************************
// **************************************************************************
*/

void
builtins [[gnu::noreturn]] (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::definition * top_table[] = {
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}

	if (!args.empty()) die_unexpected_argument(prog, args, envs);

	for ( const command * c(commands); c != commands + num_commands; ++c ) {
		args.clear();
		args.push_back(c->name);
		args.push_back("--usage");
		next_prog = arg0_of(args);
		try {
			c->func(next_prog, args, envs);
		} catch (int) {
		}
	}

	throw EXIT_SUCCESS;
}
