#ifndef QPIPD_TEST_UNIT_TEST_H_
#define QPIPD_TEST_UNIT_TEST_H_

/*
 Copyright (c) 2007, 2008 Red Hat, Inc.

 This file is part of the Qpid async store library msgstore.so.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA

 The GNU Lesser General Public License is available in the file COPYING.
 */


// Workaround so we can build against boost 1.32, 1.33 and boost 1.34.
// Remove when we no longer need to support 1.32 or 1.33.

#include <boost/version.hpp>

#if (BOOST_VERSION < 103400) // v.1.33 and earlier
# include <boost/test/auto_unit_test.hpp>
#else // v.1.34 and later
# include <boost/test/unit_test.hpp>
#endif

// Keep the test function for compilation but do not not register it.
// TODO aconway 2008-04-23: better workaround for expected failures.
// The following causes the test testUpdateTxState not to run at all.
# define QPID_AUTO_TEST_CASE_EXPECTED_FAILURES(test_name,n)             \
    namespace { struct test_name { void test_method(); };  }            \
    void test_name::test_method()
// The following runs the test testUpdateTxState, but it fails.
/*#define QPID_AUTO_TEST_CASE_EXPECTED_FAILURES(test_name,n)             \
    namespace { struct test_name { void test_method(); };  }            \
    BOOST_AUTO_TEST_CASE(name)*/

#if (BOOST_VERSION < 103300) // v.1.32 and earlier

# define QPID_AUTO_TEST_SUITE(name)
# define QPID_AUTO_TEST_CASE(name)  BOOST_AUTO_UNIT_TEST(name)
# define QPID_AUTO_TEST_SUITE_END()

#elif (BOOST_VERSION < 103400) // v.1.33

// Note the trailing ';'
# define QPID_AUTO_TEST_SUITE(name) BOOST_AUTO_TEST_SUITE(name);
# define QPID_AUTO_TEST_CASE(name)  BOOST_AUTO_TEST_CASE(name)
# define QPID_AUTO_TEST_SUITE_END() BOOST_AUTO_TEST_SUITE_END();

#else // v.1.34 and later

# define QPID_AUTO_TEST_SUITE(name) BOOST_AUTO_TEST_SUITE(name)
# define QPID_AUTO_TEST_CASE(name)  BOOST_AUTO_TEST_CASE(name)
# define QPID_AUTO_TEST_SUITE_END() BOOST_AUTO_TEST_SUITE_END()

#endif

#endif  /*!QPIPD_TEST_UNIT_TEST_H_*/
