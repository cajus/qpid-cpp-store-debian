/**
 * \file rmgr.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::rmgr (read manager). See
 * class documentation for details.
 *
 * \author Kim van der Riet
 *
 * Copyright (c) 2007, 2008, 2009 Red Hat, Inc.
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

#ifndef mrg_journal_rmgr_hpp
#define mrg_journal_rmgr_hpp

namespace mrg
{
namespace journal
{
class rmgr;
}
}

#include <cstring>
#include "jrnl/enums.hpp"
#include "jrnl/file_hdr.hpp"
#include "jrnl/pmgr.hpp"
#include "jrnl/rec_hdr.hpp"
#include "jrnl/rrfc.hpp"

namespace mrg
{
namespace journal
{

    /**
    * \brief Class for managing a read page cache of arbitrary size and number of pages.
    *
    * The read page cache works on the principle of filling as many pages as possilbe in advance of
    * reading the data. This ensures that delays caused by AIO operations are minimized.
    */
    class rmgr : public pmgr
    {
    private:
        rrfc& _rrfc;                ///< Ref to read rotating file controller
        rec_hdr _hdr;               ///< Header used to determind record type

        void* _fhdr_buffer;         ///< Buffer used for fhdr reads
        aio_cb* _fhdr_aio_cb_ptr;   ///< iocb pointer for fhdr reads
        file_hdr _fhdr;             ///< file header instance for reading file headers
        bool _fhdr_rd_outstanding;  ///< true if a fhdr read is outstanding

    public:
        rmgr(jcntl* jc, enq_map& emap, txn_map& tmap, rrfc& rrfc);
        virtual ~rmgr();

        void initialize(aio_callback* const cbp);
        iores read(void** const datapp, std::size_t& dsize, void** const xidpp,
                std::size_t& xidsize, bool& transient, bool& external, data_tok* dtokp,
                bool ignore_pending_txns);
        int32_t get_events(page_state state, timespec* const timeout, bool flush = false);
        void recover_complete();
        inline iores synchronize() { if (_rrfc.is_valid()) return RHM_IORES_SUCCESS; return aio_cycle(); }
        void invalidate();
        bool wait_for_validity(timespec* const timeout, const bool throw_on_timeout = false);

        /* TODO (if required)
        const iores get(const u_int64_t& rid, const std::size_t& dsize, const std::size_t& dsize_avail,
                const void** const data, bool auto_discard);
        const iores discard(data_tok* dtok);
        */

    private:
        void clean();
        void flush(timespec* timeout);
        iores pre_read_check(data_tok* dtokp);
        iores read_enq(rec_hdr& h, void* rptr, data_tok* dtokp);
        void consume_xid_rec(rec_hdr& h, void* rptr, data_tok* dtokp);
        void consume_filler();
        iores skip(data_tok* dtokp);
        iores aio_cycle();
        iores init_aio_reads(const int16_t first_uninit, const u_int16_t num_uninit);
        void rotate_page();
        u_int32_t dblks_rem() const;
        void set_params_null(void** const datapp, std::size_t& dsize, void** const xidpp,
                std::size_t& xidsize);
        void init_file_header_read();
    };

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_rmgr_hpp
