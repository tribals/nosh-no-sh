/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <limits>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cerrno>
#include <cstring>
#include <sys/uio.h>
#include <sys/stat.h>
#if defined(__LINUX__) || defined(__linux__)
#include <sys/sysmacros.h>
#endif
#include <unistd.h>
#include "utils.h"
#include "fdutils.h"
#include "popt.h"
#include "FileDescriptorOwner.h"
#include "FileStar.h"
#include "control_groups.h"

static inline
int
d2c ( int c )
{
	if (std::isdigit(c)) return c - '0';
	return EOF;
}

static
bool
read_first_line_number (
	std::istream & i,
	uint_least64_t & v
) {
	if (i.fail()) return false;
	v = 0;
	for (;;) {
		const int c(i.get());
		if (i.fail()) return false;
		if (i.eof() || '\n' == c) return true;
		if (!std::isdigit(c)) return false;
		v *= 10U;
		v += static_cast<unsigned>(d2c(c));
	}
}

static
bool
read_first_line_number (
	const char * filename,
	uint_least64_t & v
) {
	std::ifstream s(filename);
	if (s.fail()) return false;
	return read_first_line_number(s,v);
}

/* Main function ************************************************************
// **************************************************************************
*/

void
set_control_group_knob [[gnu::noreturn]] (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	const char * percent_of(nullptr);
	bool infinity_is_max(false);
	bool multiplier_suffixes(false);
	bool device_name_key(false);
	const char * nested_key(nullptr);
	try {
		popt::bool_definition infinity_is_max_option('\0', "infinity-is-max", "Allow the use of \"infinity\" to mean \"max\".", infinity_is_max);
		popt::bool_definition multiplier_suffixes_option('\0', "multiplier-suffixes", "Decode SI and IEEE/IEC multiplier suffixes.", multiplier_suffixes);
		popt::bool_definition device_name_key_option('\0', "device-name-key", "Decode the key from a device name into major and minor device numbers.", device_name_key);
		popt::string_definition percent_of_option('\0', "percent-of", "filename", "Decode percentages using the value read from file.", percent_of);
		popt::string_definition nested_key_option('\0', "nested-key", "key", "Specify a nested key prefix.", nested_key);
		popt::definition * top_table[] = {
			&infinity_is_max_option,
			&multiplier_suffixes_option,
			&device_name_key_option,
			&percent_of_option,
			&nested_key_option
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "{knob} {value}");

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
		die_missing_argument(prog, envs, "knob name");
	}
	const char * knob(args.front());
	args.erase(args.begin());
	if (args.empty()) {
		die_missing_argument(prog, envs, "value name");
	}
	const char * value(args.front());
	args.erase(args.begin());
	if (!args.empty()) die_unexpected_argument(prog, args, envs);

	FileStar self_cgroup(open_my_control_group_info("/proc/self/cgroup"));
	if (!self_cgroup) {
		if (ENOENT == errno) throw EXIT_SUCCESS;	// This is what we'll see on a BSD.
		die_errno(prog, envs, "/proc/self/cgroup");
	}

	std::string prefix("/sys/fs/cgroup"), current;
	if (!read_my_control_group(self_cgroup, "", current)) {
		if (!read_my_control_group(self_cgroup, "name=systemd", current))
			throw EXIT_SUCCESS;
		prefix += "/systemd";
	}

	// We had a neat system using iovec and writev here.
	// Then it turned out that Linux cgroupfs just doesn't implement writev in any reasonable way.

	std::string value_key;
	if (device_name_key) {
		if (const char * next = std::strtok(const_cast<char *>(value), " \t")) {
			struct stat s;
			if (0 > fstatat(AT_FDCWD, value, &s, AT_SYMLINK_NOFOLLOW)) {
				die_errno(prog, envs, value);
			}
			if (!S_ISBLK(s.st_mode) && !S_ISCHR(s.st_mode))
				die_invalid(prog, envs, value, "Not a device.");
			char keybuf[256];
			const int len(std::snprintf(keybuf, sizeof keybuf, "%u:%u", major(s.st_rdev), minor(s.st_rdev)));
			value_key = std::string(keybuf, len) + " ";
			value = next;
		}
	}
	std::string value_nested_key;
	if (nested_key) {
		value_nested_key = nested_key;
		value_nested_key += "=";
	}
	if (infinity_is_max) {
		if (0 == std::strcmp(value, "infinity"))
			value = "max";
	}
	std::string value_suffix(value);
	if (percent_of || multiplier_suffixes) {
		const char * end(value);
		unsigned long long number(std::strtoull(value, const_cast<char **>(&end), 0));
		if (end != value) {
			if (percent_of && '%' == end[0] && '\0' == end[1]) {
				uint_least64_t hundred;
				if (!read_first_line_number (percent_of, hundred))
					die_invalid(prog, envs, percent_of, "File does not start with a decimal number");
				++end;
				if (hundred > std::numeric_limits<uint_least64_t>::max() / 100U)
					number = (hundred / 100U) * number;
				else
					number = (hundred * number) / 100U;
			}
			if (multiplier_suffixes && end[0]) {
				if ('\0' == end[1]) switch (end[0]) {
					case 'h':	++end; number *= 100L; break;
					case 'k':	++end; number *= 1000L; break;
					case 'M':	++end; number *= 1000000L; break;
					case 'G':	++end; number *= 1000000000L; break;
					case 'T':	++end; number *= 1000000000000L; break;
					case 'E':	++end; number *= 1000000000000000L; break;
				} else
				if ('i' == end[1] && !end[2]) switch (end[0]) {
					case 'K':	end += 2; number *= 0x400UL; break;
					case 'M':	end += 2; number *= 0x100000UL; break;
					case 'G':	end += 2; number *= 0x40000000UL; break;
					case 'T':	end += 2; number *= 0x10000000000UL; break;
					case 'E':	end += 2; number *= 0x4000000000000UL; break;
				}
			}
			if ('\0' == *end) {
				char numbuf[256];
				const int len(std::snprintf(numbuf, sizeof numbuf, "%llu", number));
				value_suffix = std::string(numbuf, len);
			}
		}
	}

	const FileDescriptorOwner cgroup_knob_fd(open_writetruncexisting_at(AT_FDCWD, ((prefix + current + "/") + knob).c_str()));
	if (0 > cgroup_knob_fd.get()) {
procs_file_error:
		die_errno(prog, envs, current.c_str(), "/", knob);
	}
	const std::string v(value_key + value_nested_key + value_suffix);
	if (0 > write(cgroup_knob_fd.get(), v.data(), v.length())) goto procs_file_error;

	throw EXIT_SUCCESS;
}
