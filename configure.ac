dnl Copyright (c) 2007, 2008 Red Hat, Inc.
dnl
dnl This file is part of the Qpid async store library msgstore.so.
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License, or (at your option) any later version.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library; if not, write to the Free Software
dnl Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
dnl USA
dnl
dnl The GNU Lesser General Public License is available in the file COPYING.
dnl 
dnl Process this file with autoconf to produce a configure script.

AC_INIT([msg-store], [0.7], [rhemrg-users-list@redhat.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([dist-bzip2])

# Minimum Autoconf version required.
AC_PREREQ(2.59)

AC_CONFIG_HEADERS([config.h:config.in])
AC_CONFIG_SRCDIR([lib/MessageStoreImpl.cpp])

AC_PROG_CC_STDC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_USE_SYSTEM_EXTENSIONS
AC_LANG([C++])	

AC_ARG_ENABLE(warnings,
[  --enable-warnings   turn on lots of compiler warnings (recommended)],
[case "${enableval}" in
   yes|no) enable_WARNINGS=${enableval};;
   *)      AC_MSG_ERROR([bad value ${enableval} for warnings option]) ;;
 esac],
 [enable_WARNINGS=yes])

AC_ARG_ENABLE(cluster-tests,
[  --enable-cluster-tests   enable cluster tests with persistence],
[case "${enableval}" in
   yes|no) enable_CLUSTER_TESTS=${enableval} ;;
   *)      AC_MSG_ERROR([bad value ${enableval} for cluster test option]) ;;
 esac],
 [enable_CLUSTER_TESTS=none])

# Warnings: Enable as many as possible, keep the code clean. Please
# do not disable warnings or remove -Werror without discussing on
# rhemrg-users-list list.
#
# The following warnings are deliberately omitted, they warn on valid code.
# -Wunreachable-code -Wpadded -Winline
# -Wshadow - warns about boost headers.

if test "$enable_WARNINGS" = yes; then
  gl_COMPILER_FLAGS(-Werror)
  gl_COMPILER_FLAGS(-pedantic)
  gl_COMPILER_FLAGS(-Wall)
  gl_COMPILER_FLAGS(-Wextra)
  gl_COMPILER_FLAGS(-Wno-shadow)
  gl_COMPILER_FLAGS(-Wpointer-arith)
  gl_COMPILER_FLAGS(-Wcast-qual)
  gl_COMPILER_FLAGS(-Wcast-align)
  gl_COMPILER_FLAGS(-Wno-long-long)
  gl_COMPILER_FLAGS(-Wvolatile-register-var)
  gl_COMPILER_FLAGS(-Winvalid-pch)
  gl_COMPILER_FLAGS(-Wno-system-headers)
  AC_SUBST([WARNING_CFLAGS], [$COMPILER_FLAGS])
  AC_DEFINE([lint], 1, [Define to 1 if the compiler is checking for lint.])
  COMPILER_FLAGS=
fi

AC_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])

# Select building against qpid checkout or install.
AC_ARG_WITH([qpid-checkout],
  [AS_HELP_STRING([--with-qpid-checkout],
    [Location of qpid checkout to build against (by default use installed qpid)])])

AC_ARG_WITH([qpid-build],
  [AS_HELP_STRING([--with-qpid-build],
    [Qpid build directory if different from --with-qpid-checkout])])

AC_ARG_WITH([qpid-prefix],
  [AS_HELP_STRING([--with-qpid-prefix],
    [Location of installed qpid prefix to build against (by default "/")])],
  [],
  [with_qpid_prefix="/"])

if test x$with_qpid_checkout != x; then
   QPID_DIR=$with_qpid_checkout
   QPID_BLD=$QPID_DIR/cpp
   QPID_SRC=$QPID_DIR/cpp/src
   test -f $QPID_SRC/qpid/broker/MessageStore.h || \
     AC_MSG_ERROR([$QPID_DIR does not appear to be a valid qpid checkout.])
   test -d $QPID_DIR/python -a -d $QPID_DIR/specs || \
     AC_MSG_ERROR([$QPID_DIR does not have python and specs directories.])
   QPID_LIBS="$QPID_SRC/libqpidbroker.la $QPID_SRC/libqpidcommon.la"
   QPID_CXXFLAGS="-I$QPID_DIR/cpp/include -I$QPID_SRC"
else
  QPID_PREFIX=$with_qpid_prefix
  QPID_INCLUDE=$QPID_PREFIX/include
  QPID_LIB=$QPID_PREFIX/lib
  fail=0
  test -f $QPID_INCLUDE/qpid/broker/MessageStore.h || \
    fail=1
  # Give any/all diagnostics before failing.
  test $fail = 1 &&
    AC_MSG_ERROR([Missing required qpid libraries/headers. Install source rpms or use one of --with-qpid-checkout or --with-qpid-prefix])
  QPID_CXXFLAGS="-I$QPID_INCLUDE"
  QPID_LIBS="-L$QPID_LIB -lqpidbroker -lqpidcommon"
fi

if test x$with_qpid_build != x; then
   test x$with_qpid_checkout != x || AC_MSG_ERROR([--with-qpid-build requires --with-qpid-checkout])
   QPID_BLD=$with_qpid_build
   QPID_LIBS="$QPID_BLD/src/libqpidbroker.la $QPID_BLD/src/libqpidcommon.la"
   QPID_CXXFLAGS="-I$QPID_DIR/cpp/include -I$QPID_SRC -I$QPID_BLD/include -I$QPID_BLD/src"
fi

AC_SUBST([QPID_PREFIX])
AC_SUBST([QPID_DIR])
AC_SUBST([QPID_BLD])
AC_SUBST([QPID_LIBS])
AC_SUBST([QPID_CXXFLAGS])

# Check for cluster module to see whether to enable cluster tests
AC_MSG_CHECKING([presence of qpid cluster module])
found_cluster_so=no
if test x$with_qpid_checkout != x -a -f $QPID_SRC/.libs/cluster.so ; then
  found_cluster_so=yes
elif test -f $QPID_LIB/qpid/daemon/cluster.so ; then
  found_cluster_so=yes  
fi

case "$enable_CLUSTER_TESTS" in
  yes)
    if test $found_cluster_so = no; then
      AC_MSG_ERROR([didn't find cluster module: can't enable cluster tests])
    fi ;;
  no) ;;
  none)
    if test $found_cluster_so = yes; then
      enable_CLUSTER_TESTS=yes
    else
      enable_CLUSTER_TESTS=no    
    fi ;;
  *)
    AC_MSG_ERROR([bad value $enable_CLUSTER_TESTS for cluster tests]) ;;
esac
AC_MSG_RESULT([$enable_CLUSTER_TESTS])

# Check for libaio and libaio-devel
libaio_fail=0
AC_CHECK_LIB([aio], [io_setup], ,[libaio_fail=1])
AC_CHECK_HEADER([libaio.h], ,[libaio_fail=1])
test $libaio_fail == 1 && \
  AC_MSG_ERROR([libaio-devel package missing. Please ensure both libaio and libaio-devel are installed. (hint: "yum install libaio-devel" should do it...)])
	
# For libraries (libcommon) that use dlopen, dlerror, etc.,
# test whether we need to link with -ldl.
gl_saved_libs=$LIBS
AC_SEARCH_LIBS(dlopen, [dl],
		 [test "$ac_cv_search_dlopen" = "none required" ||
		  LIB_DLOPEN=$ac_cv_search_dlopen])
AC_SUBST([LIB_DLOPEN])
LIBS=$gl_saved_libs

# Require libdb_cxx (any version between 4.2 and 4.8), for the library, and for db_cxx.h.
db4_devel_fail=0
AC_CHECK_HEADER([db_cxx.h], ,[db4_devel_fail=1])
test $db4_devel_fail == 1 && \
  AC_MSG_ERROR([db4-devel package missing. Please ensure both db4 and db4-devel are installed. (hint: "yum install db4-devel" should do it...)])

gl_saved_libs=$LIBS
AC_SEARCH_LIBS([__db_open], [db_cxx-4.8 db_cxx-4.7 db_cxx-4.6 db_cxx-4.5 db_cxx-4.4 db_cxx-4.3 db_cxx-4.2],
		 [test "$ac_cv_search___db_open" = "none required" ||
		  LIB_BERKELEY_DB=$ac_cv_search___db_open],
		  AC_MSG_ERROR([Couldn't find required library in range db_cxx-4.2 through db_cxx-4.8]))
AC_SUBST([LIB_BERKELEY_DB])
LIBS=$gl_saved_libs

# Determine how to include db_cxx.h:
# Red Hat needs <db4/db_cxx.h>, Debian needs <db_cxx.h>.
AC_CHECK_HEADER([db4/db_cxx.h],
                [DB_CXX_HEADER_PREFIX=db4/])
if test x$DB_CXX_HEADER_PREFIX = x; then
  AC_CHECK_HEADER([db_cxx.h],
		  [DB_CXX_HEADER_PREFIX=])
fi
AC_SUBST(DB_CXX_HEADER_PREFIX)

gl_CLOCK_TIME

# We use valgrind for the tests.  See if it's available.
AC_CHECK_PROG([VALGRIND], [valgrind], [valgrind])

# If rpmlint is available we'll run it when building RPMs.
AC_CHECK_PROG([RPMLINT], [rpmlint], [rpmlint])
AM_CONDITIONAL([HAS_RPMLINT], [test -n "$RPMLINT"])

# Also doxygen for documentation...
AC_CHECK_PROG([do_doxygen], [doxygen], [yes])
AM_CONDITIONAL([DOXYGEN], [test x$do_doxygen = xyes])
AM_CONDITIONAL([DO_CLUSTER_TESTS], [test $enable_CLUSTER_TESTS = yes])

# Check for Python libraries for the Journal Tools
AM_PATH_PYTHON()

AC_CONFIG_FILES([
  Makefile
  docs/Makefile
  lib/Makefile
  lib/jrnl2/Makefile
  perf/Makefile
  tests/Makefile
  tests/cluster/Makefile
  tests/jrnl/Makefile
  tests/jrnl/jtt/Makefile
  tools/Makefile
  ])

AC_OUTPUT
