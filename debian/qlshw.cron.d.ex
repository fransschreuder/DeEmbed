#
# Regular cron jobs for the qlshw package
#
0 4	* * *	root	[ -x /usr/bin/DeEmbed_maintenance ] && /usr/bin/DeEmbed_maintenance
