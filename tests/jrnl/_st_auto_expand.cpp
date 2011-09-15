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

#include "../unit_test.h"
#include <cmath>
#include <iostream>
#include "jrnl/jcntl.hpp"

using namespace boost::unit_test;
using namespace mrg::journal;
using namespace std;

QPID_AUTO_TEST_SUITE(journal_auto_expand)

const string test_filename("_st_auto_expand");

#include "_st_helper_fns.hpp"

// === Test suite ===

QPID_AUTO_TEST_CASE(no_ae_threshold)
{
    string test_name = get_test_name(test_filename, "no_ae_threshold");
    try
    {
        string msg;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_DEFAULT_JFILES, false, 0, DEFAULT_JFSIZE_SBLKS);
        unsigned m;

        // Fill journal to just below threshold
        u_int32_t t = num_msgs_to_threshold(NUM_DEFAULT_JFILES,
                DEFAULT_JFSIZE_SBLKS * JRNL_SBLK_SIZE, LARGE_MSG_REC_SIZE_DBLKS);
        for (m=0; m<t; m++)
            enq_msg(jc, m, create_msg(msg, m, LARGE_MSG_SIZE), false);
        // This enqueue should exceed the threshold
        BOOST_CHECK_EQUAL(jc.get_enq_cnt(), t);
        enq_msg(jc, m, create_msg(msg, m, LARGE_MSG_SIZE), false, RHM_IORES_ENQCAPTHRESH);
        BOOST_CHECK_EQUAL(jc.get_enq_cnt(), t);
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(no_ae_threshold_dequeue_some)
{
    string test_name = get_test_name(test_filename, "no_ae_threshold_dequeue_some");
    try
    {
        string msg;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_DEFAULT_JFILES, false, 0, DEFAULT_JFSIZE_SBLKS);
        unsigned m;

        // Fill journal to just below threshold
        u_int32_t t = num_msgs_to_threshold(NUM_DEFAULT_JFILES,
                DEFAULT_JFSIZE_SBLKS * JRNL_SBLK_SIZE, LARGE_MSG_REC_SIZE_DBLKS);
        for (m=0; m<t; m++)
            enq_msg(jc, m, create_msg(msg, m, LARGE_MSG_SIZE), false);
        // This enqueue should exceed the threshold
        BOOST_CHECK_EQUAL(jc.get_enq_cnt(), t);
        enq_msg(jc, m, create_msg(msg, m, LARGE_MSG_SIZE), false, RHM_IORES_ENQCAPTHRESH);
        BOOST_CHECK_EQUAL(jc.get_enq_cnt(), t);

        // Dequeue 25 msgs
        #define NUM_MSGS_DEQ 25
        for (m=0; m<NUM_MSGS_DEQ; m++)
            deq_msg(jc, m, m+t);
        BOOST_CHECK_EQUAL(jc.get_enq_cnt(), u_int32_t(t-NUM_MSGS_DEQ));

        // Check we can still enqueue and dequeue
        for (m=t+NUM_MSGS_DEQ; m<t+NUM_MSGS_DEQ+NUM_MSGS; m++)
            enq_msg(jc, m, create_msg(msg, m, LARGE_MSG_SIZE), false);
        for (m=t+NUM_MSGS_DEQ; m<t+NUM_MSGS_DEQ+NUM_MSGS; m++)
            deq_msg(jc, m, m+NUM_MSGS_DEQ+NUM_MSGS);
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(no_ae_threshold_dequeue_all)
{
    string test_name = get_test_name(test_filename, "no_ae_threshold_dequeue_all");
    try
    {
        string msg;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_DEFAULT_JFILES, false, 0, DEFAULT_JFSIZE_SBLKS);
        unsigned m;

        // Fill journal to just below threshold
        u_int32_t t = num_msgs_to_threshold(NUM_DEFAULT_JFILES,
                DEFAULT_JFSIZE_SBLKS * JRNL_SBLK_SIZE, LARGE_MSG_REC_SIZE_DBLKS);
        for (m=0; m<t; m++)
            enq_msg(jc, m, create_msg(msg, m, LARGE_MSG_SIZE), false);
        // This enqueue should exceed the threshold
        BOOST_CHECK_EQUAL(jc.get_enq_cnt(), t);
        enq_msg(jc, m, create_msg(msg, m, LARGE_MSG_SIZE), false, RHM_IORES_ENQCAPTHRESH);
        BOOST_CHECK_EQUAL(jc.get_enq_cnt(), t);

        // Dequeue all msgs
        for (m=0; m<t; m++)
            deq_msg(jc, m, m+t);
        BOOST_CHECK_EQUAL(jc.get_enq_cnt(), u_int32_t(0));

        // Check we can still enqueue and dequeue
        for (m=2*t; m<2*t + NUM_MSGS; m++)
            enq_msg(jc, m, create_msg(msg, m, LARGE_MSG_SIZE), false);
        for (m=2*t; m<2*t + NUM_MSGS; m++)
            deq_msg(jc, m, m+2*t+NUM_MSGS);
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_SUITE_END()
