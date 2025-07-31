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
#include "utils.h"
#include "popt.h"

/* Main function ************************************************************
// **************************************************************************
*/

void
console_clear (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	bool c1_7bit(false);
	bool c1_8bit(false);
	try {
		popt::bool_definition c1_7bit_option('7', "7bit", "Use 7-bit C1 characters.", c1_7bit);
		popt::bool_definition c1_8bit_option('8', "8bit", "Use 8-bit C1 characters instead of UTF-8.", c1_8bit);
		popt::definition * top_table[] = {
			&c1_7bit_option,
			&c1_8bit_option
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}
	if (!args.empty()) die_unexpected_argument(prog, args, envs);
	// This order of all before scrollback is important for PuTTY for some reason.
	args.push_back("foreground");
	args.push_back("console-control-sequence");
	if (c1_7bit) args.push_back("--7bit");
	if (c1_8bit) args.push_back("--8bit");
	args.push_back("--clear");
	args.push_back("all");
	args.push_back(";");
	args.push_back("console-control-sequence");
	if (c1_7bit) args.push_back("--7bit");
	if (c1_8bit) args.push_back("--8bit");
	args.push_back("--clear");
	args.push_back("scrollback");
	next_prog = arg0_of(args);
}
