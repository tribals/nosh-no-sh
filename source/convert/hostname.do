#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:
#
# Special setup for hostname.
# This is invoked by all.do .
#

# This gets us *only* the configuration variables, safely.
read_rc() { clearenv read-conf rc.conf printenv "$1" ; }
get_var1() { read_rc "$1" || true ; }

redo-ifchange general-services rc.conf

hostname="`get_var1 hostname`"
if s="`system-control find hostname.service`"
then
	system-control set-service-env "${s}" hostname "${hostname}"
fi
printf >> "$3" '%s: %s\n' hostname "${hostname}"
