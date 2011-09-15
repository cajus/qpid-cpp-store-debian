#!/bin/bash

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


error() { echo $*; exit 1; }

# Make sure $QPID_DIR contains what we need.
if ! test -d "$QPID_DIR" ; then
    echo "WARNING: QPID_DIR is not set skipping system tests."
    exit
fi

xml_spec=$QPID_DIR/specs/amqp.0-10-qpid-errata.xml
test -f $xml_spec || error "$xml_spec not found: invalid \$QPID_DIR ?"
export PYTHONPATH=$QPID_DIR/python:$QPID_DIR/extras/qmf/src/py

echo "Using directory $TMP_DATA_DIR"

fail=0

# Run the tests with a given set of flags
BROKER_OPTS="--no-module-dir --load-module=$STORE_LIB --data-dir=$TMP_DATA_DIR --auth=no --wcache-page-size 16"
run_tests() {
    for p in `seq 1 8`; do
	$abs_srcdir/start_broker "$@" ${BROKER_OPTS} || { echo "FAIL broker start";  return 1; }
	python "$abs_srcdir/persistence.py" -s "$xml_spec" -b localhost:`cat qpidd.port` -p $p -r 3 || fail=1; 
	$abs_srcdir/stop_broker
    done
}

run_tests || fail=1

exit $fail
