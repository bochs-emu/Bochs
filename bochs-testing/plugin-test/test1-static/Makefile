CXX=g++
CXXFLAGS=-Wall -g
LIBTOOL=libtool

all: uselib

uselib: uselib.cc libmodule1.la
	$(LIBTOOL) $(CXX) $(CXXFLAGS) -o uselib uselib.cc libmodule1.la

libmodule1.la: module1.lo
	$(LIBTOOL) $(CXX) -o libmodule1.la module1.lo

%.lo: %.cc
	$(LIBTOOL) $(CXX) -c $<

clean: 
	-libtool rm libmodule1.la module1.lo uselib.exe uselib
	rm -rf .libs
