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

QPID_AUTO_TEST_SUITE(journal_read)

const string test_filename("_st_read");

#include "_st_helper_fns.hpp"

// === Test suite ===

#ifndef LONG_TEST
/*
 * ==============================================
 *                  NORMAL TESTS
 * This section contains normal "make check" tests
 * for building/packaging. These are built when
 * LONG_TEST is _not_ defined.
 * ==============================================
 */

QPID_AUTO_TEST_CASE(empty_read)
{
    string test_name = get_test_name(test_filename, "empty_read");
    try
    {
        string msg;
        string rmsg;
        string xid;
        bool transientFlag;
        bool externalFlag;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        read_msg(jc, rmsg, xid, transientFlag, externalFlag, RHM_IORES_EMPTY);
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(enqueue_read_dequeue_block)
{
    string test_name = get_test_name(test_filename, "enqueue_read_dequeue_block");
    try
    {
        string msg;
        string rmsg;
        string xid;
        bool transientFlag;
        bool externalFlag;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        for (int m=0; m<NUM_MSGS; m++)
            enq_msg(jc, m, create_msg(msg, m, MSG_SIZE), false);
        jc.flush();
        for (int m=0; m<NUM_MSGS; m++)
        {
            read_msg(jc, rmsg, xid, transientFlag, externalFlag);
            BOOST_CHECK_EQUAL(create_msg(msg, m, MSG_SIZE), rmsg);
            BOOST_CHECK_EQUAL(xid.size(), std::size_t(0));
            BOOST_CHECK_EQUAL(transientFlag, false);
            BOOST_CHECK_EQUAL(externalFlag, false);
        }
        read_msg(jc, rmsg, xid, transientFlag, externalFlag, RHM_IORES_EMPTY);
        for (int m=0; m<NUM_MSGS; m++)
            deq_msg(jc, m, m+NUM_MSGS);
        read_msg(jc, rmsg, xid, transientFlag, externalFlag, RHM_IORES_EMPTY);
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(enqueue_read_dequeue_interleaved)
{
    string test_name = get_test_name(test_filename, "enqueue_read_dequeue_interleaved");
    try
    {
        string msg;
        string rmsg;
        string xid;
        bool transientFlag;
        bool externalFlag;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        for (int m=0; m<500*NUM_MSGS; m+=2)
        {
            enq_msg(jc, m, create_msg(msg, m, MSG_SIZE), false);
            jc.flush();
            read_msg(jc, rmsg, xid, transientFlag, externalFlag);
            BOOST_CHECK_EQUAL(create_msg(msg, m, MSG_SIZE), rmsg);
            BOOST_CHECK_EQUAL(xid.size(), std::size_t(0));
            BOOST_CHECK_EQUAL(transientFlag, false);
            BOOST_CHECK_EQUAL(externalFlag, false);
            deq_msg(jc, m, m+1);
            jc.flush();
            read_msg(jc, rmsg, xid, transientFlag, externalFlag, RHM_IORES_EMPTY);
        }
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(enqueue_recovered_read_dequeue)
{
    string test_name = get_test_name(test_filename, "enqueue_recovered_read_dequeue");
    try
    {
        {
            string msg;

            test_jrnl_cb cb;
            test_jrnl jc(test_name, test_dir, test_name, cb);
            jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
            for (int m=0; m<NUM_MSGS; m++)
                enq_msg(jc, m, create_msg(msg, m, MSG_SIZE), false);
        }
        {
            string msg;
            u_int64_t hrid;
            string rmsg;
            string xid;
            bool transientFlag;
            bool externalFlag;

            test_jrnl_cb cb;
            test_jrnl jc(test_name, test_dir, test_name, cb);
            jc.recover(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS, 0, hrid);
            BOOST_CHECK_EQUAL(hrid, u_int64_t(NUM_MSGS - 1));
            jc.recover_complete();
            for (int m=0; m<NUM_MSGS; m++)
            {
                read_msg(jc, rmsg, xid, transientFlag, externalFlag);
                BOOST_CHECK_EQUAL(create_msg(msg, m, MSG_SIZE), rmsg);
                BOOST_CHECK_EQUAL(xid.size(), std::size_t(0));
                BOOST_CHECK_EQUAL(transientFlag, false);
                BOOST_CHECK_EQUAL(externalFlag, false);
            }
            read_msg(jc, rmsg, xid, transientFlag, externalFlag, RHM_IORES_EMPTY);
            for (int m=0; m<NUM_MSGS; m++)
                deq_msg(jc, m, m+NUM_MSGS);
            read_msg(jc, rmsg, xid, transientFlag, externalFlag, RHM_IORES_EMPTY);
        }
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(multi_page_enqueue_recovered_read_dequeue_block)
{
    string test_name = get_test_name(test_filename, "multi_page_enqueue_recovered_read_dequeue_block");
    try
    {
        {
            string msg;

            test_jrnl_cb cb;
            test_jrnl jc(test_name, test_dir, test_name, cb);
            jc.initialize(2*NUM_TEST_JFILES, false, 0, 10*TEST_JFSIZE_SBLKS);
            for (int m=0; m<NUM_MSGS*125; m++)
                enq_msg(jc, m, create_msg(msg, m, 16*MSG_SIZE), false);
        }
        {
            string msg;
            u_int64_t hrid;
            string rmsg;
            string xid;
            bool transientFlag;
            bool externalFlag;

            test_jrnl_cb cb;
            test_jrnl jc(test_name, test_dir, test_name, cb);
            jc.recover(2*NUM_TEST_JFILES, false, 0, 10*TEST_JFSIZE_SBLKS, 0, hrid);
            BOOST_CHECK_EQUAL(hrid, u_int64_t(NUM_MSGS*125 - 1));
            jc.recover_complete();
            for (int m=0; m<NUM_MSGS*125; m++)
            {
                read_msg(jc, rmsg, xid, transientFlag, externalFlag);
                BOOST_CHECK_EQUAL(create_msg(msg, m, 16*MSG_SIZE), rmsg);
                BOOST_CHECK_EQUAL(xid.size(), std::size_t(0));
                BOOST_CHECK_EQUAL(transientFlag, false);
                BOOST_CHECK_EQUAL(externalFlag, false);
            }
            read_msg(jc, rmsg, xid, transientFlag, externalFlag, RHM_IORES_EMPTY);
            for (int m=0; m<NUM_MSGS*125; m++)
                deq_msg(jc, m, m+NUM_MSGS*125);
            read_msg(jc, rmsg, xid, transientFlag, externalFlag, RHM_IORES_EMPTY);
        }
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(enqueue_recover_read_recovered_read_dequeue_block)
{
    string test_name = get_test_name(test_filename, "enqueue_recover_read_recovered_read_dequeue_block");
    try
    {
        {
            string msg;

            test_jrnl_cb cb;
            test_jrnl jc(test_name, test_dir, test_name, cb);
            jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
            for (int m=0; m<NUM_MSGS; m++)
                enq_msg(jc, m, create_msg(msg, m, MSG_SIZE), false);
        }
        {
            string msg;
            u_int64_t hrid;
            string rmsg;
            string xid;
            bool transientFlag;
            bool externalFlag;

            test_jrnl_cb cb;
            test_jrnl jc(test_name, test_dir, test_name, cb);
            jc.recover(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS, 0, hrid);
            BOOST_CHECK_EQUAL(hrid, u_int64_t(NUM_MSGS - 1));
            for (int m=0; m<NUM_MSGS; m++)
            {
                read_msg(jc, rmsg, xid, transientFlag, externalFlag);
                BOOST_CHECK_EQUAL(create_msg(msg, m, MSG_SIZE), rmsg);
                BOOST_CHECK_EQUAL(xid.size(), std::size_t(0));
                BOOST_CHECK_EQUAL(transientFlag, false);
                BOOST_CHECK_EQUAL(externalFlag, false);
            }
            read_msg(jc, rmsg, xid, transientFlag, externalFlag, RHM_IORES_EMPTY);
        }
        {
            string msg;
            u_int64_t hrid;
            string rmsg;
            string xid;
            bool transientFlag;
            bool externalFlag;

            test_jrnl_cb cb;
            test_jrnl jc(test_name, test_dir, test_name, cb);
            jc.recover(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS, 0, hrid);
            BOOST_CHECK_EQUAL(hrid, u_int64_t(NUM_MSGS - 1));
            for (int m=0; m<NUM_MSGS; m++)
            {
                read_msg(jc, rmsg, xid, transientFlag, externalFlag);
                BOOST_CHECK_EQUAL(create_msg(msg, m, MSG_SIZE), rmsg);
                BOOST_CHECK_EQUAL(xid.size(), std::size_t(0));
                BOOST_CHECK_EQUAL(transientFlag, false);
                BOOST_CHECK_EQUAL(externalFlag, false);
            }
            read_msg(jc, rmsg, xid, transientFlag, externalFlag, RHM_IORES_EMPTY);
            jc.recover_complete();
            for (int m=0; m<NUM_MSGS; m++)
            {
                read_msg(jc, rmsg, xid, transientFlag, externalFlag);
                BOOST_CHECK_EQUAL(create_msg(msg, m, MSG_SIZE), rmsg);
                BOOST_CHECK_EQUAL(xid.size(), std::size_t(0));
                BOOST_CHECK_EQUAL(transientFlag, false);
                BOOST_CHECK_EQUAL(externalFlag, false);
            }
            read_msg(jc, rmsg, xid, transientFlag, externalFlag, RHM_IORES_EMPTY);
            for (int m=0; m<NUM_MSGS; m++)
                deq_msg(jc, m, m+NUM_MSGS);
            read_msg(jc, rmsg, xid, transientFlag, externalFlag, RHM_IORES_EMPTY);
        }
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(delayed_read)
{
    string test_name = get_test_name(test_filename, "delayed_read");
    try
    {
        string msg;
        string rmsg;
        string xid;
        bool transientFlag;
        bool externalFlag;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        unsigned m;
        for (m=0; m<2*NUM_MSGS; m+=2)
        {
            enq_msg(jc, m, create_msg(msg, m, MSG_SIZE), false);
            deq_msg(jc, m, m+1);
        }
        enq_msg(jc, m, create_msg(msg, m, MSG_SIZE), false);
        jc.flush();
        read_msg(jc, rmsg, xid, transientFlag, externalFlag);
        BOOST_CHECK_EQUAL(msg, rmsg);
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(cache_cycled_delayed_read)
{
    string test_name = get_test_name(test_filename, "cache_cycled_delayed_read");
    try
    {
        string msg;
        string rmsg;
        string xid;
        bool transientFlag;
        bool externalFlag;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        unsigned m;
        unsigned read_buffer_size_dblks = JRNL_RMGR_PAGES * JRNL_RMGR_PAGE_SIZE * JRNL_SBLK_SIZE;
        unsigned n = num_msgs_to_full(1, read_buffer_size_dblks, 16*MSG_REC_SIZE_DBLKS, true);
        for (m=0; m<2*2*n + 20; m+=2) // fill read buffer twice + 10 msgs
        {
            enq_msg(jc, m, create_msg(msg, m, 16*MSG_SIZE), false);
            deq_msg(jc, m, m+1);
        }
        enq_msg(jc, m, create_msg(msg, m, MSG_SIZE), false);
        jc.flush();
        read_msg(jc, rmsg, xid, transientFlag, externalFlag);
        BOOST_CHECK_EQUAL(msg, rmsg);
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

#else
/*
 * ==============================================
 *                  LONG TESTS
 * This section contains long tests and soak tests,
 * and are run using target check-long (ie "make
 * check-long"). These are built when LONG_TEST is
 * defined.
 * ==============================================
 */

QPID_AUTO_TEST_CASE(multi_page_enqueue_read_dequeue_block)
{
    string test_name = get_test_name(test_filename, "multi_page_enqueue_read_dequeue_block");
    try
    {
        string msg;
        string rmsg;
        string xid;
        bool transientFlag;
        bool externalFlag;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(2*NUM_TEST_JFILES, false, 0, 10*TEST_JFSIZE_SBLKS);
        for (int i=0; i<10; i++)
        {
            for (int m=0; m<NUM_MSGS*125; m++)
                enq_msg(jc, m, create_msg(msg, m, 16*MSG_SIZE), false);
            jc.flush();
            for (int m=0; m<NUM_MSGS*125; m++)
            {
                read_msg(jc, rmsg, xid, transientFlag, externalFlag);
                BOOST_CHECK_EQUAL(create_msg(msg, m, 16*MSG_SIZE), rmsg);
                BOOST_CHECK_EQUAL(xid.size(), std::size_t(0));
                BOOST_CHECK_EQUAL(transientFlag, false);
                BOOST_CHECK_EQUAL(externalFlag, false);
            }
            read_msg(jc, rmsg, xid, transientFlag, externalFlag, RHM_IORES_EMPTY);
            for (int m=0; m<NUM_MSGS*125; m++)
                deq_msg(jc, m, m+NUM_MSGS*125);
            read_msg(jc, rmsg, xid, transientFlag, externalFlag, RHM_IORES_EMPTY);
        }
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(increasing_interval_delayed_read)
{
    string test_name = get_test_name(test_filename, "increasing_interval_delayed_read");
    try
    {
        string msg;
        string rmsg;
        string xid;
        bool transientFlag;
        bool externalFlag;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        unsigned read_buffer_size_dblks = JRNL_RMGR_PAGES * JRNL_RMGR_PAGE_SIZE * JRNL_SBLK_SIZE;
        unsigned n = num_msgs_to_full(1, read_buffer_size_dblks, MSG_REC_SIZE_DBLKS, true);
        unsigned m = 0;

        // Validate read pipeline
        enq_msg(jc, m, create_msg(msg, m, MSG_SIZE), false);
        jc.flush();
        read_msg(jc, rmsg, xid, transientFlag, externalFlag);
        deq_msg(jc, m, m+1);
        m += 2;

        // repeat the following multiple times...
        for (int i=0; i<10; i++)
        {
            // Invalidate read pipeline with large write
            unsigned t = m + (i*n) + 25;
            for (; m<t; m+=2)
            {
                enq_msg(jc, m, create_msg(msg, m, MSG_SIZE), false);
                deq_msg(jc, m, m+1);
            }

            // Revalidate read pipeline
            enq_msg(jc, m, create_msg(msg, m, MSG_SIZE), false);
            jc.flush();
            read_msg(jc, rmsg, xid, transientFlag, externalFlag);
            BOOST_CHECK_EQUAL(msg, rmsg);
            deq_msg(jc, m, m+1);
            m += 2;
        }
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

#endif

QPID_AUTO_TEST_SUITE_END()
