# Copyright (c) 2010 Red Hat, Inc.
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

bin_PROGRAMS = perf

perf_SOURCES = \
  JournalInstance.cpp \
  JournalParameters.cpp \
  JournalPerformanceTest.cpp \
  PerformanceResult.cpp \
  ScopedTimer.cpp \
  Streamable.cpp \
  TestParameters.cpp \
  JournalInstance.hpp \
  JournalParameters.hpp \
  JournalPerformanceTest.hpp \
  PerformanceResult.hpp \
  ScopedTimable.hpp \
  ScopedTimer.hpp \
  Streamable.hpp \
  TestParameters.hpp
perf_CXXFLAGS = -std=c++0x -lpthread
perf_LDADD  = -lrt $(top_builddir)/lib/msgstore.la
