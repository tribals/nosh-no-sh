#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim:set filetype=sh:
#
# This is run by the system-wide external configuration import subsystem.

files="termcap/termcap.linux termcap/termcap.interix termcap/termcap.teken termcap/termcap.jfbterm termcap/termcap.ms-terminal"

redo-ifchange /usr/share/misc/termcap ${files}
# cap_mkdb gets confused by the termcap.db file, so we make an alias with a symbolc link.
ln -f -s -- /usr/share/misc/termcap termcap/termcap.main
cap_mkdb -f "$3" termcap/termcap.main ${files}
mv "$3.db" "$3"
