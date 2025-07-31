/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <algorithm>
#include <vector>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include "utils.h"
#include "fdutils.h"
#include "ProcessEnvironment.h"
#include "FileDescriptorOwner.h"
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

struct JVMVersion {
	JVMVersion(unsigned vmaj, unsigned vmin) : vmajor(vmaj), vminor(vmin) {}

	unsigned vmajor, vminor;

	static bool parse(const std::string &, std::string &, unsigned &, unsigned &);
	bool operator == (const JVMVersion & o) const { return o.vmajor == vmajor && o.vminor == vminor; }
};

// Java version strings come in 3 forms.  Java 1.8 can be:
//   openjdk8
//   linux-oracle-jdk18
//   jre-8-oracle-x64
//   java-1.8.0-gcj-4.9-amd64
bool
JVMVersion::parse(
	const std::string & s,
	std::string & r,
	unsigned & vmajor,
	unsigned & vminor
) {
	std::vector<unsigned> d;
	unsigned * current(nullptr);
	for (std::string::const_iterator p(s.begin()), e(s.end()); ; ++p) {
		if (e == p) {
			r = std::string();
			break;
		}
		const char c(*p);
		if ('.' == c) {
			current = nullptr;
		} else
		if (std::isdigit(c)) {
			if (!current) {
				d.push_back(0U);
				current = &d.back();
			}
			*current = *current * 10U + (c - '0');
		} else {
			r = std::string(p, e);
			break;
		}
	}
	if (0U >= d.size()) return false;
	if (1U == d.size()) {
		if (17U > d[0]) {
			vmajor = 1U;
			vminor = d[0];
		} else {
			vmajor = d[0] / 10U;
			vminor = d[0] % 10U;
		}
	} else
	{
		vmajor = d[0];
		vminor = d[1];
	}
	return true;
}

struct JVMDetails : public JVMVersion {
	JVMDetails(const std::string & root, unsigned vmaj, unsigned vmin, bool f, const std::string & man) : JVMVersion(vmaj, vmin), foreign(f), fullname(root), m(man) {}

	bool foreign;
	std::string fullname, m;

	static bool parse(const std::string &, unsigned &, unsigned &, bool &, std::string &);
	bool matches (const std::list<JVMVersion> &, const std::list<std::string> &, const std::list<std::string> &) const;
	bool operator < (const JVMDetails &) const;
};

std::string
strip_initial_letters (
	const std::string & s
) {
	for (std::string::const_iterator p(s.begin()), e(s.end()); e != p; ++p) {
		const char c(*p);
		if (!std::isalpha(c) && '-' != c && '_' != c)
			return std::string(p, e);
	}
	return std::string();
}

bool
JVMDetails::parse(
	const std::string & s,
	unsigned & vmajor,
	unsigned & vminor,
	bool & foreign,
	std::string & m
) {
	std::string r;
	if (begins_with(s, "openjdk", r)) {
		foreign = false;
		m = "openjdk";
		return JVMVersion::parse(r, r, vmajor, vminor);
	} else
	if (begins_with(s, "diablo", r)) {
		foreign = false;
		m = "diablo";
		return JVMVersion::parse(strip_initial_letters(r), r, vmajor, vminor);
	} else
	if (begins_with(s, "linux-sun", r)) {
#if defined(__LINUX__) || defined(__linux__)
		foreign = true;
#else
		foreign = false;
#endif
		m = "sun";
		return JVMVersion::parse(strip_initial_letters(r), r, vmajor, vminor);
	} else
	if (begins_with(s, "linux-oracle", r)) {
#if defined(__LINUX__) || defined(__linux__)
		foreign = true;
#else
		foreign = false;
#endif
		m = "oracle";
		return JVMVersion::parse(strip_initial_letters(r), r, vmajor, vminor);
	} else
	if (begins_with(s, "java-", r)
	||  begins_with(s, "jdk-", r)
	||  begins_with(s, "jre-", r)
	||  begins_with(s, "server-jre-", r)
	) {
		foreign = false;
		if (!JVMVersion::parse(r, r, vmajor, vminor)) return false;
		if ("-openjdk" == r			// Arch convention lacks ISA suffix
		||  begins_with(r, "-openjdk-", r)	// Debian convention with ISA suffix
		) {
			m = "openjdk";
		} else
		if (begins_with(r, "-gcj-", r)) {
			m = "gnu";
		} else
		if (begins_with(r, "-sun-", r)) {
			m = "sun";
		} else
		if ("-jdk" == r	// Arch convention lacks ISA suffix and manufacturer name
		||  "-jre" == r	// Arch convention lacks ISA suffix and manufacturer name
		||  begins_with(r, "-oracle-", r)	// Debian convention with ISA suffix
		) {
			m = "oracle";
		} else
		if ("-j9" == r) {			// Arch convention lacks ISA suffix
			m = "ibm";
		} else
			return false;
		return true;
	} else
		return false;
}

bool
JVMDetails::matches (
	const std::list<JVMVersion> & versions,
	const std::list<std::string> & operatingsystems,
	const std::list<std::string> & manufacturers
) const {
	if (!versions.empty()) {
		bool found(false);
		for (std::list<JVMVersion>::const_iterator i(versions.begin()); i != versions.end(); ++i) {
			if (i->operator==(*this)) {
				found = true;
				break;
			}
		}
		if (!found) return false;
	}
	if (!operatingsystems.empty()) {
		bool found(false);
		for (std::list<std::string>::const_iterator i(operatingsystems.begin()); i != operatingsystems.end(); ++i) {
			if ((foreign ? "foreign" : "native") == *i) {
				found = true;
				break;
			}
		}
		if (!found) return false;
	}
	if (!manufacturers.empty()) {
		bool found(false);
		for (std::list<std::string>::const_iterator i(manufacturers.begin()); i != manufacturers.end(); ++i) {
			if (m  == *i) {
				found = true;
				break;
			}
		}
		if (!found) return false;
	}
	return true;
}

bool
JVMDetails::operator < (
	const JVMDetails & o
) const {
	if (vmajor < o.vmajor) return true;
	if (vmajor > o.vmajor) return false;
	if (vminor < o.vminor) return true;
	if (vminor > o.vminor) return false;
	if (foreign && !o.foreign) return true;
	if (!foreign && o.foreign) return false;
	return m < o.m;
}

typedef std::list<JVMDetails> JVMDetailsList;
}

void
find_matching_jvm (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	std::list<std::string> versions, operatingsystems, manufacturers;
	try {
		popt::string_list_definition version_option('\0', "version", "1.v", "Match this version of Java.", versions);
		popt::string_list_definition operatingsystem_option('\0', "operating-system", "native|foreign", "Match this operating system.", operatingsystems);
		popt::string_list_definition manufacturer_option('\0', "manufacturer", "m", "Match this manufacturer of Java.", manufacturers);
		popt::definition * top_table[] = {
			&version_option,
			&operatingsystem_option,
			&manufacturer_option,
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

	bool done_some(false);
	JVMDetailsList details;
	std::list<JVMVersion> parsed_versions;
	for (std::list<std::string>::const_iterator i(versions.begin()); i != versions.end(); ++i) {
		std::string remainder;
		unsigned vmajor, vminor;
		if (!JVMVersion::parse(*i, remainder, vmajor, vminor) || !remainder.empty()) {
			die_invalid_argument(prog, envs, i->c_str(), "Invalid Java version.");
		}
		parsed_versions.push_back(JVMVersion(vmajor, vminor));
	}

	if (!done_some) {
		for (const char * const * i(etc); i < etc + sizeof etc/sizeof *etc; ++i) {
			const std::string n((*i) + javavms);
			const FileStar f(std::fopen(n.c_str(), "r"));
			if (f) {
				const std::vector<std::string> java_strings(read_file(prog, envs, n.c_str(), f));
				for (std::vector<std::string>::const_iterator j(java_strings.begin()); j != java_strings.end(); ++j) {
					const std::string root(dirname_of(dirname_of(*j)));
					unsigned vmajor, vminor;
					bool foreign;
					std::string m;
					if (!JVMDetails::parse(basename_of(root.c_str()), vmajor, vminor, foreign, m)) continue;

					const JVMDetails one(root, vmajor, vminor, foreign, m);
					JVMDetailsList::iterator p(std::upper_bound(details.begin(), details.end(), one));
					details.insert(p, one);
				}
				done_some = true;
			}
		}
	}

	if (!done_some) {
		for (const char * const * i(lib); i < lib + sizeof lib/sizeof *lib; ++i) {
			const std::string n((*i) + jvm);
			FileDescriptorOwner d(open_dir_at(AT_FDCWD, n.c_str()));
			if (0 <= d.get()) {
				const FileDescriptorOwner d2(dup(d.get()));
				if (0 > d2.get()) {
					die_errno(prog, envs, n.c_str());
				}
				const DirStar ds(d);
				if (ds) {
					for (;;) {
						const dirent * entry(readdir(ds));
						if (!entry) break;
#if defined(_DIRENT_HAVE_D_NAMLEN)
						if (1 > entry->d_namlen) continue;
#endif
						if ('.' == entry->d_name[0]) continue;
#if defined(_DIRENT_HAVE_D_TYPE)
						if (DT_DIR != entry->d_type && DT_LNK != entry->d_type) continue;
#endif

						const std::string name(entry->d_name);
						std::string subdir;

						if (0 > faccessat(d2.get(), (name + subdir + "/bin/java").c_str(), F_OK, AT_EACCESS)) {
							subdir = "/jre";
							if (0 > faccessat(d2.get(), (name + subdir + "/bin/java").c_str(), F_OK, AT_EACCESS)) continue;
						}

						unsigned vmajor, vminor;
						bool foreign;
						std::string m;
						if (!JVMDetails::parse(name, vmajor, vminor, foreign, m)) continue;

						const JVMDetails one(n + "/" + name + subdir, vmajor, vminor, foreign, m);
						JVMDetailsList::iterator p(std::upper_bound(details.begin(), details.end(), one));
						details.insert(p, one);
					}
					done_some = true;
				}
			}
		}
	}

	if (!done_some) {
		for (const char * const * i(path); i < path + sizeof path/sizeof *path; ++i) {
			const std::string root(*i);
			if (0 > faccessat(AT_FDCWD, (root + "/bin/java").c_str(), F_OK, AT_EACCESS)) continue;
			unsigned vmajor, vminor;
			bool foreign;
			std::string m;
			if (!JVMDetails::parse(basename_of(root.c_str()), vmajor, vminor, foreign, m)) continue;

			const JVMDetails one(root, vmajor, vminor, foreign, m);
			JVMDetailsList::iterator p(std::upper_bound(details.begin(), details.end(), one));
			details.insert(p, one);
			done_some = true;
		}
	}

	if (!done_some)
		die_invalid(prog, envs, "JAVA_HOME", "No JVMs found.");

	for (JVMDetailsList::const_reverse_iterator i(details.rbegin()); i != details.rend(); ++i) {
		if (!i->matches(parsed_versions, operatingsystems, manufacturers)) continue;
		if (!envs.set("JAVA_HOME", i->fullname.c_str()))
			die_errno(prog, envs, "JAVA_HOME");
		return;
	}

	die_invalid(prog, envs, "JAVA_HOME", "No matching JVMs found.");
}
