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
#include "popt.h"
#include "utils.h"
#include "ttyutils.h"

namespace {

	struct simple_rewrite_definition : public popt::simple_named_definition {
	public:
		simple_rewrite_definition(char s, const char * l, const char * d, std::vector<const char *> & n, const char * w0, const char * fld[], std::size_t len) : simple_named_definition(s, l, d), next_args(n), r0(w0), fieldnames(fld), fields(len) {}
		virtual ~simple_rewrite_definition();
	protected:
		std::vector<const char *> & next_args;
		const char * const r0;
		const char * const * fieldnames;
		std::size_t fields;
		virtual void action(popt::processor &);
	};

	const char
	* b_fields[] = {
		"pid", "comm", "osrel", "emul", "command"
	},
	* c_fields[] = {
		"pid", "comm", "args"
	},
	* e_fields[] = {
		"pid", "comm", "envs"
	},
#if false // This does not really work.  JdeBP 2023-04-19
	* f_fields[] = {
		"pid", "comm", "fd", "t", "v", "fdflags", "ref", "offset", "pro", "name"
	},
#endif
	* i_fields[] = {
		"pid", "comm", "siglist", "sigmask", "sigignore", "sigcatch"
	},
	* k_fields[] = {
		"pid", "tid", "comm", "tdname", "kstack"
	},
#if false // This does not really work.  JdeBP 2023-04-19
	* l_fields[] = {
		"pid", "comm", "rlimit", "soft", "hard"
	},
	* r_fields[] = {
		"pid", "tid", "comm", "resource", "value"
	},
#endif
	* s_fields[] = {
		"pid", "comm", "euid", "ruid", "svuid", "egid", "rgid", "svgid", "umask", "crflags", "groups"
	},
	* S_fields[] = {
		"pid", "tid", "comm", "tdname", "csid", "rcsid", "oncpu", "lastcpu", "csmask"
	},
	* t_fields[] = {
		"pid", "tid", "comm", "tdname", "oncpu", "lastcpu", "pri", "state", "mwchan", "wchan"
	}
#if false // This does not really work.  JdeBP 2023-04-19
	, * x_fields[] = {
		"pid", "comm", "auxv", "value"
	}
#endif
	;

}

simple_rewrite_definition::~simple_rewrite_definition() {}
void simple_rewrite_definition::action(popt::processor & /*proc*/)
{
	for (std::size_t i(0U); i < fields; ++i) {
		next_args.insert(next_args.end(), r0);
		next_args.insert(next_args.end(), fieldnames[i]);
	}
	if (!fields)
		next_args.insert(next_args.end(), r0);
}

/* Main function ************************************************************
// **************************************************************************
*/

void
procstat (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	std::vector<const char *> next_args;
	bool all(false);
	try {
		popt::bool_definition all_option('a', nullptr, "all processes", all);
		simple_rewrite_definition b_option('b', nullptr, "binary format", next_args, "--field", b_fields, sizeof b_fields/sizeof *b_fields);
		simple_rewrite_definition c_option('c', nullptr, "command and arguments format", next_args,"--field",  c_fields, sizeof c_fields/sizeof *c_fields);
		simple_rewrite_definition e_option('e', nullptr, "environment format", next_args, "--field", e_fields, sizeof e_fields/sizeof *e_fields);
#if false // This does not really work.  JdeBP 2023-04-19
		simple_rewrite_definition f_option('f', nullptr, "file descriptor format", next_args, "--field", f_fields, sizeof f_fields/sizeof *f_fields);
#endif
		simple_rewrite_definition i_option('i', nullptr, "signal disposition format", next_args, "--field", i_fields, sizeof i_fields/sizeof *i_fields);
		simple_rewrite_definition k_option('k', nullptr, "kernel stack format", next_args, "--field", k_fields, sizeof k_fields/sizeof *k_fields);
#if false // This does not really work.  JdeBP 2023-04-19
		simple_rewrite_definition l_option('l', nullptr, "resource limits format", next_args, "--field", l_fields, sizeof l_fields/sizeof *l_fields);
		simple_rewrite_definition r_option('r', nullptr, "resource usage format", next_args, "--field", r_fields, sizeof r_fields/sizeof *r_fields);
#endif
		simple_rewrite_definition s_option('s', nullptr, "security credential format", next_args, "--field", s_fields, sizeof s_fields/sizeof *s_fields);
		simple_rewrite_definition S_option('S', nullptr, "cpuset format", next_args, "--field", S_fields, sizeof S_fields/sizeof *S_fields);
		simple_rewrite_definition t_option('t', nullptr, "thread format", next_args, "--field", t_fields, sizeof t_fields/sizeof *t_fields);
#if false // This does not really work.  JdeBP 2023-04-19
		simple_rewrite_definition x_option('x', nullptr, "auxiliary vector format", next_args, "--field", x_fields, sizeof x_fields/sizeof *x_fields);
#endif
		simple_rewrite_definition H_option('H', nullptr, "show individual threads", next_args, "--threads", nullptr, 0);
		popt::definition * top_table[] = {
			&all_option,
			&b_option,
			&c_option,
			&e_option,
#if false // This does not really work.  JdeBP 2023-04-19
			&f_option,
#endif
			&i_option,
			&k_option,
#if false // This does not really work.  JdeBP 2023-04-19
			&l_option,
			&r_option,
#endif
			&s_option,
			&S_option,
			&t_option,
#if false // This does not really work.  JdeBP 2023-04-19
			&x_option,
#endif
			&H_option,
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "[id(s)...]");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}
	if (all == !args.empty()) {
		die_usage(prog, envs, "Either -a or a list of process IDs is required, and not both.");
	}

	next_args.insert(next_args.begin(), "list-process-table");
	for (std::vector<const char *>::const_iterator p(args.begin()), e(args.end()); p != e; ++p)
		next_args.insert(next_args.end(), *p);

	args = next_args;
	next_prog = arg0_of(args);
}
