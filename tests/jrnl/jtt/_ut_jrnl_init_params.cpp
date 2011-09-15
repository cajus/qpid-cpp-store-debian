/*
 * Copyright (c) 2008 Red Hat, Inc.
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

#include "../../unit_test.h"
#include "jrnl_init_params.hpp"
#include <iostream>

using namespace boost::unit_test;
using namespace mrg::jtt;
using namespace std;

QPID_AUTO_TEST_SUITE(jtt_jrnl_init_params)

const string test_filename("_ut_jrnl_init_params");

QPID_AUTO_TEST_CASE(constructor)
{
    cout << test_filename << ".constructor: " << flush;
    const string jid = "jid";
    const string jdir = "jdir";
    const string bfn = "base filename";
    const u_int16_t num_jfiles = 123;
    const bool ae = false;
    const u_int16_t ae_max_jfiles = 456;
    const u_int32_t jfsize_sblks = 789;
    jrnl_init_params jip(jid, jdir, bfn, num_jfiles, ae, ae_max_jfiles, jfsize_sblks);
    BOOST_CHECK_EQUAL(jip.jid(), jid);
    BOOST_CHECK_EQUAL(jip.jdir(), jdir);
    BOOST_CHECK_EQUAL(jip.base_filename(), bfn);
    BOOST_CHECK_EQUAL(jip.num_jfiles(), num_jfiles);
    BOOST_CHECK_EQUAL(jip.is_ae(), ae);
    BOOST_CHECK_EQUAL(jip.ae_max_jfiles(), ae_max_jfiles);
    BOOST_CHECK_EQUAL(jip.jfsize_sblks(), jfsize_sblks);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(copy_constructor_1)
{
    cout << test_filename << ".copy_constructor_1: " << flush;
    const string jid = "jid";
    const string jdir = "jdir";
    const string bfn = "base filename";
    const u_int16_t num_jfiles = 123;
    const bool ae = false;
    const u_int16_t ae_max_jfiles = 456;
    const u_int32_t jfsize_sblks = 789;
    jrnl_init_params jip1(jid, jdir, bfn, num_jfiles, ae, ae_max_jfiles, jfsize_sblks);
    jrnl_init_params jip2(jip1);
    BOOST_CHECK_EQUAL(jip2.jid(), jid);
    BOOST_CHECK_EQUAL(jip2.jdir(), jdir);
    BOOST_CHECK_EQUAL(jip2.base_filename(), bfn);
    BOOST_CHECK_EQUAL(jip2.num_jfiles(), num_jfiles);
    BOOST_CHECK_EQUAL(jip2.is_ae(), ae);
    BOOST_CHECK_EQUAL(jip2.ae_max_jfiles(), ae_max_jfiles);
    BOOST_CHECK_EQUAL(jip2.jfsize_sblks(), jfsize_sblks);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(copy_constructor_2)
{
    cout << test_filename << ".copy_constructor_2: " << flush;
    const string jid = "jid";
    const string jdir = "jdir";
    const string bfn = "base filename";
    const u_int16_t num_jfiles = 123;
    const bool ae = false;
    const u_int16_t ae_max_jfiles = 456;
    const u_int32_t jfsize_sblks = 789;
    jrnl_init_params::shared_ptr p(new jrnl_init_params(jid, jdir, bfn, num_jfiles, ae, ae_max_jfiles, jfsize_sblks));
    jrnl_init_params jip2(p.get());
    BOOST_CHECK_EQUAL(jip2.jid(), jid);
    BOOST_CHECK_EQUAL(jip2.jdir(), jdir);
    BOOST_CHECK_EQUAL(jip2.base_filename(), bfn);
    BOOST_CHECK_EQUAL(jip2.num_jfiles(), num_jfiles);
    BOOST_CHECK_EQUAL(jip2.is_ae(), ae);
    BOOST_CHECK_EQUAL(jip2.ae_max_jfiles(), ae_max_jfiles);
    BOOST_CHECK_EQUAL(jip2.jfsize_sblks(), jfsize_sblks);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_SUITE_END()

