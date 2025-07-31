# vim: set filetype=sh:
# service list
service_with_dedicated_logger "accounts-daemon"
service_with_dedicated_logger "bluetooth"
service_with_dedicated_logger "blueman-mechanism"
service_with_dedicated_logger "colord"
service_with_dedicated_logger "console-kit-daemon"
service_with_dedicated_logger "polkitd"
# This port is disabled in the ports tree.
#service_with_dedicated_logger "packagekit"
# These have no ports in the ports tree.
#service_with_dedicated_logger "ModemManager"
#service_with_dedicated_logger "NetworkManager"
#service_with_dedicated_logger "NetworkManager-dispatcher"
#service_with_dedicated_logger "rtkit-daemon"
