#!/bin/sh

LOCAL_PATH=`dirname $0`
LOCAL_PATH=`cd $LOCAL_PATH && pwd`

export PATH=$LOCAL_PATH/..:$PATH # For our custom sdl-config

if [ \! -f bochs/configure ] ; then
	sh -c "cd bochs && ./bootstrap.sh"
fi

mkdir -p bin-$1

if [ \! -f bin-$1/Makefile ] ; then
	env CFLAGS="-Ofast -ffast-math" \
	env LIBS="-lgnustl_static" \
		../setEnvironment-$1.sh sh -c "cd bin-$1 && ../bochs/configure \
		--build=x86_64-unknown-linux-gnu --host=$2 \
		--with-sdl \
		--enable-cpu-level=6 --enable-smp --enable-x86-64 --enable-avx \
		--enable-sb16 --enable-es1370 \
		--enable-ne2000 --enable-ne1000 \
		--enable-clgd54xx --enable-voodoo \
		--enable-all-optimizations \
		--enable-usb --enable-usb-ohci \
		--disable-gameport --disable-disasm --disable-docbook" || exit 1
fi


# Fix a compilation error
mkdir -p bin-$1/iodev/network/slirp


make -j4 -C bin-$1 && mv -f bin-$1/bochs libapplication-$1.so
