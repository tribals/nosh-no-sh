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
#include <pwd.h>
#include "utils.h"
#include "ProcessEnvironment.h"
#include "UserEnvironmentSetter.h"
#include "runtime-dir.h"
#include "popt.h"

/* Main function ************************************************************
// **************************************************************************
*/

void
userenv_fromenv (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	UserEnvironmentSetter setter(envs);
	try {
		bool no_set_user(false), no_set_shell(false);
		popt::bool_definition set_dbus_option('d', "set-dbus", "Set the DBUS_SESSION_BUS_ADDRESS environment variable.", setter.set_dbus);
		popt::bool_definition set_xdg_option('x', "set-xdg", "Set the XDG_RUNTIME_DIR environment variable.", setter.set_xdg);
		popt::bool_definition set_other_option('o', "set-other", "Set other environment variables.", setter.set_other);
		popt::bool_definition toolkit_pager_option('\0', "toolkit-pager", "Use the toolkit's own pager.", setter.toolkit_pager);
		popt::definition * explicit_sets_table[] = {
			&set_other_option,
			&set_dbus_option,
			&set_xdg_option,
			&toolkit_pager_option,
		};
		popt::table_definition explicit_sets_option(sizeof explicit_sets_table/sizeof *explicit_sets_table, explicit_sets_table, "Explicitly setting things");
		popt::bool_definition default_locale_option('l', "default-locale", "Default the LANG and MM_CHARSET environment variables.", setter.default_locale);
		popt::bool_definition default_path_option('p', "default-path", "Default the PATH and MANPATH environment variables.", setter.default_path);
		popt::bool_definition default_term_option('t', "default-term", "Default the TERM environment variable.", setter.default_term);
		popt::bool_definition default_timezone_option('z', "default-timezone", "Default the TZ environment variable.", setter.default_timezone);
		popt::bool_definition default_tools_option('e', "default-tools", "Default the EDITOR, VISUAL, and PAGER environment variables.", setter.default_tools);
		popt::definition * default_fallbacks_table[] = {
			&default_path_option,
			&default_tools_option,
			&default_term_option,
			&default_timezone_option,
			&default_locale_option,
		};
		popt::table_definition default_fallbacks_option(sizeof default_fallbacks_table/sizeof *default_fallbacks_table, default_fallbacks_table, "Providing fallback defaults when not otherwise set");
		popt::bool_definition no_set_user_option('\0', "no-set-user", "Do not set the HOME, USER, and LOGNAME environment variables.", no_set_user);
		popt::bool_definition no_set_shell_option('\0', "no-set-shell", "Do not set the SHELL environment variable.", no_set_shell);
		popt::definition * inhibitions_table[] = {
			&no_set_user_option,
			&no_set_shell_option,
		};
		popt::table_definition inhibitions_option(sizeof inhibitions_table/sizeof *inhibitions_table, inhibitions_table, "Inhibitor options for normal behaviour");
		popt::definition * top_table[] = {
			&inhibitions_option,
			&default_fallbacks_option,
			&explicit_sets_option,
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "{prog}");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
		setter.set_user = !no_set_user;
		setter.set_shell = !no_set_shell;
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}

	uid_t uid(-1);
	if (const char * text = envs.query("UID")) {
		const char * old(text);
		uid = std::strtoul(text, const_cast<char **>(&text), 0);
		if (text == old || *text)
			die_invalid(prog, envs, old, "Not a number.");
	} else
		die_missing_environment_variable(prog, envs, "UID");
	const passwd * pw(nullptr);
	if (const char * logname = envs.query("LOGNAME"))
		pw = getpwnam(logname);
	if (!pw || pw->pw_uid != uid) pw = getpwuid(uid);
	setter.apply(pw);
	endpwent();
}
