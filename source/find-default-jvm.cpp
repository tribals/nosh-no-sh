/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include "utils.h"
#include "fdutils.h"
#include "ProcessEnvironment.h"
#include "FileStar.h"
#include "DirStar.h"
#include "popt.h"

/* Main function ************************************************************
// **************************************************************************
*/

namespace {
const char * path[] = { "/usr/local", "/usr/pkg", "/usr", "/" };
const char * etc[] = { "/usr/local/etc", "/etc", "/usr/pkg/etc" };
const char * lib[] = { "/usr/local/lib", "/usr/pkg/lib", "/usr/lib", "/lib" };
const std::string javavms("/javavms");
const std::string jvm("/jvm");
}

void
find_default_jvm (
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

	if (args.empty()) die_missing_next_program(prog, envs);
	if (envs.query("JAVA_HOME")) return;

	for (const char * const * i(etc); i < etc + sizeof etc/sizeof *etc; ++i) {
		const std::string n((*i) + javavms);
		const FileStar f(std::fopen(n.c_str(), "r"));
		if (f) {
			const std::vector<std::string> java_strings(read_file(prog, envs, n.c_str(), f));
			if (!java_strings.empty()) {
				const std::string root(dirname_of(dirname_of(java_strings.front())));
				if (!envs.set("JAVA_HOME", root.c_str())) {
					die_errno(prog, envs, "JAVA_HOME");
				}
				return;
			}
		}
	}

	for (const char * const * i(lib); i < lib + sizeof lib/sizeof *lib; ++i) {
		const std::string n((*i) + jvm);
		FileDescriptorOwner d(open_dir_at(AT_FDCWD, n.c_str()));
		if (0 <= d.get()) {
			if (0 <= faccessat(d.get(), "default-java/", F_OK, AT_EACCESS)) {
				if (!envs.set("JAVA_HOME", (n + "/default-java").c_str())) {
					die_errno(prog, envs, "JAVA_HOME");
				}
				return;
			}
			if (0 <= faccessat(d.get(), "default-runtime/", F_OK, AT_EACCESS)) {
				if (!envs.set("JAVA_HOME", (n + "/default-runtime").c_str())) {
					die_errno(prog, envs, "JAVA_HOME");
				}
				return;
			}
		}
	}

	for (const char * const * i(path); i < path + sizeof path/sizeof *path; ++i) {
		if (0 > faccessat(AT_FDCWD, (std::string(*i) + "/bin/java").c_str(), F_OK, AT_EACCESS)) continue;
		if (!envs.set("JAVA_HOME", *i)) {
			die_errno(prog, envs, "JAVA_HOME");
		}
		return;
	}

	if (!envs.set("JAVA_HOME", "/")) {
		die_errno(prog, envs, "JAVA_HOME");
	}
}
