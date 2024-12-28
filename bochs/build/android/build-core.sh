#!/bin/sh

LOCAL_PATH=`dirname $0`
LOCAL_PATH=`cd $LOCAL_PATH && pwd`

if [ \! -d bochs/bochs ] ; then
   ln -s ../../../../bochs bochs
fi

if [ \! -d ../../../androidsdl/project/jni/application/bochs ] ; then
   ln -s ../../../../bochs/build/android/bochs ../../../androidsdl/project/jni/application
fi

cd ../../../androidsdl
./build.sh -s release bochs
mv project/app/build/outputs/apk/release/app-release.apk ../bochs/build/android/bochs-core-debug.apk
cd ../bochs/build/android
