#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:
main="`basename "$1"`"
objects="main-exec.o builtins-${main}.o"
libraries="builtins.a utils.a"
case "`uname`" in
	Linux)
		crypt=-lcrypt
		;;
	FreeBSD)
		crypt=-lcrypt
		util=-lutil
		# Needed because emergency-login can be run before filesystems are mounted.
		static="-static"
		;;
	*BSD)
		crypt=-lcrypt
		util=-lutil
		;;
esac
redo-ifchange link ${objects} ${libraries}
exec ./link "$3" ${objects} ${libraries} ${crypt} ${static} ${util}
