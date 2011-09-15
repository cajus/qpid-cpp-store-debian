/**
 * \file pmgr.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::pmgr (page manager). See
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

#ifndef mrg_journal_pmgr_hpp
#define mrg_journal_pmgr_hpp

namespace mrg
{
namespace journal
{
    class pmgr;
    class jcntl;
}
}

#include <deque>
#include "jrnl/aio.hpp"
#include "jrnl/aio_callback.hpp"
#include "jrnl/data_tok.hpp"
#include "jrnl/deq_rec.hpp"
#include "jrnl/enq_map.hpp"
#include "jrnl/enq_rec.hpp"
#include "jrnl/fcntl.hpp"
#include "jrnl/txn_map.hpp"
#include "jrnl/txn_rec.hpp"

namespace mrg
{
namespace journal
{

    /**
    * \brief Abstract class for managing either read or write page cache of arbitrary size and
    *    number of cache_num_pages.
    */
    class pmgr
    {
    public:
        /**
        * \brief Enumeration of possible stats of a page within a page cache.
        */
        enum page_state
        {
            UNUSED,                     ///< A page is uninitialized, contains no data.
            IN_USE,                     ///< Page is in use.
            AIO_PENDING,                ///< An AIO request outstanding.
            AIO_COMPLETE                ///< An AIO request is complete.
        };

    protected:
        /**
        * \brief Page control block, carries control and state information for each page in the
        *     cache.
        */
        struct page_cb
        {
            u_int16_t _index;           ///< Index of this page
            page_state _state;          ///< Status of page
            u_int64_t _frid;            ///< First rid in page (used for fhdr init)
            u_int32_t _wdblks;          ///< Total number of dblks in page so far
            u_int32_t _rdblks;          ///< Total number of dblks in page
            std::deque<data_tok*>* _pdtokl; ///< Page message tokens list
            fcntl* _wfh;                ///< File handle for incrementing write compl counts
            fcntl* _rfh;                ///< File handle for incrementing read compl counts
            void* _pbuff;               ///< Page buffer

            page_cb(u_int16_t index);   ///< Convenience constructor
            const char* state_str() const; ///< Return state as string for this pcb
        };

        static const u_int32_t _sblksize; ///< Disk softblock size
        u_int32_t _cache_pgsize_sblks;  ///< Size of page cache cache_num_pages
        u_int16_t _cache_num_pages;     ///< Number of page cache cache_num_pages
        jcntl* _jc;                     ///< Pointer to journal controller
        enq_map& _emap;                 ///< Ref to enqueue map
        txn_map& _tmap;                 ///< Ref to transaction map
        void* _page_base_ptr;           ///< Base pointer to page memory
        void** _page_ptr_arr;           ///< Array of pointers to cache_num_pages in page memory
        page_cb* _page_cb_arr;          ///< Array of page_cb structs
        aio_cb* _aio_cb_arr;            ///< Array of iocb structs
        aio_event* _aio_event_arr;      ///< Array of io_events
        io_context_t _ioctx;            ///< AIO context for read/write operations
        u_int16_t _pg_index;            ///< Index of current page being used
        u_int32_t _pg_cntr;             ///< Page counter; determines if file rotation req'd
        u_int32_t _pg_offset_dblks;     ///< Page offset (used so far) in data blocks
        u_int32_t _aio_evt_rem;         ///< Remaining AIO events
        aio_callback* _cbp;             ///< Pointer to callback object

        enq_rec _enq_rec;               ///< Enqueue record used for encoding/decoding
        deq_rec _deq_rec;               ///< Dequeue record used for encoding/decoding
        txn_rec _txn_rec;               ///< Transaction record used for encoding/decoding

    public:
        pmgr(jcntl* jc, enq_map& emap, txn_map& tmap);
        virtual ~pmgr();

        virtual int32_t get_events(page_state state, timespec* const timeout, bool flush = false) = 0;
        inline u_int32_t get_aio_evt_rem() const { return _aio_evt_rem; }
        static const char* page_state_str(page_state ps);
        inline u_int32_t cache_pgsize_sblks() const { return _cache_pgsize_sblks; }
        inline u_int16_t cache_num_pages() const { return _cache_num_pages; }

    protected:
        virtual void initialize(aio_callback* const cbp, const u_int32_t cache_pgsize_sblks,
                const u_int16_t cache_num_pages);
        virtual void rotate_page() = 0;
        virtual void clean();
    };

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_pmgr_hpp
