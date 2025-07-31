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
console_resize (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	bool lines_only(false);
	bool c1_7bit(false);
	bool c1_8bit(false);
	try {
		popt::bool_definition lines_only_option('l', "lines", "Only change the number of lines.", lines_only);
		popt::bool_definition c1_7bit_option('7', "7bit", "Use 7-bit C1 characters.", c1_7bit);
		popt::bool_definition c1_8bit_option('8', "8bit", "Use 8-bit C1 characters instead of UTF-8.", c1_8bit);
		popt::definition * top_table[] = {
			&lines_only_option,
			&c1_7bit_option,
			&c1_8bit_option
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "[COLS×]{ROWS}");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}
	if (args.empty()) {
		die_missing_argument(prog, envs, "size");
	}
	const char * p(args.front());
	args.erase(args.begin());
	if (!args.empty()) die_unexpected_argument(prog, args, envs);
	// This must have static storage duration as we are using it in args.
	static std::string columns_arg;
	// We don't duplicate the numerical argument checking that console-control-sequence will do.
	// Rather, we just split the string at the × character.
	if (!lines_only) {
		const char *times(std::strrchr(p, static_cast<char>(0xD7)));
		if (!times) {
			times = std::strrchr(p, 'X');
			if (!times)
				times = std::strrchr(p, 'x');
		}
		if (!times) {
			die_invalid_argument(prog, envs, p, "Missing ×.");
		}
		columns_arg = std::string(p, times);
		p = ++times;
	}
	args.push_back("console-control-sequence");
	if (c1_7bit) args.push_back("--7bit");
	if (c1_8bit) args.push_back("--8bit");
	if (!lines_only) {
		args.push_back("--columns");
		args.push_back(columns_arg.c_str());
	}
	args.push_back("--rows");
	args.push_back(p);
	next_prog = arg0_of(args);
}
