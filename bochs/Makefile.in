# Copyright (C) 2001  The Bochs Project
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
#
####################################################
# NOTE: To be compatibile with nmake (microsoft vc++) please follow
# the following rules:
#   use $(VAR) not ${VAR}

prefix          = @prefix@
exec_prefix     = @exec_prefix@
srcdir          = @srcdir@
VPATH = @srcdir@
bindir          = @bindir@
libdir          = @libdir@
plugdir         = @libdir@/bochs/plugins
datarootdir     = @datarootdir@
mandir          = @mandir@
man1dir         = $(mandir)/man1
man5dir         = $(mandir)/man5
docdir          = $(datarootdir)/doc/bochs
sharedir        = $(datarootdir)/bochs
top_builddir    = .
top_srcdir      = $(srcdir)

DESTDIR =

VERSION=@VERSION@
VER_STRING=@VER_STRING@
REL_STRING=@REL_STRING@
MAN_PAGE_1_LIST=bochs bximage bxcommit bochs-dlx
MAN_PAGE_5_LIST=bochsrc
INSTALL_LIST_SHARE=bios/BIOS-bochs-* bios/VGABIOS* @INSTALL_LIST_FOR_PLATFORM@
INSTALL_LIST_DOC=CHANGES COPYING README TODO
INSTALL_LIST_BIN=bochs@EXE@ bximage@EXE@ bxcommit@EXE@
INSTALL_LIST_BIN_OPTIONAL=bochsdbg@EXE@
INSTALL_LIST_WIN32=$(INSTALL_LIST_SHARE) $(INSTALL_LIST_DOC) $(INSTALL_LIST_BIN) $(INSTALL_LIST_BIN_OPTIONAL) niclist@EXE@
INSTALL_LIST_MACOSX=$(INSTALL_LIST_SHARE) $(INSTALL_LIST_DOC) bochs.scpt
# for win32 and macosx, these files get renamed to *.txt in install process
TEXT_FILE_LIST=README CHANGES COPYING TODO VGABIOS-elpin-LICENSE VGABIOS-lgpl-README
CP=cp
CAT=cat
RM=rm
MV=mv
LN_S=ln -sf
DLXLINUX_TAR=dlxlinux4.tar.gz
DLXLINUX_TAR_URL=http://bochs.sourceforge.net/guestos/$(DLXLINUX_TAR)
DLXLINUX_ROMFILE=BIOS-bochs-latest
GUNZIP=gunzip
WGET=@WGET@
SED=sed
MKDIR=mkdir
RMDIR=rmdir
TAR=tar
CHMOD=chmod
# the GZIP variable is reserved by gzip program
GZIP_BIN=gzip -9
GUNZIP=gunzip
ZIP=zip
UNIX2DOS=unix2dos
LIBTOOL=@LIBTOOL@
DLLTOOL=dlltool
RC_CMD=@RC_CMD@

@SUFFIX_LINE@

srcdir = @srcdir@
VPATH = @srcdir@

SHELL = /bin/sh

@SET_MAKE@

CC = @CC@
CXX = @CXX@
CFLAGS = @CFLAGS@ @GUI_CFLAGS@ $(MCH_CFLAGS) $(FLA_FLAGS) @DEFINE_PLUGIN_PATH@ -DBX_SHARE_PATH='"$(sharedir)"'
CXXFLAGS = @CXXFLAGS@ @GUI_CXXFLAGS@ $(MCH_CFLAGS) $(FLA_FLAGS) @DEFINE_PLUGIN_PATH@ -DBX_SHARE_PATH='"$(sharedir)"'

LDFLAGS = @LDFLAGS@
LIBS = @LIBS@
# To compile with readline:
#   linux needs just -lreadline
#   solaris needs -lreadline -lcurses
X_LIBS = @X_LIBS@
X_PRE_LIBS = @X_PRE_LIBS@
XPM_LIB = @XPM_LIB@
GUI_LINK_OPTS_X = $(X_LIBS) $(X_PRE_LIBS) -lX11 $(XPM_LIB)
GUI_LINK_OPTS_SDL = `sdl-config --cflags --libs`
GUI_LINK_OPTS_SVGA =  -lvga -lvgagl
GUI_LINK_OPTS_BEOS = -lbe
GUI_LINK_OPTS_RFB = @RFB_LIBS@
GUI_LINK_OPTS_AMIGAOS =
GUI_LINK_OPTS_WIN32 = -luser32 -lgdi32 -lcomdlg32 -lcomctl32 -lwsock32 -lshell32
GUI_LINK_OPTS_WIN32_VCPP = user32.lib gdi32.lib winmm.lib \
  comdlg32.lib comctl32.lib wsock32.lib advapi32.lib shell32.lib
GUI_LINK_OPTS_MACOS =
GUI_LINK_OPTS_CARBON = -framework Carbon
GUI_LINK_OPTS_NOGUI =
GUI_LINK_OPTS_TERM = @GUI_LINK_OPTS_TERM@
GUI_LINK_OPTS_WX = @GUI_LINK_OPTS_WX@
GUI_LINK_OPTS = @GUI_LINK_OPTS@  @DEVICE_LINK_OPTS@
RANLIB = @RANLIB@

CFLAGS_CONSOLE = @CFLAGS@ $(MCH_CFLAGS) $(FLA_FLAGS)
CXXFLAGS_CONSOLE = @CXXFLAGS@ $(MCH_CFLAGS) $(FLA_FLAGS)
BXIMAGE_LINK_OPTS = @BXIMAGE_LINK_OPTS@

BX_INCDIRS = -I. -I$(srcdir)/. -I@INSTRUMENT_DIR@ -I$(srcdir)/@INSTRUMENT_DIR@

#SUBDIRS = iodev bx_debug

#all install uninstall: config.h#
#        for subdir in $(SUBDIRS); do #
#          echo making $@ in $$subdir; #
#          ($(MAKE) -C $$subdir $(MDEFINES) $@) || exit 1; #
#        done#



# gnu flags for clean up
#CFLAGS  = -ansi -O -g -Wunused -Wuninitialized


NONINLINE_OBJS = \
	logio.o \
	main.o \
	config.o \
	load32bitOShack.o \
	pc_system.o \
	osdep.o \
	plugin.o \
	crc.o \
	@EXTRA_BX_OBJS@

EXTERN_ENVIRONMENT_OBJS = \
	main.o \
	config.o \
	load32bitOShack.o \
	pc_system.o

DEBUGGER_LIB   = bx_debug/libdebug.a
DISASM_LIB     = disasm/libdisasm.a
INSTRUMENT_LIB = @INSTRUMENT_DIR@/libinstrument.a
FPU_LIB        = fpu/libfpu.a
READLINE_LIB   = @READLINE_LIB@
EXTRA_LINK_OPTS = @EXTRA_LINK_OPTS@

GDBSTUB_OBJS = gdbstub.o

BX_OBJS = @NONINLINE_VAR@

BX_INCLUDES = bochs.h config.h osdep.h


.@CPP_SUFFIX@.o:
	$(CXX) @DASH@c $(BX_INCDIRS) $(CXXFLAGS) @CXXFP@$< @OFP@$@
.c.o:
	$(CC) @DASH@c $(BX_INCDIRS) $(CFLAGS) $(FPU_FLAGS) $< @OFP@$@


all: @PRIMARY_TARGET@ @PLUGIN_TARGET@ bximage@EXE@ bxcommit@EXE@ @BUILD_DOCBOOK_VAR@

@EXTERNAL_DEPENDENCY@

bochs@EXE@: @IODEV_LIB_VAR@ @DEBUGGER_VAR@ \
           cpu/libcpu.a memory/libmemory.a gui/libgui.a \
           @DISASM_VAR@ @INSTRUMENT_VAR@ $(BX_OBJS) \
           $(SIMX86_OBJS) @FPU_VAR@ @GDBSTUB_VAR@ @PLUGIN_VAR@
	@LINK@ @EXPORT_DYNAMIC@ $(BX_OBJS) $(SIMX86_OBJS) \
		@IODEV_LIB_VAR@ @DEBUGGER_VAR@ cpu/libcpu.a memory/libmemory.a gui/libgui.a \
		@DISASM_VAR@ @INSTRUMENT_VAR@ @PLUGIN_VAR@ \
		@GDBSTUB_VAR@ @FPU_VAR@ \
		@NONPLUGIN_GUI_LINK_OPTS@ \
		$(MCH_LINK_FLAGS) \
		$(SIMX86_LINK_FLAGS) \
		$(READLINE_LIB) \
		$(EXTRA_LINK_OPTS) \
		$(LIBS)

# Special make target for cygwin/mingw using dlltool instead of
# libtool.  This creates a .DEF file, and exports file, an import library,
# and then links bochs.exe with the exports file.
.win32_dll_plugin_target: @IODEV_LIB_VAR@ @DEBUGGER_VAR@ \
           cpu/libcpu.a memory/libmemory.a gui/libgui.a \
           @DISASM_VAR@ @INSTRUMENT_VAR@ $(BX_OBJS) \
           $(SIMX86_OBJS) @FPU_VAR@ @GDBSTUB_VAR@ @PLUGIN_VAR@
	$(DLLTOOL) --export-all-symbols --output-def bochs.def \
		$(BX_OBJS) $(SIMX86_OBJS) \
		@IODEV_LIB_VAR@ cpu/libcpu.a memory/libmemory.a gui/libgui.a \
		@DEBUGGER_VAR@ @DISASM_VAR@ @INSTRUMENT_VAR@ @PLUGIN_VAR@ \
		@GDBSTUB_VAR@ @FPU_VAR@
	$(DLLTOOL) --dllname bochs.exe --def bochs.def --output-lib dllexports.a
	$(DLLTOOL) --dllname bochs.exe --output-exp bochs.exp --def bochs.def
	$(CXX) -o bochs.exe $(CXXFLAGS) $(LDFLAGS) -export-dynamic \
	    $(BX_OBJS) bochs.exp $(SIMX86_OBJS) \
		@IODEV_LIB_VAR@ cpu/libcpu.a memory/libmemory.a gui/libgui.a \
		@DEBUGGER_VAR@ @DISASM_VAR@ @INSTRUMENT_VAR@ @PLUGIN_VAR@ \
		@GDBSTUB_VAR@ @FPU_VAR@ \
		$(GUI_LINK_OPTS) \
		$(MCH_LINK_FLAGS) \
		$(SIMX86_LINK_FLAGS) \
		$(READLINE_LIB) \
		$(EXTRA_LINK_OPTS) \
		$(LIBS)
	touch .win32_dll_plugin_target

bochs_plugins:
	cd gui @COMMAND_SEPARATOR@
	$(MAKE) plugins
	@CD_UP_ONE@
	cd iodev @COMMAND_SEPARATOR@
	$(MAKE) plugins
	@CD_UP_ONE@

bximage@EXE@: misc/bximage.o
	@LINK_CONSOLE@ $(BXIMAGE_LINK_OPTS) misc/bximage.o

bxcommit@EXE@: misc/bxcommit.o
	@LINK_CONSOLE@ misc/bxcommit.o

niclist@EXE@: misc/niclist.o
	@LINK_CONSOLE@ misc/niclist.o

# compile with console CXXFLAGS, not gui CXXFLAGS
misc/bximage.o: $(srcdir)/misc/bximage.c $(srcdir)/iodev/hdimage.h
	$(CC) @DASH@c $(BX_INCDIRS) $(CFLAGS_CONSOLE) $(srcdir)/misc/bximage.c @OFP@$@

misc/bxcommit.o: $(srcdir)/misc/bxcommit.c $(srcdir)/iodev/hdimage.h
	$(CC) @DASH@c $(BX_INCDIRS) $(CFLAGS_CONSOLE) $(srcdir)/misc/bxcommit.c @OFP@$@

misc/niclist.o: $(srcdir)/misc/niclist.c
	$(CC) @DASH@c $(BX_INCDIRS) $(CFLAGS_CONSOLE) $(srcdir)/misc/niclist.c @OFP@$@

$(BX_OBJS): $(BX_INCLUDES)

# cannot use -C option to be compatible with Microsoft nmake
iodev/libiodev.a::
	cd iodev @COMMAND_SEPARATOR@
	$(MAKE) $(MDEFINES) libiodev.a
	@CD_UP_ONE@

bx_debug/libdebug.a::
	cd bx_debug @COMMAND_SEPARATOR@
	$(MAKE) $(MDEFINES) libdebug.a
	@CD_UP_ONE@

cpu/libcpu.a::
	cd cpu @COMMAND_SEPARATOR@
	$(MAKE) $(MDEFINES) libcpu.a
	@CD_UP_ONE@

memory/libmemory.a::
	cd memory @COMMAND_SEPARATOR@
	$(MAKE) $(MDEFINES) libmemory.a
	@CD_UP_ONE@

gui/libgui.a::
	cd gui @COMMAND_SEPARATOR@
	$(MAKE) $(MDEFINES) libgui.a
	@CD_UP_ONE@

disasm/libdisasm.a::
	cd disasm @COMMAND_SEPARATOR@
	$(MAKE) $(MDEFINES) libdisasm.a
	@CD_UP_ONE@

@INSTRUMENT_DIR@/libinstrument.a::
	cd @INSTRUMENT_DIR@ @COMMAND_SEPARATOR@
	$(MAKE) $(MDEFINES) libinstrument.a
	@CD_UP_TWO@

fpu/libfpu.a::
	cd fpu @COMMAND_SEPARATOR@
	$(MAKE) $(MDEFINES) libfpu.a
	@CD_UP_ONE@

libbochs.a:
	-rm -f libbochs.a
	ar rv libbochs.a $(EXTERN_ENVIRONMENT_OBJS)
	$(RANLIB) libbochs.a

libbochs_cpu.a: @DEBUGGER_VAR@ $(BX_OBJS)
	-rm -f libbochs_cpu.a
	ar rv libbochs_cpu.a $(BX_OBJS)
	$(RANLIB) libbochs_cpu.a

# for wxWidgets port, on win32 platform
wxbochs_resources.o: wxbochs.rc
	windres $(srcdir)/wxbochs.rc -o $@ --include-dir=`@WX_CONFIG@ --prefix`/include

# for win32 gui
win32res.o: win32res.rc bxversion.rc
	$(RC_CMD)$@ $(srcdir)/win32res.rc

#####################################################################
# Install target for all platforms.
#####################################################################

install: all @INSTALL_TARGET@

#####################################################################
# Install target for win32
#
# This is intended to be run in cygwin, since it has better scripting
# tools.
#####################################################################

install_win32: download_dlx @INSTALL_DOCBOOK_VAR@
	-mkdir -p $(prefix)
	cp obj-release/*.exe .
	for i in $(INSTALL_LIST_WIN32); do if test -f $$i; then cp $$i $(prefix); else cp $(srcdir)/$$i $(prefix); fi; done
	cp $(srcdir)/misc/sb16/sb16ctrl.example $(prefix)/sb16ctrl.txt
	cp $(srcdir)/misc/sb16/sb16ctrl.exe $(prefix)
	#cat $(srcdir)/build/win32/DOC-win32.htm | $(SED) -e 's/@VERSION@/$(VERSION)/g' > $(prefix)/DOC-win32.htm
	cp $(srcdir)/.bochsrc $(prefix)/bochsrc-sample.txt
	-mkdir $(prefix)/keymaps
	cp $(srcdir)/gui/keymaps/*.map $(prefix)/keymaps
	cat $(DLXLINUX_TAR) | (cd $(prefix) && tar xzvf -)
	echo '..\bochs' > $(prefix)/dlxlinux/start.bat
	dlxrc=$(prefix)/dlxlinux/bochsrc.txt; mv $$dlxrc $$dlxrc.orig && sed < $$dlxrc.orig 's/\/usr\/local\/bochs\/latest/../' > $$dlxrc && rm -f $$dlxrc.orig
	mv $(prefix)/README $(prefix)/README.orig
	cat $(srcdir)/build/win32/README.win32-binary $(prefix)/README.orig > $(prefix)/README
	rm -f $(prefix)/README.orig
	for i in $(TEXT_FILE_LIST); do mv $(prefix)/$$i $(prefix)/$$i.txt; done
	cd $(prefix); $(UNIX2DOS) *.txt */*.txt
	cd $(prefix); NAME=`pwd|$(SED) 's/.*\///'`; (cd ..; $(ZIP) $$NAME.zip -r $$NAME); ls -l ../$$NAME.zip

#####################################################################
# install target for unix
#####################################################################

install_unix: install_bin @INSTALL_PLUGINS_VAR@ install_man install_share install_doc @INSTALL_DOCBOOK_VAR@

install_bin::
	for i in $(DESTDIR)$(bindir); do mkdir -p $$i && test -d $$i && test -w $$i; done
	for i in $(INSTALL_LIST_BIN); do if test -f $$i; then install $$i $(DESTDIR)$(bindir); else install $(srcdir)/$$i $(DESTDIR)$(bindir); fi; done
	-for i in $(INSTALL_LIST_BIN_OPTIONAL); do if test -f $$i; then install $$i $(DESTDIR)$(bindir); else install $(srcdir)/$$i $(DESTDIR)$(bindir); fi; done

install_libtool_plugins::
	for i in $(DESTDIR)$(plugdir); do mkdir -p $$i && test -d $$i && test -w $$i; done
	list=`cd gui && echo *.la`; for i in $$list; do $(LIBTOOL) --mode=install install gui/$$i $(DESTDIR)$(plugdir); done
	list=`cd iodev && echo *.la`; for i in $$list; do $(LIBTOOL) --mode=install install iodev/$$i $(DESTDIR)$(plugdir); done
	$(LIBTOOL) --finish $(DESTDIR)$(plugdir)

install_dll_plugins::
	for i in $(DESTDIR)$(plugdir); do mkdir -p $$i && test -d $$i && test -w $$i; done
	list=`cd gui && echo *.dll`; for i in $$list; do cp gui/$$i $(DESTDIR)$(plugdir); done
	list=`cd iodev && echo *.dll`; for i in $$list; do cp iodev/$$i $(DESTDIR)$(plugdir); done

install_share::
	for i in $(DESTDIR)$(sharedir);	do mkdir -p $$i && test -d $$i && test -w $$i; done
	for i in $(INSTALL_LIST_SHARE); do if test -f $$i; then install -m 644 $$i $(DESTDIR)$(sharedir); else install -m 644 $(srcdir)/$$i $(DESTDIR)$(sharedir); fi; done
	-mkdir $(DESTDIR)$(sharedir)/keymaps
	for i in $(srcdir)/gui/keymaps/*.map; do install -m 644 $$i $(DESTDIR)$(sharedir)/keymaps/; done

install_doc::
	for i in $(DESTDIR)$(docdir); do mkdir -p $$i && test -d $$i && test -w $$i; done
	for i in $(INSTALL_LIST_DOC); do if test -f $$i; then install -m 644 $$i $(DESTDIR)$(docdir); else install -m 644 $(srcdir)/$$i $(DESTDIR)$(docdir); fi; done
	$(RM) -f $(DESTDIR)$(docdir)/README
	$(CAT) $(srcdir)/build/linux/README.linux-binary $(srcdir)/README > $(DESTDIR)$(docdir)/README
	install -m 644 $(srcdir)/.bochsrc $(DESTDIR)$(docdir)/bochsrc-sample.txt


build_docbook::
	cd doc/docbook; make

dl_docbook::
	cd doc/docbook; make dl_docs

install_docbook: build_docbook
	cd doc/docbook; make install

install_man::
	-mkdir -p $(DESTDIR)$(man1dir)
	-mkdir -p $(DESTDIR)$(man5dir)
	for i in $(MAN_PAGE_1_LIST); do cat $(srcdir)/doc/man/$$i.1 | $(SED) 's/@version@/$(VERSION)/g' | $(GZIP_BIN) -c >  $(DESTDIR)$(man1dir)/$$i.1.gz; chmod 644 $(DESTDIR)$(man1dir)/$$i.1.gz; done
	for i in $(MAN_PAGE_5_LIST); do cat $(srcdir)/doc/man/$$i.5 | $(GZIP_BIN) -c >  $(DESTDIR)$(man5dir)/$$i.5.gz; chmod 644 $(DESTDIR)$(man5dir)/$$i.5.gz; done

download_dlx: $(DLXLINUX_TAR)

$(DLXLINUX_TAR):
	$(RM) -f $(DLXLINUX_TAR)
	$(WGET) $(DLXLINUX_TAR_URL)
	test -f $(DLXLINUX_TAR)

unpack_dlx: $(DLXLINUX_TAR)
	rm -rf dlxlinux
	$(GUNZIP) -c $(DLXLINUX_TAR) | $(TAR) -xvf -
	test -d dlxlinux
	(cd dlxlinux; $(MV) bochsrc.txt bochsrc.txt.orig; $(SED) -e "s/1\.1\.2/$(VERSION)/g"  -e 's,/usr/local/bochs/latest,$(prefix)/share/bochs,g' < bochsrc.txt.orig > bochsrc.txt; rm -f bochsrc.txt.orig)

install_dlx:
	$(RM) -rf $(DESTDIR)$(sharedir)/dlxlinux
	cp -r dlxlinux $(DESTDIR)$(sharedir)/dlxlinux
	$(CHMOD) 755 $(DESTDIR)$(sharedir)/dlxlinux
	$(GZIP_BIN) $(DESTDIR)$(sharedir)/dlxlinux/hd10meg.img
	$(CHMOD) 644 $(DESTDIR)$(sharedir)/dlxlinux/*
	for i in bochs-dlx; do cp $(srcdir)/build/linux/$$i $(DESTDIR)$(bindir)/$$i; $(CHMOD) 755 $(DESTDIR)$(bindir)/$$i; done

uninstall::
	$(RM) -rf $(DESTDIR)$(sharedir)
	$(RM) -rf $(DESTDIR)$(docdir)
	$(RM) -rf $(DESTDIR)$(libdir)/bochs
	for i in bochs bximage bxcommit bochs-dlx; do rm -f $(DESTDIR)$(bindir)/$$i; done
	for i in $(MAN_PAGE_1_LIST); do $(RM) -f $(man1dir)/$$i.1.gz; done
	for i in $(MAN_PAGE_5_LIST); do $(RM) -f $(man5dir)/$$i.5.gz; done

VS2008_WORKSPACE_ZIP=build/win32/vs2008ex-workspace.zip
VS2008_WORKSPACE_FILES=vs2008/bochs.sln vs2008/*.vcproj

vs2008workspace:
	zip $(VS2008_WORKSPACE_ZIP) $(VS2008_WORKSPACE_FILES)

########
# the win32_snap target is used to create a ZIP of bochs sources configured
# for VC++.  This ZIP is stuck on the website every once in a while to make
# it easier for VC++ users to compile bochs.  First, you should
# run "sh .conf.win32-vcpp" to configure the source code, then do
# "make win32_snap" to unzip the workspace files and create the ZIP.
########
win32_snap:
	unzip $(VS2008_WORKSPACE_ZIP)
	$(MAKE) zip

tar:
	NAME=`pwd|$(SED) 's/.*\///'`; (cd ..; $(RM) -f $$NAME.zip; tar cf - $$NAME | $(GZIP_BIN) > $$NAME.tar.gz); ls -l ../$$NAME.tar.gz

zip:
	NAME=`pwd|$(SED) 's/.*\///'`; (cd ..; $(RM) -f $$NAME-msvc-src.zip; $(ZIP) $$NAME-msvc-src.zip -r $$NAME -x \*CVS\* -x \*.cvsignore ); ls -l ../$$NAME-msvc-src.zip

clean:
	@RMCOMMAND@ *.o
	@RMCOMMAND@ *.a
	@RMCOMMAND@ bochs
	@RMCOMMAND@ bochs.exe
	@RMCOMMAND@ bximage
	@RMCOMMAND@ bximage.exe
	@RMCOMMAND@ bxcommit
	@RMCOMMAND@ bxcommit.exe
	@RMCOMMAND@ niclist
	@RMCOMMAND@ niclist.exe
	@RMCOMMAND@ bochs.out
	@RMCOMMAND@ bochsout.txt
	@RMCOMMAND@ bochs.exp
	@RMCOMMAND@ bochs.def
	@RMCOMMAND@ bochs.scpt
	@RMCOMMAND@ -rf bochs.app
	@RMCOMMAND@ -rf .libs
	@RMCOMMAND@ .win32_dll_plugin_target

local-dist-clean: clean
	@RMCOMMAND@ config.h config.status config.log config.cache
	@RMCOMMAND@ .dummy `find . -name '*.dsp' -o -name '*.dsw' -o -name '*.opt' -o -name '.DS_Store'`
	@RMCOMMAND@ bxversion.h build/linux/bochs-dlx _rpm_top *.rpm
	@RMCOMMAND@ build/win32/nsis/Makefile build/win32/nsis/bochs.nsi
	@RMCOMMAND@ build/macosx/Info.plist build/macosx/script_compiled.rsrc
	@RMCOMMAND@ libtool
	@RMCOMMAND@ ltdlconf.h

all-clean: clean
	cd iodev @COMMAND_SEPARATOR@
	$(MAKE) clean
	@CD_UP_ONE@
	cd bx_debug @COMMAND_SEPARATOR@
	$(MAKE) clean
	@CD_UP_ONE@
	cd cpu @COMMAND_SEPARATOR@
	$(MAKE) clean
	@CD_UP_ONE@
	cd memory @COMMAND_SEPARATOR@
	$(MAKE) clean
	@CD_UP_ONE@
	cd gui @COMMAND_SEPARATOR@
	$(MAKE) clean
	@CD_UP_ONE@
	cd disasm @COMMAND_SEPARATOR@
	$(MAKE) clean
	@CD_UP_ONE@
	cd @INSTRUMENT_DIR@ @COMMAND_SEPARATOR@
	$(MAKE) clean
	@CD_UP_TWO@
	cd misc @COMMAND_SEPARATOR@
	$(MAKE) clean
	@CD_UP_ONE@
	cd fpu @COMMAND_SEPARATOR@
	$(MAKE) clean
	@CD_UP_ONE@
	cd doc/docbook @COMMAND_SEPARATOR@
	$(MAKE) clean
	@CD_UP_TWO@
	cd host/linux/pcidev @COMMAND_SEPARATOR@
	$(MAKE) clean
	@CD_UP_THREE@

dist-clean: local-dist-clean
	cd iodev @COMMAND_SEPARATOR@
	$(MAKE) dist-clean
	@CD_UP_ONE@
	cd bx_debug @COMMAND_SEPARATOR@
	$(MAKE) dist-clean
	@CD_UP_ONE@
	cd bios @COMMAND_SEPARATOR@
	$(MAKE) dist-clean
	@CD_UP_ONE@
	cd cpu @COMMAND_SEPARATOR@
	$(MAKE) dist-clean
	@CD_UP_ONE@
	cd memory @COMMAND_SEPARATOR@
	$(MAKE) dist-clean
	@CD_UP_ONE@
	cd gui @COMMAND_SEPARATOR@
	$(MAKE) dist-clean
	@CD_UP_ONE@
	cd disasm @COMMAND_SEPARATOR@
	$(MAKE) dist-clean
	@CD_UP_ONE@
	cd @INSTRUMENT_DIR@ @COMMAND_SEPARATOR@
	$(MAKE) dist-clean
	@CD_UP_TWO@
	cd misc @COMMAND_SEPARATOR@
	$(MAKE) dist-clean
	@CD_UP_ONE@
	cd fpu @COMMAND_SEPARATOR@
	$(MAKE) dist-clean
	@CD_UP_ONE@
	cd doc/docbook @COMMAND_SEPARATOR@
	$(MAKE) dist-clean
	@CD_UP_TWO@
	cd host/linux/pcidev @COMMAND_SEPARATOR@
	$(MAKE) dist-clean
	@CD_UP_THREE@
	@RMCOMMAND@ Makefile

###########################################
# Build app on MacOS X
###########################################
MACOSX_STUFF=build/macosx
MACOSX_STUFF_SRCDIR=$(srcdir)/$(MACOSX_STUFF)
APP=bochs.app
APP_PLATFORM=MacOS
SCRIPT_EXEC=bochs.scpt
SCRIPT_DATA=$(MACOSX_STUFF_SRCDIR)/script.data
SCRIPT_R=$(MACOSX_STUFF_SRCDIR)/script.r
SCRIPT_APPLESCRIPT=$(MACOSX_STUFF_SRCDIR)/bochs.applescript
SCRIPT_COMPILED_RSRC=$(MACOSX_STUFF)/script_compiled.rsrc
REZ=/Developer/Tools/Rez
CPMAC=/Developer/Tools/CpMac
RINCLUDES=/System/Library/Frameworks/Carbon.framework/Libraries/RIncludes
REZ_ARGS=-append -i $RINCLUDES -d SystemSevenOrLater=1 -useDF
STANDALONE_LIBDIR=`pwd`/$(APP)/Contents/$(APP_PLATFORM)/lib
OSACOMPILE=/usr/bin/osacompile
SETFILE=/Developer/Tools/Setfile

# On a MacOS X machine, you run rez, osacompile, and setfile to
# produce the script executable, which has both a data fork and a
# resource fork.  Ideally, we would just recompile the whole
# executable at build time, but unfortunately this cannot be done on
# the SF compile farm through an ssh connection because osacompile
# needs to be run locally for some reason.  Solution: If the script
# sources are changed, rebuild the executable on a MacOSX machine,
# split it into its data and resource forks and check them into CVS
# as separate files.  Then at release time, all that's left to do is
# put the data and resource forks back together to make a working script.
# (This can be done through ssh.)
#
# Sources:
# 1. script.r: resources for the script
# 2. script.data: binary data for the script
# 3. bochs.applescript: the source of the script
#
# NOTE: All of this will fail if you aren't building on an HFS+
# filesystem!  On the SF compile farm building in your user directory
# will fail, while doing the build in /tmp will work ok.

# check if this filesystem supports resource forks at all
test_hfsplus:
	$(RM) -rf test_hfsplus
	echo data > test_hfsplus
	# if you get "Not a directory", then this filesystem doesn't support resources
	echo resource > test_hfsplus/rsrc
	# test succeeded
	$(RM) -rf test_hfsplus

# Step 1 (must be done locally on MacOSX, only when sources change)
# Compile and pull out just the resource fork.  The resource fork is
# checked into CVS as script_compiled.rsrc.  Note that we don't need
# to check in the data fork of tmpscript because it is identical to the
# script.data input file.
$(SCRIPT_COMPILED_RSRC): $(SCRIPT_R) $(SCRIPT_APPLESCRIPT)
	$(RM) -f tmpscript
	$(CP) -f $(SCRIPT_DATA) tmpscript
	$(REZ) -append $(SCRIPT_R) -o tmpscript
	$(OSACOMPILE) -o tmpscript $(SCRIPT_APPLESCRIPT)
	$(CP) tmpscript/rsrc $(SCRIPT_COMPILED_RSRC)
	$(RM) -f tmpscript

# Step 2 (can be done locally or remotely on MacOSX)
# Combine the data fork and resource fork, and set attributes.
$(SCRIPT_EXEC): $(SCRIPT_DATA) $(SCRIPT_COMPILED_RSRC)
	rm -f $(SCRIPT_EXEC)
	$(CP) $(SCRIPT_DATA) $(SCRIPT_EXEC)
	if test ! -f $(SCRIPT_COMPILED_RSRC); then $(CP) $(srcdir)/$(SCRIPT_COMPILED_RSRC) $(SCRIPT_COMPILED_RSRC); fi
	$(CP) $(SCRIPT_COMPILED_RSRC) $(SCRIPT_EXEC)/rsrc
	$(SETFILE) -t "APPL" -c "aplt" $(SCRIPT_EXEC)

$(APP)/.build: bochs test_hfsplus $(SCRIPT_EXEC)
	rm -f $(APP)/.build
	$(MKDIR) -p $(APP)
	$(MKDIR) -p $(APP)/Contents
	$(CP) -f $(MACOSX_STUFF)/Info.plist $(APP)/Contents
	$(CP) -f $(MACOSX_STUFF_SRCDIR)/pbdevelopment.plist $(APP)/Contents
	echo -n "APPL????"  > $(APP)/Contents/PkgInfo
	$(MKDIR) -p $(APP)/Contents/$(APP_PLATFORM)
	$(CP) bochs $(APP)/Contents/$(APP_PLATFORM)
	$(MKDIR) -p $(APP)/Contents/Resources
	$(REZ) $(REZ_ARGS) $(MACOSX_STUFF_SRCDIR)/bochs.r -o $(APP)/Contents/Resources/bochs.rsrc
	$(CP) -f $(MACOSX_STUFF_SRCDIR)/bochs-icn.icns $(APP)/Contents/Resources
	ls -ld $(APP) $(SCRIPT_EXEC) $(SCRIPT_EXEC)/rsrc
	touch $(APP)/.build

$(APP)/.build_plugins: $(APP)/.build bochs_plugins
	rm -f $(APP)/.build_plugins
	$(MKDIR) -p $(STANDALONE_LIBDIR);
	list=`cd gui && echo *.la`; for i in $$list; do $(LIBTOOL) cp gui/$$i $(STANDALONE_LIBDIR); done;
	list=`cd iodev && echo *.la`; for i in $$list; do $(LIBTOOL) cp iodev/$$i $(STANDALONE_LIBDIR); done;
	$(LIBTOOL) --finish $(STANDALONE_LIBDIR);
	touch $(APP)/.build_plugins

install_macosx: all download_dlx install_man @INSTALL_DOCBOOK_VAR@
	-mkdir -p $(DESTDIR)$(sharedir)
	for i in $(INSTALL_LIST_MACOSX); do if test -e $$i; then $(CPMAC) -r $$i $(DESTDIR)$(sharedir); else $(CPMAC) -r $(srcdir)/$$i $(DESTDIR)$(sharedir); fi; done
	$(CPMAC) $(srcdir)/.bochsrc $(DESTDIR)$(sharedir)/bochsrc-sample.txt
	-mkdir $(DESTDIR)$(sharedir)/keymaps
	$(CPMAC) $(srcdir)/gui/keymaps/*.map $(DESTDIR)$(sharedir)/keymaps
	cat $(DLXLINUX_TAR) | (cd $(DESTDIR)$(sharedir) && tar xzvf -)
	dlxrc=$(DESTDIR)$(sharedir)/dlxlinux/bochsrc.txt; mv "$$dlxrc" "$$dlxrc.orig" && sed < "$$dlxrc.orig" 's/\/usr\/local\/bochs\/latest/../' > "$$dlxrc" && rm -f "$$dlxrc.orig"
	mv $(srcdir)/README $(srcdir)/README.orig
	cat $(srcdir)/build/macosx/README.macosx-binary $(srcdir)/README.orig > $(DESTDIR)$(sharedir)/README
	rm -f $(DESTDIR)$(sharedir)/README.orig
	$(CPMAC) $(SCRIPT_EXEC) $(DESTDIR)$(sharedir)/dlxlinux
#	for i in $(TEXT_FILE_LIST); do mv $(srcdir)/$$i $(DESTDIR)$(sharedir)/$$i.txt; done

###########################################
# BeOS make target.
# Build the binary normally, then copy the resource attributes.
###########################################
.bochs_beos_target: bochs@EXE@
	unzip $(srcdir)/build/beos/resource.zip
	copyattr -t ICON BeBochs.rsrc bochs
	copyattr -t MICN BeBochs.rsrc bochs

###########################################
# dependencies generated by
#  gcc -MM -I. -Iinstrument/stubs *.cc | sed -e 's/\.cc/.@CPP_SUFFIX@/g' -e 's,cpu/,cpu/,g'
###########################################
config.o: config.@CPP_SUFFIX@ bochs.h config.h osdep.h bx_debug/debug.h config.h \
  osdep.h bxversion.h gui/siminterface.h param_names.h memory/memory.h pc_system.h \
  plugin.h extplugin.h ltdl.h gui/gui.h instrument/stubs/instrument.h \
  iodev/iodev.h bochs.h iodev/vga.h
crc.o: crc.@CPP_SUFFIX@ config.h
gdbstub.o: gdbstub.@CPP_SUFFIX@ bochs.h config.h osdep.h bx_debug/debug.h config.h \
  osdep.h bxversion.h gui/siminterface.h memory/memory.h pc_system.h \
  plugin.h extplugin.h ltdl.h gui/gui.h instrument/stubs/instrument.h \
  cpu/cpu.h cpu/crregs.h cpu/descriptor.h cpu/instr.h cpu/lazy_flags.h \
  cpu/icache.h cpu/apic.h cpu/i387.h fpu/softfloat.h fpu/tag_w.h \
  fpu/status_w.h fpu/control_w.h cpu/xmm.h cpu/stack.h iodev/iodev.h \
  bochs.h iodev/vga.h
load32bitOShack.o: load32bitOShack.@CPP_SUFFIX@ bochs.h config.h osdep.h \
  bx_debug/debug.h config.h osdep.h bxversion.h gui/siminterface.h param_names.h \
  memory/memory.h pc_system.h plugin.h extplugin.h ltdl.h gui/gui.h \
  instrument/stubs/instrument.h cpu/cpu.h cpu/crregs.h cpu/descriptor.h \
  cpu/instr.h cpu/lazy_flags.h cpu/icache.h cpu/apic.h cpu/i387.h \
  fpu/softfloat.h fpu/tag_w.h fpu/status_w.h fpu/control_w.h cpu/xmm.h \
  iodev/iodev.h bochs.h iodev/vga.h
logio.o: logio.@CPP_SUFFIX@ bochs.h config.h osdep.h bx_debug/debug.h config.h \
  osdep.h bxversion.h gui/siminterface.h memory/memory.h pc_system.h \
  plugin.h extplugin.h ltdl.h gui/gui.h instrument/stubs/instrument.h \
  cpu/cpu.h cpu/crregs.h cpu/descriptor.h cpu/instr.h cpu/lazy_flags.h \
  cpu/icache.h cpu/apic.h cpu/i387.h fpu/softfloat.h fpu/tag_w.h \
  fpu/status_w.h fpu/control_w.h cpu/xmm.h iodev/iodev.h bochs.h \
  iodev/vga.h
main.o: main.@CPP_SUFFIX@ bochs.h config.h osdep.h bx_debug/debug.h config.h \
  osdep.h bxversion.h gui/siminterface.h param_names.h memory/memory.h pc_system.h \
  plugin.h extplugin.h ltdl.h gui/gui.h instrument/stubs/instrument.h \
  gui/textconfig.h cpu/cpu.h cpu/crregs.h cpu/descriptor.h cpu/instr.h \
  cpu/lazy_flags.h cpu/icache.h cpu/apic.h cpu/i387.h fpu/softfloat.h \
  fpu/tag_w.h fpu/status_w.h fpu/control_w.h cpu/xmm.h iodev/iodev.h \
  bochs.h iodev/vga.h
osdep.o: osdep.@CPP_SUFFIX@ bochs.h config.h osdep.h bx_debug/debug.h config.h \
  osdep.h bxversion.h gui/siminterface.h memory/memory.h pc_system.h \
  plugin.h extplugin.h ltdl.h gui/gui.h instrument/stubs/instrument.h
pc_system.o: pc_system.@CPP_SUFFIX@ bochs.h config.h osdep.h bx_debug/debug.h \
  config.h osdep.h bxversion.h gui/siminterface.h memory/memory.h \
  pc_system.h plugin.h extplugin.h ltdl.h gui/gui.h \
  instrument/stubs/instrument.h cpu/cpu.h cpu/crregs.h cpu/descriptor.h \
  cpu/instr.h cpu/lazy_flags.h cpu/icache.h cpu/apic.h cpu/i387.h \
  fpu/softfloat.h fpu/tag_w.h fpu/status_w.h fpu/control_w.h cpu/xmm.h \
  iodev/iodev.h bochs.h iodev/vga.h
plex86-interface.o: plex86-interface.@CPP_SUFFIX@ bochs.h config.h osdep.h \
  bx_debug/debug.h config.h osdep.h bxversion.h gui/siminterface.h \
  memory/memory.h pc_system.h plugin.h extplugin.h ltdl.h gui/gui.h \
  instrument/stubs/instrument.h plex86-interface.h plex86/plex86.h \
  plex86/descriptor.h
plugin.o: plugin.@CPP_SUFFIX@ bochs.h config.h osdep.h bx_debug/debug.h config.h \
  osdep.h bxversion.h gui/siminterface.h memory/memory.h pc_system.h \
  plugin.h extplugin.h ltdl.h gui/gui.h instrument/stubs/instrument.h \
  iodev/iodev.h bochs.h iodev/vga.h
