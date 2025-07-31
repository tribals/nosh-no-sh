/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include "utils.h"
#include "fdutils.h"
#include "popt.h"
#include "FileStar.h"
#include "control_groups.h"

/* Main function ************************************************************
// **************************************************************************
*/

void
delegate_control_group_to (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::top_table_definition main_option(0, nullptr, "Main options", "{account}");

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
		die_missing_argument(prog, envs, "account name");
	}
	const char * owner(args.front());
	args.erase(args.begin());
	if (!args.empty()) die_unexpected_argument(prog, args, envs);

	FileStar self_cgroup(open_my_control_group_info("/proc/self/cgroup"));
	if (!self_cgroup) {
		if (ENOENT == errno) throw EXIT_SUCCESS;	// This is what we'll see on a BSD.
		die_errno(prog, envs, "/proc/self/cgroup");
	}

	std::string prefix("/sys/fs/cgroup"), current;
	if (!read_my_control_group(self_cgroup, "", current)) {
		if (!read_my_control_group(self_cgroup, "name=systemd", current))
			return;
		prefix += "/systemd";
	}
	current = prefix + current;
	// These must have static storage duration as we are using them in args.
	static std::string dir_arg_buffer, procs_arg_buffer, subtree_control_arg_buffer;
	dir_arg_buffer = current + "/";
	procs_arg_buffer = current + "/cgroup.procs";
	subtree_control_arg_buffer = current + "/cgroup.subtree_control";

	args.clear();
	args.push_back("chown");
	args.push_back("--");
	args.push_back(owner);
	args.push_back(dir_arg_buffer.c_str());
	args.push_back(procs_arg_buffer.c_str());
	args.push_back(subtree_control_arg_buffer.c_str());
	next_prog = arg0_of(args);
}
