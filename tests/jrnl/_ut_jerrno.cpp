/*
 * Copyright (c) 2007, 2008 Red Hat, Inc.
 *
 * This file is part of the Qpid async store library msgstore.so.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 *
 * The GNU Lesser General Public License is available in the file COPYING.
 */

#include "../unit_test.h"

#include <cstring>
#include <iostream>
#include "jrnl/jerrno.hpp"

using namespace boost::unit_test;
using namespace mrg::journal;
using namespace std;

QPID_AUTO_TEST_SUITE(jerrno_suite)
using namespace mrg::journal;

const string test_filename("_ut_jerrno");

QPID_AUTO_TEST_CASE(jerrno_val)
{
    cout << test_filename << ".jerrno_val: " << flush;
    const char* m = "JERR__MALLOC";
    string malloc_msg = string(jerrno::err_msg(jerrno::JERR__MALLOC));
    BOOST_CHECK(malloc_msg.substr(0, std::strlen(m)).compare(m) == 0);
    BOOST_CHECK(std::strcmp(jerrno::err_msg(0), "<Unknown error code>") == 0);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_SUITE_END()
