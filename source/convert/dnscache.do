#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:
#
# Special setup for dnscache.
# This is invoked by all.do .
#

set_if_unset() { if test -z "`system-control print-service-env \"$1\" \"$2\"`" ; then system-control set-service-env "$1" "$2" "$3" ; echo "$s: Defaulted $2 to $3." ; fi ; }
dir_not_empty() { test -n "`/bin/ls -A \"$1\"`" ; }

# These get us *only* the configuration variables, safely.
read_rc() { clearenv read-conf rc.conf printenv "$1" ; }
list_network_addresses() { ( read_rc dnscache_network_addresses || echo 127.0.0.1 ) | fmt -w 1 ; }
show() {
	local service
	for service
	do
		if system-control is-enabled "${service}"
		then
			echo on "${service}"
		else
			echo off "${service}"
		fi
		system-control print-service-env "${service}" | sed -e 's/^/	/'
	done
}

redo-ifchange rc.conf general-services "dnscache@.socket" "dnscache.service"

list_graft_points() {
	# http://jdebp.uk./FGA/dns-private-address-split-horizon.html#WhatToDo
	for d in \
		0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0 \
		1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0 \
		E.F.9.A.2.0.0.2 \
		F.7.2.0.0.2 \
		E.2.0.0.2 \
		D.F \
		8.E.F \
		9.E.F \
		A.E.F \
		B.E.F \
		C.E.F \
		D.E.F \
		E.E.F \
		F.E.F \
		1.0.F.F \
		2.0.F.F \
	;
	do
		echo "${d}.ip6.arpa"
	done
	for d in \
		0 \
		10 \
		127 \
		254.169 \
		16.172 \
		17.172 \
		18.172 \
		19.172 \
		20.172 \
		21.172 \
		22.172 \
		23.172 \
		24.172 \
		25.172 \
		26.172 \
		27.172 \
		28.172 \
		29.172 \
		30.172 \
		31.172 \
		2.0.192 \
		168.192 \
		224 \
		255.255.255.255 \
	;
	do
		echo "${d}.in-addr.arpa"
	done
	for d in \
		localhost \
		test \
		invalid \
		example \
		example.org \
		example.net \
		example.com \
	;
	do
		echo "${d}"
	done
}

# Old system: One pre-packaged dnscache server for the entire machine
if s="`system-control find dnscache`"
then
	set_if_unset dnscache IPSEND 0.0.0.0
	set_if_unset dnscache IP 127.0.0.1
#	set_if_unset dnscache DATALIMIT 3000000
	set_if_unset dnscache CACHESIZE 1000000
	set_if_unset dnscache ROOT "root"

	install -d -m 0755 "${s}/service/root/ip"
	install -d -m 0755 "${s}/service/root/servers"
	# We do not use an on-disc seed file any more.
	test \! -e "${s}/service/seed" || chmod 0 "${s}/service/seed" 
	test -r "${s}/service/root/servers/@" || echo '127.53.0.1' > "${s}/service/root/servers/@"
	redo-ifchange "${s}/service/root/servers/@"
	list_graft_points |
	while read -r d
	do
		if ! test -r "${s}/service/root/servers/${d}"
		then
			ln "${s}/service/root/servers/@" "${s}/service/root/servers/${d}"
			redo-ifchange "${s}/service/root/servers/${d}"
		fi
	done
	dir_not_empty "${s}/service/root/ip" || touch "${s}/service/root/ip/127.0.0.1"

	show "${s}" >> "$3"
fi

# New system: Individual dnscache servers for every IP adddress in dnscache_netework_addresses in rc.conf

test -h /var/local/service-bundles/targets || { install -d -m 0755 /var/local/service-bundles && ln -s /etc/service-bundles/targets /var/local/service-bundles/ ; }
lr="/var/local/service-bundles/services/"
e="--no-systemd-quirks --escape-instance --local-bundle"

list_network_addresses |
while read -r i
do
	test -z "$i" && continue
	service="dnscache@$i"
	s="$lr/${service}"

	system-control convert-systemd-units $e --bundle-root "$lr/" "./${service}.socket"
	rm -f -- "${s}/log"
	ln -s -f -- "../../../../service-bundles/services/cyclog@dnscache" "${s}/log"

	install -d -m 0755 "${s}/service/env"
	install -d -m 0755 "${s}/service/root"
	install -d -m 0755 "${s}/service/root/ip"
	install -d -m 0755 "${s}/service/root/servers"
	# We do not use an on-disc seed file any more.
	test \! -e "${s}/service/seed" || chmod 0 "${s}/service/seed" 
	test -r "${s}/service/root/servers/@" || echo '127.53.0.1' > "${s}/service/root/servers/@"
	redo-ifchange "${s}/service/root/servers/@"
	list_graft_points |
	while read -r d
	do
		if ! test -r "${s}/service/root/servers/${d}"
		then
			ln "${s}/service/root/servers/@" "${s}/service/root/servers/${d}"
			redo-ifchange "${s}/service/root/servers/${d}"
		fi
	done
	dir_not_empty "${s}/service/root/ip" || touch "${s}/service/root/ip/127.0.0.1"
	set_if_unset "${s}/" IPSEND 0.0.0.0
	set_if_unset "${s}/" CACHESIZE 1000000
	set_if_unset "${s}/" ROOT "root"

	system-control preset --rcconf-file rc.conf "${service}"
	show "${s}" >> "$3"
done
