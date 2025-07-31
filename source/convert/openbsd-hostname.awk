## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************

{
	if ("inet" == $1 || "inet4" == $1) {
		++ipv4s;
		if ("alias" == $2) {
			++ipv4s;
			$2 = $3;
			$3 = "";
		}
		if (1 < ipv4s) {
			if ("" != aliases) {
				aliases = aliases "\n";
			}
			aliases = aliases $1;
		}
		$1 = $2;
		$2 = "netmask";
		if (1 < ipv4s)
			aliases = aliases " " $0;
		else
			ipv4_address = ipv4_address " " $0;
	} else
	if ("inet6" == $1) {
		++ipv6s;
		if ("alias" == $2) {
			++ipv6s;
			$2 = $3;
			$3 = "";
		}
		if (1 < ipv6s) {
			if ("" != aliases) {
				aliases = aliases "\n";
			}
			aliases = aliases $1;
		}
		$1 = $2;
		$2 = "prefixlen";
		if (1 < ipv6s)
			aliases = aliases " " $0;
		else
			ipv6_address = ipv6_address " " $0;
	} else
	if ("rtsol" == $1) {
		rtsol=1;
	} else
	if ("dhcp" == $1) {
		dhcp=1;
	} else
		;
}
END {
	if ("" != ipv4_address) {
		printf "ifconfig_%s=\"",iface;
		printf "AUTO ";
		if (rtsol) printf "RTSOL ";
		if (dhcp) printf "DHCP ";
		printf "inet %s %s\"\n",ipv4_address,ipv4;
	}
	if ("" != ipv6_address)
		printf "ifconfig_%s_ipv6=\"inet6 %s %s\"\n",iface,ipv6_address,ipv6;
	if ("" != aliases)
		printf "ifconfig_%s_aliases=\"%s\"\n",iface,aliases;
}
