#! /bin/bash
#
# A simple script to unload the kernel module and remove the old device nodes
# for it.
#
# Note:
# this must be run as root
#

# Check that root is executing us
if [ "$EUID" != "0" ]; then
    echo "Sorry, you need to be root for this script to work."
    echo "use 'su -c $0' and enter the root password when prompted"
    exit -1
fi

# Check if the module is already loaded
if [ "x`grep plex86 /proc/devices`" != "x" ]; then
    /sbin/rmmod plex86

    # Check that it really went (OK - I'm paranoid)
    if [ "x`grep plex86 /proc/devices`" != "x" ]; then
	echo "The kernel module failed to unload!"
	exit -1
    fi
fi

# Remove any stale device nodes
# (extend for any minor devices created in the future)
rm -f /dev/plex86

# Job done - Give a little positive feedback
echo "The kernel module is no longer installed."
exit 0
