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

#include <iostream>
#include "jrnl_init_params.hpp"
#include "jrnl_instance.hpp"
#include "jrnl/jdir.hpp"
#include "jrnl/jerrno.hpp"

using namespace boost::unit_test;
using namespace mrg::journal;
using namespace mrg::jtt;
using namespace std;

QPID_AUTO_TEST_SUITE(jtt_jrnl_instance)

const string test_filename("_ut_jrnl_instance");
const char* tdp = getenv("TMP_DATA_DIR");
const string test_dir(tdp && strlen(tdp) > 0 ? tdp : "/tmp/JttTest");

QPID_AUTO_TEST_CASE(constructor_1)
{
    cout << test_filename << ".constructor_1: " << flush;
    const string jid = "jid1";
    const string jdir = test_dir + "/test1";
    const string bfn = "test";
    const u_int16_t num_jfiles = 20;
    const bool ae = false;
    const u_int16_t ae_max_jfiles = 45;
    const u_int32_t jfsize_sblks = 128;

    args a("a1");
    using mrg::jtt::test_case;
    test_case::shared_ptr p(new test_case(1, 0, 0, 0, false, 0, 0, test_case::JTT_PERSISTNET, test_case::JDL_INTERNAL,
            "t1"));
    jrnl_instance ji(jid, jdir, bfn, num_jfiles, ae, ae_max_jfiles, jfsize_sblks);
    ji.init_tc(p, &a);
    ji.run_tc();
    ji.tc_wait_compl();
    try { jdir::verify_dir(jdir, bfn); }
    catch (const jexception& e) { BOOST_ERROR(e.what()); }
    jdir::delete_dir(jdir);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(constructor_2)
{
    cout << test_filename << ".constructor_2: " << flush;
    const string jid = "jid2";
    const string jdir = test_dir + "/test2";
    const string bfn = "test";
    const u_int16_t num_jfiles = 20;
    const bool ae = false;
    const u_int16_t ae_max_jfiles = 45;
    const u_int32_t jfsize_sblks = 128;

    args a("a2");
    using mrg::jtt::test_case;
    test_case::shared_ptr p(new test_case(2, 0, 0, 0, false, 0, 0, test_case::JTT_PERSISTNET, test_case::JDL_INTERNAL,
            "t2"));
    jrnl_init_params::shared_ptr jpp(new jrnl_init_params(jid, jdir, bfn, num_jfiles, ae, ae_max_jfiles, jfsize_sblks));
    jrnl_instance ji(jpp);
    ji.init_tc(p, &a);
    ji.run_tc();
    ji.tc_wait_compl();
    try { jdir::verify_dir(jdir, bfn); }
    catch (const jexception& e) { BOOST_ERROR(e.what()); }
    jdir::delete_dir(jdir);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(constructor_3)
{
    cout << test_filename << ".constructor_3: " << flush;
    const string jid = "jid3";
    const string jdir = test_dir + "/test3";
    const string bfn = "test";
    const u_int16_t num_jfiles = 20;
    const bool ae = false;
    const u_int16_t ae_max_jfiles = 45;
    const u_int32_t jfsize_sblks = 128;

    args a("a3");
    using mrg::jtt::test_case;
    test_case::shared_ptr p(new test_case(3, 0, 0, 0, false, 0, 0, test_case::JTT_PERSISTNET, test_case::JDL_INTERNAL,
            "t3"));
    jrnl_init_params::shared_ptr jpp(new jrnl_init_params(jid, jdir, bfn, num_jfiles, ae, ae_max_jfiles, jfsize_sblks));
    jrnl_instance ji(jpp);
    ji.init_tc(p, &a);
    ji.run_tc();
    ji.tc_wait_compl();
    try { jdir::verify_dir(jdir, bfn); }
    catch (const jexception& e) { BOOST_ERROR(e.what()); }
    jdir::delete_dir(jdir);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(recover)
{
    cout << test_filename << ".recover: " << flush;
    const string jid = "jid5";
    const string jdir = test_dir + "/test5";
    const string bfn = "test";
    const u_int16_t num_jfiles = 20;
    const bool ae = false;
    const u_int16_t ae_max_jfiles = 0;
    const u_int32_t jfsize_sblks = 128;

    args a("a4");
    using mrg::jtt::test_case;
    test_case::shared_ptr p(new test_case(5, 0, 0, 0, false, 0, 0, test_case::JTT_PERSISTNET, test_case::JDL_INTERNAL,
            "t5"));
    jrnl_init_params::shared_ptr jpp(new jrnl_init_params(jid, jdir, bfn, num_jfiles, ae, ae_max_jfiles, jfsize_sblks));
    jrnl_instance ji(jpp);
    ji.init_tc(p, &a);
    ji.run_tc();
    ji.tc_wait_compl();
    try { jdir::verify_dir(jdir, bfn); }
    catch (const jexception& e) { BOOST_ERROR(e.what()); }
    a.recover_mode = true;
    ji.init_tc(p, &a);
    ji.run_tc();
    ji.tc_wait_compl();
    try { jdir::verify_dir(jdir, bfn); }
    catch (const jexception& e) { BOOST_ERROR(e.what()); }
    jdir::delete_dir(jdir);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(recover_no_files)
{
    cout << test_filename << ".recover_no_files: " << flush;
    const string jid = "jid6";
    const string jdir = test_dir + "/test6";
    const string bfn = "test";
    const u_int16_t num_jfiles = 20;
    const bool ae = false;
    const u_int16_t ae_max_jfiles = 0;
    const u_int32_t jfsize_sblks = 128;

    args a("a5");
    a.recover_mode = true;
    using mrg::jtt::test_case;
    test_case::shared_ptr p(new test_case(6, 0, 0, 0, false, 0, 0, test_case::JTT_PERSISTNET, test_case::JDL_INTERNAL,
            "t6"));
    jrnl_init_params::shared_ptr jpp(new jrnl_init_params(jid, jdir, bfn, num_jfiles, ae, ae_max_jfiles, jfsize_sblks));
    jrnl_instance ji(jpp);
    ji.init_tc(p, &a);
    ji.run_tc();
    ji.tc_wait_compl();
    try { jdir::verify_dir(jdir, bfn); }
    catch (const jexception& e) { BOOST_ERROR(e.what()); }
    jdir::delete_dir(jdir);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_SUITE_END()

