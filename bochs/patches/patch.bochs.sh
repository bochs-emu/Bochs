----------------------------------------------------------------------
Patch name: patch.bochs.sh
Author: Bernhard Bablok
Date:

Detailed description:
 The bochs-executable does not follow typical
 unix-conventions for command-line arguments. The
 included script Bochs.sh is a wrapper for bochs.

Patch was created with:
  diff -u
Apply patch to what version:
  release version 1.4.1
Instructions:
  To patch, go to main bochs directory.
  Type "patch -p0 < THIS_PATCH_FILE".
----------------------------------------------------------------------
--- /dev/null	Tue Mar 26 11:23:49 2002
+++ Bochs.sh	Mon Aug 12 18:43:03 2002
@@ -0,0 +1,181 @@
+#!/bin/bash
+#
+# Start the Bochs emulator.
+# 
+# This script is a Unix-friendly wrapper to the bochs-executable.
+# For help, run Bochs.sh -?
+#
+# You can pass floppy-image, disk-image and cdrom-image filenames as parameters.
+# Geometry is detected automatically for all disk-images created
+# with this script or disk-images created with bximage with the following sizes:
+# 10MB, 20MB, 50MB, 100MB, 200MB, 500MB, 1024MB, 2048MB.
+#
+# $Author: cbothamy $
+# $Revision: 1.1 $
+#
+# License: GPL2 (see LICENSE)
+# -----------------------------------------------------------------------------
+
+# usage message   -------------------------------------------------------------
+usage() {
+  echo -e "\nusage: `basename $0` [options] [additional bochs arguments]\n\
+  possible options:\n\
+    -b a|c|cdrom: boot from A: (default), first disk (aka c:) or cdrom\n\
+    -f filename:  path to floppy-image (default: /dev/fd0)\n\
+    -1 filename:  path to hda-image (default: no first disk)\n\
+    -c filename:  path to cdrom-image (default: /dev/cdrom)\n\
+    -2 filename:  path to hdb-image (default: no second disk)\n\
+    -C size:      create image of given size and exit\n\
+                  (10M/30M/46M/62M/112M/126M/483M/504M/640M/886M/1280M/2215M/3495M)\n\
+  -c and -2 are mutally exclusive"
+  exit 3
+}
+
+# set system defaults   -------------------------------------------------------
+
+setDefaults() {
+  [ -z "$BOCHS" ] && BOCHS="bochs"
+  boot="boot: a"
+  floppyPath="/dev/fd0"
+  floppySize="1_44"
+  hdaPath=""
+  hdbPath=""
+  cdromPath="/dev/cdrom"
+}
+
+# translate size to disk geometry   -------------------------------------------
+
+getHDGeom() {
+  name=$1
+  size=`find $name -printf "%s"`
+  case $size in
+      10653696) hdGeom="cyl=306, heads=4, spt=17";;
+      10321920) hdGeom="cyl=20, heads=16, spt=63";;
+      20643840) hdGeom="cyl=40, heads=16, spt=63";;
+      21411840) hdGeom="cyl=615, heads=4, spt=17";;
+      32117760) hdGeom="cyl=615, heads=6, spt=17";;
+      49090560) hdGeom="cyl=940, heads=6, spt=17";;
+      52125696) hdGeom="cyl=101, heads=16, spt=63";;
+      65454080) hdGeom="cyl=940, heads=8, spt=17";;
+     104767488) hdGeom="cyl=203, heads=16, spt=63";;
+     117504000) hdGeom="cyl=900, heads=15, spt=17";;
+     132120576) hdGeom="cyl=256, heads=16, spt=63";;
+     209534976) hdGeom="cyl=406, heads=16, spt=63";;
+     495452160) hdGeom="cyl=1024, heads=15, spt=63";;
+     523837440) hdGeom="cyl=1015, heads=16, spt=63";;
+     528482304) hdGeom="cyl=1024, heads=16, spt=63";;
+     670924800) hdGeom="cyl=1300, heads=16, spt=63";;
+     928972800) hdGeom="cyl=1800, heads=16, spt=63";;
+    1073479680) hdGeom="cyl=2080, heads=16, spt=63";;
+    1341849600) hdGeom="cyl=2600, heads=16, spt=63";;
+    2147475456) hdGeom="cyl=4161, heads=16, spt=63";;
+    2322432000) hdGeom="cyl=4500, heads=16, spt=63";;
+    3664281600) hdGeom="cyl=7100, heads=16, spt=63";;
+#    5020M) hdGeom="cyl=10200, heads=16, spt=63";;
+#    7088M) hdGeom="cyl=14400, heads=16, spt=63";;
+#   10041M) hdGeom="cyl=20400, heads=16, spt=63";;
+#   12206M) hdGeom="cyl=24800, heads=16, spt=63";;
+#   16045M) hdGeom="cyl=32600, heads=16, spt=63";;
+#   20672M) hdGeom="cyl=42000, heads=16, spt=63";;
+#   25003M) hdGeom="cyl=50800, heads=16, spt=63";;
+#   29974M) hdGeom="cyl=60900, heads=16, spt=63";;
+#   32238M) hdGeom="cyl=65500, heads=16, spt=63";;
+          *) echo "error: image $name has illegal size $size"; exit 3;;
+  esac
+}
+
+# create bochs disk-images   --------------------------------------------------
+
+createDisk() {
+  local bs count name=$1 size=$2
+  case $size in
+	0M) return;;
+       10|10M) bs=4k; count=2601;;
+       20|20M) bs=10k; count=2091;;
+       30|30M) bs=45k; count=697;;
+       46|46M) bs=60k; count=799;;
+       62|62M) bs=80k; count=799;;
+      112|112M) bs=270k; count=425;;
+      126|126M) bs=2048k; count=63;;
+      483|483M) bs=2560k; count=189;;
+      504|504M) bs=4096k; count=126;;
+      640|640M) bs=2400k; count=273;;
+      886|886M) bs=4800k; count=189;;
+     1280|1280M) bs=4800k; count=273;;
+     2215|2215M) bs=4000k; count=567;;
+     3495|3495M) bs=7200k; count=497;;
+          *) echo "error: illegal size $size"; exit 3;;
+  esac
+  echo -e "creating $name.$size"
+  dd if=/dev/zero of=$name.$size bs=$bs count=$count
+}
+
+# set floppy-size   -----------------------------------------------------------
+
+setFloppySize() {
+  local name=$1
+  if [ "$name" != "/dev/fd0" ]; then
+    if [ ! -f $name ]; then
+      echo "error: floppy-image $name does not exist"
+      exit 3
+    fi
+    floppySize=`find $name -printf "%s"`
+    if [ $floppySize -eq 1474560 ]; then
+      floppySize="1_44"
+    elif [ $floppySize -eq 2949120 ]; then
+      floppySize="2_88"
+    else
+      echo "error: invalid size for floppy-image"
+      exit 3
+    fi
+  fi
+}
+
+# main program   --------------------------------------------------------------
+
+setDefaults
+while getopts ":b:f:1:c:2:C:h" opt; do
+   case $opt in
+      b) boot="boot: $OPTARG";;
+      f) floppyPath=$OPTARG;;
+      1) hdaPath=$OPTARG;;
+      c) cdromPath=$OPTARG; hdbPath="";;
+      2) hdbPath=$OPTARG; cdromPath="";;
+      C) createDisk "hd" $OPTARG;exit 0;;
+      h) usage;;
+      ?) usage;;
+   esac
+done
+let OPTIND-=1
+shift $OPTIND
+
+setFloppySize $floppyPath
+floppyArg="floppya: $floppySize=$floppyPath, status=inserted"
+
+if [ ! -z "$hdaPath" ]; then
+  if [ ! -f "$hdaPath" ]; then
+    echo "error: disk-image $hdaPath does not exist!"
+    exit 3
+  fi
+  getHDGeom $hdaPath
+  hdaArg="diskc: file=$hdaPath, $hdGeom"
+fi
+
+if [ ! -z "$hdbPath" ]; then
+  if [ ! -f "$hdbPath" ]; then
+    echo "error: disk-image $hdbPath does not exist!"
+    exit 3
+  fi
+  getHDGeom $hdbPath
+  hdbArg="diskd: file=$hdbPath, $hdGeom"
+fi
+
+if [ ! -z "$cdromPath" ]; then
+  if [ "$cdromPath" != "/dev/cdrom" -a ! -f $cdromPath ]; then
+    echo "error: cdrom-image $cdromPath does not exist"
+    exit 3
+  fi
+  cdromArg="cdromd: dev=$cdromPath, status=inserted"
+fi
+
+${BOCHS} -nocp "$boot" "$floppyArg" "$hdaArg" "$hdbArg" "$cdromArg" "$@" &
