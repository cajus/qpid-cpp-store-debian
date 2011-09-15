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
#include <iostream>
#include <sys/stat.h>
#include "test_case.hpp"
#include "test_case_set.hpp"

using namespace boost::unit_test;
using namespace mrg::jtt;
using namespace std;

QPID_AUTO_TEST_SUITE(jtt_test_case_set)

const string csv_file("_ut_test_case_set.csv");
const string test_filename("_ut_test_case_set");

// === Helper functions ===

bool check_csv_file(const char* filename)
{
    struct stat s;
    if (::stat(filename, &s))
        return false;
    if (S_ISREG(s.st_mode))
        return true;
    return false;
}

// === Test suite ===

QPID_AUTO_TEST_CASE(constructor)
{
    cout << test_filename << ".constructor: " << flush;
    test_case_set tcs;
    BOOST_CHECK(tcs.empty());
    BOOST_CHECK_EQUAL(tcs.size(), unsigned(0));
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(append_1)
{
    cout << test_filename << ".append_1: " << flush;
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

    test_case_set tcs;
    tcs.append(test_case_num, num_msgs, min_data_size, max_data_size, auto_deq, min_xid_size,
            max_xid_size, transient, external, comment);
    BOOST_CHECK(!tcs.empty());
    BOOST_CHECK_EQUAL(tcs.size(), unsigned(1));
    test_case::shared_ptr tcp = tcs[0];
    BOOST_CHECK_EQUAL(tcp->test_case_num(), test_case_num);
    BOOST_CHECK_EQUAL(tcp->num_msgs(), num_msgs);
    BOOST_CHECK_EQUAL(tcp->min_data_size(), min_data_size);
    BOOST_CHECK_EQUAL(tcp->max_data_size(), max_data_size);
    BOOST_CHECK_EQUAL(tcp->min_xid_size(), min_xid_size);
    BOOST_CHECK_EQUAL(tcp->max_xid_size(), max_xid_size);
    BOOST_CHECK_EQUAL(tcp->transient(), transient);
    BOOST_CHECK_EQUAL(tcp->external(), external);
    BOOST_CHECK_EQUAL(tcp->comment(), comment);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(append_2)
{
    cout << test_filename << ".append_2: " << flush;
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

    test_case::shared_ptr tcp(new test_case(test_case_num, num_msgs, min_data_size, max_data_size,
            auto_deq, min_xid_size, max_xid_size, transient, external, comment));
    test_case_set tcs;
    tcs.append(tcp);
    BOOST_CHECK(!tcs.empty());
    BOOST_CHECK_EQUAL(tcs.size(), unsigned(1));
    tcp = tcs[0];
    BOOST_CHECK_EQUAL(tcp->test_case_num(), test_case_num);
    BOOST_CHECK_EQUAL(tcp->num_msgs(), num_msgs);
    BOOST_CHECK_EQUAL(tcp->min_data_size(), min_data_size);
    BOOST_CHECK_EQUAL(tcp->max_data_size(), max_data_size);
    BOOST_CHECK_EQUAL(tcp->min_xid_size(), min_xid_size);
    BOOST_CHECK_EQUAL(tcp->max_xid_size(), max_xid_size);
    BOOST_CHECK_EQUAL(tcp->transient(), transient);
    BOOST_CHECK_EQUAL(tcp->external(), external);
    BOOST_CHECK_EQUAL(tcp->comment(), comment);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(append_from_csv)
{
    cout << test_filename << ".append_from_csv: " << flush;
    test_case_set tcs;
    BOOST_REQUIRE_MESSAGE(check_csv_file(csv_file.c_str()), "Test CSV file \"" << csv_file <<
            "\" is missing.");
    tcs.append_from_csv(csv_file, false);
    BOOST_CHECK(!tcs.empty());
    BOOST_CHECK_EQUAL(tcs.size(), unsigned(44));
    BOOST_CHECK_EQUAL(tcs.ignored(), unsigned(0));
    tcs.clear();
    BOOST_CHECK(tcs.empty());
    tcs.append_from_csv(csv_file, true);
    BOOST_CHECK(!tcs.empty());
    BOOST_CHECK_EQUAL(tcs.size(), unsigned(18));
    BOOST_CHECK_EQUAL(tcs.ignored(), unsigned(26));
    cout << "ok" << endl;
}

QPID_AUTO_TEST_SUITE_END()
