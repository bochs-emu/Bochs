#!/bin/sh
#
# $Id$
#
# Make a DMG of Bochs.  This script must be run from the main source 
# directory, e.g. "./build/macosx/make-dmg.sh".  If you haven't run
# configure yet, it runs .conf.macosx for you.  Then it creates a 
# temporary directory _dmg_top and does a make install into that
# directory, and builds a disk image.  At the end it cleans up the
# temporary directory.
#

VERSION=@VERSION@    # substituted in with configure script
VERSION=2.0.pre4
BUILDROOT=./_dmg_top
INSTALL_PREFIX=$BUILDROOT/Bochs-${VERSION}
DMG=./Bochs-${VERSION}.dmg

# test if we're in the right directory.  if not, bomb.
echo '-- Is the script run from the right directory?'
if test -f main.cc -a -f bochs.h; then 
  echo yes
else
  echo no
  echo ERROR: Run it from the top of the Bochs source tree, where bochs.h is found.
  exit 10
fi

# test if configure has been run already.  if not, run .conf.macosx.
configured=0
echo '-- Has configure been run already?'
if test -f config.h -a -f Makefile; then
  echo yes
else
  echo no.  I will run .conf.macosx now.
  /bin/sh -x .conf.macosx
  conf_retcode=$?
  configured=1
  if test "$conf_retcode" != 0; then
    echo ERROR: configure failed. Correct errors in .conf.macosx and try again.
    exit 20
  fi 
fi

# remove any leftovers from previous image creation.
echo "-- Removing leftovers from previous runs"
rm -rf ${BUILDROOT} ${BUILDROOT}.dmg ${DMG}

# make new buildroot directory
echo "-- Making ${BUILDROOT} directory"
mkdir ${BUILDROOT} && mkdir ${INSTALL_PREFIX}
if test $? != 0; then 
  echo ERROR: mkdir ${BUILDROOT} or mkdir ${INSTALL_PREFIX} failed
  exit 30
fi

# run make and then make install into it
echo "-- Running make"
make
if test $? != 0; then
  echo ERROR: make failed
  exit 40
fi

echo "-- Running make install with prefix=${INSTALL_PREFIX}"
make install prefix=${INSTALL_PREFIX}
if test $? != 0; then
  echo ERROR: make install with prefix=${INSTALL_PREFIX} failed
  exit 50
fi

# create new disk image
echo "-- Making a disk image with root at ${BUILDROOT}, using diskimage.pl"
./build/macosx/diskimage.pl ${BUILDROOT}
if test $? != 0; then
  echo ERROR: diskimage.pl script failed
  exit 60
fi

if test ! -f ${BUILDROOT}.dmg; then
  echo ERROR: diskimage.pl succeeded but I cannot find the image ${BUILDROOT}.dmg.
  exit 70
fi

# rename to the right thing
echo "-- Renaming the output disk image to ${DMG}"
mv ${BUILDROOT}.dmg ${DMG}
if test $? != 0; then
  echo ERROR: rename failed
  exit 80
fi

echo "-- Done!  The final disk image is "
ls -l ${DMG}

echo "-- Cleaning up the temporary files in ${BUILDROOT}"
rm -rf ${BUILDROOT}

exit 0
