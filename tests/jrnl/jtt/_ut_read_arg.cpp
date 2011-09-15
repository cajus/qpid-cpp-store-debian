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
#include <boost/test/unit_test_log.hpp>
#include "read_arg.hpp"
#include <iostream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;
using namespace mrg::jtt;
using namespace boost::unit_test;
using namespace std;

QPID_AUTO_TEST_SUITE(jtt_read_arg)

const string test_filename("_ut_read_arg");

QPID_AUTO_TEST_CASE(constructor)
{
    cout << test_filename << ".constructor: " << flush;
    read_arg ra1;
    BOOST_CHECK_EQUAL(ra1.val(), read_arg::NONE);
    BOOST_CHECK_EQUAL(ra1.str(), "NONE");
    read_arg ra2(read_arg::NONE);
    BOOST_CHECK_EQUAL(ra2.val(), read_arg::NONE);
    BOOST_CHECK_EQUAL(ra2.str(), "NONE");
    read_arg ra3(read_arg::ALL);
    BOOST_CHECK_EQUAL(ra3.val(), read_arg::ALL);
    BOOST_CHECK_EQUAL(ra3.str(), "ALL");
    read_arg ra4(read_arg::RANDOM);
    BOOST_CHECK_EQUAL(ra4.val(), read_arg::RANDOM);
    BOOST_CHECK_EQUAL(ra4.str(), "RANDOM");
    read_arg ra5(read_arg::LAZYLOAD);
    BOOST_CHECK_EQUAL(ra5.val(), read_arg::LAZYLOAD);
    BOOST_CHECK_EQUAL(ra5.str(), "LAZYLOAD");
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(set_val)
{
    cout << test_filename << ".set_val: " << flush;
    read_arg ra;
    BOOST_CHECK_EQUAL(ra.val(), read_arg::NONE);
    BOOST_CHECK_EQUAL(ra.str(), "NONE");
    ra.set_val(read_arg::ALL);
    BOOST_CHECK_EQUAL(ra.val(), read_arg::ALL);
    BOOST_CHECK_EQUAL(ra.str(), "ALL");
    ra.set_val(read_arg::RANDOM);
    BOOST_CHECK_EQUAL(ra.val(), read_arg::RANDOM);
    BOOST_CHECK_EQUAL(ra.str(), "RANDOM");
    ra.set_val(read_arg::LAZYLOAD);
    BOOST_CHECK_EQUAL(ra.val(), read_arg::LAZYLOAD);
    BOOST_CHECK_EQUAL(ra.str(), "LAZYLOAD");
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(parse)
{
    cout << test_filename << ".parse: " << flush;
    read_arg ra;
    ra.parse("LAZYLOAD");
    BOOST_CHECK_EQUAL(ra.val(), read_arg::LAZYLOAD);
    BOOST_CHECK_EQUAL(ra.str(), "LAZYLOAD");
    ra.parse("ALL");
    BOOST_CHECK_EQUAL(ra.val(), read_arg::ALL);
    BOOST_CHECK_EQUAL(ra.str(), "ALL");
    BOOST_CHECK_THROW(ra.parse(""), po::invalid_option_value)
    BOOST_CHECK_EQUAL(ra.val(), read_arg::ALL);
    BOOST_CHECK_EQUAL(ra.str(), "ALL");
    BOOST_CHECK_THROW(ra.parse("abc123"), po::invalid_option_value)
    BOOST_CHECK_EQUAL(ra.val(), read_arg::ALL);
    BOOST_CHECK_EQUAL(ra.str(), "ALL");
    ra.parse("NONE");
    BOOST_CHECK_EQUAL(ra.val(), read_arg::NONE);
    BOOST_CHECK_EQUAL(ra.str(), "NONE");
    ra.parse("RANDOM");
    BOOST_CHECK_EQUAL(ra.val(), read_arg::RANDOM);
    BOOST_CHECK_EQUAL(ra.str(), "RANDOM");
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(istream_)
{
    cout << test_filename << ".istream_: " << flush;
    read_arg ra;
    istringstream ss1("LAZYLOAD", ios::in);
    ss1 >> ra;
    BOOST_CHECK_EQUAL(ra.val(), read_arg::LAZYLOAD);
    BOOST_CHECK_EQUAL(ra.str(), "LAZYLOAD");
    istringstream ss2("ALL", ios::in);
    ss2 >> ra;
    BOOST_CHECK_EQUAL(ra.val(), read_arg::ALL);
    BOOST_CHECK_EQUAL(ra.str(), "ALL");
    istringstream ss3("NONE", ios::in);
    ss3 >> ra;
    BOOST_CHECK_EQUAL(ra.val(), read_arg::NONE);
    BOOST_CHECK_EQUAL(ra.str(), "NONE");
    istringstream ss4("RANDOM", ios::in);
    ss4 >> ra;
    BOOST_CHECK_EQUAL(ra.val(), read_arg::RANDOM);
    BOOST_CHECK_EQUAL(ra.str(), "RANDOM");
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(ostream_)
{
    cout << test_filename << ".ostream_: " << flush;
    ostringstream s1;
    read_arg ra(read_arg::LAZYLOAD);
    s1 << ra;
    BOOST_CHECK_EQUAL(s1.str(), "LAZYLOAD");
    ra.set_val(read_arg::ALL);
    ostringstream s2;
    s2 << ra;
    BOOST_CHECK_EQUAL(s2.str(), "ALL");
    ra.set_val(read_arg::NONE);
    ostringstream s3;
    s3 << ra;
    BOOST_CHECK_EQUAL(s3.str(), "NONE");
    ra.set_val(read_arg::RANDOM);
    ostringstream s4;
    s4 << ra;
    BOOST_CHECK_EQUAL(s4.str(), "RANDOM");
    cout << "ok" << endl;
}

QPID_AUTO_TEST_SUITE_END()
