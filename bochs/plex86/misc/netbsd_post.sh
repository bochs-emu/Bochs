#! /bin/sh
#
# This is the post-install script called by modload after it has loaded
# the module.
#
# Sample modload commandline:
#
#	modload -e plex86_lkmentry -p netbsd_post.sh plex86.o
# 
if [ $# -ne 3 ]; then
    echo "$0 should only be called from modload(8) with 3 args"
    exit 1
fi

rm -f /dev/plex86
mknod /dev/plex86 c $3 0
exit 0
