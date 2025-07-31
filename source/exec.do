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
		uuid=-luuid
		rt=-lrt
		;;
	FreeBSD)
		util=-lutil
		static="-static"
		;;
	*BSD)
		util=-lutil
		;;
esac
redo-ifchange link ${objects} ${libraries}
exec ./link "$3" ${objects} ${libraries} ${uuid} ${rt} ${util} ${static}
