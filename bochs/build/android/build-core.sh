#!/bin/sh

LOCAL_PATH=`dirname $0`
LOCAL_PATH=`cd $LOCAL_PATH && pwd`

if [ \! -d bochs/bochs ] ; then
   ln -s $ANDROID_BOCHS_HOME bochs
fi

if [ \! -d $ANDROID_SDL_HOME/project/jni/application/bochs ] ; then
   ln -s $ANDROID_BOCHS_HOME/build/android/bochs $ANDROID_SDL_HOME/project/jni/application
fi

cd $ANDROID_SDL_HOME
./build.sh -s release bochs
mv project/app/build/outputs/apk/release/app-release.apk $ANDROID_BOCHS_HOME/build/android/bochs-core-debug.apk
cd $ANDROID_BOCHS_HOME/build/android
