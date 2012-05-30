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


abs_srcdir=@abs_srcdir@

TMP_DATA_DIR=$(abs_srcdir)/../tmp_data_dir

TESTS = \
  run_federation_sys_tests

LONG_TESTS = \
  run_long_federation_sys_tests

EXTRA_DIST = \
  federation_tests_env.sh \
  run_federation_sys_tests \
  run_long_federation_sys_tests

TESTS_ENVIRONMENT = \
  QPID_DIR=$(QPID_DIR) \
  QPID_BLD=$(QPID_BLD) \
  TMP_DATA_DIR=$(TMP_DATA_DIR) \
  abs_srcdir=$(abs_srcdir)
  
check-long: all
	$(MAKE) check TESTS="$(LONG_TESTS)" SUBDIRS=.

# END
    
