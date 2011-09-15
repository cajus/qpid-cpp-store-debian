/**
 * \file txn_map.cpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::txn_map (transaction map). See
 * comments in file txn_map.hpp for details.
 *
 * \author Kim van der Riet
 *
 * Copyright (c) 2007, 2008, 2009, 2010 Red Hat Inc.
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

#include "jrnl/txn_map.hpp"

#include <iomanip>
#include "jrnl/jerrno.hpp"
#include "jrnl/jexception.hpp"
#include "jrnl/slock.hpp"
#include <sstream>

namespace mrg
{
namespace journal
{

// return/error codes
int16_t txn_map::TMAP_RID_NOT_FOUND = -2;
int16_t txn_map::TMAP_XID_NOT_FOUND = -1;
int16_t txn_map::TMAP_OK = 0;
int16_t txn_map::TMAP_NOT_SYNCED = 0;
int16_t txn_map::TMAP_SYNCED = 1;

txn_data_struct::txn_data_struct(const u_int64_t rid, const u_int64_t drid, const u_int16_t pfid,
		const bool enq_flag, const bool commit_flag):
        _rid(rid),
        _drid(drid),
        _pfid(pfid),
        _enq_flag(enq_flag),
        _commit_flag(commit_flag),
        _aio_compl(false)
{}

txn_map::txn_map():
        _map(),
        _pfid_txn_cnt()
{}

txn_map::~txn_map() {}

void
txn_map::set_num_jfiles(const u_int16_t num_jfiles)
{
    _pfid_txn_cnt.resize(num_jfiles, 0);
}

u_int32_t
txn_map::get_txn_pfid_cnt(const u_int16_t pfid) const
{
    return _pfid_txn_cnt.at(pfid);
}

bool
txn_map::insert_txn_data(const std::string& xid, const txn_data& td)
{
    bool ok = true;
    slock s(_mutex);
    xmap_itr itr = _map.find(xid);
    if (itr == _map.end()) // not found in map
    {
        txn_data_list list;
        list.push_back(td);
        std::pair<xmap_itr, bool> ret = _map.insert(xmap_param(xid, list));
        if (!ret.second) // duplicate
            ok = false;
    }
    else
        itr->second.push_back(td);
    _pfid_txn_cnt.at(td._pfid)++;
    return ok;
}

const txn_data_list
txn_map::get_tdata_list(const std::string& xid)
{
    slock s(_mutex);
    return get_tdata_list_nolock(xid);
}

const txn_data_list
txn_map::get_tdata_list_nolock(const std::string& xid)
{
    xmap_itr itr = _map.find(xid);
    if (itr == _map.end()) // not found in map
        return _empty_data_list;
    return itr->second;
}

const txn_data_list
txn_map::get_remove_tdata_list(const std::string& xid)
{
    slock s(_mutex);
    xmap_itr itr = _map.find(xid);
    if (itr == _map.end()) // not found in map
        return _empty_data_list;
    txn_data_list list = itr->second;
    _map.erase(itr);
    for (tdl_itr i=list.begin(); i!=list.end(); i++)
        _pfid_txn_cnt.at(i->_pfid)--;
    return list;
}

bool
txn_map::in_map(const std::string& xid)
{
    slock s(_mutex);
    xmap_itr itr= _map.find(xid);
    return itr != _map.end();
}

u_int32_t
txn_map::enq_cnt()
{
    return cnt(true);
}

u_int32_t
txn_map::deq_cnt()
{
    return cnt(true);
}

u_int32_t
txn_map::cnt(const bool enq_flag)
{
    slock s(_mutex);
    u_int32_t c = 0;
    for (xmap_itr i = _map.begin(); i != _map.end(); i++)
    {
        for (tdl_itr j = i->second.begin(); j < i->second.end(); j++)
        {
            if (j->_enq_flag == enq_flag)
                c++;
        }
    }
    return c;
}

int16_t
txn_map::is_txn_synced(const std::string& xid)
{
    slock s(_mutex);
    xmap_itr itr = _map.find(xid);
    if (itr == _map.end()) // not found in map
        return TMAP_XID_NOT_FOUND;
    bool is_synced = true;
    for (tdl_itr litr = itr->second.begin(); litr < itr->second.end(); litr++)
    {
        if (!litr->_aio_compl)
        {
            is_synced = false;
            break;
        }
    }
    return is_synced ? TMAP_SYNCED : TMAP_NOT_SYNCED;
}

int16_t
txn_map::set_aio_compl(const std::string& xid, const u_int64_t rid)
{
    slock s(_mutex);
    xmap_itr itr = _map.find(xid);
    if (itr == _map.end()) // xid not found in map
        return TMAP_XID_NOT_FOUND;
    for (tdl_itr litr = itr->second.begin(); litr < itr->second.end(); litr++)
    {
        if (litr->_rid == rid)
        {
            litr->_aio_compl = true;
            return TMAP_OK; // rid found
        }
    }
    // xid present, but rid not found
    return TMAP_RID_NOT_FOUND;
}

bool
txn_map::data_exists(const std::string& xid, const u_int64_t rid)
{
    bool found = false;
    {
        slock s(_mutex);
        txn_data_list tdl = get_tdata_list_nolock(xid);
        tdl_itr itr = tdl.begin();
        while (itr != tdl.end() && !found)
        {
            found = itr->_rid == rid;
            itr++;
        }
    }
    return found;
}

bool
txn_map::is_enq(const u_int64_t rid)
{
    bool found = false;
    {
        slock s(_mutex);
        for (xmap_itr i = _map.begin(); i != _map.end() && !found; i++)
        {
            txn_data_list list = i->second;
            for (tdl_itr j = list.begin(); j < list.end() && !found; j++)
            {
                if (j->_enq_flag)
                    found = j->_rid == rid;
                else
                    found = j->_drid == rid;
            }
        }
    }
    return found;
}

void
txn_map::xid_list(std::vector<std::string>& xv)
{
    xv.clear();
    {
        slock s(_mutex);
        for (xmap_itr itr = _map.begin(); itr != _map.end(); itr++)
            xv.push_back(itr->first);
    }
}

} // namespace journal
} // namespace mrg
