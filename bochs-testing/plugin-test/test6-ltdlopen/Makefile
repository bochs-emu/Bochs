top_builddir = ..
srcdir = .


CXX=g++
CXXFLAGS=-g -O2
LDFLAGS= -no-undefined
LIBS=-L`pwd`/../ltdl -lltdl
LIBTOOL=$(SHELL) $(top_builddir)/libtool
RPATH=`pwd`/lib

all: uselib libmodule1.la ### libmodule2.la

uselib: libmain.la
	$(LIBTOOL) $(CXX) $(LDFLAGS) -o uselib libmain.la $(LIBS)

libmain.la: main.lo
	$(LIBTOOL) $(CXX) $(LDFLAGS) -o libmain.la main.lo -rpath ${RPATH} $(LIBS)
	mkdir -p lib bin
	$(LIBTOOL) cp libmain.la ${RPATH}

libmodule1.la: module1.lo libmain.la
	$(LIBTOOL) $(CXX) $(LDFLAGS) -o libmodule1.la module1.lo -rpath ${RPATH} libmain.la
	mkdir -p lib bin
	$(LIBTOOL) cp libmodule1.la ${RPATH}

libmodule2.la: module2.lo libmain.la
	$(LIBTOOL) $(CXX) $(LDFLAGS) -o libmodule2.la module2.lo -rpath ${RPATH} libmain.la
	mkdir -p lib bin
	$(LIBTOOL) cp libmodule2.la ${RPATH}

%.lo: %.cc
	$(LIBTOOL) $(CXX) $(CXXFLAGS) -c $<

test:
	@echo "*** Running test in `pwd`"
	-./uselib
	@echo "*** Test done in `pwd`"

clean:
	-$(LIBTOOL) rm libmodule1.la module1.lo libmodule2.la module2.lo
	rm -rf *.o *.lo *.la uselib.exe bin lib uselib uselib.exe
	rm -rf .libs
