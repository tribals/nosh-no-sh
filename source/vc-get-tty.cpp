/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#if !defined(_GNU_SOURCE)
#include <sys/syslimits.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>
#include <grp.h>
#include "popt.h"
#include "utils.h"
#include "ProcessEnvironment.h"

/* terminal processing ******************************************************
// **************************************************************************
*/

namespace {

// This program only targets virtual terminals provided by system console drivers.
// So we just hardwire what we KNOW to be the TERM value appropriate to each system.
inline
const char *
default_term(
	bool user_space_virtual_terminal
) {
#if defined(__LINUX__) || defined(__linux__)
	// As of 2025, there is a terminfo definition for "linux-16color" but none for "linux-256color".
	// There are termcap definitions for "linux-16color" and "linux-256color" supplied in the external configuration import subsystem.
	static_cast<void>(user_space_virtual_terminal);	// Silences a compiler warning.
	return "linux-16color";
#elif defined(__FreeBSD__) || defined(__DragonFly__)
	// As of 2010 the FreeBSD kernel virtual terminal emulator itself is "teken", and this type is in terminfo as of 2017.
	// As of 2025, there is a terminfo definition for "teken-16color" but none for "teken-256color".
	// There are termcap definitions for "teken"/"teken-8color", "teken-16color" and "teken-256color" supplied in the external configuration import subsystem.
	return user_space_virtual_terminal ? "teken-16color" : "teken";
#elif defined(__NetBSD__)
	// This is the case for kernel virtual terminal emulators configured as "vt100".
	// The old "pcvtXX" type sort of works as well, but is officially wrong and there are corner case mis-matches.
	// The "wsvt25" type in terminfo was superseded in 2014 with NetBSD 6.0.
	static_cast<void>(user_space_virtual_terminal);	// Silences a compiler warning.
	return "netbsd6";
#elif defined(__OpenBSD__)
	// This is the case for kernel virtual terminal emulators configured as "vt220".
	static_cast<void>(user_space_virtual_terminal);	// Silences a compiler warning.
	return "pccon";
#else
	// MacOS X doesn't even have kernel virtual terminal emulators.
#	error "Don't know what the terminal type for your kernel virtual terminal emulator is."
#endif
}

// This program only targets virtual terminals provided by system console drivers.
// So we just hardwire what we KNOW to be the COLORTERM value appropriate to each system.
inline
const char *
default_colorterm()
{
#if defined(__LINUX__) || defined(__linux__)
	return "linux-256color-truecolor";
#elif defined(__FreeBSD__) || defined(__DragonFly__)
	return "teken-256color-truecolor";
#elif defined(__NetBSD__)
	// This is the case for kernel virtual terminal emulators configured as "vt100".
	return "netbsd6-256color-truecolor";
#elif defined(__OpenBSD__)
	// This is the case for kernel virtual terminal emulators configured as "vt220".
	return "pccon-256color-truecolor";
#else
	// MacOS X doesn't even have kernel virtual terminal emulators.
#	error "Don't know what the terminal type for your kernel virtual terminal emulator is."
#endif
}

// Get the UID to which the TTY should be reset.
inline uid_t get_tty_uid() { return geteuid(); }

// Get the GID to which the TTY should be reset.
inline
gid_t
get_tty_gid()
{
	// If there's a group named "tty", assume the sysop wants all TTYs to belong to it at TTY reset.
	if (struct group * g = getgrnam("tty"))
		return g->gr_gid;
	// Or this program could be set-GID "tty".
	return getegid();
}

// This is the equivalent of grantpt() for virtual consoles.
inline
int
grantvc (
	const char * tty
) {
	int rc;
	rc = chmod(tty, 0600);
	if (0 != rc) return rc;
	rc = chown(tty, get_tty_uid(), get_tty_gid());
	if (0 != rc) return rc;
	return rc;

}

// This is the equivalent of unlockpt() for virtual consoles.
inline
int
unlockvc (
	const char * /*tty*/
) {
	return 0;
}

}

/* Main function ************************************************************
// **************************************************************************
*/

void
vc_get_tty (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	bool user_space_virtual_terminal(false);
	const char * term(nullptr);

	const char * prog(basename_of(args[0]));
	try {
		popt::string_definition term_option('\0', "term", "type", "Specify an alternative TERM environment variable value.", term);
		popt::definition * top_table[] = {
			&term_option
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "{virtual-console-id} {prog}");

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
	const char * tty_base(args.front());
	args.erase(args.begin());
	next_prog = arg0_of(args);

	std::string tty_str;
	const char * tty(tty_base);
	if ('/' != tty[0]) {
		// Attempt to expand into either /dev/XXX, /var/dev/XXX, or /run/dev/XXX .
		struct stat buf;
		tty_str = "/dev/";
		tty_str += tty_base;
		if (0 == stat(tty_str.c_str(), &buf))
			tty = tty_str.c_str();
		else {
			tty_str = "/var/dev/";
			tty_str += tty_base;
			if (0 == stat(tty_str.c_str(), &buf)) {
				user_space_virtual_terminal = true;
				tty = tty_str.c_str();
			} else {
				tty_str = "/run/dev/";
				tty_str += tty_base;
				if (0 == stat(tty_str.c_str(), &buf)) {
					user_space_virtual_terminal = true;
					tty = tty_str.c_str();
				}
			}
		}
	}

	if (!term) term = default_term(user_space_virtual_terminal);

	if (0 != grantvc(tty) || 0 != unlockvc(tty)) {
		die_errno(prog, envs, tty);
	}

	// Export the tty name to the environment so that later programs can get hold of it.
	envs.set("TTY", tty);

	envs.set("TERM", term);
	if (user_space_virtual_terminal)
		envs.set("COLORTERM", default_colorterm());
}
