/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_LOGIN_BANNER_INFORMATION_H)
#define INCLUDE_LOGIN_BANNER_INFORMATION_H

#include <string>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/param.h>

struct ProcessEnvironment;

struct LoginBannerInformation {
	LoginBannerInformation(const char *, const ProcessEnvironment &);

	const char * query_line() const { return line; }
	const char * query_sysname() const { return uts.sysname; }
	const char * query_nodename() const { return uts.nodename; }
	const char * query_release() const { return uts.release; }
	const char * query_version() const { return uts.version; }
	const char * query_machine() const { return uts.machine; }
	unsigned long query_users() const { return users; }
	const std::string & query_pretty_sysname() const { return pretty_sysname; }
	const std::string & query_pretty_nodename() const { return pretty_nodename; }
	const std::string & query_deployment() const { return deployment; }
	const std::string & query_location() const { return location; }
	const char * query_hostname() const { return hostname; }
	const char * query_nisdomainname() const { return nisdomainname; }
	const char * query_dnsdomainname() const { return dnsdomainname; }
protected:
	struct utsname uts;
	const char * line;
	unsigned long users;
	std::string pretty_sysname;
	std::string pretty_nodename;
	std::string deployment;
	std::string location;
	const char * hostname;
	const char * nisdomainname;
	const char * dnsdomainname;
private:
	char nisdomainname_buf[MAXHOSTNAMELEN];
	char hostname_buf[MAXHOSTNAMELEN];
};

#endif
