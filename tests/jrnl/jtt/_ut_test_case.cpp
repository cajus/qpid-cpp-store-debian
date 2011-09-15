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
#include <cstddef>
#include <iomanip>
#include <iostream>
#include "test_case.hpp"
#include "test_case_result.hpp"

using namespace boost::unit_test;
using namespace mrg::jtt;
using namespace std;

QPID_AUTO_TEST_SUITE(jtt_test_case)

const string test_filename("_ut_test_case");

QPID_AUTO_TEST_CASE(constructor)
{
    cout << test_filename << ".constructor: " << flush;
    const unsigned test_case_num = 0x12345;
    const u_int32_t num_msgs = 0x100;
    const std::size_t min_data_size = 0x1000;
    const std::size_t max_data_size = 0;
    const bool auto_deq = true;
    const std::size_t min_xid_size = 0x200;
    const std::size_t max_xid_size = 0x200;
    using mrg::jtt::test_case;
    const test_case::transient_t transient = test_case::JTT_PERSISTNET;
    const test_case::external_t external = test_case::JDL_INTERNAL;
    const string comment = "This is a test";

    test_case tc(test_case_num, num_msgs, min_data_size, max_data_size, auto_deq,
            min_xid_size, max_xid_size, transient, external, comment);
    BOOST_CHECK_EQUAL(tc.test_case_num(), test_case_num);
    BOOST_CHECK_EQUAL(tc.num_msgs(), num_msgs);
    BOOST_CHECK_EQUAL(tc.min_data_size(), min_data_size);
    BOOST_CHECK_EQUAL(tc.max_data_size(), max_data_size);
    BOOST_CHECK_EQUAL(tc.auto_deq(), auto_deq);
    BOOST_CHECK_EQUAL(tc.min_xid_size(), min_xid_size);
    BOOST_CHECK_EQUAL(tc.max_xid_size(), max_xid_size);
    BOOST_CHECK_EQUAL(tc.transient(), transient);
    BOOST_CHECK_EQUAL(tc.external(), external);
    BOOST_CHECK_EQUAL(tc.comment(), comment);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(results)
{
    cout << test_filename << ".results: " << flush;
    const unsigned test_case_num = 0x12345;
    const u_int32_t num_msgs = 0x100;
    const std::size_t min_data_size = 0x1000;
    const std::size_t max_data_size = 0;
    const bool auto_deq = true;
    const std::size_t min_xid_size = 0x200;
    const std::size_t max_xid_size = 0x200;
    using mrg::jtt::test_case;
    const test_case::transient_t transient = test_case::JTT_PERSISTNET;
    const test_case::external_t external = test_case::JDL_INTERNAL;
    const string comment = "This is a test";
    const unsigned num_results = 20;

    test_case tc(test_case_num, num_msgs, min_data_size, max_data_size, auto_deq,
            min_xid_size, max_xid_size, transient, external, comment);
    for (unsigned i=0; i<num_results; i++)
    {
        ostringstream oss;
        oss << "JID_" << setfill('0') << setw(2) << i;
        test_case_result::shared_ptr p(new test_case_result(oss.str()));
        tc.add_result(p);
    }
    BOOST_CHECK_EQUAL(tc.num_results(), num_results);
    test_case_result_agregation ave = tc.average();
    unsigned i=0;
    for (test_case_result_agregation::tcrp_list_citr j=ave.rlist_begin(); j!=ave.rlist_end();
            i++,j++)
    {
        ostringstream oss;
        oss << "JID_" << setfill('0') << setw(2) << i;
        BOOST_CHECK_EQUAL((*j)->jid(), oss.str());
    }
    for (unsigned i=0; i<num_results; i++)
    {
        ostringstream oss;
        oss << "JID_" << setfill('0') << setw(2) << i;
        BOOST_CHECK_EQUAL(ave[i]->jid(), oss.str());
    }
    tc.clear();
    BOOST_CHECK_EQUAL(tc.num_results(), unsigned(0));
    cout << "ok" << endl;
}

QPID_AUTO_TEST_SUITE_END()
