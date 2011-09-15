# Copyright (c) 2007, 2008 Red Hat, Inc.
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

AM_CXXFLAGS = $(WARNING_CFLAGS) $(APR_CXXFLAGS) $(QPID_CXXFLAGS) -DBOOST_TEST_DYN_LINK

INCLUDES=-I$(top_srcdir)/lib -I$(abs_builddir)/../lib -I$(top_srcdir)/lib/gen

TMP_DATA_DIR=$(abs_srcdir)/tmp_data_dir
TMP_PYTHON_TEST_DIR=$(abs_srcdir)/python_tests.tmp
 
if DO_CLUSTER_TESTS
SUBDIRS = jrnl . cluster
else
SUBDIRS = jrnl .
endif

TESTS =						    \
  SimpleTest                    \
  OrderingTest                  \
  TransactionalTest             \
  TwoPhaseCommitTest            \
  run_python_tests              \
  system_test.sh                \
  clean.sh

LONG_TESTS =					\
  SimpleTest                    \
  OrderingTest                  \
  TransactionalTest             \
  TwoPhaseCommitTest            \
  run_long_python_tests			\
  system_test.sh                \
  clean.sh

SHORT_TESTS =					\
  SimpleTest                    \
  TransactionalTest             \
  run_short_python_tests        \
  clean.sh

check_PROGRAMS =                \
  SimpleTest                    \
  OrderingTest                  \
  TransactionalTest             \
  TwoPhaseCommitTest

UNIT_TEST_SRCS = unit_test.cpp unit_test.h
UNIT_TEST_LDADD = -lboost_unit_test_framework $(top_builddir)/lib/msgstore.la

SimpleTest_SOURCES = SimpleTest.cpp $(UNIT_TEST_SRCS)
SimpleTest_LDADD  = $(UNIT_TEST_LDADD) -lrt

OrderingTest_SOURCES = OrderingTest.cpp $(UNIT_TEST_SRCS)
OrderingTest_LDADD  = $(UNIT_TEST_LDADD) -lrt

TransactionalTest_SOURCES = TransactionalTest.cpp $(UNIT_TEST_SRCS)
TransactionalTest_LDADD  = $(UNIT_TEST_LDADD) -lrt

TwoPhaseCommitTest_SOURCES = TwoPhaseCommitTest.cpp $(UNIT_TEST_SRCS)
TwoPhaseCommitTest_LDADD  = $(UNIT_TEST_LDADD) -lrt

EXTRA_DIST = \
    clean.sh \
    failing_python_tests.txt \
    python_tests \
    persistence.py \
    run_long_python_tests \
    run_python_tests \
    run_short_python_tests \
    run_test \
    start_broker \
    stop_broker \
    system_test.sh \
    tests_env.sh \
    MessageUtils.h \
    vg_check \
    .valgrindrc \
    .valgrind.supp

TESTS_ENVIRONMENT = \
  QPID_DIR=$(QPID_DIR) \
  QPID_BLD=$(QPID_BLD) \
  VALGRIND=$(VALGRIND) \
  abs_srcdir=$(abs_srcdir) \
  STORE_LIB=$(abs_builddir)/../lib/.libs/msgstore.so \
  TMP_DATA_DIR=$(TMP_DATA_DIR) \
  TMP_PYTHON_TEST_DIR=$(TMP_PYTHON_TEST_DIR) \
  $(srcdir)/run_test

# Note: Auto-recursion is not supported for custom targets, so add a ${MAKE} -C for each dir in the SUBDIRS list above.
check-long: all
	$(MAKE) -C jrnl check-long
	$(MAKE) check TESTS="$(LONG_TESTS)" SUBDIRS=.
if DO_CLUSTER_TESTS
	$(MAKE) -C cluster check-long
endif
 
check-short: all
	$(MAKE) check TESTS="$(SHORT_TESTS)" SUBDIRS=.
	
