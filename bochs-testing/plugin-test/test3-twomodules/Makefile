CXX=g++
CXXFLAGS=-Wall -g
LIBTOOL=libtool
RPATH=`pwd`/lib

all: uselib

uselib: uselib.cc libmodule1.la libmodule2.la
	$(LIBTOOL) $(CXX) $(CXXFLAGS) -o uselib uselib.cc libmodule1.la libmodule2.la

libmodule1.la: module1.lo
	$(LIBTOOL) $(CXX) -no-undefined -o libmodule1.la module1.lo -rpath ${RPATH}
	mkdir -p lib bin
	$(LIBTOOL) cp libmodule1.la ${RPATH}

libmodule2.la: module2.lo
	$(LIBTOOL) $(CXX) -no-undefined -o libmodule2.la module2.lo -rpath ${RPATH}
	mkdir -p lib bin
	$(LIBTOOL) cp libmodule2.la ${RPATH}

%.lo: %.cc
	$(LIBTOOL) $(CXX) -c $<

clean:
	-libtool rm libmodule1.la module1.lo libmodule2.la module2.lo
	rm -rf *.o uselib.exe bin lib uselib uselib.exe
	rm -rf .libs
