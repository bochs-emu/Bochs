top_builddir = ..
top_srcdir = ./..
srcdir = .


CXX=g++
CXXFLAGS=-mno-cygwin -I${top_srcdir}/libltdl -ggdb
LDFLAGS=-mno-cygwin -no-undefined -ggdb
LIBS= ${top_builddir}/libltdl/.libs/libltdlc.al

LIBTOOL=$(SHELL) $(top_builddir)/libtool
RPATH=`pwd`/lib

all: uselib module1.dll module2.dll

uselib: main.cc main.h
	$(CXX) $(CXXFLAGS) -c -DDLL_EXPORT -o main.o ${srcdir}/main.cc
	dlltool --output-def uselib.def main.o
	dlltool --dllname uselib.exe --def uselib.def --output-lib uselib.a
	dlltool --dllname uselib.exe --output-exp uselib.exp --def uselib.def
	$(CXX) $(CXXFLAGS) -o uselib uselib.exp main.o ${LIBS}
	#rm uselib.exp uselib.def

module1.dll: module1.cc main.h main.cc
	$(CXX) $(CXXFLAGS) -shared -o module1.dll ${srcdir}/module1.cc uselib.a

module2.dll: module2.cc main.h main.cc
	$(CXX) $(CXXFLAGS) -shared -o module2.dll ${srcdir}/module2.cc uselib.a

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<

test:
	@echo "*** Running test in `pwd`"
	-./uselib
	@echo "*** Test done in `pwd`"

clean:
	-$(LIBTOOL) rm libmodule1.la module1.lo libmodule2.la module2.lo
	-rm -rf *.o *.lo *.la *.dll uselib.exe bin lib uselib uselib.exe
	-rm -rf .libs
