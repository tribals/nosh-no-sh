/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <cerrno>
#include <unistd.h>
#include "popt.h"
#include "utils.h"
#include "ProcessEnvironment.h"
#include "DefaultEnvironment.h"

/* Main function ************************************************************
// **************************************************************************
*/

void
login_shell (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::definition * top_table[] = {
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

	if (!args.empty()) die_unexpected_argument(prog, args, envs);

	const char * shell = envs.query("SHELL");
	if (!shell || !*shell) shell = DefaultEnvironment::UserLogin::SHELL;
	static std::string arg_storage;
	arg_storage.clear();
	arg_storage.push_back('-');
	arg_storage += shell;

	args.push_back(arg_storage.c_str());
	next_prog = shell;
}
