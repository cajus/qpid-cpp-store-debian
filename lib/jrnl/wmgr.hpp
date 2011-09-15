/**
 * \file wmgr.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::wmgr (read manager). See
 * class documentation for details.
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

#ifndef mrg_journal_wmgr_hpp
#define mrg_journal_wmgr_hpp

namespace mrg
{
namespace journal
{
class wmgr;
}
}

#include <cstring>
#include "jrnl/enums.hpp"
#include "jrnl/pmgr.hpp"
#include "jrnl/wrfc.hpp"
#include <set>

namespace mrg
{
namespace journal
{

    /**
    * \brief Class for managing a write page cache of arbitrary size and number of pages.
    *
    * The write page cache works on the principle of caching the write data within a page until
    * that page is either full or flushed; this initiates a single AIO write operation to store
    * the data on disk.
    *
    * The maximum disk throughput is achieved by keeping the write operations of uniform size.
    * Waiting for a page cache to fill achieves this; and in high data volume/throughput situations
    * achieves the optimal disk throughput. Calling flush() forces a write of the current page cache
    * no matter how full it is, and disrupts the uniformity of the write operations. This should
    * normally only be done if throughput drops and there is a danger of a page of unwritten data
    * waiting around for excessive time.
    *
    * The usual tradeoff between data storage latency and throughput performance applies.
    */
    class wmgr : public pmgr
    {
    private:
        wrfc& _wrfc;                    ///< Ref to write rotating file controller
        u_int32_t _max_dtokpp;          ///< Max data writes per page
        u_int32_t _max_io_wait_us;      ///< Max wait in microseconds till submit
        void* _fhdr_base_ptr;           ///< Base pointer to file header memory
        void** _fhdr_ptr_arr;           ///< Array of pointers to file headers memory
        aio_cb** _fhdr_aio_cb_arr;      ///< Array of iocb pointers for file header writes
        u_int32_t _cached_offset_dblks; ///< Amount of unwritten data in page (dblocks)
        std::deque<data_tok*> _ddtokl;  ///< Deferred dequeue data_tok list
        u_int32_t _jfsize_dblks;        ///< Journal file size in dblks (NOT sblks!)
        u_int32_t _jfsize_pgs;          ///< Journal file size in cache pages
        u_int16_t _num_jfiles;          ///< Number of files used in iocb mallocs

        // TODO: Convert _enq_busy etc into a proper threadsafe lock
        // TODO: Convert to enum? Are these encodes mutually exclusive?
        bool _enq_busy;                 ///< Flag true if enqueue is in progress
        bool _deq_busy;                 ///< Flag true if dequeue is in progress
        bool _abort_busy;               ///< Flag true if abort is in progress
        bool _commit_busy;              ///< Flag true if commit is in progress

        enum _op_type { WMGR_ENQUEUE = 0, WMGR_DEQUEUE, WMGR_ABORT, WMGR_COMMIT };
        static const char* _op_str[];

        enq_rec _enq_rec;               ///< Enqueue record used for encoding/decoding
        deq_rec _deq_rec;               ///< Dequeue record used for encoding/decoding
        txn_rec _txn_rec;               ///< Transaction record used for encoding/decoding
        std::set<std::string> _txn_pending_set; ///< Set containing xids of pending commits/aborts

    public:
        wmgr(jcntl* jc, enq_map& emap, txn_map& tmap, wrfc& wrfc);
        wmgr(jcntl* jc, enq_map& emap, txn_map& tmap, wrfc& wrfc, const u_int32_t max_dtokpp,
                const u_int32_t max_iowait_us);
        virtual ~wmgr();

        void initialize(aio_callback* const cbp, const u_int32_t wcache_pgsize_sblks,
                const u_int16_t wcache_num_pages, const u_int32_t max_dtokpp,
                const u_int32_t max_iowait_us, std::size_t eo = 0);
        iores enqueue(const void* const data_buff, const std::size_t tot_data_len,
                const std::size_t this_data_len, data_tok* dtokp, const void* const xid_ptr,
                const std::size_t xid_len, const bool transient, const bool external);
        iores dequeue(data_tok* dtokp, const void* const xid_ptr, const std::size_t xid_len,
                const bool txn_coml_commit);
        iores abort(data_tok* dtokp, const void* const xid_ptr, const std::size_t xid_len);
        iores commit(data_tok* dtokp, const void* const xid_ptr, const std::size_t xid_len);
        iores flush();
        int32_t get_events(page_state state, timespec* const timeout, bool flush = false);
        bool is_txn_synced(const std::string& xid);
        inline bool curr_pg_blocked() const { return _page_cb_arr[_pg_index]._state != UNUSED; }
        inline bool curr_file_blocked() const { return _wrfc.aio_cnt() > 0; }
        inline u_int32_t unflushed_dblks() { return _cached_offset_dblks; }

        // Debug aid
        const std::string status_str() const;

    private:
        void initialize(aio_callback* const cbp, const u_int32_t wcache_pgsize_sblks,
                const u_int16_t wcache_num_pages);
        iores pre_write_check(const _op_type op, const data_tok* const dtokp,
                const std::size_t xidsize = 0, const std::size_t dsize = 0, const bool external = false)
                const;
        void dequeue_check(const std::string& xid, const u_int64_t drid);
        void file_header_check(const u_int64_t rid, const bool cont, const u_int32_t rec_dblks_rem);
        void flush_check(iores& res, bool& cont, bool& done);
        iores write_flush();
        iores rotate_file();
        void dblk_roundup();
        void write_fhdr(u_int64_t rid, u_int16_t fid, u_int16_t lid, std::size_t fro);
        void rotate_page();
        void clean();
    };

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_wmgr_hpp
