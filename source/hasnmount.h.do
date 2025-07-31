#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:
redo-ifchange trynmount.cpp compile link
if ( ./compile trynmount.o trynmount.cpp trynmount.d && ./link trynmount trynmount.o )
	#>/dev/null 2>&1 
then
	echo '#define HAS_NMOUNT 1' > "$3"
else
	echo '/* sysdep: -nmount */' > "$3"
fi
