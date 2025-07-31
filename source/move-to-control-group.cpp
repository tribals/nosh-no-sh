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
#include "FileDescriptorOwner.h"
#include "FileStar.h"
#include "control_groups.h"

/* Main function ************************************************************
// **************************************************************************
*/

void
move_to_control_group (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::top_table_definition main_option(0, nullptr, "Main options", "{cgroup} {prog}");

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
		die_missing_argument(prog, envs, "control group name");
	}
	const char * group(args.front());
	args.erase(args.begin());
	next_prog = arg0_of(args);

	FileStar self_cgroup(open_my_control_group_info("/proc/self/cgroup"));
	if (!self_cgroup) {
		if (ENOENT == errno) return;	// This is what we'll see on a BSD.
		die_errno(prog, envs, "/proc/self/cgroup");
	}

	std::string prefix("/sys/fs/cgroup"), current;
	if (!read_my_control_group(self_cgroup, "", current)) {
		if (!read_my_control_group(self_cgroup, "name=systemd", current))
			return;
		prefix += "/systemd";
	}

	if ('/' != *group) {
		current += "/";
		current += group;
	} else {
		current = group;
	}

	current = prefix + current;

	if (0 > mkdirat(AT_FDCWD, current.c_str(), 0755)) {
		if (EEXIST != errno)
			die_errno(prog, envs, current.c_str());
	}

	const FileDescriptorOwner cgroup_procs_fd(open_appendexisting_at(AT_FDCWD, (current + "/cgroup.procs").c_str()));
	if (0 > cgroup_procs_fd.get()) {
procs_file_error:
		die_errno(prog, envs, current.c_str(), "/cgroup.procs");
	}
	if (0 > write(cgroup_procs_fd.get(), "0\n", 2)) goto procs_file_error;
}
