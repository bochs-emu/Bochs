CXX=g++
CXXFLAGS=-Wall -g
#CXXFLAGS=-Wall -g -mno-cygwin
LIBTOOL=libtool
RPATH=`pwd`/lib

all: uselib

uselib: uselib.cc libmodule1.la
	$(LIBTOOL) $(CXX) $(CXXFLAGS) -o uselib uselib.cc libmodule1.la

libmodule1.la: module1.lo
	$(LIBTOOL) $(CXX) -no-undefined -o libmodule1.la module1.lo -rpath ${RPATH}
	mkdir -p lib bin
	$(LIBTOOL) cp libmodule1.la ${RPATH}

%.lo: %.cc %.h
	$(LIBTOOL) $(CXX) -c $<

clean:
	-libtool rm libmodule1.la module1.lo
	rm -rf *.o uselib.exe bin lib uselib uselib.exe
	rm -rf .libs
