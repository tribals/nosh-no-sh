/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <csignal>
#include <sys/types.h>
#include <sys/ioctl.h>
#if defined(__LINUX__) || defined(__linux__)
#include <sys/vt.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include "popt.h"
#include "utils.h"
#include "fdutils.h"
#include "FileDescriptorOwner.h"

/* Main function ************************************************************
// **************************************************************************
*/

static const char tty[] = "/dev/tty";

void
detach_controlling_tty (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::top_table_definition main_option(0, nullptr, "Main options", "{prog}");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}

	const FileDescriptorOwner fd(open_readwriteexisting_at(AT_FDCWD, tty));
	if (0 <= fd.get()) {
		if (!isatty(fd.get()))
			die_invalid(prog, envs, tty, "Not a terminal device.");
		if (0 > ioctl(fd.get(), TIOCNOTTY, 0))
			die_errno(prog, envs, "TIOCNOTTY", tty);
	}
}
