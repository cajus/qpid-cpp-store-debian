# Copyright (c) 2007, 2008, 2009 Red Hat, Inc.
#
# This file is part of the Qpid async store library msgstore.so.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
# USA
#
# The GNU Lesser General Public License is available in the file COPYING.

abs_builddir=@abs_builddir@
abs_srcdir=@abs_srcdir@

AM_CXXFLAGS = $(WARNING_CFLAGS) -pthread -DBOOST_TEST_DYN_LINK

INCLUDES=-I$(top_srcdir)/lib

TMP_DATA_DIR=$(abs_srcdir)/../tmp_data_dir

SUBDIRS = jtt .

TESTS_ENVIRONMENT = \
  VALGRIND=$(VALGRIND) \
  TMP_DATA_DIR=$(TMP_DATA_DIR) \
  $(srcdir)/../run_test

all-local: .valgrindrc .valgrind.supp
.valgrindrc: $(top_srcdir)/tests/.valgrindrc
	cp $^ .
.valgrind.supp: $(top_srcdir)/tests/.valgrind.supp
	cp $^ .

TESTS = \
  _ut_time_ns \
  _ut_jexception \
  _ut_jerrno \
  _ut_rec_hdr \
  _ut_jdir \
  _ut_jinf \
  _ut_enq_map \
  _ut_txn_map \
  _ut_lpmgr \
  _st_basic \
  _st_basic_txn \
  _st_read \
  _st_read_txn \
  _st_auto_expand

LONG_TESTS = \
  _ut_long_lpmgr \
  _st_long_basic \
  _st_long_read \
  run-journal-tests

check_PROGRAMS = \
  _ut_time_ns \
  _ut_jexception \
  _ut_jerrno \
  _ut_rec_hdr \
  _ut_jinf \
  _ut_jdir \
  _ut_enq_map \
  _ut_txn_map \
  _ut_lpmgr \
  _ut_long_lpmgr \
  _st_basic \
  _st_basic_txn \
  _st_long_basic \
  _st_long_read \
  _st_read \
  _st_read_txn \
  _st_auto_expand

UNIT_TEST_SRCS = ../unit_test.cpp
UNIT_TEST_LDADD = -lboost_unit_test_framework $(top_builddir)/lib/msgstore.la

_ut_time_ns_SOURCES = _ut_time_ns.cpp $(UNIT_TEST_SRCS)
_ut_time_ns_LDADD = $(UNIT_TEST_LDADD)

_ut_jexception_SOURCES = _ut_jexception.cpp $(UNIT_TEST_SRCS)
_ut_jexception_LDADD = $(UNIT_TEST_LDADD) -lrt

_ut_jerrno_SOURCES = _ut_jerrno.cpp $(UNIT_TEST_SRCS)
_ut_jerrno_LDADD = $(UNIT_TEST_LDADD) -lrt

_ut_rec_hdr_SOURCES = _ut_rec_hdr.cpp $(UNIT_TEST_SRCS)
_ut_rec_hdr_LDADD = $(UNIT_TEST_LDADD) -lrt

_ut_jinf_SOURCES = _ut_jinf.cpp $(UNIT_TEST_SRCS)
_ut_jinf_LDADD = $(UNIT_TEST_LDADD) -lrt

_ut_jdir_SOURCES = _ut_jdir.cpp $(UNIT_TEST_SRCS)
_ut_jdir_LDADD = $(UNIT_TEST_LDADD) -lrt

_ut_enq_map_SOURCES = _ut_enq_map.cpp $(UNIT_TEST_SRCS)
_ut_enq_map_LDADD = $(UNIT_TEST_LDADD) -lrt

_ut_txn_map_SOURCES = _ut_txn_map.cpp $(UNIT_TEST_SRCS)
_ut_txn_map_LDADD = $(UNIT_TEST_LDADD) -lrt

_ut_lpmgr_SOURCES = _ut_lpmgr.cpp $(UNIT_TEST_SRCS)
_ut_lpmgr_LDADD = $(UNIT_TEST_LDADD) -lrt

_ut_long_lpmgr_SOURCES = _ut_lpmgr.cpp $(UNIT_TEST_SRCS)
_ut_long_lpmgr_CPPFLAGS = $(AM_CXXFLAGS) -DLONG_TEST
_ut_long_lpmgr_LDADD = $(UNIT_TEST_LDADD) -lrt

_st_basic_SOURCES = _st_basic.cpp _st_helper_fns.hpp $(UNIT_TEST_SRCS)
_st_basic_LDADD = $(UNIT_TEST_LDADD) -lrt

_st_long_basic_SOURCES = _st_basic.cpp _st_helper_fns.hpp $(UNIT_TEST_SRCS)
_st_long_basic_CPPFLAGS = $(AM_CXXFLAGS) -DLONG_TEST
_st_long_basic_LDADD = $(UNIT_TEST_LDADD) -lrt

_st_basic_txn_SOURCES = _st_basic_txn.cpp _st_helper_fns.hpp $(UNIT_TEST_SRCS)
_st_basic_txn_LDADD = $(UNIT_TEST_LDADD) -lrt

_st_read_SOURCES = _st_read.cpp _st_helper_fns.hpp $(UNIT_TEST_SRCS)
_st_read_LDADD = $(UNIT_TEST_LDADD) -lrt

_st_long_read_SOURCES = _st_read.cpp _st_helper_fns.hpp $(UNIT_TEST_SRCS)
_st_long_read_CPPFLAGS = $(AM_CXXFLAGS) -DLONG_TEST
_st_long_read_LDADD = $(UNIT_TEST_LDADD) -lrt

_st_read_txn_SOURCES = _st_read_txn.cpp _st_helper_fns.hpp $(UNIT_TEST_SRCS)
_st_read_txn_LDADD = $(UNIT_TEST_LDADD) -lrt

_st_auto_expand_SOURCES = _st_auto_expand.cpp _st_helper_fns.hpp $(UNIT_TEST_SRCS)
_st_auto_expand_LDADD = $(UNIT_TEST_LDADD) -lrt

EXTRA_DIST = \
  run-journal-tests \
  tests.ods

# Note: Auto-recursion is not supported for custom targets, so add a $(MAKE) -C for each dir in the SUBDIRS list above.
check-long:
	$(MAKE) -C jtt check-long
	$(MAKE) check TESTS="$(LONG_TESTS)" SUBDIRS=.
