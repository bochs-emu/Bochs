#! /bin/bash
#
# A simple script to load up the kernel module and create the device nodes
# for it.
#
# Note:
# this must be run as root
#

# Work out where the module is
kmodule="`dirname $0`/../kernel/plex86.o"

# Check that root is executing us
if [ "$EUID" != "0" ]; then
    echo "Sorry, you need to be root for this script to work."
    echo "use 'su -c $0' and enter the root password when prompted"
    exit -1
fi

# Check if the module exists
if [ ! -f "$kmodule" ]; then
    echo "The kernel module ($kmodule) does not exist!"
    exit -1
fi

# Check if the module is already loaded
if [ "x`grep plex86 /proc/devices`" != "x" ]; then
    echo "The kernel module is already loaded!"
    exit -1
fi

# Remove any stale device nodes
# (extend for any minor devices created in the future)
rm -f /dev/plex86

# Load up the module with insmod
/sbin/insmod $kmodule

# Check if the module loaded
major=`grep plex86 /proc/devices | awk '/plex86/ {print $1;}'`
if [ "x$major" = "x" ]; then
    echo "The kernel module failed to load!"
    exit -1
fi

# Create the device node and set its permissions
# (extend for any minor devices created in the future)
if [ ! -c /dev/plex86 ]; then
    /bin/mknod /dev/plex86 c $major 0
fi
chmod a+rw /dev/plex86

# Job done - Give a little positive feedback
echo "The kernel module is sucessfully installed."
exit 0
