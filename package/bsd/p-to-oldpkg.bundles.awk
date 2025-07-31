{
	two=$2;
	gsub("^\"|\"$","",two);
	three=$3;
	gsub("^\"|\"$","",three);
	four=$4;
	gsub("^\"|\"$","",four);
	pwescaped2=two
	gsub("_","_m",pwescaped2);
	gsub("@","_a",pwescaped2);
	gsub(":","_c",pwescaped2);
	gsub(";","_h",pwescaped2);
	gsub(",","_v",pwescaped2);
	gsub("\\.","_d",pwescaped2);
	gsub("\\+","_p",pwescaped2);
	pwescaped3=two
	gsub("_","_m",pwescaped3);
	gsub("@","_a",pwescaped3);
	gsub(":","_c",pwescaped3);
	gsub(";","_h",pwescaped3);
	gsub(",","_v",pwescaped3);
	gsub("\\.","_d",pwescaped3);
	gsub("\\+","_p",pwescaped3);
	if ("logfile_owning_user" == $1) {
		user=pwescaped2 "-log"
		if (level > 1) {
			print "@newgroup",user;
			print "@newuser",user "::::::/sbin/nologin";
		} else
		{
			print "@exec","groupadd",user;
			print "@exec","useradd","-s","/sbin/nologin","-d","/nonexistent","-g",user,user;
		}
	} else
	if ("file_owning_user" == $1) {
		user=pwescaped2
		if (level > 1) {
			print "@newgroup",user;
			print "@newuser",user ":::::" pwescaped3 ":/sbin/nologin";
		} else
		{
			print "@exec","groupadd",user;
			print "@exec","useradd","-s","/sbin/nologin","-d","/home/" pwescaped3,"-g",user,user;
		}
	} else
	if ("non_file_owning_user" == $1) {
		user=pwescaped2
		if (level > 1) {
			print "@newgroup",user;
			print "@newuser",user "::::::/sbin/nologin";
		} else
		{
			print "@exec","groupadd",user;
			print "@exec","useradd","-s","/sbin/nologin","-d","/nonexistent","-g",user,user;
		}
	} else
	if ("user_vt_user" == $1) {
		user="user-vt-" pwescaped2
		if (level > 1) {
			print "@newuser",user "in";
		} else
		{
			print "@exec","useradd","-s","/sbin/nologin","-d","/nonexistent","-g",user,user;
		}
	} else
	if ("user_vt_group" == $1) {
		user="user-vt-" pwescaped2
		if (level > 1) {
			print "@newgroup",user;
		} else
		{
			print "@exec","useradd","-s","/sbin/nologin","-d","/nonexistent","-g",user,user;
		}
	} else
	if ("service_with_dedicated_logger" == $1) {
		user=pwescaped2 "-log"
		if (level > 1) {
			print "@newgroup",user;
			print "@newuser",user "::::::/sbin/nologin";
		} else
		{
			print "@exec","groupadd",user;
			print "@exec","useradd","-s","/sbin/nologin","-d","/nonexistent","-g",user,user;
		}
		print "@owner",user;
		print "@mode","0755";
		if (level > 1) {
			print "@dir","var/log/sv/" two;
		}
	} else
	if ("login_service_with_dedicated_logger" == $1) {
		user="ttylogin_a" pwescaped2 "-log"
		if (level > 1) {
			print "@newgroup",user;
			print "@newuser",user "::::::/sbin/nologin";
		} else
		{
			print "@exec","groupadd",user;
			print "@exec","useradd","-s","/sbin/nologin","-d","/nonexistent","-g",user,user;
		}
		print "@owner",user;
		print "@mode","0755";
		if (level > 1) {
			print "@dir","var/log/sv/ttylogin@" two;
		}
	} else
	if ("kvt_login_service_with_dedicated_logger" == $1) {
		if (level > 1) {
			user="ttylogin_attyC" pwescaped2 "-log"
			print "@newgroup",user;
			print "@newuser",user "::::::/sbin/nologin";
		} else
		{
			user="ttylogin_attyE" pwescaped2 "-log"
			print "@exec","groupadd",user;
			print "@exec","useradd","-s","/sbin/nologin","-d","/nonexistent","-g",user,user;
		}
		print "@owner",user;
		print "@mode","0755";
		if (level > 1) {
			print "@dir","var/log/sv/ttylogin@ttyC" two;
		}
	} else
	if ("kvt_realizer_service_with_dedicated_logger" == $1) {
		if (level > 1) {
			user="console-kvt-realizer_attyC" pwescaped2 "-log"
			print "@newgroup",user;
			print "@newuser",user "::::::/sbin/nologin";
		} else
		{
			user="console-kvt-realizer_attyE" pwescaped2 "-log"
			print "@exec","groupadd",user;
			print "@exec","useradd","-s","/sbin/nologin","-d","/nonexistent","-g",user,user;
		}
		print "@owner",user;
		print "@mode","0755";
		if (level > 1) {
			print "@dir","var/log/sv/ttylogin@ttyC" two;
		}
	} else
	if ("serial_tty_login_service_with_dedicated_logger" == $1) {
		if (level > 1) {
			user="ttylogin_atty0" pwescaped2 "-log"
			print "@newgroup",user;
			print "@newuser",user "::::::/sbin/nologin";
		} else
		{
			user="ttylogin_adty0" pwescaped2 "-log"
			print "@exec","groupadd",user;
			print "@exec","useradd","-s","/sbin/nologin","-d","/nonexistent","-g",user,user;
		}
		print "@owner",user;
		print "@mode","0755";
		if (level > 1) {
			print "@dir","var/log/sv/ttylogin@tty0" two;
		}
	} else
	if ("serial_tty_callout_service_with_dedicated_logger" == $1) {
		if (level > 1) {
			user="ttycallout_atty0" pwescaped2 "-log"
			print "@newgroup",user;
			print "@newuser",user "::::::/sbin/nologin";
		} else
		{
			user="ttycallout_adty0" pwescaped2 "-log"
			print "@exec","groupadd",user;
			print "@exec","useradd","-s","/sbin/nologin","-d","/nonexistent","-g",user,user;
		}
		print "@owner",user;
		print "@mode","0755";
		if (level > 1) {
			print "@dir","var/log/sv/ttycallout@tty0" two;
		}
	} else
	if ("socket_with_dedicated_logger" == $1) {
		user=pwescaped2 "-log"
		if (level > 1) {
			print "@newgroup",user;
			print "@newuser",user "::::::/sbin/nologin";
		} else
		{
			print "@exec","groupadd",user;
			print "@exec","useradd","-s","/sbin/nologin","-d","/nonexistent","-g",user,user;
		}
		print "@owner",user;
		print "@mode","0755";
		if (level > 1) {
			print "@dir","var/log/sv/" two;
		}
	} else
	if ("timer_with_dedicated_logger" == $1) {
		user=pwescaped2 "-log"
		if (level > 1) {
			print "@newgroup",user;
			print "@newuser",user "::::::/sbin/nologin";
		} else
		{
			print "@exec","groupadd",user;
			print "@exec","useradd","-s","/sbin/nologin","-d","/nonexistent","-g",user,user;
		}
		print "@owner",user;
		print "@mode","0755";
		if (level > 1) {
			print "@dir","var/log/sv/" two;
		}
	} else
	if ("fan_in_logger" == $1) {
		user=pwescaped2 "-log"
		if (level > 1) {
			print "@newgroup",user;
			print "@newuser",user "::::::/sbin/nologin";
		} else
		{
			print "@exec","groupadd",user;
			print "@exec","useradd","-s","/sbin/nologin","-d","/nonexistent","-g",user,user;
		}
		print "@owner",user;
		print "@mode","0755";
		if (level > 1) {
			print "@dir","var/log/sv/" two;
		}
	} else
	if ("service_only" == $1) {
		;
	} else
	if ("service_only_no_run" == $1) {
		;
	} else
	if ("target" == $1) {
		;
	} else
	if ("user_tty" == $1) {
		;
	} else
	if ("directory" == $1) {
                gsub("^/","",three);
		print "@owner",pwescaped2;
		print "@mode",four;
		if (level > 1) {
			print "@dir",three;
		}
	} else
		;
}
