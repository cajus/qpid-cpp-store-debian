# Copyright (c) 2008 Red Hat, Inc.
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

AM_CXXFLAGS = $(WARNING_CFLAGS) -I$(top_srcdir)/lib -pthread -DBOOST_TEST_DYN_LINK

TMP_DATA_DIR=$(abs_srcdir)/../../tmp_data_dir

LINK_BDB = $(top_builddir)/lib/msgstore.la

TESTS_ENVIRONMENT = \
  VALGRIND=$(VALGRIND) \
  abs_srcdir=$(abs_srcdir) \
  TMP_DATA_DIR=$(TMP_DATA_DIR) \
  $(srcdir)/../../run_test

all-local: .valgrindrc .valgrind.supp
.valgrindrc: $(top_srcdir)/tests/.valgrindrc
	cp $^ .
.valgrind.supp: $(top_srcdir)/tests/.valgrind.supp
	cp $^ .

TESTS =

LONG_TESTS = \
    _ut_data_src \
    _ut_long_data_src \
    _ut_jrnl_init_params \
    _ut_read_arg \
    _ut_test_case \
    _ut_test_case_result \
    _ut_test_case_result_agregation \
    _ut_test_case_set \
    _ut_jrnl_instance

check_PROGRAMS = jtt \
    _ut_data_src \
    _ut_long_data_src \
    _ut_jrnl_init_params \
    _ut_read_arg \
    _ut_test_case \
    _ut_test_case_result \
    _ut_test_case_result_agregation \
    _ut_test_case_set \
    _ut_jrnl_instance

jtt_SOURCES = \
    args.cpp \
    data_src.cpp \
    jrnl_init_params.cpp \
    jrnl_instance.cpp \
    main.cpp \
    read_arg.cpp \
    test_case.cpp \
    test_case_result.cpp \
    test_case_result_agregation.cpp \
    test_case_set.cpp \
    test_mgr.cpp \
    args.hpp \
    data_src.hpp \
    jrnl_init_params.hpp \
    jrnl_instance.hpp \
    read_arg.hpp \
    test_case.hpp \
    test_case_result.hpp \
    test_case_result_agregation.hpp \
    test_case_set.hpp \
    test_mgr.hpp

jtt_LDADD = -laio -lrt -lboost_program_options $(LINK_BDB)

_ut_data_src_SOURCES = \
     _ut_data_src.cpp \
     data_src.cpp \
     ../../unit_test.cpp
_ut_data_src_LDADD = -lboost_unit_test_framework -lrt $(LINK_BDB)

_ut_long_data_src_SOURCES = \
     _ut_data_src.cpp \
     data_src.cpp \
     ../../unit_test.cpp
_ut_long_data_src_CXXFLAGS = $(AM_CXXFLAGS) -DLONG_TEST
_ut_long_data_src_LDADD = -lboost_unit_test_framework -lrt $(LINK_BDB)

_ut_jrnl_init_params_SOURCES = \
    _ut_jrnl_init_params.cpp \
    jrnl_init_params.cpp \
    ../../unit_test.cpp
_ut_jrnl_init_params_LDADD =  -lboost_unit_test_framework -lrt $(LINK_BDB)

_ut_read_arg_SOURCES = \
    _ut_read_arg.cpp \
    args.cpp \
    data_src.cpp \
    jrnl_init_params.cpp \
    jrnl_instance.cpp \
    read_arg.cpp \
    test_case.cpp \
    test_case_result.cpp \
    test_case_result_agregation.cpp \
    ../../unit_test.cpp
_ut_read_arg_LDADD = -lboost_unit_test_framework -lrt -lboost_program_options $(LINK_BDB)

_ut_jrnl_instance_SOURCES = \
    _ut_jrnl_instance.cpp \
    args.cpp \
    data_src.cpp \
    jrnl_init_params.cpp \
    jrnl_instance.cpp \
    read_arg.cpp \
    test_case.cpp \
    test_case_result.cpp \
    test_case_result_agregation.cpp \
    ../../unit_test.cpp
_ut_jrnl_instance_LDADD =  -lboost_unit_test_framework -laio -lrt -lboost_program_options $(LINK_BDB)

_ut_test_case_SOURCES = \
    _ut_test_case.cpp \
    test_case.cpp \
    test_case_result.cpp \
    test_case_result_agregation.cpp \
    ../../unit_test.cpp
_ut_test_case_LDADD = -lboost_unit_test_framework -lrt $(LINK_BDB)

_ut_test_case_result_SOURCES = \
    _ut_test_case_result.cpp \
    test_case_result.cpp \
    ../../unit_test.cpp
_ut_test_case_result_LDADD = -lboost_unit_test_framework -lrt $(LINK_BDB)

_ut_test_case_result_agregation_SOURCES = \
    _ut_test_case_result_agregation.cpp \
    test_case_result.cpp \
    test_case_result_agregation.cpp \
    ../../unit_test.cpp
_ut_test_case_result_agregation_LDADD = -lboost_unit_test_framework -lrt $(LINK_BDB)

_ut_test_case_set_SOURCES = \
    _ut_test_case_set.cpp \
    test_case.cpp \
    test_case_set.cpp \
    test_case_result.cpp \
    test_case_result_agregation.cpp \
    ../../unit_test.cpp
_ut_test_case_set_LDADD = -lboost_unit_test_framework -lrt $(LINK_BDB)

EXTRA_DIST = \
    jfile_chk.py \
    jtt.csv \
    _ut_test_case_set.csv

check-long:
	$(MAKE) check TESTS="$(LONG_TESTS)" SUBDIRS=.
