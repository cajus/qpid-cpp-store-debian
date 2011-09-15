/*
 * Copyright (c) 2007, 2008, 2009, 2010 Red Hat, Inc.
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

#include <iostream>
#include "jrnl/enq_map.hpp"
#include "jrnl/jerrno.hpp"

using namespace boost::unit_test;
using namespace mrg::journal;
using namespace std;

QPID_AUTO_TEST_SUITE(enq_map_suite)

const string test_filename("_ut_enq_map");

QPID_AUTO_TEST_CASE(constructor)
{
    cout << test_filename << ".constructor: " << flush;
    enq_map e1;
    BOOST_CHECK(e1.empty());
    BOOST_CHECK_EQUAL(e1.size(), u_int32_t(0));
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(insert_get)
{
    cout << test_filename << ".insert_get: " << flush;
    u_int16_t pfid;
    u_int64_t rid;
    u_int16_t pfid_start = 0x2000U;
    u_int64_t rid_begin = 0xffffffff00000000ULL;
    u_int64_t rid_end   = 0xffffffff00000200ULL;

    // insert with no dups
    u_int64_t rid_incr_1 = 4ULL;
    enq_map e2;
    e2.set_num_jfiles(pfid_start + (rid_end - rid_begin)/rid_incr_1);
    for (rid = rid_begin, pfid = pfid_start; rid < rid_end; rid += rid_incr_1, pfid++)
        BOOST_CHECK_EQUAL(e2.insert_pfid(rid, pfid), enq_map::EMAP_OK);
    BOOST_CHECK(!e2.empty());
    BOOST_CHECK_EQUAL(e2.size(), u_int32_t(128));

    // get
    u_int64_t rid_incr_2 = 6ULL;
    for (u_int64_t rid = rid_begin; rid < rid_end; rid += rid_incr_2)
    {
        BOOST_CHECK_EQUAL(e2.is_enqueued(rid), (rid%rid_incr_1 ? false : true));
        u_int16_t exp_pfid = pfid_start + (u_int16_t)((rid - rid_begin)/rid_incr_1);
        int16_t ret_fid = e2.get_pfid(rid);
        if (ret_fid < enq_map::EMAP_OK) // fail
        {
            BOOST_CHECK_EQUAL(ret_fid, enq_map::EMAP_RID_NOT_FOUND);
            BOOST_CHECK(rid%rid_incr_1);
        }
        else
        {
            BOOST_CHECK_EQUAL(ret_fid, exp_pfid);
            BOOST_CHECK(rid%rid_incr_1 == 0);
        }
        if ((rid + rid_incr_2)%(8 * rid_incr_2) == 0)
            pfid++;
    }

    // insert with dups
    for (rid = rid_begin, pfid = pfid_start; rid < rid_end; rid += rid_incr_2, pfid++)
    {
        int16_t res = e2.insert_pfid(rid, pfid);
        if (res < enq_map::EMAP_OK) // fail
        {
            BOOST_CHECK_EQUAL(res, enq_map::EMAP_DUP_RID);
            BOOST_CHECK(rid%rid_incr_1 == 0);
        }
        else
            BOOST_CHECK(rid%rid_incr_1);
    }
    BOOST_CHECK_EQUAL(e2.size(), u_int32_t(171));
    e2.clear();
    BOOST_CHECK(e2.empty());
    BOOST_CHECK_EQUAL(e2.size(), u_int32_t(0));
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(get_remove)
{
    cout << test_filename << ".get_remove: " << flush;
    u_int16_t pfid;
    u_int64_t rid;
    u_int16_t pfid_start = 0x3000U;
    u_int64_t rid_begin = 0xeeeeeeee00000000ULL;
    u_int64_t rid_end   = 0xeeeeeeee00000200ULL;

    u_int64_t rid_incr_1 = 4ULL;
    u_int64_t num_incr_1 = (rid_end - rid_begin)/rid_incr_1;
    enq_map e3;
    e3.set_num_jfiles(pfid_start + (rid_end - rid_begin)/rid_incr_1);
    for (rid = rid_begin, pfid = pfid_start; rid < rid_end; rid += rid_incr_1, pfid++)
        BOOST_CHECK_EQUAL(e3.insert_pfid(rid, pfid), enq_map::EMAP_OK);
    BOOST_CHECK_EQUAL(e3.size(), num_incr_1);

    u_int64_t rid_incr_2 = 6ULL;
    for (rid = rid_begin, pfid = pfid_start; rid < rid_end; rid += rid_incr_2, pfid++)
    {
        u_int16_t exp_pfid = pfid_start + (u_int16_t)((rid - rid_begin)/rid_incr_1);
        int16_t ret_fid = e3.get_remove_pfid(rid);
        if (ret_fid < enq_map::EMAP_OK) // fail
        {
            BOOST_CHECK_EQUAL(ret_fid, enq_map::EMAP_RID_NOT_FOUND);
            BOOST_CHECK(rid%rid_incr_1);
        }
        else
        {
            BOOST_CHECK_EQUAL(ret_fid, exp_pfid);
            BOOST_CHECK(rid%rid_incr_1 == 0);
        }
    }
    BOOST_CHECK_EQUAL(e3.size(), u_int32_t(85));
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(lock)
{
    cout << test_filename << ".lock: " << flush;
    u_int16_t pfid;
    u_int64_t rid;
    u_int16_t pfid_start = 0x4000U;
    u_int64_t rid_begin = 0xdddddddd00000000ULL;
    u_int64_t rid_end   = 0xdddddddd00000200ULL;

    // insert, every second entry is locked
    u_int64_t rid_incr_1 = 4ULL;
    u_int64_t num_incr_1 = (rid_end - rid_begin)/rid_incr_1;
    bool locked = false;
    enq_map e4;
    e4.set_num_jfiles(pfid_start + (rid_end - rid_begin)/rid_incr_1);
    for (rid = rid_begin, pfid = pfid_start; rid < rid_end; rid += rid_incr_1, pfid++)
    {
        BOOST_CHECK_EQUAL(e4.insert_pfid(rid, pfid, locked), enq_map::EMAP_OK);
        locked = !locked;
    }
    BOOST_CHECK_EQUAL(e4.size(), num_incr_1);

    // unlock and lock non-existent rids
    int16_t res = e4.lock(1ULL);
    if (res < enq_map::EMAP_OK)
        BOOST_CHECK_EQUAL(res, enq_map::EMAP_RID_NOT_FOUND);
    else
        BOOST_ERROR("Failed to detect locking non-existent rid.");
    res = e4.unlock(2ULL);
    if (res < enq_map::EMAP_OK)
        BOOST_CHECK_EQUAL(res, enq_map::EMAP_RID_NOT_FOUND);
    else
        BOOST_ERROR("Failed to detect unlocking non-existent rid.");

    // get / unlock
    for (u_int64_t rid = rid_begin; rid < rid_end; rid += rid_incr_1)
    {
        int16_t fid = e4.get_pfid(rid);
        if (fid < enq_map::EMAP_OK) // fail
        {
            BOOST_CHECK_EQUAL(fid, enq_map::EMAP_LOCKED);
            BOOST_CHECK(rid%(2*rid_incr_1));
            // unlock, read, then relock
            BOOST_CHECK_EQUAL(e4.unlock(rid), enq_map::EMAP_OK);
            BOOST_CHECK(e4.get_pfid(rid) >= enq_map::EMAP_OK);
            BOOST_CHECK_EQUAL(e4.lock(rid), enq_map::EMAP_OK);
            fid = e4.get_pfid(rid);
            if (fid < enq_map::EMAP_OK) // fail
                BOOST_CHECK_EQUAL(fid, enq_map::EMAP_LOCKED);
            else
                BOOST_ERROR("Failed to prevent getting locked record");
        }
    }

    // remove all; if locked, use with txn_flag true; should ignore all locked records
    for (u_int64_t rid = rid_begin; rid < rid_end; rid += rid_incr_1)
        BOOST_CHECK(e4.get_remove_pfid(rid, true) >= enq_map::EMAP_OK);
    BOOST_CHECK(e4.empty());
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(lists)
{
    cout << test_filename << ".lists: " << flush;
    u_int16_t pfid;
    u_int64_t rid;
    u_int16_t pfid_start = 0x5000UL;
    u_int64_t rid_begin = 0xdddddddd00000000ULL;
    u_int64_t rid_end   = 0xdddddddd00000200ULL;

    // insert, every second entry is locked
    u_int64_t rid_incr_1 = 4ULL;
    u_int64_t num_incr_1 = (rid_end - rid_begin)/rid_incr_1;
    vector<u_int64_t> rid_list;
    vector<u_int16_t> pfid_list;
    enq_map e5;
    e5.set_num_jfiles(pfid_start + (rid_end - rid_begin)/rid_incr_1);
    for (rid = rid_begin, pfid = pfid_start; rid < rid_end; rid += rid_incr_1, pfid++)
    {
        BOOST_CHECK_EQUAL(e5.insert_pfid(rid, pfid), enq_map::EMAP_OK);
        rid_list.push_back(rid);
        pfid_list.push_back(pfid);
    }
    BOOST_CHECK_EQUAL(e5.size(), num_incr_1);
    BOOST_CHECK_EQUAL(rid_list.size(), num_incr_1);
    BOOST_CHECK_EQUAL(pfid_list.size(), num_incr_1);

    vector<u_int64_t> ret_rid_list;
    e5.rid_list(ret_rid_list);
    BOOST_CHECK_EQUAL(ret_rid_list.size(), num_incr_1);
    for (unsigned i=0; i<ret_rid_list.size(); i++)
        BOOST_CHECK_EQUAL(rid_list[i], ret_rid_list[i]);

    vector<u_int16_t> ret_pfid_list;
    e5.pfid_list(ret_pfid_list);
    BOOST_CHECK_EQUAL(ret_pfid_list.size(), num_incr_1);
    for (unsigned i=0; i<ret_pfid_list.size(); i++)
        BOOST_CHECK_EQUAL(pfid_list[i], ret_pfid_list[i]);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(enq_count)
{
    cout << test_filename << ".enq_count: " << flush;

    enq_map e6;

    // Check the allocation and cleanup as the file size is set both up and down
    e6.set_num_jfiles(24);
    e6.set_num_jfiles(0);
    e6.set_num_jfiles(100);
    e6.set_num_jfiles(4);

    // Add 100 enqueues to file 1, check that the counts match
    for (u_int16_t pfid=0; pfid<4; pfid++)
        BOOST_CHECK_EQUAL(e6.get_enq_cnt(pfid), u_int32_t(0));
    for (u_int64_t rid=0; rid<100; rid++)
        BOOST_CHECK_EQUAL(e6.insert_pfid(rid, 1), enq_map::EMAP_OK);
    for (u_int16_t pfid=0; pfid<4; pfid++)
    {
        if (pfid == 1)
            BOOST_CHECK_EQUAL(e6.get_enq_cnt(pfid), u_int32_t(100));
        else
            BOOST_CHECK_EQUAL(e6.get_enq_cnt(pfid), u_int32_t(0));
    }

    // Now remove 10 from file 1, check that the counts match
    for (u_int64_t rid=0; rid<100; rid+=10)
        //e6.Xget_remove_pfid(rid);
        BOOST_CHECK(e6.get_remove_pfid(rid) >= enq_map::EMAP_OK);
    for (u_int16_t pfid=0; pfid<4; pfid++)
    {
        if (pfid == 1)
            BOOST_CHECK_EQUAL(e6.get_enq_cnt(pfid), u_int32_t(90));
        else
            BOOST_CHECK_EQUAL(e6.get_enq_cnt(pfid), u_int32_t(0));
    }

    // Now resize the file up and make sure the count in file 1 still exists
    e6.set_num_jfiles(8);
    for (u_int16_t pfid=0; pfid<8; pfid++)
    {
        if (pfid == 1)
            BOOST_CHECK_EQUAL(e6.get_enq_cnt(pfid), u_int32_t(90));
        else
            BOOST_CHECK_EQUAL(e6.get_enq_cnt(pfid), u_int32_t(0));
    }

    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(stress)
{
    cout << test_filename << ".stress: " << flush;
    u_int64_t rid;
    u_int64_t rid_cnt;
    u_int64_t rid_begin = 0xffffffff00000000ULL;
    u_int64_t num_rid = 10;

    enq_map e7;
    e7.set_num_jfiles(rid_begin + num_rid);

    // insert even rids with no dups
    for (rid = rid_begin, rid_cnt = u_int64_t(0); rid_cnt < num_rid; rid += 2ULL, rid_cnt++)
        BOOST_CHECK_EQUAL(e7.insert_pfid(rid, u_int16_t(0)), enq_map::EMAP_OK);
    BOOST_CHECK_EQUAL(e7.size(), num_rid);

    // insert odd rids with no dups
    for (rid = rid_begin + 1, rid_cnt = u_int64_t(0); rid_cnt < num_rid; rid += 2ULL, rid_cnt++)
        BOOST_CHECK_EQUAL(e7.insert_pfid(rid, u_int16_t(0)), enq_map::EMAP_OK);
    BOOST_CHECK_EQUAL(e7.size(), num_rid * 2);

    // remove even rids
    for (rid = rid_begin, rid_cnt = u_int64_t(0); rid_cnt < num_rid; rid += 2ULL, rid_cnt++)
        BOOST_CHECK(e7.get_remove_pfid(rid) >= enq_map::EMAP_OK);
    BOOST_CHECK_EQUAL(e7.size(), num_rid);

    cout << "ok" << endl;
}

QPID_AUTO_TEST_SUITE_END()
