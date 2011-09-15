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

QPID_AUTO_TEST_SUITE(journal_read_txn)

const string test_filename("_st_read_txn");

#include "_st_helper_fns.hpp"

// === Test suite ===

QPID_AUTO_TEST_CASE(tx_enqueue_commit_block)
{
    string test_name = get_test_name(test_filename, "tx_enqueue_commit_block");
    try
    {
        string msg;
        string xid;
        string rmsg;
        string rxid;
        bool transientFlag;
        bool externalFlag;
        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        create_xid(xid, 0, XID_SIZE);
        for (int m=0; m<NUM_MSGS; m++)
            enq_txn_msg(jc, m, create_msg(msg, m, MSG_SIZE), xid, false);
        jc.flush();
        read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_TXPENDING);
        txn_commit(jc, NUM_MSGS, xid);
        jc.flush();
        for (int m=0; m<NUM_MSGS; m++)
        {
            read_msg(jc, rmsg, rxid, transientFlag, externalFlag);
            BOOST_CHECK_EQUAL(create_msg(msg, m, MSG_SIZE), rmsg);
            BOOST_CHECK_EQUAL(rxid, xid);
            BOOST_CHECK_EQUAL(transientFlag, false);
            BOOST_CHECK_EQUAL(externalFlag, false);
        }
        read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_EMPTY);
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(tx_enqueue_commit_interleaved)
{
    string test_name = get_test_name(test_filename, "tx_enqueue_commit_interleaved");
    try
    {
        string msg;
        string xid;
        string rmsg;
        string rxid;
        bool transientFlag;
        bool externalFlag;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        for (int m=0; m<NUM_MSGS; m++)
        {
            create_xid(xid, 2*m, XID_SIZE);
            enq_txn_msg(jc, 2*m, create_msg(msg, 2*m, MSG_SIZE), xid, false);
            jc.flush();
            read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_TXPENDING);
            txn_commit(jc, 2*m+1, xid);
            jc.flush();
            read_msg(jc, rmsg, rxid, transientFlag, externalFlag);
            BOOST_CHECK_EQUAL(create_msg(msg, 2*m, MSG_SIZE), rmsg);
            BOOST_CHECK_EQUAL(rxid, xid);
            BOOST_CHECK_EQUAL(transientFlag, false);
            BOOST_CHECK_EQUAL(externalFlag, false);
        }
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(tx_enqueue_abort_block)
{
    string test_name = get_test_name(test_filename, "tx_enqueue_abort_block");
    try
    {
        string msg;
        string xid;
        string rmsg;
        string rxid;
        bool transientFlag;
        bool externalFlag;
        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        create_xid(xid, 1, XID_SIZE);
        for (int m=0; m<NUM_MSGS; m++)
            enq_txn_msg(jc, m, create_msg(msg, m, MSG_SIZE), xid, false);
        jc.flush();
        read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_TXPENDING);
        txn_abort(jc, NUM_MSGS, xid);
        jc.flush();
        read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_EMPTY);
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(tx_enqueue_abort_interleaved)
{
    string test_name = get_test_name(test_filename, "tx_enqueue_abort_interleaved");
    try
    {
        string msg;
        string xid;
        string rmsg;
        string rxid;
        bool transientFlag;
        bool externalFlag;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        for (int m=0; m<NUM_MSGS; m++)
        {
            create_xid(xid, 2*m, XID_SIZE);
            enq_txn_msg(jc, 2*m, create_msg(msg, 2*m, MSG_SIZE), xid, false);
            jc.flush();
            read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_TXPENDING);
            txn_abort(jc, 2*m+1, xid);
            jc.flush();
            read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_EMPTY);
        }
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(tx_enqueue_commit_dequeue_block)
{
    string test_name = get_test_name(test_filename, "tx_enqueue_commit_dequeue_block");
    try
    {
        string msg;
        string xid;
        string rmsg;
        string rxid;
        bool transientFlag;
        bool externalFlag;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        create_xid(xid, 2, XID_SIZE);
        for (int m=0; m<NUM_MSGS; m++)
            enq_txn_msg(jc, m, create_msg(msg, m, MSG_SIZE), xid, false);
        txn_commit(jc, NUM_MSGS, xid);
        for (int m=0; m<NUM_MSGS; m++)
            deq_msg(jc, m, m+NUM_MSGS+1);
        jc.flush();
        read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_EMPTY);
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(tx_enqueue_commit_dequeue_interleaved)
{
    string test_name = get_test_name(test_filename, "tx_enqueue_commit_dequeue_interleaved");
    try
    {
        string msg;
        string xid;
        string rmsg;
        string rxid;
        bool transientFlag;
        bool externalFlag;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        for (int m=0; m<NUM_MSGS; m++)
        {
            create_xid(xid, 3*m, XID_SIZE);
            enq_txn_msg(jc, 3*m, create_msg(msg, m, MSG_SIZE), xid, false);
            txn_commit(jc, 3*m+1, xid);
            deq_msg(jc, 3*m, 3*m+2);
            jc.flush();
            read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_EMPTY);
        }
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(enqueue_tx_dequeue_commit_block)
{
    string test_name = get_test_name(test_filename, "enqueue_tx_dequeue_commit_block");
    try
    {
        string msg;
        string xid;
        string rmsg;
        string rxid;
        bool transientFlag;
        bool externalFlag;

        create_xid(xid, 3, XID_SIZE);
        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        for (int m=0; m<NUM_MSGS; m++)
            enq_msg(jc, m, create_msg(msg, m, MSG_SIZE), false);
        for (int m=0; m<NUM_MSGS; m++)
            deq_txn_msg(jc, m, m+NUM_MSGS, xid);
        jc.flush();
        read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_TXPENDING);
        txn_commit(jc, 2*NUM_MSGS, xid);
        jc.flush();
        read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_EMPTY);
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(enqueue_tx_dequeue_commit_interleaved)
{
    string test_name = get_test_name(test_filename, "enqueue_tx_dequeue_commit_interleaved");
    try
    {
        string msg;
        string xid;
        string rmsg;
        string rxid;
        bool transientFlag;
        bool externalFlag;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        for (int m=0; m<NUM_MSGS; m++)
        {
            enq_msg(jc, 3*m, create_msg(msg, 3*m, MSG_SIZE), false);
            create_xid(xid, 3*m, XID_SIZE);
            deq_txn_msg(jc, 3*m, 3*m+1, xid);
            jc.flush();
            read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_TXPENDING);
            txn_commit(jc, 3*m+2, xid);
            jc.flush();
            read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_EMPTY);
        }
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(enqueue_tx_dequeue_abort_block)
{
    string test_name = get_test_name(test_filename, "enqueue_tx_dequeue_abort_block");
    try
    {
        string msg;
        string xid;
        string rmsg;
        string rxid;
        bool transientFlag;
        bool externalFlag;

        create_xid(xid, 4, XID_SIZE);
        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        for (int m=0; m<NUM_MSGS; m++)
            enq_msg(jc, m, create_msg(msg, m, MSG_SIZE), false);
        for (int m=0; m<NUM_MSGS; m++)
            deq_txn_msg(jc, m, m+NUM_MSGS, xid);
        jc.flush();
        read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_TXPENDING);
        txn_abort(jc, 2*NUM_MSGS, xid);
        jc.flush();
        for (int m=0; m<NUM_MSGS; m++)
        {
            read_msg(jc, rmsg, rxid, transientFlag, externalFlag);
            BOOST_CHECK_EQUAL(create_msg(msg, m, MSG_SIZE), rmsg);
            BOOST_CHECK_EQUAL(rxid.length(), std::size_t(0));
            BOOST_CHECK_EQUAL(transientFlag, false);
            BOOST_CHECK_EQUAL(externalFlag, false);
        }
        read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_EMPTY);
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(enqueue_tx_dequeue_abort_interleaved)
{
    string test_name = get_test_name(test_filename, "enqueue_tx_dequeue_abort_interleaved");
    try
    {
        string msg;
        string xid;
        string rmsg;
        string rxid;
        bool transientFlag;
        bool externalFlag;

        test_jrnl_cb cb;
        test_jrnl jc(test_name, test_dir, test_name, cb);
        jc.initialize(NUM_TEST_JFILES, false, 0, TEST_JFSIZE_SBLKS);
        for (int m=0; m<NUM_MSGS; m++)
        {
            enq_msg(jc, 3*m, create_msg(msg, 3*m, MSG_SIZE), false);
            create_xid(xid, 3*m, XID_SIZE);
            deq_txn_msg(jc, 3*m, 3*m+1, xid);
            jc.flush();
            read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_TXPENDING);
            txn_abort(jc, 3*m+2, xid);
            jc.flush();
            read_msg(jc, rmsg, rxid, transientFlag, externalFlag);
            read_msg(jc, rmsg, rxid, transientFlag, externalFlag, RHM_IORES_EMPTY);
        }
    }
    catch(const exception& e) { BOOST_FAIL(e.what()); }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_SUITE_END()
