/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include "utils.h"
#include "ProcessEnvironment.h"
#include "popt.h"

/* Main function ************************************************************
// **************************************************************************
*/

static
char *
save (
	ProcessEnvironment & envs,
	const char * var
) {
	const char * val = envs.query(var);
	if (!val) return nullptr;
	const size_t len(std::strlen(val));
	char * sav(new char[len + 1]);
	if (sav) memcpy(sav, val, len + 1);
	return sav;
}

static
void
restore (
	ProcessEnvironment & envs,
	const char * var,
	char * & val
) {
	if (val) {
		envs.set(var, val);
		delete[] val; val = nullptr;
	}
}

void
clearenv (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	bool keep_path(false);
	bool keep_user(false);
	bool keep_shell(false);
	bool keep_term(false);
	bool keep_locale(false);
	bool keep_uidgid(false);
	try {
		popt::bool_definition keep_path_option('p', "keep-path", "Keep the PATH and LD_LIBRARY_PATH environment variables.", keep_path);
		popt::bool_definition keep_user_option('h', "keep-user", "Keep the HOME, USER, and LOGNAME environment variables.", keep_user);
		popt::bool_definition keep_shell_option('s', "keep-shell", "Keep the SHELL environment variable.", keep_shell);
		popt::bool_definition keep_term_option('t', "keep-term", "Keep the TERM and TTY environment variables.", keep_term);
		popt::bool_definition keep_locale_option('l', "keep-locale", "Keep the LANG and LC_... environment variables.", keep_locale);
		popt::bool_definition keep_uidgid_option('l', "keep-uidgid", "Keep the UID, GID, and GIDLIST environment variables.", keep_uidgid);
		popt::definition * top_table[] = {
			&keep_path_option,
			&keep_user_option,
			&keep_shell_option,
			&keep_term_option,
			&keep_locale_option,
			&keep_uidgid_option
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

	char * path(nullptr);
	char * ld_library_path(nullptr);
	char * shell(nullptr);
	char * shlvl(nullptr);
	char * env(nullptr);
	char * home(nullptr);
	char * user(nullptr);
	char * logname(nullptr);
	char * term(nullptr);
	char * termpath(nullptr);
	char * terminfo(nullptr);
	char * terminfopath(nullptr);
	char * colorterm(nullptr);
	char * xterm_version(nullptr);
	char * vte_version(nullptr);
	char * tty(nullptr);
	char * lang(nullptr);
	char * lc_all(nullptr);
	char * lc_collate(nullptr);
	char * lc_ctype(nullptr);
	char * lc_messages(nullptr);
	char * lc_monetary(nullptr);
	char * lc_numeric(nullptr);
	char * lc_time(nullptr);
	char * uid(nullptr);
	char * gid(nullptr);
	char * gidlist(nullptr);

	if (keep_path) {
		path = save(envs, "PATH");
		ld_library_path = save(envs, "LD_LIBRARY_PATH");
	}
	if (keep_user) {
		home = save(envs, "HOME");
		user = save(envs, "USER");
		logname = save(envs, "LOGNAME");
	}
	if (keep_shell) {
		shell = save(envs, "SHELL");
		shlvl = save(envs, "SHLVL");
		env = save(envs, "ENV");
	}
	if (keep_term) {
		term = save(envs, "TERM");
		termpath = save(envs, "TERMPATH");
		terminfo = save(envs, "TERMINFO");
		terminfopath = save(envs, "TERMINFO_DIRS");
		colorterm = save(envs, "COLORTERM");
		xterm_version = save(envs, "XTERM_VERSION");
		vte_version = save(envs, "VTE_VERSION");
		tty = save(envs, "TTY");
	}
	if (keep_locale) {
		lang = save(envs, "LANG");
		lc_all = save(envs, "LC_ALL");
		lc_collate = save(envs, "LC_COLLATE");
		lc_ctype = save(envs, "LC_CTYPE");
		lc_messages = save(envs, "LC_MESSAGES");
		lc_monetary = save(envs, "LC_MONETARY");
		lc_numeric = save(envs, "LC_NUMERIC");
		lc_time = save(envs, "LC_TIME");
	}
	if (keep_uidgid) {
		uid = save(envs, "UID");
		gid = save(envs, "GID");
		gidlist = save(envs, "GIDLIST");
	}

	if (!envs.clear()) {
		die_errno(prog, envs, "clearenv");
	}

	restore(envs, "PATH", path);
	restore(envs, "LD_LIBRARY_PATH", ld_library_path);
	restore(envs, "HOME", home);
	restore(envs, "USER", user);
	restore(envs, "LOGNAME", logname);
	restore(envs, "SHELL", shell);
	restore(envs, "SHLVL", shlvl);
	restore(envs, "ENV", env);
	restore(envs, "TERM", term);
	restore(envs, "TERMPATH", termpath);
	restore(envs, "TERMINFO", terminfo);
	restore(envs, "TERMINFO_DIRS", terminfopath);
	restore(envs, "COLORTERM", colorterm);
	restore(envs, "XTERM_VERSION", xterm_version);
	restore(envs, "VTE_VERSION", vte_version);
	restore(envs, "TTY", tty);
	restore(envs, "LANG", lang);
	restore(envs, "LC_ALL", lc_all);
	restore(envs, "LC_COLLATE", lc_collate);
	restore(envs, "LC_CTYPE", lc_ctype);
	restore(envs, "LC_MESSAGES", lc_messages);
	restore(envs, "LC_MONETARY", lc_monetary);
	restore(envs, "LC_NUMERIC", lc_numeric);
	restore(envs, "LC_TIME", lc_time);
	restore(envs, "UID", uid);
	restore(envs, "GID", gid);
	restore(envs, "GIDLIST", gidlist);
}
