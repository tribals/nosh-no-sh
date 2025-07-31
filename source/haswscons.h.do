#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:
redo-ifchange trywscons.cpp compile link
if ( ./compile trywscons.o trywscons.cpp trywscons.d && ./link trywscons trywscons.o )
	#>/dev/null 2>&1 
then
	echo '#define HAS_WSCONS 1' > "$3"
else
	echo '/* sysdep: -wscons */' > "$3"
fi
