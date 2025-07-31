/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_USER_ENVIRONMENT_SETTER_H)
#define INCLUDE_USER_ENVIRONMENT_SETTER_H

#include <pwd.h>
#include <string>

struct ProcessEnvironment;
struct LoginClassRecordOwner;

/// \brief An actor for setting per-user environment variables.
struct UserEnvironmentSetter {
	UserEnvironmentSetter(ProcessEnvironment &);
	void apply(const passwd *);
	bool set_user;
	bool set_shell;
	bool default_locale;
	bool default_path;
	bool default_term;
	bool default_timezone;
	bool default_tools;
	bool set_dbus;
	bool set_xdg;
	bool set_other;
	bool toolkit_pager;
protected:
	void set_if (const std::string & var, bool cond, const std::string & val) ;
	void set (const passwd & pw, const char * var, const char * val, bool is_path) ;
	void set_vec (const passwd & pw, const char * vec) ;
	void default_str (const char * var, const char * val) ;
	void default_str (const char * var, const char * cap, const passwd & pw, LoginClassRecordOwner & lc_system, LoginClassRecordOwner & lc_user, const char * def) ;
	void default_pathstr (const char * var, const char * cap, const passwd & pw, LoginClassRecordOwner & lc_system, LoginClassRecordOwner & lc_user, const char * def) ;
	void set_vec (const char * cap, const passwd & pw, LoginClassRecordOwner & lc) ;
	void set_vec (const char * cap, const passwd & pw, LoginClassRecordOwner & lc_system, LoginClassRecordOwner & lc_user) ;
	void prependpath (const char * var, const std::string & val) ;
	ProcessEnvironment & envs;
};

#endif
