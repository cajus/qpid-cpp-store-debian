/**
 * \file data_tok.cpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::data_tok (data block token).
 * See comments in file data_tok.hpp for details.
 *
 * \author Kim van der Riet
 *
 * Copyright (c) 2007, 2008 Red Hat, Inc.
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

#include "jrnl/data_tok.hpp"

#include <iomanip>
#include "jrnl/jerrno.hpp"
#include "jrnl/jexception.hpp"
#include "jrnl/slock.hpp"
#include <sstream>

namespace mrg
{
namespace journal
{

// Static members

u_int64_t data_tok::_cnt = 0;
smutex data_tok::_mutex;

data_tok::data_tok():
    _wstate(NONE),
    _rstate(UNREAD),
    _dsize(0),
    _dblks_written(0),
    _dblks_read(0),
    _pg_cnt(0),
    _fid(0),
    _rid(0),
    _xid(),
    _dequeue_rid(0),
    _external_rid(false)
{
    slock s(_mutex);
    _icnt = _cnt++;
}

data_tok::~data_tok() {}

const char*
data_tok::wstate_str() const
{
    return wstate_str(_wstate);
}

const char*
data_tok::wstate_str(write_state wstate)
{
    switch (wstate)
    {
        case NONE:
            return "NONE";
        case ENQ_CACHED:
            return "ENQ_CACHED";
        case ENQ_PART:
            return "ENQ_PART";
        case ENQ_SUBM:
            return "ENQ_SUBM";
        case ENQ:
            return "ENQ";
        case DEQ_CACHED:
            return "DEQ_CACHED";
        case DEQ_PART:
            return "DEQ_PART";
        case DEQ_SUBM:
            return "DEQ_SUBM";
        case DEQ:
            return "DEQ";
        case ABORT_CACHED:
            return "ABORT_CACHED";
        case ABORT_PART:
            return "ABORT_PART";
        case ABORT_SUBM:
            return "ABORT_SUBM";
        case ABORTED:
            return "ABORTED";
        case COMMIT_CACHED:
            return "COMMIT_CACHED";
        case COMMIT_PART:
            return "COMMIT_PART";
        case COMMIT_SUBM:
            return "COMMIT_SUBM";
        case COMMITTED:
            return "COMMITTED";
    }
    // Not using default: forces compiler to ensure all cases are covered.
    return "<wstate unknown>";
}

const char*
data_tok::rstate_str() const
{
    return rstate_str(_rstate);
}

const char*
data_tok::rstate_str(read_state rstate)
{
    switch (rstate)
    {
        case NONE:
            return "NONE";
        case READ_PART:
            return "READ_PART";
        case SKIP_PART:
            return "SKIP_PART";
        case READ:
            return "READ";
    // Not using default: forces compiler to ensure all cases are covered.
    }
    return "<rstate unknown>";
}

void
data_tok::set_rstate(const read_state rstate)
{
    if (_wstate != ENQ && rstate != UNREAD)
    {
        std::ostringstream oss;
        oss << "Attempted to change read state to " << rstate_str(rstate);
        oss << " while write state is not enqueued (wstate ENQ); wstate=" << wstate_str() << ".";
        throw jexception(jerrno::JERR_DTOK_ILLEGALSTATE, oss.str(), "data_tok",
                "set_rstate");
    }
    _rstate = rstate;
}

void
data_tok::reset()
{
    _wstate = NONE;
    _rstate = UNREAD;
    _dsize = 0;
    _dblks_written = 0;
    _dblks_read = 0;
    _pg_cnt = 0;
    _fid = 0;
    _rid = 0;
    _xid.clear();
}

// debug aid
std::string
data_tok::status_str() const
{
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    oss << "dtok id=0x" << _icnt << "; ws=" << wstate_str() << "; rs=" << rstate_str();
    oss << "; fid=0x" << _fid << "; rid=0x" << _rid << "; xid=";
    for (unsigned i=0; i<_xid.size(); i++)
    {
        if (isprint(_xid[i]))
            oss << _xid[i];
        else
            oss << "/" << std::setw(2) << (int)((char)_xid[i]);
    }
    oss << "; drid=0x" << _dequeue_rid << " extrid=" << (_external_rid?"T":"F");
    oss << "; ds=0x" << _dsize << "; dw=0x" << _dblks_written << "; dr=0x" << _dblks_read;
    oss << " pc=0x" << _pg_cnt;
    return oss.str();
}

} // namespace journal
} // namespace mrg
