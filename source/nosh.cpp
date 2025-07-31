/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cctype>
#include "utils.h"
#include "fdutils.h"

/* Main function ************************************************************
// **************************************************************************
*/

namespace {

#if defined(__LINUX__) || defined(__linux__)
#	define FD_PREFIX "/proc/self/fd/"
#else
#	define FD_PREFIX "/dev/fd/"
#endif

inline
void
undo_open_exec_fd_bodge(
	const char * name
) {
	const std::size_t l(std::strlen(name));
	if (l < sizeof FD_PREFIX) return;	// It must have at least 1 digit character after the prefix.
	if (0 != std::strncmp(name, FD_PREFIX, sizeof FD_PREFIX - 1)) return;
	name += sizeof FD_PREFIX - 1;
	int fd(0);
	while (const int c = *name) {
		if (!std::isdigit(c)) return;
		fd = fd * 10 + (c - '0');
		++name;
	}
	set_close_on_exec(fd, true);
}

}

void
nosh (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args.front()));
	args.erase(args.begin());

	if (args.empty()) die_missing_argument(prog, envs, "script name");
	const char * const name(args.front());
	args.erase(args.begin());
	if (!args.empty()) die_unexpected_argument(prog, args, envs);

	// These must have static storage duration as we are using them in args.
	static std::vector<std::string> args_storage;
	args_storage = read_file(prog, envs, name);
	const std::vector<const char *> new_args(convert_args_storage(args_storage));
	if (new_args.empty()) {
		die_usage(prog, envs, "No arguments in script.");
	}
	undo_open_exec_fd_bodge(name);
	args = new_args;
	next_prog = arg0_of(args);
}
