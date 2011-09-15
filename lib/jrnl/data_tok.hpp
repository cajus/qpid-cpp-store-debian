/**
 * \file data_tok.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::data_tok (data block token).
 * See class documentation for details.
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

#ifndef mrg_journal_data_tok_hpp
#define mrg_journal_data_tok_hpp

namespace mrg
{
namespace journal
{
class data_tok;
}
}

#include <cassert>
#include <cstddef>
#include "jrnl/smutex.hpp"
#include <pthread.h>
#include <string>
#include <sys/types.h>

namespace mrg
{

namespace journal
{

    /**
    * \class data_tok
    * \brief Data block token (data_tok) used to track wstate of a data block through asynchronous
    *     I/O process
    */
    class data_tok
    {
    public:
        // TODO: Fix this, separate write state from operation
        // ie: wstate = NONE, CACHED, PART, SUBM, COMPL
        //     op = ENQUEUE, DEQUEUE, ABORT, COMMIT
        enum write_state
        {
            NONE,       ///< Data block not sent to journal
            ENQ_CACHED, ///< Data block enqueue written to page cache
            ENQ_PART,   ///< Data block part-submitted to AIO, waiting for page buffer to free up
            ENQ_SUBM,   ///< Data block enqueue submitted to AIO
            ENQ,        ///< Data block enqueue AIO write complete (enqueue complete)
            DEQ_CACHED, ///< Data block dequeue written to page cache
            DEQ_PART,   ///< Data block part-submitted to AIO, waiting for page buffer to free up
            DEQ_SUBM,   ///< Data block dequeue submitted to AIO
            DEQ,        ///< Data block dequeue AIO write complete (dequeue complete)
            ABORT_CACHED,
            ABORT_PART,
            ABORT_SUBM,
            ABORTED,
            COMMIT_CACHED,
            COMMIT_PART,
            COMMIT_SUBM,
            COMMITTED
        };

        enum read_state
        {
            UNREAD,     ///< Data block not read
            READ_PART,  ///< Data block is part-read; waiting for page buffer to fill
            SKIP_PART,  ///< Prev. dequeued dblock is part-skipped; waiting for page buffer to fill
            READ        ///< Data block is fully read
        };

    protected:
        static smutex _mutex;
        static u_int64_t _cnt;
        u_int64_t   _icnt;
        write_state _wstate;        ///< Enqueued / dequeued state of data
        read_state  _rstate;        ///< Read state of data
        std::size_t _dsize;         ///< Data size in bytes
        u_int32_t   _dblks_written; ///< Data blocks read/written
        u_int32_t   _dblks_read;    ///< Data blocks read/written
        u_int32_t   _pg_cnt;        ///< Page counter - incr for each page containing part of data
        u_int16_t   _fid;           ///< FID containing header of enqueue record
        u_int64_t   _rid;           ///< RID of data set by enqueue operation
        std::string _xid;           ///< XID set by enqueue operation
        u_int64_t   _dequeue_rid;   ///< RID of data set by dequeue operation
        bool        _external_rid;  ///< Flag to indicate external setting of rid

    public:
        data_tok();
        virtual ~data_tok();

        inline u_int64_t id() const { return _icnt; }
        inline write_state wstate() const { return _wstate; }
        const char* wstate_str() const;
        static const char* wstate_str(write_state wstate);
        inline read_state rstate() const { return _rstate; }
        const char* rstate_str() const;
        static const char* rstate_str(read_state rstate);
        inline bool is_writable() const { return _wstate == NONE || _wstate == ENQ_PART; }
        inline bool is_enqueued() const { return _wstate == ENQ; }
        inline bool is_readable() const { return _wstate == ENQ; }
        inline bool is_read() const { return _rstate == READ; }
        inline bool is_dequeueable() const { return _wstate == ENQ || _wstate == DEQ_PART; }
        inline void set_wstate(const write_state wstate) { _wstate = wstate; }
        void set_rstate(const read_state rstate);
        inline std::size_t dsize() const { return _dsize; }
        inline void set_dsize(std::size_t dsize) { _dsize = dsize; }

        inline u_int32_t dblocks_written() const { return _dblks_written; }
        inline void incr_dblocks_written(u_int32_t dblks_written)
                { _dblks_written += dblks_written; }
        inline void set_dblocks_written(u_int32_t dblks_written) { _dblks_written = dblks_written; }

        inline u_int32_t dblocks_read() const { return _dblks_read; }
        inline void incr_dblocks_read(u_int32_t dblks_read) { _dblks_read += dblks_read; }
        inline void set_dblocks_read(u_int32_t dblks_read) { _dblks_read = dblks_read; }

        inline u_int32_t pg_cnt() const { return _pg_cnt; }
        inline u_int32_t incr_pg_cnt() { return ++_pg_cnt; }
        inline u_int32_t decr_pg_cnt() { assert(_pg_cnt != 0); return --_pg_cnt; }

        inline u_int16_t fid() const { return _fid; }
        inline void set_fid(const u_int16_t fid) { _fid = fid; }
        inline u_int64_t rid() const { return _rid; }
        inline void set_rid(const u_int64_t rid) { _rid = rid; }
        inline u_int64_t dequeue_rid() const {return _dequeue_rid; }
        inline void set_dequeue_rid(const u_int64_t rid) { _dequeue_rid = rid; }
        inline bool external_rid() const { return _external_rid; }
        inline void set_external_rid(const bool external_rid) { _external_rid = external_rid; }

        inline bool has_xid() const { return !_xid.empty(); }
        inline const std::string& xid() const { return _xid; }
        inline void clear_xid() { _xid.clear(); }
        inline void set_xid(const std::string& xid) { _xid.assign(xid); }
        inline void set_xid(const void* xidp, const std::size_t xid_len)
                { _xid.assign((const char*)xidp, xid_len); }

        void reset();

        // debug aid
        std::string status_str() const;
    };

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_data_tok_hpp
