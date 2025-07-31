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
#include "ProcessEnvironment.h"

/* Main function ************************************************************
// **************************************************************************
*/

void
open_controlling_tty (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
#if defined(_GNU_SOURCE)
	bool vhangup(false);
#else
	bool revoke(false);
#endif
	bool exclusive(false);

	const char * prog(basename_of(args[0]));
	try {
#if defined(_GNU_SOURCE)
		popt::bool_definition vhangup_option('\0', "vhangup", "Execute the vhangup call.", vhangup);
#else
		popt::bool_definition revoke_option('\0', "revoke", "Execute the revoke call.", revoke);
#endif
		popt::bool_definition exclusive_option('\0', "exclusive", "Attempt to set exclusive mode.", exclusive);
		popt::definition * top_table[] = {
#if defined(_GNU_SOURCE)
			&vhangup_option,
#else
			&revoke_option,
#endif
			&exclusive_option
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

	const char * tty(envs.query("TTY"));
	if (!tty) die_missing_environment_variable(prog, envs, "TTY");

#if !defined(_GNU_SOURCE)
	if (revoke) {
		if (0 > ::revoke(tty)) {
			die_errno(prog, envs, "revoke", tty);
		}
	}
#endif

	// The System V way of acquiring the controlling TTY is just to open it.
	// This is so full of ifs and buts on Linux that it is ridiculous; and it doesn't forcibly remove control.
	// So we just use the BSD mechanism, which works on both the BSDs and Linux.
	int fd(open_readwriteexisting_at(AT_FDCWD, tty));
	if (fd < 0) {
#if defined(_GNU_SOURCE)
error_exit:
#endif
		message_fatal_errno(prog, envs, tty);
close_exit:
		if (fd >= 0) close(fd);
		throw EXIT_FAILURE;
	}
	if (!isatty(fd)) {
		std::fprintf(stderr, "%s: FATAL: %s: %s\n", prog, tty, "Not a terminal device.");
		goto close_exit;
	}
	if (0 > set_close_on_exec(fd, false)) {
		message_fatal_errno(prog, envs, "FD_CLOEXEC", tty);
		goto close_exit;
	}
	if (0 > set_non_blocking(fd, false)) {
		message_fatal_errno(prog, envs, "O_NONBLOCK", tty);
		goto close_exit;
	}

	if (0 > ioctl(fd, exclusive ? TIOCEXCL : TIOCNXCL, 1)) {
		const int error(errno);
		std::fprintf(stderr, "%s: WARNING: %s: %s: %s\n", prog, exclusive ? "TIOEXCL" : "TIOCNXCL", tty, std::strerror(error));
	}

	// This is the BSD way of acquiring the controlling TTY.
	// On Linux, the 1 flag causes the forcible removal of this controlling TTY from existing processes, if we have the privileges.
	if (0 > ioctl(fd, TIOCSCTTY, 1)) {
		const int error(errno);
		std::fprintf(stderr, "%s: WARNING: %s: %s: %s\n", prog, "TIOCSCTTY", tty, std::strerror(error));
	}

#if defined(_GNU_SOURCE)
	if (vhangup) {
		// We are be about to do vhangup() with an open fd to the TTY.
		// We don't want to be terminated by the hangup signal ourselves.
		struct sigaction sa;
		sa.sa_handler=SIG_IGN;
		sa.sa_flags=0;
		sigemptyset(&sa.sa_mask);
		sigaction(SIGHUP,&sa,nullptr);

		// Unfortunately, the TTY has to actually ALREADY be OUR controlling TTY for this to work.
		if (::vhangup()) goto error_exit;

		sa.sa_handler=SIG_DFL;
		sa.sa_flags=0;
		sigemptyset(&sa.sa_mask);
		sigaction(SIGHUP,&sa,nullptr);

		// Because we've just hung up, we have to re-open.
		// This causes a race condition against any other privileged process also attempting to open this TTY.
		close(fd);
		fd = open(tty, O_RDWR, 0);
		if (fd < 0) goto error_exit;
	}
#endif

	// Now just dup the file descriptors.
	if (0 > dup2(fd, STDIN_FILENO)) {
		message_fatal_errno(prog, envs, "stdin");
		goto close_exit;
	}
	if (0 > dup2(fd, STDOUT_FILENO)) {
		message_fatal_errno(prog, envs, "stdout");
		goto close_exit;
	}
	if (0 > dup2(fd, STDERR_FILENO)) {
		message_fatal_errno(prog, envs, "stderr");
		goto close_exit;
	}

	if ((STDIN_FILENO != fd) && (STDOUT_FILENO != fd) && (STDERR_FILENO != fd))
		close(fd);
	fd = -1;
}
