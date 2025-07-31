#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:

(

awk '!x[$0]++' ../package/service-generators |
while read -r i
do
	echo service-generators/"$i"
done

awk '!x[$0]++' ../package/conversion-tools |
while read -r i
do
	echo convert/"$i"
done

) |
xargs -r redo-ifchange

exec redo-ifchange \
	systemd/system/service-manager.socket \
	;
