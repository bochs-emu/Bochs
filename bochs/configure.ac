dnl // Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT
AC_CONFIG_SRCDIR([bochs.h])
AC_REVISION([[$Id$]])
AC_CONFIG_HEADERS([config.h])

dnl // Put Bochs version information right here so that it gets substituted
dnl // into all the right places.
VERSION="2.8.devel"
VER_MAJOR=2
VER_MINOR=8
dnl // this should be > 90 for pre-release of next version otherwise revision level
VER_REVISION=0
dnl // this should be 0 for release and 1 for development version
VER_DEV=1
REL_STRING="Built from GitHub snapshot after release 2.8"
REL_TIMESTAMP=`grep "Updated:" $srcdir/README | sed 's/Updated: //'`

changequote(<<, >>)
changequote([, ])

dnl Detect host and target
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AC_MSG_CHECKING(if you are configuring for another platform)
if test "$cross_compiling" = yes -o "$target_os" = "windows"; then
  AC_MSG_RESULT(yes)
  cross_configure=1
else
  AC_MSG_RESULT(no)
  cross_configure=0
fi

AC_ARG_ENABLE([static-link],
  [AS_HELP_STRING([--enable-static-link],
    [link libraries statically @<:@default=disabled@:>@])],
  [enable_static_link="$enableval"],
  [enable_static_link=no]
)

# this case statement defines the compile flags which are needed to
# compile bochs on a platform.  Don't put things like optimization settings
# into the configure.in file, since people will want to be able to change
# those settings by defining CFLAGS and CXXFLAGS before running configure.
NO_LT=0
MSVC_TARGET=0
need_dlcompat_for_plugins=0
case "$target" in
  *-pc-windows* | *-pc-winnt*)
    DEFAULT_GUI=win32 # default to win32 gui
    NO_LT=1           # do not use libtool at all
    case "$target" in
      x86_64*)
        MSVC_TARGET=64
        AC_DEFINE(MSVC_TARGET, 64)
        ;;
      *)
        MSVC_TARGET=32
        AC_DEFINE(MSVC_TARGET, 32)
        ;;
    esac
    ;;
  *-cygwin* | *-mingw32* | *-msys)
    NO_LT=1   # do not use libtool at all
    if test "$enable_static_link" = yes; then
      EXTRA_LINK_OPTS="${EXTRA_LINK_OPTS} -static"
    fi
    if test "$with_term" = yes; then
      # ncurses won't compile with -DWIN32
      # also, I can't get it to link without this -DBROKEN_LINKER=1 hack.
      # see /usr/include/curses.h for details.
      ADD_FLAGS="-DBROKEN_LINKER=1"
    else
      # default case
      ADD_FLAGS="-DWIN32"   # required for cygwin/mingw/msys compile+win32 gui
      DEFAULT_GUI=win32     # default to win32 gui
    fi
    ;;
  *-macosx* | *-darwin*)
    ADD_FLAGS="-fpascal-strings -fno-common -Wno-four-char-constants -Wno-unknown-pragmas -Dmacintosh"      # required for macosx compile
    DEFAULT_GUI=carbon                # default to carbon
    need_dlcompat_for_plugins=1
    ;;
  *-macos*)
    DEFAULT_GUI=macos                 # macos defaults to macos
    ;;
  *-amigaos* | *-morphos*)
    DEFAULT_GUI=amigaos               # amigaos or morphos defaults to amigaos gui
    ;;
  *-solaris*)
    ADD_FLAGS="-D_XOPEN_SOURCE_EXTENDED=1 -D__EXTENSIONS__"      # required for correct function prototypes
    LIBS="$LIBS -lsocket -lnsl"
    DEFAULT_GUI=x11
    ;;
  *)
    DEFAULT_GUI=x11
    ;;
esac
if test "${ADD_FLAGS:+set}" = set; then
      CFLAGS="$CFLAGS $ADD_FLAGS"
      CXXFLAGS="$CXXFLAGS $ADD_FLAGS"
      CPPFLAGS="$CPPFLAGS $ADD_FLAGS"
fi

case "$target" in
  x86_64*)
    BOCHS_BITS=64
    ;;
  *)
    BOCHS_BITS=32
    ;;
esac

AC_MSG_CHECKING(for standard CFLAGS on this platform)
AC_MSG_RESULT($ADD_FLAGS)

dnl // make sure the default gui is enabled if no other chosen
if (test "$with_sdl" != yes) && \
   (test "$with_sdl2" != yes) && \
   (test "$with_x11" != yes) && \
   (test "$with_win32" != yes) && \
   (test "$with_nogui" != yes) && \
   (test "$with_term" != yes) && \
   (test "$with_rfb" != yes) && \
   (test "$with_vncsrv" != yes) && \
   (test "$with_amigaos" != yes) && \
   (test "$with_carbon" != yes) && \
   (test "$with_wx" != yes) && \
   (test "$with_macos" != yes); then
  # use DEFAULT_GUI.  Set the appropriate variable.
  # DEFAULT_GUI must be set to one of the names above.  Otherwise, no
  # valid $with_* variable will be set and who knows what will happen?
  eval "with_${DEFAULT_GUI}=yes"
fi

AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET

if test "$NO_LT" != 1; then
  dnl------------ libtool configuration
  dnl Configure libtool, and default to shared libraries.  Libtool will only be
  dnl used for compiling and linking plugins.
  AC_DISABLE_STATIC
  dnl Check for dlopen support
  AC_LIBTOOL_DLOPEN
  dnl Configure libtool
  AC_PROG_LIBTOOL
  dnl Configure the ltdl library.  This must go after AC_PROG_LIBTOOL or
  dnl else it disables shared libraries somehow.
  AC_LIB_LTDL
else
  RANLIB="echo"
fi

PKG_PROG_PKG_CONFIG
if test "x$PKG_CONFIG" = x; then
  AC_PATH_XTRA
fi

AC_C_BIGENDIAN
AC_CHECK_SIZEOF(unsigned char)
AC_CHECK_SIZEOF(unsigned short)
AC_CHECK_SIZEOF(unsigned int)
AC_CHECK_SIZEOF(unsigned long)
AC_CHECK_SIZEOF(unsigned long long)
if test "$MSVC_TARGET" = 0; then
  AC_CHECK_SIZEOF(int *)
fi
AC_CHECK_FUNCS(getenv, AC_DEFINE(BX_HAVE_GETENV))
AC_CHECK_FUNCS(setenv, AC_DEFINE(BX_HAVE_SETENV))
AC_CHECK_FUNCS(snprintf, AC_DEFINE(BX_HAVE_SNPRINTF))
AC_CHECK_FUNCS(vsnprintf, AC_DEFINE(BX_HAVE_VSNPRINTF))

if test "$MSVC_TARGET" = 0; then
  AC_CHECK_FUNCS(select, AC_DEFINE(BX_HAVE_SELECT))
  AC_CHECK_TYPE(socklen_t, AC_DEFINE(BX_HAVE_SOCKLEN_T), , [#include <sys/types.h>
  #include <sys/socket.h>])
  AC_CHECK_FUNCS(strtoull, AC_DEFINE(BX_HAVE_STRTOULL))
  AC_CHECK_FUNCS(strtouq, AC_DEFINE(BX_HAVE_STRTOUQ))
  AC_CHECK_FUNCS(strdup, AC_DEFINE(BX_HAVE_STRDUP))
  AC_CHECK_FUNCS(strrev, AC_DEFINE(BX_HAVE_STRREV))
  AC_CHECK_FUNCS(stricmp, AC_DEFINE(BX_HAVE_STRICMP))
  AC_CHECK_FUNCS(strcasecmp, AC_DEFINE(BX_HAVE_STRCASECMP))
  AC_CHECK_FUNCS(mkstemp, AC_DEFINE(BX_HAVE_MKSTEMP))
  AC_CHECK_HEADER(sys/mman.h, AC_DEFINE(BX_HAVE_SYS_MMAN_H))
  AC_CHECK_FUNCS(gettimeofday, AC_DEFINE(BX_HAVE_GETTIMEOFDAY))
  AC_CHECK_FUNCS(usleep, AC_DEFINE(BX_HAVE_USLEEP))

  AC_MSG_CHECKING(for __builtin_bswap32)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[
    __builtin_bswap32(0x12345678);
  ]])],[
    AC_DEFINE(BX_HAVE___BUILTIN_BSWAP32, 1, [Define to 1 if you have the '__builtin_bswap32' function.])
    AC_MSG_RESULT(yes)
  ],[
    AC_MSG_RESULT(no)
  ])
  AC_MSG_CHECKING(for __builtin_bswap64)
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[
    __builtin_bswap64(0x12345678);
  ]])],[
    AC_DEFINE(BX_HAVE___BUILTIN_BSWAP64, 1, [Define to 1 if you have the '__builtin_bswap64' function.])
    AC_MSG_RESULT(yes)
  ],[
    AC_MSG_RESULT(no)
  ])
  AC_CHECK_FUNCS(tmpfile64, AC_DEFINE(BX_HAVE_TMPFILE64))
  AC_CHECK_FUNCS(fseek64, AC_DEFINE(BX_HAVE_FSEEK64))
  AC_CHECK_FUNCS(fseeko64, AC_DEFINE(BX_HAVE_FSEEKO64))
  AC_CHECK_TYPE(ssize_t, AC_DEFINE(BX_HAVE_SSIZE_T), , [#include <sys/types.h>])
else
  AC_DEFINE(BX_HAVE_SELECT, 1)
  AC_DEFINE(BX_HAVE_SOCKLEN_T, 0)
  AC_DEFINE(BX_HAVE_STRTOULL, 1)
  AC_DEFINE(BX_HAVE_STRTOUQ, 0)
  AC_DEFINE(BX_HAVE_STRDUP, 1)
  AC_DEFINE(BX_HAVE_STRREV, 1)
  AC_DEFINE(BX_HAVE_STRICMP, 1)
  AC_DEFINE(BX_HAVE_STRCASECMP, 0)
  AC_DEFINE(BX_HAVE_MKSTEMP, 0)
  AC_DEFINE(BX_HAVE_SYS_MMAN_H, 0)
  AC_DEFINE(BX_HAVE_GETTIMEOFDAY, 0)
  AC_DEFINE(BX_HAVE_USLEEP, 0)
  AC_DEFINE(BX_HAVE___BUILTIN_BSWAP32, 0)
  AC_DEFINE(BX_HAVE___BUILTIN_BSWAP64, 0)
  AC_DEFINE(BX_HAVE_TMPFILE64, 0)
  AC_DEFINE(BX_HAVE_FSEEK64, 0)
  AC_DEFINE(BX_HAVE_FSEEKO64, 0)
  AC_DEFINE(BX_HAVE_SSIZE_T, 0)
  AC_DEFINE(BX_HAVE_SETENV, 0)
  if test "$MSVC_TARGET" = 64; then
    AC_DEFINE(SIZEOF_INT_P, 8)
  else
    AC_DEFINE(SIZEOF_INT_P, 4)
  fi
fi
AC_CHECK_FUNCS(sleep, AC_DEFINE(BX_HAVE_SLEEP))
AC_CHECK_FUNCS(nanosleep, AC_DEFINE(BX_HAVE_NANOSLEEP))
AC_CHECK_FUNCS(abort, AC_DEFINE(BX_HAVE_ABORT))
AC_CHECK_MEMBER(struct sockaddr_in.sin_len, AC_DEFINE(BX_HAVE_SOCKADDR_IN_SIN_LEN), , [#include <sys/socket.h>
#include <netinet/in.h> ])
AC_CHECK_FUNCS(mktime, AC_DEFINE(BX_HAVE_MKTIME))

dnl As of autoconf 2.53, the standard largefile test fails for Linux/gcc.
dnl It does not put the largefiles arguments into CFLAGS, even though Linux/gcc
dnl does need them.  So we do it ourselves.
AC_SYS_LARGEFILE

dnl we need to define _FILE_OFFSET_BITS or _LARGE_FILES on the compiler command
dnl line because otherwise the system headers risk being included before
dnl config.h which defines these constants leading to inconsistent
dnl sizeof(off_t) in different source files of the same program and linking
dnl problems
case "x$ac_cv_sys_file_offset_bits" in #(
  xno | xunknown) ;;
  *)
    if test "x$ac_cv_sys_file_offset_bits" = "x64"; then
        BX_LARGEFILE_FLAGS="-D_FILE_OFFSET_BITS=64 -D_LARGE_FILES"
    else
        BX_LARGEFILE_FLAGS="-D_LARGE_FILES"
    fi
    CPPFLAGS="$CPPFLAGS $BX_LARGEFILE_FLAGS"
    CFLAGS="$CFLAGS $BX_LARGEFILE_FLAGS"
    CXXFLAGS="$CXXFLAGS $BX_LARGEFILE_FLAGS"
    ;;
esac
dnl ---end of largefile tests

AC_MSG_CHECKING(if compiler allows __attribute__)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[typedef struct { } __attribute__ ((packed)) junk;]])],[AC_MSG_RESULT(yes)],[
      AC_MSG_RESULT(no)
      AC_DEFINE(BX_NO_ATTRIBUTES)
    ])

AC_LANG_SAVE
AC_LANG([C++])
AC_MSG_CHECKING(for set)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <set>]], [[]])],[
     AC_MSG_RESULT(yes)
     AC_DEFINE(BX_HAVE_SET)
   ],[AC_MSG_RESULT(no)])
AC_MSG_CHECKING(for set.h)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <set.h>]], [[]])],[
     AC_MSG_RESULT(yes)
     AC_DEFINE(BX_HAVE_SET_H)
   ],[AC_MSG_RESULT(no)])
AC_MSG_CHECKING(for map)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <map>]], [[]])],[
     AC_MSG_RESULT(yes)
     AC_DEFINE(BX_HAVE_MAP)
   ],[AC_MSG_RESULT(no)])
AC_MSG_CHECKING(for map.h)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <map.h>]], [[]])],[
     AC_MSG_RESULT(yes)
     AC_DEFINE(BX_HAVE_MAP_H)
   ],[AC_MSG_RESULT(no)])
AC_LANG_RESTORE

dnl Implement a check for each gui library to see if has a chance of compiling.
if test "$with_all_libs" = yes; then
  if test "$cross_configure" = 1; then
    AC_MSG_WARN([[Using --with-all-libs while cross configuring is very unlikely to be what you want.  You should specify the --with-* args yourself.]])
  fi

  if test "$with_amigaos" != yes; then
    can_compile_amigaos=1
    AC_CHECK_HEADER([proto/intuition.h], [], [ can_compile_amigaos=0 ])
    AC_CHECK_HEADER([intuition/intuitionbase.h], [], [ can_compile_amigaos=0 ])
    AC_CHECK_HEADER([cybergraphx/cybergraphics.h], [], [ can_compile_amigaos=0 ])
    AC_CHECK_HEADER([devices/trackdisk.h], [], [ can_compile_amigaos=0 ])
    if test $can_compile_amigaos = 1; then
      with_amigaos=yes
    fi
  fi

  if test "$with_carbon" != yes; then
    can_compile_carbon=1
    AC_CHECK_HEADER([Carbon.h], [], [ can_compile_carbon=0 ])
    AC_CHECK_HEADER([ApplicationServices/ApplicationServices.h], [], [ can_compile_carbon=0 ])
    if test $can_compile_carbon = 1; then
      with_carbon=yes
    fi
  fi

  if test "$with_win32" != yes; then
    can_compile_win32=1
    AC_CHECK_HEADER([windows.h], [], [ can_compile_win32=0 ])
    AC_CHECK_HEADER([commctrl.h], [], [ can_compile_win32=0 ])
    if test $can_compile_win32 = 1; then
      with_win32=yes
    fi
  fi

  if test "$with_sdl" != yes -a "$with_sdl2" != yes; then
    can_compile_sdl=2
    AC_CHECK_HEADER([SDL2/SDL.h], [], [ can_compile_sdl=0 ])
    AC_CHECK_HEADER([SDL2/SDL_main.h], [], [ can_compile_sdl=0 ])
    if test $can_compile_sdl != 2; then
      can_compile_sdl=1
      AC_CHECK_HEADER([SDL/SDL.h], [], [ can_compile_sdl=0 ])
      AC_CHECK_HEADER([SDL/SDL_main.h], [], [ can_compile_sdl=0 ])
      if test $can_compile_sdl = 1; then
        with_sdl=yes
      fi
    else
      with_sdl2=yes
    fi
  fi

  if test "$with_x11" != yes; then
    can_compile_x11=1
    AC_CHECK_HEADER([X11/Xlib.h], [], [ can_compile_x11=0 ])
    AC_CHECK_HEADER([X11/Xutil.h], [], [ can_compile_x11=0 ])
    if test $can_compile_x11 = 1; then
      with_x11=yes
    fi
  fi

  if test "$with_rfb" != yes; then
    can_compile_rfb=1
    case $target in
      *-pc-windows* | *-pc-winnt* | *-cygwin* | *-mingw32* | *-msys)
	AC_CHECK_HEADER([winsock2.h], [], [ can_compile_rfb=0 ])
	AC_CHECK_HEADER([process.h], [], [ can_compile_rfb=0 ])
        ;;
      *)
	AC_CHECK_HEADER([sys/socket.h], [], [ can_compile_rfb=0 ])
	AC_CHECK_HEADER([netinet/tcp.h], [], [ can_compile_rfb=0 ])
	AC_CHECK_HEADER([pthread.h], [], [ can_compile_rfb=0 ])
	;;
    esac
    if test $can_compile_rfb = 1; then
      with_rfb=yes
    fi
  fi

  if test "$with_vncsrv" != yes; then
    can_compile_vncsrv=1
    case $target in
      *-pc-windows* | *-pc-winnt*)
        can_compile_vncsrv=0
        ;;
      *-cygwin* | *-mingw32* | *-msys)
        AC_CHECK_HEADER([rfb/rfb.h], [], [ can_compile_vncsrv=0 ])
        ;;
      *)
        AC_CHECK_HEADER([rfb/rfb.h], [], [ can_compile_vncsrv=0 ])
        AC_CHECK_HEADER([pthread.h], [], [ can_compile_vncsrv=0 ])
        ;;
    esac
    if test $can_compile_vncsrv = 1; then
      with_vncsrv=yes
    fi
  fi

  if test "$with_term" != yes; then
    can_compile_term=1
    AC_CHECK_HEADER([curses.h], [], [ can_compile_term=0 ])
    AC_CHECK_HEADER([signal.h], [], [ can_compile_term=0 ])
    if test $can_compile_term = 1; then
      with_term=yes
    fi
  fi

  if test "$with_nogui" != yes; then
    with_nogui=yes
  fi
fi    # end of if $with_all_libs = yes

if test "$with_sdl" = yes -a "$with_sdl2" = yes; then
  AC_MSG_ERROR([[--with-sdl and --with-sdl2 are mutually exclusive]])
fi

AC_MSG_CHECKING(for idle hack)
AC_ARG_ENABLE(idle-hack,
  AS_HELP_STRING([--enable-idle-hack], [use Roland Mainz's idle hack (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_USE_IDLE_HACK, 1)
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_USE_IDLE_HACK, 0)
   fi],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_USE_IDLE_HACK, 0)
    ]
  )
AC_SUBST(BX_USE_IDLE_HACK)

AC_CHECK_HEADER(dlfcn.h, AC_DEFINE(BX_HAVE_DLFCN_H, 1))
AC_CHECK_HEADER(assert.h, AC_DEFINE(HAVE_ASSERT_H, 1))

AC_MSG_CHECKING(for plugins support)
AC_ARG_ENABLE(plugins,
  AS_HELP_STRING([--enable-plugins], [enable plugin support (no)]),
  [if test "$enableval" = yes; then
     bx_plugins=1
   else
     bx_plugins=0
   fi],
  [
    bx_plugins=0
    ]
  )

USER_PLUGINS_MAKEFILE=''
if test "$bx_plugins" = 1; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(BX_PLUGINS, 1)
  GUI_NON_PLUGIN_OBJS='$(OBJS_THAT_CANNOT_BE_PLUGINS)'
  GUI_PLUGIN_OBJS='$(OBJS_THAT_CAN_BE_PLUGINS)'
  DEFINE_PLUGIN_PATH='-DBX_PLUGIN_PATH="\"${plugdir}\""'
  # and the $(OBJS_THAT_SUPPORT_OTHER_PLUGINS) will be built and linked
  if test "$NO_LT" != 1; then
    # by special link rules for the plugins that they support.
    LIBS="$LIBS $LIBADD_DL"
    have_ltdl=0
    AC_CHECK_HEADER(ltdl.h, [AC_CHECK_LIB(ltdl, lt_dlinit, [have_ltdl=1]]))
    if test "$have_ltdl" = 1; then
      PLUGIN_LIB="-lltdl"
      AC_DEFINE(BX_HAVE_LTDL,1)
    else
      AC_MSG_ERROR([Plugin support requires libltdl installed on your system])
    fi
  fi
  PLUGIN_TARGET=bochs_plugins
  PLUGIN_TARGET_2=plugins_gcc
  INSTALL_PLUGINS_VAR=install_libtool_plugins
  NONPLUGIN_GUI_LINK_OPTS=''
  if test -d user_plugins; then
    USER_PLUGINS_MAKEFILE='user_plugins/Makefile'
  fi
else
  AC_MSG_RESULT(no)
  bx_plugins=0
  AC_DEFINE(BX_PLUGINS, 0)
  GUI_NON_PLUGIN_OBJS='$(OBJS_THAT_CANNOT_BE_PLUGINS) $(OBJS_THAT_CAN_BE_PLUGINS) $(OBJS_THAT_SUPPORT_OTHER_PLUGINS)'
  GUI_PLUGIN_OBJS=''
  NONPLUGIN_GUI_LINK_OPTS='$(GUI_LINK_OPTS)'
fi
AC_SUBST(DEFINE_PLUGIN_PATH)

# copy gui variables into iodev variables.  Later, we will add to the gui
# objs list, according to which display libraries are enabled.
IODEV_NON_PLUGIN_OBJS=$GUI_NON_PLUGIN_OBJS
IODEV_PLUGIN_OBJS=$GUI_PLUGIN_OBJS
IODEV_EXT_NON_PLUGIN_OBJS=$GUI_NON_PLUGIN_OBJS
IODEV_EXT_PLUGIN_OBJS=$GUI_PLUGIN_OBJS

# on MacOSX if they enabled plugins, make sure that dlopen() was found.
# It is provided by a library called dlcompat.
if test "$bx_plugins" = 1 -a "$need_dlcompat_for_plugins" = 1; then
  have_dlopen=0
  AC_CHECK_LIB(dl, dlopen, [have_dlopen=1])
  AC_MSG_CHECKING(if you have dlcompat, required for MacOSX plugins)
  if test "$have_dlopen" = 0; then
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([To use plugins on MacOSX you must use a library called dlcompat.  The configure script was not able to find dlcompat.  If it is already installed then you must set up your environment variables to point to it, as is done in .conf.macosx.  If you cannot resolve this, you should turn off plugins.])
  else
    AC_MSG_RESULT(yes)
  fi
fi

AC_MSG_CHECKING(if compiler allows LL for 64-bit constants)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[ { 42LL; } ]])],[AC_MSG_RESULT(yes)],[
      AC_MSG_RESULT(no)
      AC_DEFINE(BX_64BIT_CONSTANTS_USE_LL, 0)
    ])

AC_MSG_CHECKING(for A20 support)
AC_ARG_ENABLE(a20-pin,
  AS_HELP_STRING([--enable-a20-pin], [compile in support for A20 pin (yes)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_A20, 1)
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_A20, 0)
   fi
   ],
  [
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_A20, 1)
    ]
  )

use_x86_64=0
AC_MSG_CHECKING(for x86-64 support)
AC_ARG_ENABLE(x86-64,
  AS_HELP_STRING([--enable-x86-64], [compile in support for x86-64 instructions (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    OBJS64='$(OBJS64)'
    AC_DEFINE(BX_SUPPORT_X86_64, 1)
    use_x86_64=1
   else
    AC_MSG_RESULT(no)
    OBJS64=''
    AC_DEFINE(BX_SUPPORT_X86_64, 0)
   fi
   ],
  [
    AC_MSG_RESULT(no)
    OBJS64=''
    AC_DEFINE(BX_SUPPORT_X86_64, 0)
    ]
  )
AC_SUBST(OBJS64)

use_smp=0
AC_MSG_CHECKING(for SMP support)
AC_ARG_ENABLE(smp,
  AS_HELP_STRING([--enable-smp], [compile in support for SMP configurations (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_SMP, 1)
    AC_DEFINE(BX_USE_CPU_SMF, 0)
    use_smp=1
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_SMP, 0)
   fi
   ],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_SMP, 0)
    ]
  )

AC_MSG_CHECKING(for cpu level)
AC_ARG_ENABLE(cpu-level,
  AS_HELP_STRING([--enable-cpu-level], [select cpu level (3,4,5,6 - default is 6)]),
  [case "$enableval" in
     3)
       AC_MSG_RESULT(3)
       AC_DEFINE(BX_CPU_LEVEL, 3)
       ;;
     4)
       AC_MSG_RESULT(4)
       AC_DEFINE(BX_CPU_LEVEL, 4)
       ;;
     5)
       AC_MSG_RESULT(5)
       AC_DEFINE(BX_CPU_LEVEL, 5)
       ;;
     6)
       AC_MSG_RESULT(6)
       AC_DEFINE(BX_CPU_LEVEL, 6)
       ;;
     *)
       echo " "
       echo "ERROR: you must supply a valid CPU level to --enable-cpu-level"
       exit 1
       ;;
   esac
   bx_cpu_level=$enableval
   if test "$use_smp" = 1 -a "$enableval" -lt 5; then
     echo "ERROR: with >1 processor, use --enable-cpu-level=5 or more"
     exit 1
   fi
   if test "$use_x86_64" = 1 -a "$enableval" -lt 6; then
     echo "ERROR: --enable-cpu-level=6 required for x86-64 emulation"
     exit 1
   fi
  ],
  [
    AC_MSG_RESULT(6)
    AC_DEFINE(BX_CPU_LEVEL, 6)
    bx_cpu_level=6
  ]
  )

AC_MSG_CHECKING(for larger than 32 bit physical address emulation)
AC_ARG_ENABLE(long-phy-address,
  AS_HELP_STRING([--enable-long-phy-address], [compile in support for physical address larger than 32 bit (yes, if cpu level >= 5)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_PHY_ADDRESS_LONG, 1)
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_PHY_ADDRESS_LONG, 0)
   fi
   ],
  [
    if test "$bx_cpu_level" -ge 5; then
      AC_MSG_RESULT(yes)
      AC_DEFINE(BX_PHY_ADDRESS_LONG, 1)
    else
      AC_MSG_RESULT(no)
      AC_DEFINE(BX_PHY_ADDRESS_LONG, 0)
    fi
    ]
  )

AC_MSG_CHECKING(for large ramfile support)
AC_ARG_ENABLE(large-ramfile,
  AS_HELP_STRING([--enable-large-ramfile], [enable large ramfile support (yes)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_LARGE_RAMFILE, 1)
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_LARGE_RAMFILE, 0)
   fi],
  [
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_LARGE_RAMFILE, 1)
    ]
  )
AC_SUBST(BX_LARGE_RAMFILE)

AC_MSG_CHECKING(for repeated IO and mem copy speedups)
AC_ARG_ENABLE(repeat-speedups,
  AS_HELP_STRING([--enable-repeat-speedups], [support repeated IO and mem copy speedups (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    speedup_repeat=1
   else
    AC_MSG_RESULT(no)
    speedup_repeat=0
   fi],
  [
    AC_MSG_RESULT(no)
    speedup_repeat=0
    ]
  )

AC_MSG_CHECKING(for gcc / MSVC nmake fast function calls optimization)
AC_ARG_ENABLE(fast-function-calls,
  AS_HELP_STRING([--enable-fast-function-calls], [support for fast function calls (no - gcc on x86 and MSVC nmake only)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    speedup_fastcall=1
   else
    AC_MSG_RESULT(no)
    speedup_fastcall=0
   fi],
  [
    AC_MSG_RESULT(no)
    speedup_fastcall=0
    ]
  )

AC_MSG_CHECKING(for handlers chaining speedups)
AC_ARG_ENABLE(handlers-chaining,
  AS_HELP_STRING([--enable-handlers-chaining], [support handlers-chaining emulation speedups (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    speedup_handlers_chaining=1
   else
    AC_MSG_RESULT(no)
    speedup_handlers_chaining=0
   fi],
  [
    AC_MSG_RESULT(no)
    speedup_handlers_chaining=0
    ]
  )

AC_MSG_CHECKING(for trace linking speedups support)
AC_ARG_ENABLE(trace-linking,
  AS_HELP_STRING([--enable-trace-linking], [enable trace linking speedups support (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    enable_trace_linking=1
   else
    AC_MSG_RESULT(no)
    enable_trace_linking=0
   fi],
  [
    AC_MSG_RESULT(no)
    enable_trace_linking=0
    ]
  )

AC_MSG_CHECKING(support for configurable MSR registers)
AC_ARG_ENABLE(configurable-msrs,
  AS_HELP_STRING([--enable-configurable-msrs], [support for configurable MSR registers (yes if cpu level >= 5)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_CONFIGURE_MSRS, 1)
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_CONFIGURE_MSRS, 0)
   fi],
  [
    if test "$bx_cpu_level" -ge 5; then
      AC_MSG_RESULT(yes)
      AC_DEFINE(BX_CONFIGURE_MSRS, 1)
    else
      AC_MSG_RESULT(no)
      AC_DEFINE(BX_CONFIGURE_MSRS, 0)
    fi
    ]
  )

AC_MSG_CHECKING(show IPS)
AC_ARG_ENABLE(show-ips,
  AS_HELP_STRING([--enable-show-ips], [show IPS in Bochs status bar / log file (yes)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SHOW_IPS, 1)
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SHOW_IPS, 0)
   fi],
  [
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SHOW_IPS, 1)
    ]
  )

AC_MSG_CHECKING(for use of .cpp as suffix)
AC_ARG_ENABLE(cpp,
  AS_HELP_STRING([--enable-cpp], [use .cpp as C++ suffix (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    SUFFIX_LINE='.SUFFIXES: .cpp'
    CPP_SUFFIX='cpp'
   else
    AC_MSG_RESULT(no)
    SUFFIX_LINE='.SUFFIXES: .cc'
    CPP_SUFFIX='cc'
   fi],
  [
    AC_MSG_RESULT(no)
    SUFFIX_LINE='.SUFFIXES: .cc'
    CPP_SUFFIX='cc'
    ]
  )
AC_SUBST(SUFFIX_LINE)
AC_SUBST(CPP_SUFFIX)

if test "$enable_cpp" = yes; then
  echo "moving .cc source files to .cpp"
  sourcefiles=`find . -name "*.cc" -print`
  if test "$sourcefiles" != ""; then
    for ccname in $sourcefiles
    do
      cppname=`echo $ccname | sed -e "s/\.cc$/.cpp/"`
      echo "mv $ccname $cppname"
      mv   $ccname $cppname
    done
  else
    echo "no more .cc source files to rename"
  fi
fi



AC_MSG_CHECKING(for Bochs internal debugger support)
AC_ARG_ENABLE(debugger,
  AS_HELP_STRING([--enable-debugger], [compile in support for Bochs internal debugger (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_DEBUGGER, 1)
    DEBUGGER_VAR='$(DEBUGGER_LIB)'
    NO_FLEX_YACC=''
    AC_PATH_PROG(FLEX, flex, not_found)
    AC_PATH_PROG(YACC, yacc, not_found)
    AC_MSG_CHECKING(whether to generate parser / lexer files)
    if test "$FLEX" = not_found -o "$YACC" = not_found -o "$MSVC_TARGET" != 0; then
      NO_FLEX_YACC='#'
      AC_MSG_RESULT(no)
    else
      AC_MSG_RESULT(yes)
    fi
    AC_SUBST(NO_FLEX_YACC)
    bx_debugger=1
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_DEBUGGER, 0)
    DEBUGGER_VAR=''
    bx_debugger=0
   fi
   ],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_DEBUGGER, 0)
    DEBUGGER_VAR=''
    bx_debugger=0
    ]
  )
AC_SUBST(DEBUGGER_VAR)

gui_debugger=0
AC_MSG_CHECKING(enable Bochs internal debugger GUI)
AC_ARG_ENABLE(debugger-gui,
  AS_HELP_STRING([--enable-debugger-gui], [compile in support for Bochs internal debugger GUI (yes, if debugger is on)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    gui_debugger=1
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_DEBUGGER_GUI, 0)
   fi],
  [
    if test "$bx_debugger" = 1; then
      AC_MSG_RESULT(yes)
      gui_debugger=1
    else
      AC_MSG_RESULT(no)
      AC_DEFINE(BX_DEBUGGER_GUI, 0)
    fi
    ]
  )

bx_gdb_stub=0
AC_MSG_CHECKING(for gdb stub enable)
AC_ARG_ENABLE(gdb-stub,
  AS_HELP_STRING([--enable-gdb-stub], [enable gdb stub support (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_GDBSTUB, 1)
    GDBSTUB_VAR='$(GDBSTUB_OBJS)'
    bx_gdb_stub=1
    if test "$bx_debugger" = 1; then
      AC_MSG_ERROR([[--enable-debugger and --enable-gdb-stub are mutually exclusive]])
    fi
    AC_MSG_WARN([This Bochs feature is not maintained yet and may fail])
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_GDBSTUB, 0)
    GDBSTUB_VAR=''
   fi],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_GDBSTUB, 0)
    GDBSTUB_VAR=''
    ]
  )
AC_SUBST(GDBSTUB_VAR)


AC_MSG_CHECKING(for I/O Interface to the debugger)
IODEBUG_OBJS=''
AC_ARG_ENABLE(iodebug,
  AS_HELP_STRING([--enable-iodebug], [enable I/O interface to debugger (yes, if debugger is on)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_IODEBUG, 1)
    IODEBUG_OBJS='iodebug.o'
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_IODEBUG, 0)
   fi
   ],
  [
    if test "$bx_debugger" = 1; then
      # enable by default if debugger is on
      AC_MSG_RESULT(yes)
      AC_DEFINE(BX_SUPPORT_IODEBUG, 1)
      IODEBUG_OBJS='iodebug.o'
    else
      AC_MSG_RESULT(no)
    fi
    ]
  )
AC_SUBST(IODEBUG_OBJS)


AC_MSG_CHECKING(for ALL optimizations enabled)
AC_ARG_ENABLE(all-optimizations,
  AS_HELP_STRING([--enable-all-optimizations], [compile in all possible optimizations (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    speedups_all=1
   else
    AC_MSG_RESULT(no)
    speedups_all=0
   fi
   ],
  [
    AC_MSG_RESULT(no)
    speedups_all=0
    ]
  )

#
# Optimizations section.  Decide what the status of various optimizations
# should be based on configure choices and other factors.
#

if test "$speedups_all" = 1; then
  # Configure requested to force all options enabled.
  speedup_repeat=1
  speedup_fastcall=1
  speedup_handlers_chaining=1
  enable_trace_linking=1
fi

if test "$speedup_repeat" = 1; then
  AC_DEFINE(BX_SUPPORT_REPEAT_SPEEDUPS, 1)
else
  AC_DEFINE(BX_SUPPORT_REPEAT_SPEEDUPS, 0)
fi

if test "$speedup_fastcall" = 1; then
  AC_DEFINE(BX_FAST_FUNC_CALL, 1)
else
  AC_DEFINE(BX_FAST_FUNC_CALL, 0)
fi

if test "$bx_gdb_stub" = 1 -a "$speedup_handlers_chaining" = 1; then
  speedup_handlers_chaining=0
  echo "ERROR: handlers-chaining speedups are not supported with internal debugger or gdbstub yet"
fi

if test "$speedup_handlers_chaining" = 1; then
  AC_DEFINE(BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS, 1)
else
  AC_DEFINE(BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS, 0)
fi

if test "$enable_trace_linking" = 1; then
  AC_DEFINE(BX_ENABLE_TRACE_LINKING, 1)
else
  AC_DEFINE(BX_ENABLE_TRACE_LINKING, 0)
fi

READLINE_LIB=""
rl_without_curses_ok=no
rl_with_curses_ok=no

AC_MSG_CHECKING(whether user wants readline)
AC_ARG_ENABLE(readline,
  AS_HELP_STRING([--enable-readline], [use readline library, if available (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
     want_readline=yes
   else
     AC_MSG_RESULT(no)
     want_readline=no
   fi],
  [
    AC_MSG_RESULT(yes)
    want_readline=yes
    ]
 )

use_readline=0
AC_MSG_CHECKING(whether to use readline)
if test "$want_readline" = yes; then
  if test "$bx_debugger" = 1; then

    AC_MSG_CHECKING(if readline works without -lcurses)
    OLD_LIBS=$LIBS
    LIBS="$LIBS -lreadline"
    AC_RUN_IFELSE([AC_LANG_SOURCE([[
      #include <stdio.h>
      #include <readline/readline.h>
      int main() { rl_initialize(); return 0; }
      ]])],[ AC_MSG_RESULT(yes)
        rl_without_curses_ok=yes ],[ AC_MSG_RESULT(no) 
    ],[])
    AC_MSG_CHECKING(if readline works with -lcurses)
    LIBS="$LIBS -lcurses"
    AC_RUN_IFELSE([AC_LANG_SOURCE([[
      #include <stdio.h>
      #include <readline/readline.h>
      int main() { rl_initialize(); return 0; }
      ]])],[AC_MSG_RESULT(yes)
       rl_with_curses_ok=yes ],[ AC_MSG_RESULT(no) 
    ],[])
    LIBS=$OLD_LIBS

    if test "$rl_without_curses_ok" = yes; then
      AC_MSG_RESULT(yes)
      AC_DEFINE(HAVE_LIBREADLINE, 1)
      READLINE_LIB="-lreadline"
      use_readline=1
    elif test "$rl_with_curses_ok" = yes; then
      AC_MSG_RESULT(yes)
      AC_DEFINE(HAVE_LIBREADLINE, 1)
      READLINE_LIB="-lreadline -lcurses"
      use_readline=1
    else
      AC_MSG_RESULT(no)
      echo WARNING: The readline library was disabled because it was not found.
    fi
  else
    AC_MSG_RESULT(no)
  fi
else
  AC_MSG_RESULT(no)
fi
AC_SUBST(READLINE_LIB)

AC_CHECK_HEADER(readline/history.h,
  AC_DEFINE(HAVE_READLINE_HISTORY_H)
  )

INSTRUMENT_DIR='instrument/stubs'

AC_MSG_CHECKING(for instrumentation support)
AC_ARG_ENABLE(instrumentation,
  AS_HELP_STRING([--enable-instrumentation=instrument-dir], [compile in support for instrumentation (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_INSTRUMENTATION, 1)
    INSTRUMENT_VAR='$(INSTRUMENT_LIB)'
   elif test "$enableval" = no; then
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_INSTRUMENTATION, 0)
    INSTRUMENT_VAR=''
   else
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_INSTRUMENTATION, 1)
    INSTRUMENT_DIR=$enableval
    INSTRUMENT_VAR='$(INSTRUMENT_LIB)'
   fi],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_INSTRUMENTATION, 0)
    INSTRUMENT_VAR=''
    ]
  )
AC_SUBST(INSTRUMENT_DIR)
AC_SUBST(INSTRUMENT_VAR)

AC_MSG_CHECKING(enable logging)
AC_ARG_ENABLE(logging,
  AS_HELP_STRING([--enable-logging], [enable logging (yes)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_NO_LOGGING, 0)
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_NO_LOGGING, 1)
   fi],
  [
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_NO_LOGGING, 0)
    ]
  )

AC_MSG_CHECKING(enable statistics collection)
AC_ARG_ENABLE(stats,
  AS_HELP_STRING([--enable-stats], [enable statistics collection (yes)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_ENABLE_STATISTICS, 1)
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_ENABLE_STATISTICS, 0)
   fi],
  [
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_ENABLE_STATISTICS, 1)
    ]
  )

AC_MSG_CHECKING(enable assert checks)
AC_ARG_ENABLE(assert-checks,
  AS_HELP_STRING([--enable-assert-checks], [enable BX_ASSERT checks (yes, if debugger is on)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_ASSERT_ENABLE, 1)
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_ASSERT_ENABLE, 0)
   fi],
  [
    if test "$bx_debugger" = 1; then
      AC_MSG_RESULT(yes)
      AC_DEFINE(BX_ASSERT_ENABLE, 1)
    else
      AC_MSG_RESULT(no)
      AC_DEFINE(BX_ASSERT_ENABLE, 0)
    fi
    ]
  )

support_fpu=1
AC_MSG_CHECKING(for FPU emulation)
FPU_VAR=''
AC_ARG_ENABLE(fpu,
  AS_HELP_STRING([--enable-fpu], [compile in FPU emulation (yes)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_FPU, 1)
    FPU_VAR='$(FPU_LIB) $(SOFTFLOAT_LIB)'
   elif test "$enableval" = no; then
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_FPU, 0)
    support_fpu=0
   else
    echo " "
    echo "ERROR: --enable-fpu does not accept a path"
    exit 1
   fi
   ],
  [
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_FPU, 1)
    FPU_VAR='$(FPU_LIB) $(SOFTFLOAT_LIB)'
    ]
  )
AC_SUBST(FPU_VAR)

if test "$MSVC_TARGET" = 0; then
  GNU_MAKE_ONLY=""
  NMAKE_ONLY="# "
else
  GNU_MAKE_ONLY="# "
  NMAKE_ONLY=""
fi
AC_SUBST(GNU_MAKE_ONLY)
AC_SUBST(NMAKE_ONLY)

support_vmx=0
AC_MSG_CHECKING(for VMX support)
AC_ARG_ENABLE(vmx,
  AS_HELP_STRING([--enable-vmx], [VMX (virtualization extensions) emulation (--enable-vmx=[no|1|2])]),
  [case "$enableval" in
    no | 0)
      support_vmx=0
      AC_MSG_RESULT(no)
      AC_DEFINE(BX_SUPPORT_VMX, 0)
      ;;
    yes | 1)
      support_vmx=1
      AC_MSG_RESULT(1)
      AC_DEFINE(BX_SUPPORT_VMX, 1)
      ;;
    2)
      support_vmx=2
      AC_MSG_RESULT(2)
      AC_DEFINE(BX_SUPPORT_VMX, 2)
      ;;
    *)
      echo "ERROR: --enable-vmx=$enableval not understood. Use --enable-vmx=[no|1|2]"
      exit 1
      ;;
   esac
   ],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_VMX, 0)
  ]
  )

if test "$support_vmx" -gt 0 -a "$bx_cpu_level" -lt 6; then
  AC_MSG_ERROR([for VMX support the CPU level must be set to 6])
fi

if test "$support_vmx" -gt 1 -a "$use_x86_64" = 0; then
  AC_MSG_ERROR([VMXx2 support require x86-64 enabled])
fi

support_svm=0
AC_MSG_CHECKING(for SVM support)
AC_ARG_ENABLE(svm,
  AS_HELP_STRING([--enable-svm], [SVM (AMD: secure virtual machine) emulation (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_SVM, 1)
    support_svm=1
   elif test "$enableval" = no; then
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_SVM, 0)
   fi
   ],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_SVM, 0)
    ]
  )

if test "$support_svm" -gt 0 -a "$use_x86_64" = 0; then
  AC_MSG_ERROR([SVM support require x86-64 enabled])
fi

support_pkeys=0
AC_MSG_CHECKING(for User-Mode Protection Keys support)
AC_ARG_ENABLE(protection-keys,
  AS_HELP_STRING([--enable-protection-keys], [User-Mode Protection Keys support (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_PKEYS, 1)
    support_pkeys=1
   elif test "$enableval" = no; then
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_PKEYS, 0)
   fi
   ],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_PKEYS, 0)
  ]
  )

if test "$support_pkeys" -gt 0 -a "$use_x86_64" = 0; then
  AC_MSG_ERROR([User-Mode Protection Keys feature emulation require x86-64 enabled])
fi

support_cet=0
AC_MSG_CHECKING(for Control Flow Enforcement Technology support)
AC_ARG_ENABLE(cet,
  AS_HELP_STRING([--enable-cet], [Control Flow Enforcement Technology support (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_CET, 1)
    support_cet=1
   elif test "$enableval" = no; then
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_CET, 0)
   fi
   ],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_CET, 0)
  ]
  )

if test "$support_cet" -gt 0 -a "$use_x86_64" = 0; then
  AC_MSG_ERROR([Control Flow Enforcement Technology feature emulation require x86-64 enabled])
fi

support_uintr=0
AC_MSG_CHECKING(for User Level Interrupt support)
AC_ARG_ENABLE(uintr,
  AS_HELP_STRING([--enable-uintr], [for User Level Interrupt support (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_UINTR, 1)
    support_uintr=1
   elif test "$enableval" = no; then
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_UINTR, 0)
   fi
   ],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_UINTR, 0)
  ]
  )

if test "$support_uintr" -gt 0 -a "$use_x86_64" = 0; then
  AC_MSG_ERROR([User Level Interrupt feature emulation require x86-64 enabled])
fi

AC_MSG_CHECKING(for 3DNow! support)
AC_ARG_ENABLE(3dnow,
  AS_HELP_STRING([--enable-3dnow], [3DNow! support (no - incomplete)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_3DNOW, 1)
   elif test "$enableval" = no; then
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_3DNOW, 0)
   fi
   ],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_3DNOW, 0)
  ]
  )

AC_MSG_CHECKING(for alignment check support)
AC_ARG_ENABLE(alignment-check,
  AS_HELP_STRING([--enable-alignment-check], [alignment check (#AC) support (yes, if cpu level > 3)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_ALIGNMENT_CHECK, 1)
   elif test "$enableval" = no; then
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_ALIGNMENT_CHECK, 0)
   fi
   ],
  [
    if test "$bx_cpu_level" -gt 3; then
      AC_MSG_RESULT(yes)
      AC_DEFINE(BX_SUPPORT_ALIGNMENT_CHECK, 1)
    else
      AC_MSG_RESULT(no)
      AC_DEFINE(BX_SUPPORT_ALIGNMENT_CHECK, 0)
    fi
    ]
  )

AC_MSG_CHECKING(for MONITOR/MWAIT instructions support (experimental))
AC_ARG_ENABLE(monitor_mwait,
  AS_HELP_STRING([--enable-monitor-mwait], [support for MONITOR/MWAIT instructions (yes, if cpu level > 5 - experimental)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_MONITOR_MWAIT, 1)
   elif test "$enableval" = no; then
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_MONITOR_MWAIT, 0)
   fi
   ],
  [
    if test "$bx_cpu_level" -gt 5; then
      AC_MSG_RESULT(yes)
      AC_DEFINE(BX_SUPPORT_MONITOR_MWAIT, 1)
    else
      AC_MSG_RESULT(no)
      AC_DEFINE(BX_SUPPORT_MONITOR_MWAIT, 0)
    fi
  ]
  )

AC_MSG_CHECKING(for limited hardware performance monitoring emulation support (experimental))
AC_ARG_ENABLE(perfmon,
  AS_HELP_STRING([--enable-perfmon], [support for limited hardware performance monitoring emulation (yes, if cpu level > 5 - experimental)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_PERFMON, 1)
   elif test "$enableval" = no; then
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_PERFMON, 0)
   fi
   ],
  [
    if test "$bx_cpu_level" -gt 5; then
      AC_MSG_RESULT(yes)
      AC_DEFINE(BX_SUPPORT_PERFMON, 1)
    else
      AC_MSG_RESULT(no)
      AC_DEFINE(BX_SUPPORT_PERFMON, 0)
    fi
  ]
  )

AC_MSG_CHECKING(for memory type support)
AC_ARG_ENABLE(memtype,
  AS_HELP_STRING([--enable-memtype], [support for memory type]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_MEMTYPE, 1)
   elif test "$enableval" = no; then
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_MEMTYPE, 0)
   fi
   ],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_MEMTYPE, 0)
  ]
  )

support_avx=0
AVX_LIB_VAR=''
AC_MSG_CHECKING(for AVX instructions support)
AC_ARG_ENABLE(avx,
  AS_HELP_STRING([--enable-avx], [support for AVX instructions (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AVX_LIB_VAR='$(AVX_LIB)'
    AC_DEFINE(BX_SUPPORT_AVX, 1)
    support_avx=1
   elif test "$enableval" = no; then
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_AVX, 0)
   fi
   ],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_AVX, 0)
  ]
  )
AC_SUBST(AVX_LIB_VAR)

if test "$support_avx" -gt 0 -a "$use_x86_64" = 0; then
  AC_MSG_ERROR([for AVX support x86-64 emulation must be compiled in!])
fi

support_evex=0
AC_MSG_CHECKING(for EVEX prefix and AVX-512 extensions support)
AC_ARG_ENABLE(evex,
  AS_HELP_STRING([--enable-evex], [support for EVEX prefix and AVX-512 extensions (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_EVEX, 1)
    support_evex=1
   elif test "$enableval" = no; then
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_EVEX, 0)
   fi
   ],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_EVEX, 0)
  ]
  )

if test "$support_evex" -gt 0 -a "$support_avx" = 0; then
  AC_MSG_ERROR([for EVEX and AVX-512 support AVX emulation must be compiled in!])
fi

support_amx=0
AC_MSG_CHECKING(for AMX extensions support)
AC_ARG_ENABLE(amx,
  AS_HELP_STRING([--enable-amx], [support for AMX extensions (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_AMX, 1)
    support_amx=1
   elif test "$enableval" = no; then
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_AMX, 0)
   fi
   ],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_AMX, 0)
  ]
  )

if test "$support_amx" -gt 0 -a "$support_evex" = 0; then
  AC_MSG_ERROR([for AMX support AVX and EVEX emulation must be compiled in!])
fi

AC_MSG_CHECKING(for x86 debugger support)
AC_ARG_ENABLE(x86-debugger,
  AS_HELP_STRING([--enable-x86-debugger], [x86 debugger support (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_X86_DEBUGGER, 1)
   elif test "$enableval" = no; then
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_X86_DEBUGGER, 0)
   fi
   ],
  [
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_X86_DEBUGGER, 1)
    ]
  )

dnl devices configuration section

PCI_OBJS=''
AC_MSG_CHECKING(for i440FX PCI support)
AC_ARG_ENABLE(pci,
  AS_HELP_STRING([--enable-pci], [enable i440FX PCI support (yes)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_PCI, 1)
    pci=1
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_PCI, 0)
    pci=0
   fi],
  [
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_PCI, 1)
    pci=1
    ]
  )
if test "$pci" = "1"; then
  PCI_OBJS='pci.o pci2isa.o pci_ide.o acpi.o hpet.o'
fi
AC_SUBST(PCI_OBJS)


AC_MSG_CHECKING(for PCI host device mapping support)
AC_ARG_ENABLE(pcidev,
  AS_HELP_STRING([--enable-pcidev], [enable PCI host device mapping support (no - linux host only)]),
  [
    if test "$enableval" = "yes"; then
      AC_MSG_RESULT(yes)
      if test "$pci" != "1"; then
        AC_MSG_ERROR([PCI host device mapping requires PCI support])
      fi
      case "$target" in
        *-linux*)
          AC_MSG_NOTICE([Linux detected as host for PCI host device mapping])
          linux_version=`uname -r`
          case "$linux_version" in
            2.2*)
              AC_MSG_ERROR([Linux kernel 2.4, 2.6 or 3.x is required for PCI host device mapping])
              ;;
            2.4*)
              PCIDEV_MODULE_MAKE_ALL="all-kernel24"
              KERNEL_MODULE_SUFFIX="o"
              ;;
            *)
              PCIDEV_MODULE_MAKE_ALL="all-kernel26"
              KERNEL_MODULE_SUFFIX="ko"
              ;;
          esac
          KERNELDIR="/lib/modules/$linux_version/build"
          LSMOD="lsmod"
          INSMOD="insmod"
          RMMOD="rmmod"
          DEPMOD="depmod"
          CLEAN_PCIDEV_VAR=clean_pcidev
          AC_SUBST(KERNELDIR)
          AC_SUBST(LSMOD)
          AC_SUBST(INSMOD)
          AC_SUBST(RMMOD)
          AC_SUBST(DEPMOD)
          AC_SUBST(PCIDEV_MODULE_MAKE_ALL)
          AC_SUBST(KERNEL_MODULE_SUFFIX)
          AC_SUBST(CLEAN_PCIDEV_VAR)
        ;;
        *)
          AC_MSG_ERROR([PCI host device mapping requires Linux as host])
        ;;
      esac
      AC_DEFINE(BX_SUPPORT_PCIDEV, 1)
      PCI_OBJS="$PCI_OBJS pcidev.o"
      AC_MSG_WARN([This Bochs feature is not maintained yet and may fail])
    else
      AC_MSG_RESULT(no)
      AC_DEFINE(BX_SUPPORT_PCIDEV, 0)
    fi
  ],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_PCIDEV, 0)
  ]
)

use_usb=0
use_usb_uhci=0
USBHC_OBJS=''
UHCICORE_OBJ=''
USBHC_DLL_TARGETS=''
AC_MSG_CHECKING(for USB UHCI support)
AC_ARG_ENABLE(usb,
  AS_HELP_STRING([--enable-usb], [enable USB UHCI support (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_USB_UHCI, 1)
    USBHC_OBJS="usb_uhci.o"
    UHCICORE_OBJ="uhci_core.o"
    USBHC_DLL_TARGETS="bx_usb_uhci.dll"
    use_usb=1
    use_usb_uhci=1
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_USB_UHCI, 0)
   fi],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_USB_UHCI, 0)
    ]
  )

use_usb_ohci=0
AC_MSG_CHECKING(for USB OHCI support)
AC_ARG_ENABLE(usb-ohci,
  AS_HELP_STRING([--enable-usb-ohci], [enable USB OHCI support (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_USB_OHCI, 1)
    USBHC_OBJS="$USBHC_OBJS usb_ohci.o"
    USBHC_DLL_TARGETS="$USBHC_DLL_TARGETS bx_usb_ohci.dll"
    use_usb=1
    use_usb_ohci=1
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_USB_OHCI, 0)
   fi],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_USB_OHCI, 0)
    ]
  )

use_usb_ehci=0
AC_MSG_CHECKING(for USB EHCI support)
AC_ARG_ENABLE(usb-ehci,
  AS_HELP_STRING([--enable-usb-ehci], [enable USB EHCI support (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_USB_EHCI, 1)
    USBHC_OBJS="$USBHC_OBJS usb_ehci.o"
    USBHC_DLL_TARGETS="$USBHC_DLL_TARGETS bx_usb_ehci.dll"
    use_usb=1
    use_usb_ehci=1
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_USB_EHCI, 0)
   fi],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_USB_EHCI, 0)
    ]
  )

use_usb_xhci=0
AC_MSG_CHECKING(for USB xHCI support)
AC_ARG_ENABLE(usb-xhci,
  AS_HELP_STRING([--enable-usb-xhci], [enable USB xHCI support (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_USB_XHCI, 1)
    USBHC_OBJS="$USBHC_OBJS usb_xhci.o"
    USBHC_DLL_TARGETS="$USBHC_DLL_TARGETS bx_usb_xhci.dll"
    use_usb=1
    use_usb_xhci=1
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_USB_XHCI, 0)
   fi],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_USB_XHCI, 0)
    ]
  )

if test "$use_usb" = 1; then
  if test "$use_usb_ehci" = 1; then
    if test "$use_usb_uhci" = 0 -a "$use_usb_ohci" = 0; then
      AC_MSG_ERROR([To configure USB EHCI, you must also enable UHCI or OHCI])
    fi
  fi
  if test "$pci" != "1"; then
    AC_MSG_ERROR([USB requires PCI support])
  fi
  USB_LIB_VAR='iodev/usb/libusb.a'
  AC_SUBST(USB_LIB_VAR)
fi
AC_SUBST(USBHC_OBJS)
AC_SUBST(USBHC_DLL_TARGETS)

usb_debugger=0
AC_MSG_CHECKING(enable Bochs USB debugger)
AC_ARG_ENABLE(usb-debugger,
  AS_HELP_STRING([--enable-usb-debugger], [compile in support for Bochs USB debugger (yes, if debugger is on and USB is on)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    usb_debugger=1
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_USB_DEBUGGER, 0)
    usb_debugger=0
   fi],
  [
    if test "$bx_debugger" = 1 -a "$use_usb" = 1; then
      AC_MSG_RESULT(yes)
      usb_debugger=1
    else
      AC_MSG_RESULT(no)
      AC_DEFINE(BX_USB_DEBUGGER, 0)
      usb_debugger=0
    fi
    ]
  )

networking=no
NETDEV_OBJS=''
NETDEV_DLL_TARGETS=''
AC_MSG_CHECKING(for NE2000 support)
AC_ARG_ENABLE(ne2000,
  AS_HELP_STRING([--enable-ne2000], [enable NE2000 support (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_NE2K, 1)
    NETDEV_OBJS='ne2k.o'
    NETDEV_DLL_TARGETS="bx_ne2k.dll"
    networking=yes
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_NE2K, 0)
   fi],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_NE2K, 0)
    ]
  )

AC_MSG_CHECKING(for PCI pseudo NIC support)
AC_ARG_ENABLE(pnic,
  AS_HELP_STRING([--enable-pnic], [enable PCI pseudo NIC support (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_PCIPNIC, 1)
    NETDEV_OBJS="$NETDEV_OBJS pcipnic.o"
    NETDEV_DLL_TARGETS="$NETDEV_DLL_TARGETS bx_pcipnic.dll"
    networking=yes
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_PCIPNIC, 0)
   fi],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_PCIPNIC, 0)
    ]
  )

AC_MSG_CHECKING(for Intel(R) Gigabit Ethernet support)
AC_ARG_ENABLE(e1000,
  AS_HELP_STRING([--enable-e1000], [enable Intel(R) Gigabit Ethernet support (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    if test "$pci" != "1"; then
      AC_MSG_ERROR([E1000 network adapter requires PCI support])
    fi
    AC_DEFINE(BX_SUPPORT_E1000, 1)
    NETDEV_OBJS="$NETDEV_OBJS e1000.o"
    NETDEV_DLL_TARGETS="$NETDEV_DLL_TARGETS bx_e1000.dll"
    networking=yes
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_E1000, 0)
   fi],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_E1000, 0)
    ]
  )

NETLOW_OBJS=''
SLIRP_OBJS=''
SLIRP_OBJS2=''
SLIRP_LINK_OPTS=''
if test "$networking" = yes; then
  NETLOW_OBJS='eth_null.o eth_vnet.o'
  ethernet_modules='null vnet'
  can_compile_slirp=0
  AC_MSG_CHECKING(for slirp networking support)
  AC_ARG_ENABLE(using-libslirp,
    AS_HELP_STRING([--enable-using-libslirp], [enable using libslirp instead of builtin slirp (no)]),
    [if test "$enableval" = yes; then
      if test "x$PKG_CONFIG" != x; then
        $PKG_CONFIG --exists slirp
        if test x$? = x0; then
          can_compile_slirp=1
          if test "$bx_plugins" = 1; then
            SLIRP_LINK_OPTS="`$PKG_CONFIG --libs slirp`"
          else
            DEVICE_LINK_OPTS="$DEVICE_LINK_OPTS `$PKG_CONFIG --libs slirp`"
          fi
          AC_MSG_RESULT(libslirp)
          AC_DEFINE(BX_HAVE_LIBSLIRP, 1)
        fi
      fi
    fi], [])
  if test $can_compile_slirp = 0; then
    AC_MSG_RESULT(builtin)
    case "$target" in
      *-cygwin* | *-mingw32* | *-msys)
        can_compile_slirp=1
        DEVICE_LINK_OPTS="$DEVICE_LINK_OPTS -liphlpapi"
        SLIRP_OBJS='$(SLIRP_OBJS)'
        ;;
      *-pc-windows* | *-pc-winnt*)
        can_compile_slirp=1
        DEVICE_LINK_OPTS="$DEVICE_LINK_OPTS iphlpapi.lib"
        SLIRP_OBJS='$(SLIRP_OBJS)'
        ;;
      *)
        AC_CHECK_HEADER([netinet/in.h],[can_compile_slirp=1])
        if test $can_compile_slirp = 1; then
          if test "$bx_plugins" = 1; then
            SLIRP_OBJS2='$(SLIRP_OBJS:.o=.lo)'
          else
            SLIRP_OBJS='$(SLIRP_OBJS)'
          fi
        fi
        ;;
    esac
  fi
  if test $can_compile_slirp = 1; then
    NETLOW_OBJS="$NETLOW_OBJS eth_slirp.o"
    ethernet_modules="$ethernet_modules slirp"
    AC_DEFINE(BX_NETMOD_SLIRP, 1)
    NETLOW_OBJS="$NETLOW_OBJS eth_socket.o"
    ethernet_modules="$ethernet_modules socket"
    AC_DEFINE(BX_NETMOD_SOCKET, 1)
  fi
  if test "$MSVC_TARGET" = 0; then
    AC_CHECK_HEADER(net/bpf.h, [
        NETLOW_OBJS="$NETLOW_OBJS eth_fbsd.o"
        ethernet_modules="$ethernet_modules fbsd"
        AC_DEFINE(BX_NETMOD_FBSD, 1)
      ])
    AC_CHECK_HEADER(netpacket/packet.h, [
        NETLOW_OBJS="$NETLOW_OBJS eth_linux.o"
        ethernet_modules="$ethernet_modules linux"
        AC_DEFINE(BX_NETMOD_LINUX, 1)
      ])
  fi
  case "$target" in
    *-pc-windows* | *-pc-winnt* | *-cygwin* | *-mingw32* | *-msys)
    NETLOW_OBJS="$NETLOW_OBJS eth_win32.o"
    ethernet_modules="$ethernet_modules win32"
    AC_DEFINE(BX_NETMOD_WIN32, 1)
    ;;
    *-macosx* | *-darwin*)
    NETLOW_OBJS="$NETLOW_OBJS eth_tuntap.o"
    ethernet_modules="$ethernet_modules tuntap"
    AC_DEFINE(BX_NETMOD_TUNTAP, 1)
    ;;
    *)
    AC_CHECK_HEADER(net/if.h, [
        use_vde=yes
        AC_DEFINE(BX_HAVE_NET_IF_H, 1)
      ], [],
      [
      #include <sys/types.h>
      #include <sys/socket.h>
    ])
    AC_CHECK_HEADER(linux/netlink.h, [
        use_vde=yes
      ], [],
      [
      #include <asm/types.h>
      #include <sys/socket.h>
    ])

    if test "$use_vde" = yes; then
      NETLOW_OBJS="$NETLOW_OBJS eth_vde.o"
      ethernet_modules="$ethernet_modules vde"
      AC_DEFINE(BX_NETMOD_VDE, 1)
    fi

    AC_CHECK_HEADER(net/if_tun.h, [
        use_tuntap=yes
      ], [],
      [
      #include <sys/types.h>
      #include <sys/socket.h>
      #include <net/if.h>
    ])
    AC_CHECK_HEADER(linux/if_tun.h, [
        use_tuntap=yes
      ], [],
      [
      #include <asm/types.h>
      #include <sys/socket.h>
    ])
    if test "$use_tuntap" = yes; then
      NETLOW_OBJS="$NETLOW_OBJS eth_tuntap.o"
      ethernet_modules="$ethernet_modules tuntap"
      AC_DEFINE(BX_NETMOD_TUNTAP, 1)
    fi

    ;;
  esac
  NETWORK_LIB_VAR='iodev/network/libnetwork.a'
  AC_SUBST(NETWORK_LIB_VAR)
  AC_DEFINE(BX_NETWORKING, 1)
  AC_MSG_CHECKING(for ethernet modules)
  AC_MSG_RESULT($ethernet_modules)
else
  AC_DEFINE(BX_NETWORKING, 0)
fi
AC_SUBST(NETDEV_OBJS)
AC_SUBST(NETLOW_OBJS)
AC_SUBST(NETDEV_DLL_TARGETS)
AC_SUBST(SLIRP_OBJS)
AC_SUBST(SLIRP_OBJS2)
AC_SUBST(SLIRP_LINK_OPTS)


dnl // serial mode 'socket' needs ws2_32.dll in non-plugin mode
if test "$bx_plugins" = 0; then
  case $target in
    *-pc-windows*)
      DEVICE_LINK_OPTS="$DEVICE_LINK_OPTS ws2_32.lib"
      ;;
    *-mingw32* | *-cygwin* | *-msys)
      DEVICE_LINK_OPTS="$DEVICE_LINK_OPTS -lws2_32"
      ;;
  esac
fi

AC_MSG_CHECKING(for raw serial support)
AC_ARG_ENABLE(raw-serial,
  AS_HELP_STRING([--enable-raw-serial], [use raw serial port access (no - incomplete)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_USE_RAW_SERIAL, 1)
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_USE_RAW_SERIAL, 0)
   fi],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_USE_RAW_SERIAL, 0)
    ]
  )

DISPLAY_OBJS=''
DISPLAY_EXTRA_OBJS=''
DISPLAY_DLL_TARGETS=''
AC_MSG_CHECKING(for CLGD54XX emulation)
AC_ARG_ENABLE(clgd54xx,
  AS_HELP_STRING([--enable-clgd54xx], [enable CLGD54XX emulation (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_CLGD54XX, 1)
    DISPLAY_OBJS="svga_cirrus.o"
    DISPLAY_DLL_TARGETS="bx_svga_cirrus.dll"
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_CLGD54XX, 0)
   fi],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_CLGD54XX, 0)
    ]
  )

AC_MSG_CHECKING(for 3dfx Voodoo Graphics emulation)
AC_ARG_ENABLE(voodoo,
  AS_HELP_STRING([--enable-voodoo], [enable 3dfx Voodoo Graphics emulation (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    if test "$pci" != "1"; then
      AC_MSG_ERROR([3dfx Voodoo Graphics emulation requires PCI support])
    fi
    AC_DEFINE(BX_SUPPORT_VOODOO, 1)
    DISPLAY_OBJS="$DISPLAY_OBJS voodoo.o"
    DISPLAY_EXTRA_OBJS="$DISPLAY_EXTRA_OBJS banshee.o"
    DISPLAY_DLL_TARGETS="$DISPLAY_DLL_TARGETS bx_voodoo.dll"
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_VOODOO, 0)
   fi],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_VOODOO, 0)
    ]
  )
AC_SUBST(DISPLAY_OBJS)
AC_SUBST(DISPLAY_EXTRA_OBJS)
AC_SUBST(DISPLAY_DLL_TARGETS)


AC_MSG_CHECKING(for lowlevel CDROM support)
AC_ARG_ENABLE(cdrom,
  AS_HELP_STRING([--enable-cdrom], [lowlevel CDROM support (yes)]),
  [if test "$enableval" = no; then
     bx_cdrom=0
   else
     bx_cdrom=1
   fi],
  [ bx_cdrom=1 ]
  )

CDROM_OBJS='cdrom.o'
if test "$bx_cdrom" = 1; then
  AC_MSG_RESULT(yes)
  case $target in
    *-pc-windows* | *-pc-winnt* | *-cygwin* | *-mingw32* | *.msys)
      CDROM_OBJS="$CDROM_OBJS cdrom_win32.o"
      AC_DEFINE(LOWLEVEL_CDROM, cdrom_win32_c)
      AC_DEFINE(BX_SUPPORT_CDROM, 1)
      ;;
    *)
      AC_CHECK_HEADER(IOKit/storage/IOCDMedia.h,
        can_use_osx_cdrom=yes
        )
      if test "$with_amigaos" = yes; then
        # use the amiga cdrom file instead.
        CDROM_OBJS="$CDROM_OBJS cdrom_amigaos.o"
        AC_DEFINE(LOWLEVEL_CDROM, cdrom_amigaos_c)
      elif test "$can_use_osx_cdrom" = yes; then
        # use cdrom_osx
        AC_MSG_RESULT(Using OSX IOKit CD Interface)
        CDROM_OBJS="$CDROM_OBJS cdrom_osx.o"
        AC_DEFINE(LOWLEVEL_CDROM, cdrom_osx_c)
        EXTRA_LINK_OPTS="${EXTRA_LINK_OPTS} -framework IOKit -framework CoreFoundation "'$(GUI_LINK_OPTS)'
      else
        CDROM_OBJS="$CDROM_OBJS cdrom_misc.o"
        AC_DEFINE(LOWLEVEL_CDROM, cdrom_misc_c)
      fi
      AC_DEFINE(BX_SUPPORT_CDROM, 1)
      ;;
  esac
else
  AC_MSG_RESULT(no)
  AC_DEFINE(BX_SUPPORT_CDROM, 0)
fi

AC_SUBST(CDROM_OBJS)


SOUNDHW_OBJS=''
SOUNDHW_DLL_TARGETS=''
SOUNDLOW_OBJS=''
GAME_OBJS=''
soundlow_drivers=''
soundlow_default=''
soundcard_present=0
gameport_present=0
AC_MSG_CHECKING(for Sound Blaster 16 support)
AC_ARG_ENABLE(sb16,
  AS_HELP_STRING([--enable-sb16], [Sound Blaster 16 Support (no)]),
  [if test "$enableval" = yes; then
     AC_MSG_RESULT(yes)
     SOUNDHW_OBJS='sb16.o'
     SOUNDHW_DLL_TARGETS="bx_sb16.dll"
     soundcard_present=1
     AC_DEFINE(BX_SUPPORT_SB16, 1)
     GAME_OBJS='gameport.o'
     gameport_present=1
   else
     AC_MSG_RESULT(no)
     AC_DEFINE(BX_SUPPORT_SB16, 0)
   fi],

  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_SB16, 0)
    ]
  )

AC_MSG_CHECKING(for ES1370 soundcard support)
AC_ARG_ENABLE(es1370,
  AS_HELP_STRING([--enable-es1370], [enable ES1370 soundcard support (no)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    if test "$pci" != "1"; then
      AC_MSG_ERROR([ES1370 soundcard requires PCI support])
    fi
    SOUNDHW_OBJS="$SOUNDHW_OBJS es1370.o"
    SOUNDHW_DLL_TARGETS="$SOUNDHW_DLL_TARGETS bx_es1370.dll"
    soundcard_present=1
    AC_DEFINE(BX_SUPPORT_ES1370, 1)
    GAME_OBJS='gameport.o'
    gameport_present=1
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_ES1370, 0)
   fi],
  [
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_ES1370, 0)
    ]
  )

if test "$soundcard_present" = 1; then
  AC_DEFINE(BX_SUPPORT_SOUNDLOW, 1)
  case "$target" in
    *-linux-android*)
      SOUNDLOW_OBJS='soundsdl.o'
      soundlow_drivers="sdl"
      soundlow_default="sdl"
      AC_DEFINE(BX_HAVE_SOUND_SDL, 1)
      ;;
    *-linux* | *-freebsd*)
      SOUNDLOW_OBJS='soundoss.o'
      soundlow_drivers="oss"
      soundlow_default="oss"
      AC_DEFINE(BX_HAVE_SOUND_OSS, 1)
      bx_have_alsa=0
      AC_CHECK_HEADER([alsa/asoundlib.h], [bx_have_alsa=1])
      if test "$bx_have_alsa" = 1; then
        SOUNDLOW_OBJS="$SOUNDLOW_OBJS soundalsa.o"
        soundlow_default="alsa"
        soundlow_drivers="alsa $soundlow_drivers"
        AC_DEFINE(BX_HAVE_SOUND_ALSA, 1)
        if test "$bx_plugins" = 1; then
          ALSA_SOUND_LINK_OPTS="$ALSA_SOUND_LINK_OPTS -lasound"
        else
          DEVICE_LINK_OPTS="$DEVICE_LINK_OPTS -lasound"
        fi
      fi
      ;;
    *-pc-windows*)
      SOUNDLOW_OBJS='soundwin.o'
      DEVICE_LINK_OPTS="$DEVICE_LINK_OPTS winmm.lib"
      soundlow_drivers="win"
      soundlow_default="win"
      AC_DEFINE(BX_HAVE_SOUND_WIN, 1)
      ;;
    *-cygwin* | *-mingw32* | *-msys)
      SOUNDLOW_OBJS='soundwin.o'
      DEVICE_LINK_OPTS="$DEVICE_LINK_OPTS -lwinmm"
      soundlow_drivers="win"
      soundlow_default="win"
      AC_DEFINE(BX_HAVE_SOUND_WIN, 1)
      ;;
    *-macosx* | *-macos* | *-apple-darwin*)
      SOUNDLOW_OBJS='soundosx.o'
      DEVICE_LINK_OPTS="$DEVICE_LINK_OPTS -framework CoreServices -framework AudioUnit -framework AudioToolbox"
      soundlow_drivers="osx"
      soundlow_default="osx"
      AC_DEFINE(BX_HAVE_SOUND_OSX, 1)
      ;;
    *)
      SOUNDLOW_OBJS=''
      soundlow_default="dummy"
      ;;
  esac
  if test "$with_sdl" = yes; then
    SOUNDLOW_OBJS="$SOUNDLOW_OBJS soundsdl.o"
    if test "$bx_plugins" = 1; then
      SDL_SOUND_LINK_OPTS="$SDL_SOUND_LINK_OPTS `sdl-config --libs`"
    fi
    soundlow_drivers="$soundlow_drivers sdl"
    if test "$soundlow_default" = "dummy"; then
      soundlow_default="sdl"
    fi
    AC_DEFINE(BX_HAVE_SOUND_SDL, 1)
  elif test "$with_sdl2" = yes; then
    SOUNDLOW_OBJS="$SOUNDLOW_OBJS soundsdl.o"
    if test "$bx_plugins" = 1; then
      SDL_SOUND_LINK_OPTS="$SDL_SOUND_LINK_OPTS `sdl2-config --libs`"
    fi
    soundlow_drivers="$soundlow_drivers sdl"
    if test "$soundlow_default" = "dummy"; then
      soundlow_default="sdl"
    fi
    AC_DEFINE(BX_HAVE_SOUND_SDL, 1)
    SDL2_VERSION=`sdl2-config --version`
    # SDL version >= 2.0.5 supports audio capture
    sdl2_audio_capture=0
    [
    case x$SDL2_VERSION in
      x2.0.[0-4]) ;;
      x2.0.[5-9]) sdl2_audio_capture=1 ;;
      *) ;;
    esac
    ]
    if test "$sdl2_audio_capture" = 1; then
      AC_DEFINE(BX_HAVE_SDL2_AUDIO_CAPTURE, 1)
    fi
  fi
  if test "$cross_configure" = 0 -a "$DEFAULT_GUI" != win32; then
    if test "x$PKG_CONFIG" != x; then
      $PKG_CONFIG --exists libpulse-simple
      if test x$? = x0; then
        SOUNDLOW_OBJS="$SOUNDLOW_OBJS soundpulse.o"
        soundlow_drivers="$soundlow_drivers pulse"
        AC_DEFINE(BX_HAVE_SOUND_PULSE, 1)
        if test "$bx_plugins" = 1; then
          PULSE_SOUND_LINK_OPTS="$PULSE_SOUND_LINK_OPTS `$PKG_CONFIG --libs libpulse-simple`"
        else
          DEVICE_LINK_OPTS="$DEVICE_LINK_OPTS `$PKG_CONFIG --libs libpulse-simple`"
        fi
      fi
    fi
  fi
  if test "$MSVC_TARGET" = 0; then
    AC_CHECK_HEADER([samplerate.h], [bx_have_libsamplerate=1])
    if test "$bx_have_libsamplerate" = 1; then
      DEVICE_LINK_OPTS="$DEVICE_LINK_OPTS -lsamplerate"
      AC_DEFINE(BX_HAVE_LIBSAMPLERATE, 1)
    else
      AC_CHECK_HEADER([soxr-lsr.h], [bx_have_soxr_lsr=1])
      if test "$bx_have_soxr_lsr" = 1; then
        DEVICE_LINK_OPTS="$DEVICE_LINK_OPTS -lsoxr-lsr"
        AC_DEFINE(BX_HAVE_SOXR_LSR, 1)
      fi
    fi
  fi
  SOUND_LIB_VAR='iodev/sound/libsound.a'
  AC_SUBST(SOUND_LIB_VAR)
  AC_MSG_CHECKING(for sound lowlevel modules)
  AC_MSG_RESULT($soundlow_drivers)
  AC_MSG_CHECKING(for default sound lowlevel module)
  AC_MSG_RESULT($soundlow_default)
  AC_DEFINE_UNQUOTED(BX_SOUND_LOWLEVEL_NAME, "$soundlow_default")
else
  AC_DEFINE(BX_SUPPORT_SOUNDLOW, 0)
fi
AC_SUBST(SOUNDHW_OBJS)
AC_SUBST(SOUNDLOW_OBJS)
AC_SUBST(ALSA_SOUND_LINK_OPTS)
AC_SUBST(PULSE_SOUND_LINK_OPTS)
AC_SUBST(SDL_SOUND_LINK_OPTS)
AC_SUBST(SOUNDHW_DLL_TARGETS)


AC_MSG_CHECKING(for standard PC gameport support)
AC_ARG_ENABLE(gameport,
  AS_HELP_STRING([--enable-gameport], [enable standard PC gameport support (yes, if soundcard present)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_GAMEPORT, 1)
    if test "$gameport_present" = 0; then
      GAME_OBJS='gameport.o'
      case $target in
        *-cygwin* | *-mingw32* | *-msys)
           DEVICE_LINK_OPTS="$DEVICE_LINK_OPTS -lwinmm"
           ;;
      esac
    fi
   else
    AC_MSG_RESULT(no)
    GAME_OBJS=''
    AC_DEFINE(BX_SUPPORT_GAMEPORT, 0)
   fi],
  [
    if test "$gameport_present" = 1; then
      AC_DEFINE(BX_SUPPORT_GAMEPORT, 1)
      AC_MSG_RESULT(yes)
    else
      AC_DEFINE(BX_SUPPORT_GAMEPORT, 0)
      AC_MSG_RESULT(no)
    fi]
  )
AC_SUBST(GAME_OBJS)


BUSM_OBJS=''
bx_busmouse=0
AC_MSG_CHECKING(for Busmouse support)
AC_ARG_ENABLE(busmouse,
  AS_HELP_STRING([--enable-busmouse], [enable Busmouse support (InPort & Standard)]),
  [if test "$enableval" = yes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(BX_SUPPORT_BUSMOUSE, 1)
    BUSM_OBJS='busmouse.o'
    bx_busmouse=1
   else
    AC_MSG_RESULT(no)
    AC_DEFINE(BX_SUPPORT_BUSMOUSE, 0)
   fi],
  [
    AC_DEFINE(BX_SUPPORT_BUSMOUSE, 0)
    AC_MSG_RESULT(no)]
  )
AC_SUBST(BUSM_OBJS)


AC_PATH_PROG(DOCBOOK2HTML, docbook2html, not_found)
AC_CHECK_PROGS([JADE], [jade openjade], not_found)
AC_MSG_CHECKING(whether to build docbook documentation)
build_docbook=0
AC_ARG_ENABLE(docbook,
  AS_HELP_STRING([--enable-docbook], [build the Docbook documentation (yes, if docbook present)]),
  [
    if test "$enableval" = yes; then build_docbook=1; fi
  ],
  [
    dnl Not specified. If they have docbook2html or jade, then yes.
    if test "$DOCBOOK2HTML" != not_found; then
      build_docbook=1
    else
      if test "$JADE" != not_found; then build_docbook=1; fi
    fi
    ]
  )

if test $build_docbook = 1; then
  AC_MSG_RESULT(yes)
  BUILD_DOCBOOK_VAR=build_docbook
  INSTALL_DOCBOOK_VAR=install_docbook
  CLEAN_DOCBOOK_VAR=clean_docbook
  if test "$DOCBOOK2HTML" = not_found -a "$JADE" = not_found; then
    echo "ERROR: docbook2html or jade must be installed to build the documentation"
    exit 1
  fi
else
  AC_MSG_RESULT(no)
fi
AC_SUBST(BUILD_DOCBOOK_VAR)
AC_SUBST(INSTALL_DOCBOOK_VAR)
AC_SUBST(CLEAN_DOCBOOK_VAR)
AC_SUBST(DOCBOOK2HTML)

PRIMARY_TARGET='bochs'
PLUGIN_LIBNAME_TRANSFORMATION='%.o=libbx_%.la'
GUI_PLUGIN_NAME_TRANSFORMATION='%.o=libbx_%_gui.la'
IMG_PLUGIN_NAME_TRANSFORMATION='%.o=libbx_%_img.la'

if test "$bx_plugins" = 0; then
  DISPLAY_LIB_VAR='iodev/display/libdisplay.a'
  AC_SUBST(DISPLAY_LIB_VAR)
fi
HDIMAGE_LIB_VAR='iodev/hdimage/libhdimage.a'
AC_SUBST(HDIMAGE_LIB_VAR)
IODEV_LIB_VAR='iodev/libiodev.a'
NONINLINE_VAR='$(NONINLINE_OBJS)'

AC_SUBST(IODEV_LIB_VAR)
AC_SUBST(EXTRA_BX_OBJS)
AC_SUBST(NONINLINE_VAR)
AC_SUBST(INLINE_VAR)
AC_SUBST(EXTERNAL_DEPENDENCY)
AC_SUBST(RC_CMD)


AC_ARG_WITH(x11,
  [  --with-x11                        use X11 GUI],
  )

AC_ARG_WITH(win32,
  [  --with-win32                      use Win32 GUI],
  )

AC_ARG_WITH(macos,
  [  --with-macos                      use Macintosh/CodeWarrior environment],
  )

AC_ARG_WITH(carbon,
  [  --with-carbon                     compile for MacOS X with Carbon GUI],
  )

AC_ARG_WITH(nogui,
  [  --with-nogui                      no native GUI, just use blank stubs],
  )

AC_ARG_WITH(term,
  [  --with-term                       textmode terminal environment],
  )

AC_ARG_WITH(rfb,
  [  --with-rfb                        use RFB protocol, works with VNC viewer],
  )

AC_ARG_WITH(vncsrv,
  [  --with-vncsrv                     use LibVNCServer, works with VNC viewer],
  )

AC_ARG_WITH(amigaos,
  [  --with-amigaos                    use AmigaOS (or MorphOS) GUI],
  )

AC_ARG_WITH(sdl,
  [  --with-sdl                        use SDL libraries],
  )

AC_ARG_WITH(sdl2,
  [  --with-sdl2                       use SDL2 libraries],
  )

AC_ARG_WITH(wx,
  [  --with-wx                         use wxWidgets libraries],
  )

AC_ARG_WITH(all-libs,
  [  --with-all-libs                   compile all guis that Bochs supports],
  )

dnl // DASH is option prefix for your platform
dnl // SLASH is directory for your platform
dnl // CXXFP is C++ File Prefix; the flag that tells the compiler
dnl //   this is a C++ source file
dnl // CFP is C File Prefix; the flag that tells the compiler
dnl //   this is a C source file
dnl // OFP is Object File Prefix; the flag that tells the compiler
dnl //   generate an object file with this name
DASH="-"
SLASH="/"
CXXFP=""
CFP=""
OFP="-o "
MAKELIB="ar rv \$@"
RMCOMMAND="rm -f "
# first define link variables without libtool
LINK="\$(CXX) -o \$@ \$(CXXFLAGS) \$(LDFLAGS)"
LINK_CONSOLE="\$(CXX) -o \$@ \$(CXXFLAGS_CONSOLE) \$(LDFLAGS)"
# unless NO_LT is set, add libtool to the front of the link command
if test "$NO_LT" != 1; then
  LINK="\$(LIBTOOL) --mode=link --tag CXX $LINK"
  LINK_CONSOLE="\$(LIBTOOL) --mode=link --tag CXX $LINK_CONSOLE"
fi
EXE=""
COMMAND_SEPARATOR="&& \\"
CD_UP_ONE="echo done"
CD_UP_TWO="echo done"
CD_UP_THREE="echo done"

INSTALL_TARGET=install_unix
INSTALL_LIST_FOR_PLATFORM=

# check for existence of and usable version of wxWidgets.  This comes
# early so that it could affect which one gets chosen.
if test x$WX_CONFIG = x; then
  if test "$enable_static_link" = yes; then
    AC_CHECK_PROG(WX_CONFIG, [ wx-config-static --version ], wx-config-static, not_found)
  else
    AC_CHECK_PROG(WX_CONFIG, [ wx-config --version ], wx-config, not_found)
  fi
fi
AC_MSG_CHECKING(for wxWidgets configuration script)
AC_MSG_RESULT($WX_CONFIG)

ok_wx_version=0
wx_multi_lib=0
needs_gtk2=0
wx_needs_gdk_version=0
wx_msw_unicode=0
AC_MSG_CHECKING(for wxWidgets library version)
if test x$WX_CONFIG != xnot_found; then
  WX_VERSION=`$WX_CONFIG --version`
  WX_BASENAME=`$WX_CONFIG --basename`
  [
  if test "$WX_BASENAME" = wx_mswu; then
    wx_msw_unicode=1
  fi
  # test that version >= 2.3.2.  Anybody have a better way to do this? Yuck.
  case x$WX_VERSION in
    x2.[012]*) ;; # less than version 2.3.x
    x2.3.[01]) ;; # less than version 2.3.2
    x2.3.[2-4]*) ok_wx_version=1 ;;  # 2.3.2 or greater
    x2.[4-5]*) ok_wx_version=1 ;;  # version 2.4 / 2.5
    x2.[6-9]*)  # version 2.6 or greater
      ok_wx_version=1
      wx_multi_lib=1
      case x$WX_BASENAME in
        xwx_gtk2|xwx_gtk2u) wx_needs_gdk_version=2 ;;
        *) ;;
      esac
      ;;
    x3*)  # version 3
      ok_wx_version=1
      wx_multi_lib=1
      case x$WX_BASENAME in
        xwx_gtk2|xwx_gtk2u) wx_needs_gdk_version=2 ;;
        xwx_gtk3|xwx_gtk3u) wx_needs_gdk_version=3 ;;
        *) ;;
      esac
      ;;
    *) ;; # who knows?
  esac
  ]
fi
AC_MSG_RESULT($WX_VERSION)
if test $wx_msw_unicode = 1; then
  AC_DEFINE(WX_MSW_UNICODE, 1)
fi
if test "$cross_configure" = 0; then
  if test "$ok_wx_version" = 1; then
    # detected a usable version
    if test "$with_all_libs" = yes; then
      with_wx=yes
    fi
  else
    if test "$with_wx" = yes; then
      AC_MSG_WARN([Bochs for wxWidgets cannot be compiled here, disabling it])
    fi
    with_wx=no
  fi
fi

AC_MSG_CHECKING(for default gui on this platform)
AC_MSG_RESULT($DEFAULT_GUI)

display_libs=""
GUI_DLL_TARGETS=""
use_gui_console=0

# the $with_* variable tells the gui library to use, but does NOT necessarily
# indicate the platform.  Settings that depend on the platform should be
# handled later.

XPM_LIB=''
XRANDR_LIB=''
if test "$with_x11" = yes; then
  display_libs="$display_libs X11"
  if test "$no_x" = yes; then
    echo ERROR: X windows gui was selected, but X windows libraries were not found.
    exit 1
  fi

  check_xpm=0
  AC_MSG_CHECKING(whether user wants XPM support)
  AC_ARG_ENABLE(xpm,
    AS_HELP_STRING([--enable-xpm], [enable the check for XPM support (yes)]),
    [if test "$enableval" = yes; then
      check_xpm=0
      AC_MSG_RESULT(yes)
      check_xpm=1
     else
      AC_MSG_RESULT(no)
      check_xpm=0
     fi],
    [
      AC_MSG_RESULT(yes)
      check_xpm=1
      ]
    )
  if test $check_xpm = 1; then
    AC_CHECK_HEADER(X11/xpm.h, [
        AC_DEFINE(BX_HAVE_XPM_H)
        XPM_LIB='-lXpm'
      ])
  fi
  AC_CHECK_HEADER(X11/extensions/Xrandr.h, [
    AC_DEFINE(BX_HAVE_XRANDR_H)
    XRANDR_LIB='-lXrandr'
    ])

  if test "x$PKG_CONFIG" != x; then
    X_CFLAGS="`$PKG_CONFIG --cflags x11`"
    X_LIBS="`$PKG_CONFIG --libs x11` $XPM_LIB $XRANDR_LIB"
  else
    X_LIBS="$X_LIBS -lX11 $XPM_LIB $XRANDR_LIB"
  fi

  AC_DEFINE(BX_WITH_X11, 1)
  SPECIFIC_GUI_OBJS="$SPECIFIC_GUI_OBJS \$(GUI_OBJS_X11)"
  GUI_CFLAGS="$GUI_CFLAGS \$(X_CFLAGS)"
  GUI_CXXFLAGS="$GUI_CXXFLAGS \$(X_CFLAGS)"
  GUI_LINK_OPTS="$GUI_LINK_OPTS \$(GUI_LINK_OPTS_X)"
  # The enhanced X debugger depends on GTK2
  if test "$gui_debugger" = 1 -o "$usb_debugger" = 1; then
    needs_gtk2=1
  fi
  use_gui_console=1
fi
AC_SUBST(X_CFLAGS)
AC_SUBST(X_LIBS)

if test "$with_sdl" = yes; then
  display_libs="$display_libs sdl"
  GUI_DLL_TARGETS="$GUI_DLL_TARGETS bx_sdl_gui.dll"
  AC_DEFINE(BX_WITH_SDL, 1)
  SPECIFIC_GUI_OBJS="$SPECIFIC_GUI_OBJS \$(GUI_OBJS_SDL)"
  # GUI_*FLAGS are added to the compilation of every bochs file, not just
  # the files in gui/*.cc.
  SDL_CFLAGS=`sdl-config --cflags`
  GUI_CFLAGS="$GUI_CFLAGS $SDL_CFLAGS"
  GUI_CXXFLAGS="$GUI_CXXFLAGS $SDL_CFLAGS"
  GUI_LINK_OPTS_SDL="`sdl-config --libs`"
  GUI_LINK_OPTS="$GUI_LINK_OPTS \$(GUI_LINK_OPTS_SDL)"
  # The plugin version uses multi-threading support in Bochs core
  if test "$bx_plugins" = 1; then
    NONPLUGIN_GUI_LINK_OPTS="`sdl-config --libs`"
  fi
  # The enhanced X debugger depends on GTK2
  if test "$DEFAULT_GUI" != win32; then
    if test "$gui_debugger" = 1 -o "$usb_debugger" = 1; then
      needs_gtk2=1
    fi
  fi
  if test "$with_win32" != yes -a "$with_wx" != yes; then
    case $target in
      *-pc-windows*)
         RC_CMD="rc /fo"
         EXTRA_BX_OBJS="$EXTRA_BX_OBJS win32res.o"
         ;;
      *-cygwin* | *-mingw32* | *-msys)
         RC_CMD="windres -I. -o "
         EXTRA_BX_OBJS="$EXTRA_BX_OBJS win32res.o"
         ;;
    esac
  fi
  use_gui_console=1
fi

if test "$with_sdl2" = yes; then
  display_libs="$display_libs sdl2"
  GUI_DLL_TARGETS="$GUI_DLL_TARGETS bx_sdl2_gui.dll"
  AC_DEFINE(BX_WITH_SDL2, 1)
  SPECIFIC_GUI_OBJS="$SPECIFIC_GUI_OBJS \$(GUI_OBJS_SDL2)"
  # GUI_*FLAGS are added to the compilation of every bochs file, not just
  # the files in gui/*.cc.
  SDL2_CFLAGS=`sdl2-config --cflags`
  GUI_CFLAGS="$GUI_CFLAGS $SDL2_CFLAGS"
  GUI_CXXFLAGS="$GUI_CXXFLAGS $SDL2_CFLAGS"
  GUI_LINK_OPTS_SDL2="`sdl2-config --libs`"
  GUI_LINK_OPTS="$GUI_LINK_OPTS \$(GUI_LINK_OPTS_SDL2)"
  # The plugin version uses multi-threading support in Bochs core
  if test "$bx_plugins" = 1; then
    NONPLUGIN_GUI_LINK_OPTS="`sdl2-config --libs`"
  fi
  # The enhanced X debugger depends on GTK2
  if test "$DEFAULT_GUI" != win32; then
    if test "$gui_debugger" = 1 -o "$usb_debugger" = 1; then
      needs_gtk2=1
    fi
  fi
  if test "$with_win32" != yes -a "$with_wx" != yes; then
    case $target in
      *-pc-windows*)
         RC_CMD="rc /fo"
         EXTRA_BX_OBJS="$EXTRA_BX_OBJS win32res.o"
         ;;
      *-cygwin* | *-mingw32* | *-msys)
         RC_CMD="windres -I. -o "
         EXTRA_BX_OBJS="$EXTRA_BX_OBJS win32res.o"
         ;;
    esac
  fi
  use_gui_console=1
fi

if test "$with_rfb" = yes; then
  display_libs="$display_libs rfb"
  GUI_DLL_TARGETS="$GUI_DLL_TARGETS bx_rfb_gui.dll"
  AC_DEFINE(BX_WITH_RFB, 1)
  SPECIFIC_GUI_OBJS="$SPECIFIC_GUI_OBJS \$(GUI_OBJS_RFB)"
  GUI_LINK_OPTS="$GUI_LINK_OPTS \$(GUI_LINK_OPTS_RFB)"
  use_gui_console=1
fi

if test "$with_vncsrv" = yes; then
  if test "$PKG_CONFIG" != not_found; then
    $PKG_CONFIG --exists libvncserver
    if test x$? = x0; then
      VNCSRV_CXXFLAGS="`$PKG_CONFIG --cflags libvncserver`"
      GUI_LINK_OPTS_VNCSRV="`$PKG_CONFIG --libs libvncserver`"
    fi
  else
    AC_CHECK_PROG(LIBVNCSERVER_CONFIG, [ libvncserver-config --version ], libvncserver-config, not_found)
    AC_MSG_CHECKING(for LibVNCServer configuration script)
    AC_MSG_RESULT($LIBVNCSERVER_CONFIG_CONFIG)
    VNCSRV_CXXFLAGS="`$LIBVNCSERVER_CONFIG --cflags`"
    GUI_LINK_OPTS_VNCSRV="`$LIBVNCSERVER_CONFIG --libs`"
  fi
  if test "$GUI_LINK_OPTS_VNCSRV" != ""; then
    display_libs="$display_libs vncsrv"
    AC_DEFINE(BX_WITH_VNCSRV, 1)
    SPECIFIC_GUI_OBJS="$SPECIFIC_GUI_OBJS \$(GUI_OBJS_VNCSRV)"
    GUI_CXXFLAGS="$GUI_CXXFLAGS $VNCSRV_CXXFLAGS"
    if test "$bx_plugins" = 0; then
      GUI_LINK_OPTS="$GUI_LINK_OPTS \$(GUI_LINK_OPTS_VNCSRV)"
    fi
  else
    AC_MSG_WARN([Bochs for LibVNCServer cannot be compiled here, disabling it])
    with_vncsrv=no
  fi
  use_gui_console=1
fi

if test "$with_amigaos" = yes; then
  display_libs="$display_libs amigaos"
  AC_DEFINE(BX_WITH_AMIGAOS, 1)
  SPECIFIC_GUI_OBJS="$SPECIFIC_GUI_OBJS \$(GUI_OBJS_AMIGAOS)"
  GUI_LINK_OPTS="$GUI_LINK_OPTS \$(GUI_LINK_OPTS_AMIGAOS)"
fi

if test "$with_win32" = yes; then
  display_libs="$display_libs win32"
  GUI_DLL_TARGETS="$GUI_DLL_TARGETS bx_win32_gui.dll"
  AC_DEFINE(BX_WITH_WIN32, 1)
  SPECIFIC_GUI_OBJS="$SPECIFIC_GUI_OBJS \$(GUI_OBJS_WIN32)"
  case $target in
    *-pc-windows*)
       if test "$with_wx" != yes; then
         RC_CMD="rc /fo"
         EXTRA_BX_OBJS="$EXTRA_BX_OBJS win32res.o"
       fi
       GUI_LINK_OPTS="$GUI_LINK_OPTS \$(GUI_LINK_OPTS_WIN32_VCPP)"
       ;;
    *-cygwin* | *-mingw32* | *-msys)
       if test "$with_wx" != yes; then
         if test "$cross_compiling" = no; then
           RC_CMD="windres -I. -o "
         else
           RC_CMD="$WINDRES -I. -o "
         fi
         EXTRA_BX_OBJS="$EXTRA_BX_OBJS win32res.o"
       fi
       GUI_LINK_OPTS="$GUI_LINK_OPTS \$(GUI_LINK_OPTS_WIN32)"
       ;;
    *) echo Unsupported compile setup: GUI library is win32, but target is neither windows nor cygwin.
       ;;
  esac
fi

if test "$with_macos" = yes; then
  display_libs="$display_libs macos"
  AC_DEFINE(BX_WITH_MACOS, 1)
  SPECIFIC_GUI_OBJS="$SPECIFIC_GUI_OBJS \$(GUI_OBJS_MACOS)"
  GUI_LINK_OPTS="$GUI_LINK_OPTS \$(GUI_LINK_OPTS_MACOS)"
  AC_MSG_WARN([This Bochs feature is not maintained yet and may fail])
fi

if test "$with_carbon" = yes; then
  display_libs="$display_libs carbon"
  AC_DEFINE(BX_WITH_CARBON, 1)
  SPECIFIC_GUI_OBJS="$SPECIFIC_GUI_OBJS \$(GUI_OBJS_CARBON)"
  GUI_LINK_OPTS="$GUI_LINK_OPTS \$(GUI_LINK_OPTS_CARBON)"
  if test "$bx_plugins" = 1; then
    PRIMARY_TARGET=bochs.app/.build_plugins   # only for carbon application
  else
    PRIMARY_TARGET=bochs.app/.build   # only for carbon application
  fi
  INSTALL_TARGET='install_macosx'
  AC_MSG_WARN([This Bochs feature is not maintained yet and may fail])
fi

if test "$with_term" = yes; then
  display_libs="$display_libs term"
  AC_DEFINE(BX_WITH_TERM, 1)
  SPECIFIC_GUI_OBJS="$SPECIFIC_GUI_OBJS \$(GUI_OBJS_TERM)"
  GUI_LINK_OPTS="$GUI_LINK_OPTS \$(GUI_LINK_OPTS_TERM)"
  use_curses=yes
fi

if test "$with_wx" = yes; then
  display_libs="$display_libs wxWidgets"
  if test "$cross_configure" = 1; then
    true  # do not insist, if configuring for another machine
  else
    if test x$ok_wx_version != x1; then
      echo ERROR: A usable version of wxWidgets was not found.
      echo Upgrade the library or choose another gui.
      exit 1
    fi
  fi
  AC_DEFINE(BX_WITH_WX, 1)
  SPECIFIC_GUI_OBJS="$SPECIFIC_GUI_OBJS \$(GUI_OBJS_WX)"
  SPECIFIC_GUI_SUPPORT_OBJS="$SPECIFIC_GUI_SUPPORT_OBJS \$(GUI_OBJS_WX_SUPPORT)"
  WX_CFLAGS="`$WX_CONFIG --cflags`"
  WX_CXXFLAGS="`$WX_CONFIG --cxxflags`"
  if test "$wx_needs_gdk_version" = 2; then
    GDK_CFLAGS="`$PKG_CONFIG --cflags gdk-2.0`"
    WX_CFLAGS="$WX_CFLAGS $GDK_CFLAGS"
    WX_CXXFLAGS="$WX_CXXFLAGS $GDK_CFLAGS"
    AC_DEFINE(WX_GDK_VERSION, 2)
  fi
  if test "$wx_needs_gdk_version" = 3; then
    GDK_CFLAGS="`$PKG_CONFIG --cflags gdk-3.0`"
    WX_CFLAGS="$WX_CFLAGS $GDK_CFLAGS"
    WX_CXXFLAGS="$WX_CXXFLAGS $GDK_CFLAGS"
    AC_DEFINE(WX_GDK_VERSION, 3)
  fi
  if test "$wx_multi_lib" = 1; then
    GUI_LINK_OPTS_WX="`$WX_CONFIG --libs core,base`"
  else
    GUI_LINK_OPTS_WX="`$WX_CONFIG --libs`"
  fi
  # GUI_C*FLAGS are added to the compilation of every bochs file, not just
  # the files in gui/*.cc.  They are not used when building other things
  # such as bximage.
  GUI_CFLAGS="$GUI_CFLAGS $WX_CFLAGS"
  GUI_CXXFLAGS="$GUI_CXXFLAGS $WX_CXXFLAGS"
  GUI_LINK_OPTS="$GUI_LINK_OPTS \$(GUI_LINK_OPTS_WX)"
  # using debugger with readline is failing due to thread/signal handler
  # problems.
  case $target in
    *-pc-windows* | *-cygwin* | *-mingw32* | *-msys)
       EXTRA_BX_OBJS="$EXTRA_BX_OBJS wxbochs_resources.o"
	   GUI_LINK_OPTS_WX="$GUI_LINK_OPTS_WX"
       ;;
    *)
      # This is required for XStringToKeysym
      if test "$with_x11" != yes -a "$bx_plugins" != 1; then
        GUI_LINK_OPTS="$GUI_LINK_OPTS -lX11"
      fi
      ;;
  esac
  # The enhanced X debugger depends on GTK2
  if test "$DEFAULT_GUI" != win32; then
    if test "$gui_debugger" = 1 -o "$usb_debugger" = 1; then
      needs_gtk2=1
    fi
  fi
fi

if test "$with_nogui" = yes; then
  display_libs="$display_libs nogui"
  GUI_DLL_TARGETS="$GUI_DLL_TARGETS bx_nogui_gui.dll"
  AC_DEFINE(BX_WITH_NOGUI, 1)
  SPECIFIC_GUI_OBJS="$SPECIFIC_GUI_OBJS \$(GUI_OBJS_NOGUI)"
fi

AC_MSG_CHECKING(for display libraries)
AC_MSG_RESULT($display_libs)

if test "$display_libs" = ""; then
  echo "No display libraries were selected. Use --with-* options to enable"
  echo "individual display libraries, or --with-all-libs to detect the ones"
  echo "that Bochs supports."
  exit 1
fi

# some display libraries and the enhanced debugger may depend on the GTK+ software package
bx_have_gtk_version=0
if test "$needs_gtk2" = 1; then
  # pkg-config is required to set TOOLKIT_CXXFLAGS and LIBS
  if test "x$PKG_CONFIG" != x; then
    # if wxGTK is based on GTK 3.0, use it for gui debugger to avoid conflicts
    if test "$with_wx" = yes -a "$wx_needs_gdk_version" = 2; then
      $PKG_CONFIG --exists gtk+-2.0
      if test x$? = x0; then
        TOOLKIT_CXXFLAGS="`$PKG_CONFIG --cflags gtk+-2.0`"
        LIBS="$LIBS `$PKG_CONFIG --libs gtk+-2.0`"
        bx_have_gtk_version=2
      fi
    else
      $PKG_CONFIG --exists gtk+-3.0
      if test x$? = x0; then
        TOOLKIT_CXXFLAGS="`$PKG_CONFIG --cflags gtk+-3.0`"
        LIBS="$LIBS `$PKG_CONFIG --libs gtk+-3.0`"
        bx_have_gtk_version=3
      fi
    fi
  else
    echo "ERROR: pkg-config was not found, unable to access the gtk+-2.0 package."
    echo "The gui debugger support is disabled now."
  fi
fi
AC_DEFINE_UNQUOTED(BX_HAVE_GTK_VERSION, $bx_have_gtk_version)

dnl To use wxWidgets you must select it as both the configuration interface
dnl and the display library.  In the simplest case where the user has
dnl only configured with --with-wx, set both defaults to wxWidgets.
dnl Without this setting, the config interface would default to the text
dnl mode config interface, and then the wxWidgets display library would not
dnl be usable.
if test "$display_libs" = " wxWidgets"; then
  AC_MSG_CHECKING(for default configuration interface)
  AC_MSG_RESULT(wxWidgets)
  AC_MSG_CHECKING(for default display library)
  AC_MSG_RESULT(wxWidgets)
  AC_DEFINE(BX_DEFAULT_CONFIG_INTERFACE, "wx")
  AC_DEFINE(BX_DEFAULT_DISPLAY_LIBRARY, "wx")
  AC_DEFINE(BX_USE_TEXTCONFIG, 0)
  AC_DEFINE(HAVE_LIBREADLINE, 0)
  READLINE_LIB=""
else
  if test "$DEFAULT_GUI" = win32; then
    AC_DEFINE(BX_DEFAULT_CONFIG_INTERFACE, "win32config")
  else
    AC_DEFINE(BX_DEFAULT_CONFIG_INTERFACE, "textconfig")
  fi
  if test "$DEFAULT_GUI" = x11; then
    AC_DEFINE(BX_DEFAULT_DISPLAY_LIBRARY, "x")
  else
    AC_DEFINE_UNQUOTED(BX_DEFAULT_DISPLAY_LIBRARY, "$DEFAULT_GUI")
  fi
  AC_DEFINE(BX_USE_TEXTCONFIG, 1)
fi

if test "$use_gui_console" = 1; then
  AC_DEFINE(BX_USE_GUI_CONSOLE, 1)
else
  AC_DEFINE(BX_USE_GUI_CONSOLE, 0)
fi

AC_SUBST(SPECIFIC_GUI_OBJS)
AC_SUBST(SPECIFIC_GUI_SUPPORT_OBJS)
AC_SUBST(GUI_CFLAGS)
AC_SUBST(GUI_CXXFLAGS)
AC_SUBST(WX_CONFIG)
AC_SUBST(WX_CFLAGS)
AC_SUBST(WX_CXXFLAGS)
AC_SUBST(TOOLKIT_CXXFLAGS)

AC_CHECK_PROGS(WGET, wget curl fetch, not_found)
if test "$WGET" = not_found; then
  AC_MSG_WARN([cannot find wget (or equivalent) command])
fi
# everybody has to re-invent the wheel. differently.
test "${WGET}" = "curl" && WGET="curl -O"

# modify settings based on target platform
case "$target" in
  *-macosx*)
    AC_DEFINE(BX_HAVE_STRDUP, 0)
    ;;
  *-macos*)
    AC_DEFINE(BX_HAVE_STRDUP, 0)
    AC_DEFINE(BX_PLUGINS, 0)
    AC_DEFINE(BX_HAVE_DLFCN_H, 0)
    AC_DEFINE(BX_HAVE_SOCKLEN_T, 0)
    AC_DEFINE(BX_HAVE_SELECT, 0)
    AC_DEFINE(BX_HAVE_GETTIMEOFDAY, 0)
    AC_DEFINE(BX_NO_ATTRIBUTES, 1)
    AC_DEFINE(BX_NETMOD_TUNTAP, 0)
    ;;
  *-pc-windows*)
    INSTALL_TARGET='install_win32'
    CC="cl"
    CXX="$CC"
    #C_OPT="/Zi"   # for debugging
    if test "$speedup_fastcall" = 1; then
      C_OPT="/O2 /Gr"   # optimize for speed, using fastcall everywhere
    else
      C_OPT="/O2"   # optimize for speed, no fastcall
    fi
    if test "$bx_debugger" = 1; then
      C_OPT="$C_OPT /EHsc"
    else
      C_OPT="$C_OPT /EHs-c-"
    fi
    C_OPT="$C_OPT /std:c++20"
    if test "$bx_plugins" = 1; then
      CFLAGS="/nologo /MD /W3 /DNDEBUG /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS $C_OPT"
    else
      CFLAGS="/nologo /MT /W3 /DNDEBUG /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS $C_OPT"
    fi
    CXXFLAGS="$CFLAGS"
    DASH="/"
    SLASH="\\"
    CXXFP="/Tp"
    CFP="/Tc"
    OFP="/Fo"
    MAKE=nmake
    MAKELIB="lib /nologo /subsystem:console /verbose /out:\$@"
    RMCOMMAND="-del"
    #L_OPT="/debug"    # for debugging
    L_OPT=""           # no debug info
    LINK="link $L_OPT /nologo /subsystem:console /incremental:no /out:\$@ "
    LINK_CONSOLE=$LINK
    EXE=".exe"
    PRIMARY_TARGET="bochs.exe"
    BXIMAGE_LINK_OPTS="user32.lib"
    # also compile bxhub and niclist if networking is on
    if test "$networking" = yes; then
      OPTIONAL_TARGET="$OPTIONAL_TARGET bxhub.exe niclist.exe"
      BXHUB_FLAG="/DBXHUB"
      BXHUB_LINK_OPTS="ws2_32.lib"
    fi
    COMMAND_SEPARATOR=""
    CD_UP_ONE="cd .."
    CD_UP_TWO="cd ..\.."
    CD_UP_THREE="cd ..\..\.."
    have_gettimeofday=0  # even though it may exist in build environment
    AC_DEFINE(BX_64BIT_CONSTANTS_USE_LL, 0)
    AC_DEFINE(BX_CPP_INLINE, __forceinline)
    AC_DEFINE(BX_NO_ATTRIBUTES, 1)
    AC_DEFINE(HAVE_LIBREADLINE, 0)
    AC_DEFINE(HAVE_READLINE_HISTORY_H, 0)
    READLINE_LIB="";
    AC_DEFINE(BX_HAVE_DLFCN_H, 0)
    ;;
  *-cygwin*)
    EXE=".exe"
    PRIMARY_TARGET="bochs.exe"
    BXIMAGE_LINK_OPTS="-luser32"
    if test "$networking" = yes; then
      OPTIONAL_TARGET="$OPTIONAL_TARGET bxhub.exe niclist.exe"
      BXHUB_FLAG="-DBXHUB"
      BXHUB_LINK_OPTS="-lws2_32"
    fi
    AC_DEFINE(BX_HAVE_SELECT, 1)
    ;;
  *-mingw32* | *-msys)
    BXIMAGE_LINK_OPTS="-luser32"
    if test "$enable_static_link" = yes; then
      BXIMAGE_LINK_OPTS="${BXIMAGE_LINK_OPTS} -static"
    fi
    if test "$networking" = yes; then
      OPTIONAL_TARGET="$OPTIONAL_TARGET bxhub niclist"
      BXHUB_FLAG="-DBXHUB"
      BXHUB_LINK_OPTS="-lws2_32"
      if test "$enable_static_link" = yes; then
        BXHUB_LINK_OPTS="${BXHUB_LINK_OPTS} -static"
      fi
    fi
    AC_DEFINE(BX_HAVE_SELECT, 1)
    ;;
  *)
    if test "$networking" = yes; then
      OPTIONAL_TARGET="$OPTIONAL_TARGET bxhub"
      BXHUB_FLAG="-DBXHUB"
    fi
    ;;
esac

# bximage specific settings
case "$target" in
  *-pc-windows*)
    BXIMAGE_FLAG="/DBXIMAGE"
    CXXFLAGS_CONSOLE="/nologo /MT /W3 /EHs-c- /DNDEBUG /DWIN32 /D_WINDOWS /D_CRT_SECURE_NO_WARNINGS $C_OPT"
    ;;
  *)
    BXIMAGE_FLAG="-DBXIMAGE"
    CXXFLAGS_CONSOLE="$CXXFLAGS"
    ;;
esac
AC_SUBST(BXIMAGE_FLAG)
AC_SUBST(BXIMAGE_LINK_OPTS)
AC_SUBST(BXHUB_FLAG)
AC_SUBST(BXHUB_LINK_OPTS)
AC_SUBST(CXXFLAGS_CONSOLE)

ENH_DBG_OBJS=""
if test "$gui_debugger" = 1; then
  if test "$bx_have_gtk_version" -ge 2; then
    ENH_DBG_OBJS="enh_dbg.o gtk_enh_dbg_osdep.o"
    AC_DEFINE(BX_DEBUGGER_GUI, 1)
  elif test "$DEFAULT_GUI" = win32 -o "$with_win32" = yes; then
    ENH_DBG_OBJS="enh_dbg.o win32_enh_dbg_osdep.o"
    AC_DEFINE(BX_DEBUGGER_GUI, 1)
  else
    AC_MSG_WARN([The Bochs debugger gui cannot be compiled here, disabling it])
    AC_DEFINE(BX_DEBUGGER_GUI, 0)
  fi
fi
AC_SUBST(ENH_DBG_OBJS)

USB_DBG_OBJS=""
if test "$usb_debugger" = 1; then
  if test "$bx_have_gtk_version" -ge 3; then
    USB_DBG_OBJS="usb_debug.o gtk_usb_debug.o"
    AC_DEFINE(BX_USB_DEBUGGER, 1)
  elif test "$DEFAULT_GUI" = win32 -o "$with_win32" = yes; then
    USB_DBG_OBJS="usb_debug.o win32usb.o"
    AC_DEFINE(BX_USB_DEBUGGER, 1)
  else
    AC_MSG_WARN([The USB debugger cannot be compiled here, disabling it])
    AC_DEFINE(BX_USB_DEBUGGER, 0)
  fi
fi
AC_SUBST(USB_DBG_OBJS)

CI_PLUGIN_OBJS="textconfig.o"
EXPORT_DYNAMIC="-export-dynamic"
case $target in
  *-pc-windows* | *-pc-winnt*)
    if test "$bx_plugins" = 1; then
      # set variables for building DLL plugins
      PLUGIN_LIBNAME_TRANSFORMATION='%.o=bx_%.dll'
      GUI_PLUGIN_NAME_TRANSFORMATION='%.o=bx_%_gui.dll'
      IMG_PLUGIN_NAME_TRANSFORMATION='%.o=bx_%_img.dll'
      INSTALL_PLUGINS_VAR=install_dll_plugins
      WIN32_DLL_IMPORT_LIB="bochs.lib"
      LINK_DLL="link /dll /nologo /subsystem:console /incremental:no /out:\$@"
      LIBS="$LIBS advapi32.lib comctl32.lib comdlg32.lib gdi32.lib shell32.lib user32.lib"
      PLUGIN_TARGET_2=plugins_msvc
      LINK_VAR="_VCPP"
      EXT_MSVC_DLL_RULES="!INCLUDE makeincl.vc"
      echo -e "# DLL PLUGIN TEST\n" > iodev/makeincl.vc
      IODEV_DLL_TARGETS=""
      IODEV_DLL_LIST="biosdev cmos dma extfpuirq harddrv ioapic parallel pic speaker unmapped"
      if test "$pci" = "1"; then
        IODEV_DLL_LIST="$IODEV_DLL_LIST acpi pci pci2isa pci_ide hpet"
      fi
      if test "$bx_debugger" = 1; then
        IODEV_DLL_LIST="$IODEV_DLL_LIST iodebug"
      fi
      if test "$bx_busmouse" = 1; then
        IODEV_DLL_LIST="$IODEV_DLL_LIST busmouse"
      fi
      for i in $IODEV_DLL_LIST
      do
        echo -e "bx_$i.dll: $i.o" >> iodev/makeincl.vc
        echo -e "\tlink /dll /nologo /subsystem:console /incremental:no /out:\$@ $i.o \$(WIN32_DLL_IMPORT_LIBRARY)\n" >> iodev/makeincl.vc
        IODEV_DLL_TARGETS="$IODEV_DLL_TARGETS bx_$i.dll"
      done
    else
      if test "$with_win32" != yes; then
        LIBS="$LIBS comctl32.lib"
      fi
    fi
    CI_PLUGIN_OBJS="$CI_PLUGIN_OBJS \$(CI_OBJS_WIN32)"
    CI_SUPPORT_OBJS="\$(CI_OBJS_WIN32_SUPPORT)"
    EXPORT_DYNAMIC=""
    ;;
  *-cygwin* | *-mingw* | *-msys)
    if test "$bx_plugins" = 1; then
      # set variables for building DLL plugins
      PRIMARY_TARGET=".win32_dll_plugin_target"
      PLUGIN_LIBNAME_TRANSFORMATION='%.o=bx_%.dll'
      GUI_PLUGIN_NAME_TRANSFORMATION='%.o=bx_%_gui.dll'
      IMG_PLUGIN_NAME_TRANSFORMATION='%.o=bx_%_img.dll'
      INSTALL_PLUGINS_VAR=install_dll_plugins
      WIN32_DLL_IMPORT_LIB="dllexports.a"
      LINK_DLL="\$(CXX) \$(CXXFLAGS) -shared -o \$@"
      if test "$cross_compiling" = no; then
        DLLTOOL="dlltool"
      else
        DLLTOOL="$DLLTOOL"
      fi
    else
      if test "$with_win32" != yes; then
        LIBS="$LIBS -lcomctl32"
      fi
    fi
    CI_PLUGIN_OBJS="$CI_PLUGIN_OBJS \$(CI_OBJS_WIN32)"
    CI_SUPPORT_OBJS="\$(CI_OBJS_WIN32_SUPPORT)"
    EXPORT_DYNAMIC=""
    ;;
esac
AC_SUBST(CI_PLUGIN_OBJS)
AC_SUBST(CI_SUPPORT_OBJS)
AC_SUBST(EXPORT_DYNAMIC)
AC_SUBST(DLLTOOL)

if test "$use_curses" = yes; then
  if test "x$PKG_CONFIG" != x; then
    AC_CHECK_LIB(curses, mvaddch, GUI_LINK_OPTS_TERM="`$PKG_CONFIG --libs curses`")
    AC_CHECK_LIB(ncurses, mvaddch, GUI_LINK_OPTS_TERM="`$PKG_CONFIG --libs ncurses`")
    AC_CHECK_LIB(termlib, mvaddch, GUI_LINK_OPTS_TERM="`$PKG_CONFIG --libs termlib`")
    AC_CHECK_LIB(pdcurses, mvaddch, GUI_LINK_OPTS_TERM="`$PKG_CONFIG --libs pdcurses`")
  else
    AC_CHECK_LIB(curses, mvaddch, GUI_LINK_OPTS_TERM='-lcurses')
    AC_CHECK_LIB(ncurses, mvaddch, GUI_LINK_OPTS_TERM='-lncurses')
    AC_CHECK_LIB(termlib, mvaddch, GUI_LINK_OPTS_TERM='-ltermlib')
    AC_CHECK_LIB(pdcurses, mvaddch, GUI_LINK_OPTS_TERM='-lpdcurses')
  fi
  if test "$GUI_LINK_OPTS_TERM" = ""; then
    echo Curses library not found: tried curses, ncurses, termlib and pdcurses.
    exit 1
  fi
fi

if test "$with_term" = yes; then
  old_LIBS="$LIBS"
  LIBS="$LIBS $GUI_LINK_OPTS_TERM"
  AC_CHECK_FUNCS(color_set, AC_DEFINE(BX_HAVE_COLOR_SET, 1))
  AC_CHECK_FUNCS(mvhline, AC_DEFINE(BX_HAVE_MVHLINE, 1))
  AC_CHECK_FUNCS(mvvline, AC_DEFINE(BX_HAVE_MVVLINE, 1))
  LIBS="$old_LIBS"
fi

if test "$with_rfb" = yes; then
  # we need the socket function
  case $target in
    *-pc-windows* | *-pc-winnt*)
      RFB_LIBS="$RFB_LIBS ws2_32.lib"
      have_socket=yes
      ;;
    *-mingw32* | *-msys)
      RFB_LIBS="$RFB_LIBS -lws2_32"
      have_socket=yes
      ;;
    *-cygwin*)
      # is this okay without a check ?
      have_socket=yes
      ;;
    *)
      AC_CHECK_FUNCS(socket, have_socket=yes)
      if test "$have_socket" != yes; then
        AC_CHECK_LIB(socket, socket,
          [
            RFB_LIBS="$RFB_LIBS -lsocket"
            have_socket=yes
          ])
      fi
      ;;
  esac
  if test "$have_socket" != yes; then
    echo 'ERROR: socket function required for RFB compile'
    exit 1
  fi
fi

# The ACX_PTHREAD function was written by
# Steven G. Johnson <stevenj@alum.mit.edu> and
# Alejandro Forero Cuervo <bachue@bachue.com>
# I found it in the ac-archive project on Source Forge.

AC_DEFUN([ACX_PTHREAD], [
AC_REQUIRE([AC_CANONICAL_HOST])
acx_pthread_ok=no

# First, check if the POSIX threads header, pthread.h, is available.
# If it isn't, don't bother looking for the threads libraries.
AC_CHECK_HEADER(pthread.h, , acx_pthread_ok=noheader)

# We must check for the threads library under a number of different
# names; the ordering is very important because some systems
# (e.g. DEC) have both -lpthread and -lpthreads, where one of the
# libraries is broken (non-POSIX).

# First of all, check if the user has set any of the PTHREAD_LIBS,
# etcetera environment variables, and if threads linking works using
# them:
if test x"$PTHREAD_LIBS$PTHREAD_CFLAGS" != x; then
	save_CFLAGS="$CFLAGS"
	CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
	CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
	save_LIBS="$LIBS"
	LIBS="$PTHREAD_LIBS $LIBS"
	AC_MSG_CHECKING([for pthread_join in LIBS=$PTHREAD_LIBS with CFLAGS=$PTHREAD_CFLAGS])
	AC_TRY_LINK_FUNC(pthread_join, acx_pthread_ok=yes)
	AC_MSG_RESULT($acx_pthread_ok)
	if test x"$acx_pthread_ok" = xno; then
		PTHREAD_LIBS=""
		PTHREAD_CFLAGS=""
	fi
	LIBS="$save_LIBS"
	CFLAGS="$save_CFLAGS"
fi

# Create a list of thread flags to try.  Items starting with a "-" are
# C compiler flags, and other items are library names, except for "none"
# which indicates that we try without any flags at all.

acx_pthread_flags="pthreads none -Kthread -kthread lthread -pthread -pthreads -mthreads pthread --thread-safe -mt"

# The ordering *is* (sometimes) important.  Some notes on the
# individual items follow:

# pthreads: AIX (must check this before -lpthread)
# none: in case threads are in libc; should be tried before -Kthread and
#       other compiler flags to prevent continual compiler warnings
# -Kthread: Sequent (threads in libc, but -Kthread needed for pthread.h)
# -kthread: FreeBSD kernel threads (preferred to -pthread since SMP-able)
# lthread: LinuxThreads port on FreeBSD (also preferred to -pthread)
# -pthread: Linux/gcc (kernel threads), BSD/gcc (userland threads)
# -pthreads: Solaris/gcc
# -mthreads: Mingw32/gcc, Lynx/gcc
# -mt: Sun Workshop C (may only link SunOS threads [-lthread], but it
#      doesn't hurt to check since this sometimes defines pthreads too;
#      also defines -D_REENTRANT)
# pthread: Linux, etcetera
# --thread-safe: KAI C++

case "${host_cpu}-${host_os}" in
	*solaris*)

	# On Solaris (at least, for some versions), libc contains stubbed
	# (non-functional) versions of the pthreads routines, so link-based
	# tests will erroneously succeed.  (We need to link with -pthread or
	# -lpthread.)  (The stubs are missing pthread_cleanup_push, or rather
	# a function called by this macro, so we could check for that, but
	# who knows whether they'll stub that too in a future libc.)  So,
	# we'll just look for -pthreads and -lpthread first:

	acx_pthread_flags="-pthread -pthreads pthread -mt $acx_pthread_flags"
	;;
esac

if test x"$acx_pthread_ok" = xno; then
for flag in $acx_pthread_flags; do

	case $flag in
		none)
		AC_MSG_CHECKING([whether pthreads work without any flags])
		;;

		-*)
		AC_MSG_CHECKING([whether pthreads work with $flag])
		PTHREAD_CFLAGS="$flag"
		;;

		*)
		AC_MSG_CHECKING([for the pthreads library -l$flag])
		PTHREAD_LIBS="-l$flag"
		;;
	esac

	save_LIBS="$LIBS"
	save_CFLAGS="$CFLAGS"
	LIBS="$PTHREAD_LIBS $LIBS"
	CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

	# Check for various functions.  We must include pthread.h,
	# since some functions may be macros.  (On the Sequent, we
	# need a special flag -Kthread to make this header compile.)
	# We check for pthread_join because it is in -lpthread on IRIX
	# while pthread_create is in libc.  We check for pthread_attr_init
	# due to DEC craziness with -lpthreads.  We check for
	# pthread_cleanup_push because it is one of the few pthread
	# functions on Solaris that doesn't have a non-functional libc stub.
	# We try pthread_create on general principles.
	AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <pthread.h>]], [[pthread_t th; pthread_join(th, 0);
		     pthread_attr_init(0); pthread_cleanup_push(0, 0);
		     pthread_create(0,0,0,0); pthread_cleanup_pop(0); ]])],[acx_pthread_ok=yes],[])

	LIBS="$save_LIBS"
	CFLAGS="$save_CFLAGS"

	AC_MSG_RESULT($acx_pthread_ok)
	if test "x$acx_pthread_ok" = xyes; then
		break;
	fi

	PTHREAD_LIBS=""
	PTHREAD_CFLAGS=""
done
fi

# Various other checks:
if test "x$acx_pthread_ok" = xyes; then
	save_LIBS="$LIBS"
	LIBS="$PTHREAD_LIBS $LIBS"
	save_CFLAGS="$CFLAGS"
	CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

	# Detect AIX lossage: threads are created detached by default
	# and the JOINABLE attribute has a nonstandard name (UNDETACHED).
	AC_MSG_CHECKING([for joinable pthread attribute])
	AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <pthread.h>]], [[int attr=PTHREAD_CREATE_JOINABLE;]])],[ok=PTHREAD_CREATE_JOINABLE],[ok=unknown])
	if test x"$ok" = xunknown; then
		AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <pthread.h>]], [[int attr=PTHREAD_CREATE_UNDETACHED;]])],[ok=PTHREAD_CREATE_UNDETACHED],[ok=unknown])
	fi
	if test x"$ok" != xPTHREAD_CREATE_JOINABLE; then
		AC_DEFINE(PTHREAD_CREATE_JOINABLE, $ok,
			  [Define to the necessary symbol if this constant
			   uses a non-standard name on your system.])
	fi
	AC_MSG_RESULT(${ok})
	if test x"$ok" = xunknown; then
		AC_MSG_WARN([we do not know how to create joinable pthreads])
	fi

	AC_MSG_CHECKING([if more special flags are required for pthreads])
	flag=no
	case "${host_cpu}-${host_os}" in
		*-aix* | *-freebsd*)     flag="-D_THREAD_SAFE";;
		*solaris* | alpha*-osf*) flag="-D_REENTRANT";;
	esac
	AC_MSG_RESULT(${flag})
	if test "x$flag" != xno; then
		PTHREAD_CFLAGS="$flag $PTHREAD_CFLAGS"
	fi

	LIBS="$save_LIBS"
	CFLAGS="$save_CFLAGS"

	# More AIX lossage: must compile with cc_r
	AC_CHECK_PROG(PTHREAD_CC, cc_r, cc_r, ${CC})
else
	PTHREAD_CC="$CC"
fi

AC_SUBST(PTHREAD_LIBS)
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_CC)

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$acx_pthread_ok" = xyes; then
	ifelse([$1],,AC_DEFINE(HAVE_PTHREAD,1,[Define if you have POSIX threads libraries and header files.]),[$1])
	:
else
	acx_pthread_ok=no
	$2
fi

])dnl ACX_PTHREAD

pthread_ok=no
ACX_PTHREAD([
  pthread_ok=yes
  #echo Using PTHREAD_LIBS=$PTHREAD_LIBS
  #echo Using PTHREAD_CFLAGS=$PTHREAD_CFLAGS
  #echo Using PTHREAD_CC=$PTHREAD_CC
  ])


# since some features need the pthread library, check that it was found.
# But on win32 platforms, the pthread library is not needed.
case "$target" in
  *-pc-windows* | *-pc-winnt* | *-cygwin* | *-mingw32* | *-msys)
    # pthread not needed for win32 platform
    ;;
  *)
    # assuming that some GUIs and the sound subsystem require pthreads
    if test "$pthread_ok" = yes; then
      if test "$with_rfb" = yes; then
        RFB_LIBS="$RFB_LIBS $PTHREAD_LIBS"
      fi
      if test "$with_vncsrv" = yes; then
        GUI_LINK_OPTS_VNCSRV="$GUI_LINK_OPTS_VNCSRV $PTHREAD_LIBS"
      fi
      if test "$soundcard_present" = 1; then
        if test "$bx_plugins" = 1; then
          ALSA_SOUND_LINK_OPTS="$ALSA_SOUND_LINK_OPTS $PTHREAD_LIBS"
          PULSE_SOUND_LINK_OPTS="$PULSE_SOUND_LINK_OPTS $PTHREAD_LIBS"
          SDL_SOUND_LINK_OPTS="$SDL_SOUND_LINK_OPTS $PTHREAD_LIBS"
        else
          DEVICE_LINK_OPTS="$DEVICE_LINK_OPTS $PTHREAD_LIBS"
        fi
      fi
      CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
      CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
      CC="$PTHREAD_CC"
    else
      echo ERROR: the pthread library is required, but could not be found.; exit 1
    fi
	;;
esac

dnl // DEPRECATED configure options - force users to remove them

dnl AC_MSG_CHECKING(for misaligned SSE support (deprecated))
dnl AC_ARG_ENABLE(misaligned-sse,
dnl   AS_HELP_STRING([--enable-misaligned-sse], [misaligned SSE support (deprecated)]),
dnl   [AC_MSG_RESULT($enableval)
dnl    AC_MSG_ERROR([DEPRECATED - moved to runtime .bochsrc option])
dnl   ],
dnl   [
dnl    AC_MSG_RESULT(no)
dnl   ])

# Create some subdirectories for when you run configure from some other
# directory (part #1).
if test ! -d instrument; then mkdir instrument; fi
if test ! -d build; then mkdir build; fi
if test ! -d build/linux; then mkdir build/linux; fi
if test ! -d build/win32; then mkdir build/win32; fi
if test ! -d build/win32/nsis; then mkdir build/win32/nsis; fi

AC_SUBST(INSTALL_TARGET)
AC_SUBST(INSTALL_LIST_FOR_PLATFORM)
AC_SUBST(RFB_LIBS)
AC_SUBST(GUI_LINK_OPTS_VNCSRV)
AC_SUBST(GUI_LINK_OPTS_SDL)
AC_SUBST(GUI_LINK_OPTS_SDL2)
AC_SUBST(DEVICE_LINK_OPTS)
AC_SUBST(GUI_LINK_OPTS)
AC_SUBST(GUI_LINK_OPTS_TERM)
AC_SUBST(GUI_LINK_OPTS_WX)
AC_SUBST(NONPLUGIN_GUI_LINK_OPTS)
AC_SUBST(DASH)
AC_SUBST(SLASH)
AC_SUBST(CXXFP)
AC_SUBST(CFP)
AC_SUBST(OFP)
AC_SUBST(MAKELIB)
AC_SUBST(RMCOMMAND)
AC_SUBST(LINK)
AC_SUBST(LINK_CONSOLE)
AC_SUBST(LINK_DLL)
AC_SUBST(LINK_VAR)
AC_SUBST(EXE)
AC_SUBST(PRIMARY_TARGET)
AC_SUBST(OPTIONAL_TARGET)
AC_SUBST(PLUGIN_LIBNAME_TRANSFORMATION)
AC_SUBST(GUI_PLUGIN_NAME_TRANSFORMATION)
AC_SUBST(IMG_PLUGIN_NAME_TRANSFORMATION)
AC_SUBST(COMMAND_SEPARATOR)
AC_SUBST(CD_UP_ONE)
AC_SUBST(CD_UP_TWO)
AC_SUBST(CD_UP_THREE)
AC_SUBST(VERSION)
AC_SUBST(VER_MAJOR)
AC_SUBST(VER_MINOR)
AC_SUBST(VER_REVISION)
AC_SUBST(VER_DEV)
AC_SUBST(REL_STRING)
AC_SUBST(REL_TIMESTAMP)
AC_SUBST(EXTRA_LINK_OPTS)
AC_SUBST(GUI_NON_PLUGIN_OBJS)
AC_SUBST(GUI_PLUGIN_OBJS)
AC_SUBST(IODEV_NON_PLUGIN_OBJS)
AC_SUBST(IODEV_PLUGIN_OBJS)
AC_SUBST(IODEV_EXT_NON_PLUGIN_OBJS)
AC_SUBST(IODEV_EXT_PLUGIN_OBJS)
AC_SUBST(PLUGIN_LIB)
AC_SUBST(PLUGIN_TARGET)
AC_SUBST(PLUGIN_TARGET_2)
AC_SUBST(EXT_MSVC_DLL_RULES)
AC_SUBST(GUI_DLL_TARGETS)
AC_SUBST(BOCHS_BITS)
AC_SUBST(IODEV_DLL_TARGETS)
AC_SUBST(WIN32_DLL_IMPORT_LIB)
AC_SUBST(INSTALL_PLUGINS_VAR)

AC_PATH_PROG(GZIP, gzip)
AC_PATH_PROG(TAR, tar)
AC_PATH_PROG(IASL, iasl, not_found)

AC_CONFIG_FILES([Makefile iodev/Makefile iodev/display/Makefile iodev/hdimage/Makefile \
	 iodev/usb/Makefile iodev/network/Makefile iodev/sound/Makefile \
	 bx_debug/Makefile bios/Makefile cpu/Makefile cpu/avx/Makefile \
	 cpu/cpudb/Makefile cpu/fpu/Makefile cpu/softfloat3e/Makefile memory/Makefile \
	 gui/Makefile ${INSTRUMENT_DIR}/Makefile misc/Makefile \
	 doc/docbook/Makefile build/linux/bochs-dlx bxversion.h bxversion.rc \
	 build/macosx/Info.plist build/win32/nsis/Makefile \
	 build/win32/nsis/bochs.nsi host/linux/pcidev/Makefile ${USER_PLUGINS_MAKEFILE}])
AC_OUTPUT

# Create some subdirectories for when you run configure from some other
# directory (part #2).
if test ! -d cpu/cpudb/amd; then mkdir cpu/cpudb/amd; fi
if test ! -d cpu/cpudb/intel; then mkdir cpu/cpudb/intel; fi
if test ! -d cpu/decoder; then mkdir cpu/decoder; fi
if test ! -d iodev/network/slirp; then mkdir iodev/network/slirp; fi
