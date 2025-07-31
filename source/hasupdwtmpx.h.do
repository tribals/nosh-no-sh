#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:
redo-ifchange tryupdwtmpx.cpp compile link
if ( ./compile tryupdwtmpx.o tryupdwtmpx.cpp tryupdwtmpx.d && ./link tryupdwtmpx tryupdwtmpx.o )
	#>/dev/null 2>&1 
then
	echo '#define HAS_UDPWTMPX 1' > "$3"
else
	echo '/* sysdep: -updwtmpx */' > "$3"
fi
