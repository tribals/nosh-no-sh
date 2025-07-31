/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstddef>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.h"
#include "service-manager-client.h"
#include "popt.h"

/* System control subcommands ***********************************************
// **************************************************************************
*/

void
print_service_scripts (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::bool_definition user_option('u', "user", "Communicate with the per-user manager.", per_user_mode);
		popt::definition * main_table[] = {
			&user_option
		};
		popt::top_table_definition main_option(sizeof main_table/sizeof *main_table, main_table, "Main options", "{service(s)...}");

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
		die_missing_argument(prog, envs, "service name(s)");
	}

	for (std::vector<const char *>::const_iterator i(args.begin()); args.end() != i; ++i) {
		std::string path, name, suffix;
		const int bundle_dir_fd(open_bundle_directory(envs, "", *i, path, name, suffix));
		if (0 > bundle_dir_fd) {
			die_errno(prog, envs, *i);
		}
		const int service_dir_fd(open_service_dir(bundle_dir_fd));
		if (0 > service_dir_fd) {
			die_errno(prog, envs, path.c_str(), name.c_str(), "service");
		}
		const pid_t child(fork());
		if (0 > child) {
			die_errno(prog, envs, "fork");
		}
		if (0 == child) {
			fchdir(service_dir_fd);
			close(service_dir_fd);
			close(bundle_dir_fd);
			args.clear();
			args.push_back("grep");
			args.push_back("^");
			args.push_back("start");
			args.push_back("stop");
			args.push_back("run");
			if (0 <= access("service", F_OK))
				args.push_back("service");
			args.push_back("restart");
			next_prog = arg0_of(args);
			return;
		} else {
			int status, code;
			wait_blocking_for_exit_of(child, status, code);
		}
		close(service_dir_fd);
		close(bundle_dir_fd);
	}

	throw EXIT_SUCCESS;
}
