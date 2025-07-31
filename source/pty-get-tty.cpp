/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include "popt.h"
#include "utils.h"
#include "ProcessEnvironment.h"

/* Main function ************************************************************
// **************************************************************************
*/

enum { PTY_BACK_END_FILENO = 4 };

void
pty_get_tty (
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

	const int fd(posix_openpt(O_RDWR|O_NOCTTY
#if defined(__LINUX__) || defined(__linux__)
				|O_NDELAY
#endif
				));
	if (fd == -1) {
error_exit:
		const int error(errno);
		if (fd >= 0) close(fd);
		die_errno(prog, envs, error, "ptmx");
	}

	if (0 != grantpt(fd) || 0 != unlockpt(fd)) goto error_exit;

	const char * tty(ptsname(fd));
	if (!tty) goto error_exit;

	if (0 > dup2(fd, PTY_BACK_END_FILENO)) goto error_exit;
	if (PTY_BACK_END_FILENO != fd)
		close(fd);

	// Export the tty name to the environment so that later programs can get hold of it.
	envs.set("TTY", tty);
}
