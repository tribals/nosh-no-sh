/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <unistd.h>
#include <cstddef>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <cerrno>
#include "utils.h"
#include "popt.h"

/* The kernel module shims **************************************************
// **************************************************************************
*/

void
load_kernel_module (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::top_table_definition main_option(0, nullptr, "Main options", "{module(s)...}");

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
		die_missing_argument(prog, envs, "module name(s)");
	}

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(__OpenBSD__)
	args.insert(args.begin(), "-n");
	args.insert(args.begin(), "kldload");
#elif defined(__NetBSD__)
	args.insert(args.begin(), "modload");
#elif defined(__LINUX__) || defined(__linux__)
	args.insert(args.begin(), "modprobe");
#else
#error "Don't know how to load kernel modules on your platform."
#endif
	next_prog = arg0_of(args);
}

void
unload_kernel_module (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::top_table_definition main_option(0, nullptr, "Main options", "{module(s)...}");

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
		die_missing_argument(prog, envs, "module name(s)");
	}

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(__OpenBSD__)
	args.insert(args.begin(), "kldunload");
#elif defined(__NetBSD__)
	args.insert(args.begin(), "modunload");
#elif defined(__LINUX__) || defined(__linux__)
	args.insert(args.begin(), "--remove");
	args.insert(args.begin(), "modprobe");
#else
#error "Don't know how to unload kernel modules on your platform."
#endif
	next_prog = arg0_of(args);
}
