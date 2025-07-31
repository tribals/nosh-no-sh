/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include "popt.h"
#include "utils.h"
#include "ttyutils.h"

/* Option processing ********************************************************
// **************************************************************************
*/

namespace {
	bool
	is_command (const std::string & text)
	{
		return text == "command";
	}
	bool
	is_command_or_args (const std::string & text)
	{
		return text == "command" || text == "args";
	}
	bool
	is_pid (const char * text)
	{
		return 0 == std::strcmp(text, "pid");
	}

	enum {
		DEFL = 0x01,
		FULL = 0x02,
		JOBS = 0x04,
		LONG = 0x08,
		USER = 0x10,
	};
	struct column {
		const char * name;
		unsigned flags;
	} const columns[] =
	{
		{ "flags", 	               LONG		},
		{ "state", 	          JOBS|LONG|USER	},
		{ "user", 	          JOBS|     USER	},
		{ "uid", 	     FULL|     LONG		},
		{ "pid", 	DEFL|FULL|JOBS|LONG|USER	},
		{ "ppid", 	     FULL|JOBS|LONG		},
		{ "pgid", 	          JOBS			},
		{ "sid", 	          JOBS			},
		{ "jobc", 	          JOBS			},
		{ "pcpu", 	     FULL|     LONG|USER	},
		{ "pri", 	               LONG		},
		{ "nice", 	               LONG		},
		{ "address", 	               LONG		},
		{ "size", 	               LONG		},
		{ "mem", 	                    USER	},
		{ "vsz", 	               LONG|USER	},
		{ "rss", 	               LONG|USER	},
		{ "wchan", 	               LONG		},
		{ "start", 	     FULL|          USER	},
		{ "tty", 	DEFL|FULL|JOBS|LONG|USER	},
		{ "time", 	DEFL|FULL|JOBS|LONG|USER	},
		{ "args",	DEFL|FULL|JOBS|LONG|USER	},
	};

	std::list<std::string>
	split(
		const popt::string_list_definition::list_type & l
	) {
		std::list<std::string> r;
		for (popt::string_list_definition::list_type::const_iterator p(l.begin()), e(l.end()); p != e; ++p) {
			const char * text(p->c_str());
			for (;;) {
				if (const char * comma = std::strchr(text, ',')) {
					r.push_back(std::string(text, comma));
					text = comma + 1;
				} else
				{
					r.push_back(text);
					break;
				}
			}
		}
		return r;
	}
}

/* Main function ************************************************************
// **************************************************************************
*/

void
ps (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	bool all(false), tree(false), environment(false), wide(false), threads(false), non_unicode(false);
	popt::string_list_definition::list_type O_args, o_args, p_args;
	unsigned format_flags(DEFL);
	try {
		bool all_users(false), session_leaders(false), f(false), j(false), l(false), u(false);
		popt::bool_definition all_option('A', nullptr, "all processes", all);
		popt::bool_definition all_users_option('a', nullptr, "all users", all_users);
		popt::bool_definition session_leaders_option('x', nullptr, "session leaders", session_leaders);
		popt::bool_definition tree_option('d', "tree", "include tree field before any arguments vector field", tree);
		popt::bool_definition environment_option('e', "envs", "include environment vector after any arguments vector field", environment);
		popt::bool_definition non_unicode_option('7', "7-bit", "Do not use Unicode box drawing characters.", non_unicode);
		popt::bool_definition wide_option('w', "wide", "compatibility option, ignored", wide);
		popt::string_list_definition O_option('O', nullptr, "name(s)", "select field(s), augmenting any preselected format", O_args);
		popt::string_list_definition o_option('o', "output", "name(s)", "select field(s), replacing any preselected format", o_args);
		popt::string_list_definition p_option('p', "process-id", "IDs", "select only these process(es)", p_args);
		popt::bool_definition full_option('f', "full", "full format", f);
		popt::bool_definition jobs_option('j', "jobs", "jobs format", j);
		popt::bool_definition long_option('l', "long", "long format", l);
		popt::bool_definition user_option('u', "user", "user format", u);
		popt::bool_definition threads_option('H', nullptr, "show individual threads", threads);
		popt::definition * selection_table[] = {
			&all_option,
			&all_users_option,
			&session_leaders_option,
			&p_option,
			&threads_option,
		};
		popt::table_definition selection_table_option(sizeof selection_table/sizeof *selection_table, selection_table, "Selection options");
		popt::definition * format_table[] = {
			&full_option,
			&jobs_option,
			&long_option,
			&user_option,
			&O_option,
			&o_option,
			&tree_option,
			&environment_option,
			&non_unicode_option,
		};
		popt::table_definition format_table_option(sizeof format_table/sizeof *format_table, format_table, "Format options");
		popt::definition * top_table[] = {
			&format_table_option,
			&selection_table_option,
			&wide_option,
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;

		if (all_users ^ session_leaders) {
			die_usage(prog, envs, "-a and -x must be used together.");
		}
		if (all && (all_users || session_leaders))
			std::fprintf(stderr, "%s: WARNING: %s\n", prog, "-A implies -a -x already.");
		all |= (all_users && session_leaders);
		format_flags =
			(f ? FULL : 0U) |
			(j ? JOBS : 0U) |
			(l ? LONG : 0U) |
			(u ? USER : 0U) |
			DEFL ;
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}
	if (!args.empty()) die_unexpected_argument(prog, args, envs);

	const std::list<std::string> processes(split(p_args));
	if (all == !processes.empty())
		die_usage(prog, envs, "Either -p or -A is required, and not both.");

	std::list<std::string> fields;
	if (o_args.empty()) {
		fields = split(O_args);
		std::list<std::string>::const_iterator insertion_point(fields.begin());
		for (const column * p(columns), * const e(columns + sizeof columns/sizeof *columns); p < e; ++p) {
			if (p->flags & format_flags) {
				insertion_point = fields.insert(insertion_point, p->name);
				++insertion_point;
			}
			if (is_pid(p->name))
				insertion_point = fields.end();
		}
	} else
	{
		fields = split(o_args);
	}
	for (std::list<std::string>::const_iterator p(fields.begin()), e(fields.end()); p != e; ++p) {
		if (is_command_or_args(*p)) {
			if (tree) {
				p = fields.insert(p, "tree");
				++p;
			}
			if (environment) {
				++p;
				p = fields.insert(p, "envs");
			}
		}
	}

	// This must have static storage duration as we are using it in args.
	static std::vector<std::string> args_storage;

	args_storage.clear();
	for (std::list<std::string>::const_iterator p(fields.begin()), e(fields.end()); p != e; ++p) {
		args_storage.insert(args_storage.end(), "--field");
		if (is_command(*p))
			args_storage.insert(args_storage.end(), format_flags & FULL ? "args" : "command");
		else
			args_storage.insert(args_storage.end(), *p);
	}
	for (std::list<std::string>::const_iterator p(processes.begin()), e(processes.end()); p != e; ++p)
		args_storage.insert(args_storage.end(), *p);

	args = convert_args_storage(args_storage);
	if (threads)
		args.insert(args.begin(), "--threads");
	if (non_unicode) {
		args.insert(args.begin(), "ascii");
		args.insert(args.begin(), "--tui-level");
	}
	args.insert(args.begin(), "list-process-table");
	next_prog = arg0_of(args);
}
