#!/bin/sh

LOCAL_PATH=`dirname $0`
LOCAL_PATH=`cd $LOCAL_PATH && cd .. && pwd`
SDL_LIBDIR=`cd $LOCAL_PATH && cd ../../libs/$1 && pwd`

export PATH=$LOCAL_PATH:$PATH # For our custom sdl-config

if [ \! -f bochs/configure ] ; then
	sh -c "cd bochs && ./bootstrap.sh"
fi

mkdir -p bin-$1

if [ "$1" = armeabi-v7a ] || [ "$1" = x86 ]; then
    export ARCH_CFLAG="-DANDROID_32BIT"
fi
if [ "$1" = arm64-v8a ] || [ "$1" = x86_64 ]; then
    export ARCH_CFLAG="-DANDROID_ARM64"
fi


if [ \! -f bin-$1/Makefile ] ; then
	env CFLAGS="-Ofast -Wno-narrowing -DANDROID $ARCH_CFLAG" \
    env CLANG=1 \
		$ANDROID_SDL_HOME/project/jni/application/setEnvironment-$1.sh sh -c "cd bin-$1 && ../bochs/configure \
		--build=x86_64-unknown-linux-gnu --host=$2 --with-sdl --exec-prefix=$SDL_LIBDIR \
		--enable-cpu-level=6 --enable-smp --enable-3dnow --enable-x86-64 \
        	--enable-vmx=2 --enable-avx --enable-evex \
		--enable-sb16 --enable-es1370 \
		--enable-ne2000 --enable-pnic --enable-e1000 \
		--enable-clgd54xx --enable-voodoo \
		--enable-all-optimizations \
		--enable-usb --enable-usb-ohci --enable-usb-ehci --enable-usb-xhci \
		--enable-busmouse --enable-large-ramfile \
		--disable-gameport --disable-docbook" || exit 1
fi

if [ -f bin-$1/gui/Makefile ]; then
       sed -i -u 's/-D_FILE_OFFSET_BITS=64//' bin-$1/gui/Makefile
fi

if [ -f bin-$1/memory/Makefile ]; then
       sed -i -u 's/-D_FILE_OFFSET_BITS=64//' bin-$1/memory/Makefile
fi

# Fix a compilation error
#mkdir -p bin-$1/iodev/network/slirp
#mkdir -p bin-$1/cpu/decoder

make -j4 -C bin-$1 && mv -f bin-$1/bochs libapplication-$1.so
