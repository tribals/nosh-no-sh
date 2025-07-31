/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>
#include "popt.h"
#include "utils.h"
#include "ttyname.h"
#include "ProcessEnvironment.h"

/* terminal processing ******************************************************
// **************************************************************************
*/

namespace {

// This is the equivalent of grantpt() except with a target UID instead of the process's own.
inline
int
giveown (
	const char * tty,
	uid_t uid
) {
	int rc;
	rc = chmod(tty, 0600);
	if (0 != rc) return rc;
	rc = chown(tty, uid, -1);
	if (0 != rc) return rc;
	return rc;

}

}

/* Main function ************************************************************
// **************************************************************************
*/

void
login_giveown_controlling_terminal (
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

	if (args.empty()) {
		die_missing_argument(prog, envs, "TTY name");
	}

	const char * tty(get_controlling_tty_filename(envs));
	uid_t uid(-1);
	if (const char * text = envs.query("UID")) {
		const char * old(text);
		uid = std::strtoul(text, const_cast<char **>(&text), 0);
		if (text == old || *text)
			die_invalid(prog, envs, "UID", old, "Not a number.");
	} else 
		die_missing_environment_variable(prog, envs, "UID");

	if (0 != giveown(tty, uid)) {
		die_errno(prog, envs, tty);
	}
}
