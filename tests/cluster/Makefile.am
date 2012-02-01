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

INCLUDES=-I$(top_srcdir)/lib $(QPID_CXXFLAGS)

TMP_DATA_DIR=$(abs_srcdir)/../tmp_data_dir

all-local: .valgrindrc .valgrind.supp
.valgrindrc: $(top_srcdir)/tests/.valgrindrc
	cp $^ .
.valgrind.supp: $(top_srcdir)/tests/.valgrind.supp
	cp $^ .

TESTS = \
  run_cpp_cluster_tests \
  run_python_cluster_tests \
  system_test.sh

LONG_TESTS = \
  run_long_python_cluster_tests

TESTS_ENVIRONMENT = \
  BOOST_TEST_SHOW_PROGRESS=yes \
  STORE_ENABLE=1 \
  QPID_DIR=$(QPID_DIR) \
  QPID_BLD=$(QPID_BLD) \
  VALGRIND=$(VALGRIND) \
  STORE_LIB=$(abs_builddir)/../../lib/.libs/msgstore.so \
  TMP_DATA_DIR=$(TMP_DATA_DIR) \
  abs_srcdir=$(abs_srcdir) \
  abs_builddir=$(abs_builddir)

EXTRA_DIST = \
  cluster_tests_env.sh \
  run_cpp_cluster_tests \
  run_python_cluster_tests \
  run_long_python_cluster_tests

check-long:
	$(MAKE) check TESTS="$(LONG_TESTS)" SUBDIRS=.

