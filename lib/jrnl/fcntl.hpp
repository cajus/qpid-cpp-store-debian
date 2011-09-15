/**
 * \file fcntl.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::fcntl (non-logging file
 * handle), used for controlling journal log files. See class documentation for
 * details.
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

#ifndef mrg_journal_fcntl_hpp
#define mrg_journal_fcntl_hpp

namespace mrg
{
namespace journal
{
class fcntl;
}
}

#include <cstddef>
#include <string>
#include "jrnl/rcvdat.hpp"
#include <sys/types.h>

namespace mrg
{
namespace journal
{

    /**
    * \class fcntl
    * \brief Journal file controller. There is one instance per journal file.
    */
    class fcntl
    {
    protected:
        std::string _fname;             ///< File name
        u_int16_t _pfid;                ///< Physical file ID (file number in order of creation)
        u_int16_t _lfid;                ///< Logical file ID (ordinal number in ring store)
        const u_int32_t _ffull_dblks;   ///< File size in dblks (incl. file header)
        int _wr_fh;                     ///< Write file handle
        u_int32_t _rec_enqcnt;          ///< Count of enqueued records
        u_int32_t _rd_subm_cnt_dblks;   ///< Read file count (data blocks) for submitted AIO
        u_int32_t _rd_cmpl_cnt_dblks;   ///< Read file count (data blocks) for completed AIO
        u_int32_t _wr_subm_cnt_dblks;   ///< Write file count (data blocks) for submitted AIO
        u_int32_t _wr_cmpl_cnt_dblks;   ///< Write file count (data blocks) for completed AIO
        u_int16_t _aio_cnt;             ///< Outstanding AIO operations on this file
        bool _fhdr_wr_aio_outstanding;  ///< Outstanding file header write on this file

    public:
        // Constructors with implicit initialize() and open()
        fcntl(const std::string& fbasename, const u_int16_t pfid, const u_int16_t lfid, const u_int32_t jfsize_sblks,
                const rcvdat* const ro);
        virtual ~fcntl();

        virtual bool reset(const rcvdat* const ro = 0);
        virtual void rd_reset();
        virtual bool wr_reset(const rcvdat* const ro = 0);

        virtual int open_wr_fh();
        virtual void close_wr_fh();
        inline bool is_wr_fh_open() const { return _wr_fh >= 0; }

        inline const std::string& fname() const { return _fname; }
        inline u_int16_t pfid() const { return _pfid; }
        inline u_int16_t lfid() const { return _lfid; }
        inline void set_lfid(const u_int16_t lfid) { _lfid = lfid; }
        inline int wr_fh() const { return _wr_fh; }
        inline u_int32_t enqcnt() const { return _rec_enqcnt; }
        inline u_int32_t incr_enqcnt() { return ++_rec_enqcnt; }
        u_int32_t add_enqcnt(u_int32_t a);
        u_int32_t decr_enqcnt();
        u_int32_t subtr_enqcnt(u_int32_t s);

        inline u_int32_t rd_subm_cnt_dblks() const { return _rd_subm_cnt_dblks; }
        inline std::size_t rd_subm_offs() const { return _rd_subm_cnt_dblks * JRNL_DBLK_SIZE; }
        u_int32_t add_rd_subm_cnt_dblks(u_int32_t a);

        inline u_int32_t rd_cmpl_cnt_dblks() const { return _rd_cmpl_cnt_dblks; }
        inline std::size_t rd_cmpl_offs() const { return _rd_cmpl_cnt_dblks * JRNL_DBLK_SIZE; }
        u_int32_t add_rd_cmpl_cnt_dblks(u_int32_t a);

        inline u_int32_t wr_subm_cnt_dblks() const { return _wr_subm_cnt_dblks; }
        inline std::size_t wr_subm_offs() const { return _wr_subm_cnt_dblks * JRNL_DBLK_SIZE; }
        u_int32_t add_wr_subm_cnt_dblks(u_int32_t a);

        inline u_int32_t wr_cmpl_cnt_dblks() const { return _wr_cmpl_cnt_dblks; }
        inline std::size_t wr_cmpl_offs() const { return _wr_cmpl_cnt_dblks * JRNL_DBLK_SIZE; }
        u_int32_t add_wr_cmpl_cnt_dblks(u_int32_t a);

        inline u_int16_t aio_cnt() const { return _aio_cnt; }
        inline u_int16_t incr_aio_cnt() { return ++_aio_cnt; }
        u_int16_t decr_aio_cnt();

        inline bool wr_fhdr_aio_outstanding() { return _fhdr_wr_aio_outstanding; }
        inline void set_wr_fhdr_aio_outstanding(const bool wfao) { _fhdr_wr_aio_outstanding = wfao; }

        // Derived helper functions

        inline bool rd_void() const { return _wr_cmpl_cnt_dblks == 0; }
        inline bool rd_empty() const { return _wr_cmpl_cnt_dblks <= JRNL_SBLK_SIZE; }
        inline u_int32_t rd_remaining_dblks() const { return _wr_cmpl_cnt_dblks - _rd_subm_cnt_dblks; }
        inline bool is_rd_full() const { return _wr_cmpl_cnt_dblks == _rd_subm_cnt_dblks; }
        inline bool is_rd_compl() const { return _wr_cmpl_cnt_dblks == _rd_cmpl_cnt_dblks; }
        inline u_int32_t rd_aio_outstanding_dblks() const { return _rd_subm_cnt_dblks - _rd_cmpl_cnt_dblks; }
        inline bool rd_file_rotate() const { return is_rd_full() && is_wr_compl(); }

        inline bool wr_void() const { return _wr_subm_cnt_dblks == 0; }
        inline bool wr_empty() const { return _wr_subm_cnt_dblks <= JRNL_SBLK_SIZE; }
        inline u_int32_t wr_remaining_dblks() const { return _ffull_dblks - _wr_subm_cnt_dblks; }
        inline bool is_wr_full() const { return _ffull_dblks == _wr_subm_cnt_dblks; }
        inline bool is_wr_compl() const { return _ffull_dblks == _wr_cmpl_cnt_dblks; }
        inline u_int32_t wr_aio_outstanding_dblks() const { return _wr_subm_cnt_dblks - _wr_cmpl_cnt_dblks; }
        inline bool wr_file_rotate() const { return is_wr_full(); }

        // Debug aid
        const std::string status_str() const;

    protected:
        virtual void initialize(const std::string& fbasename, const u_int16_t pfid, const u_int16_t lfid,
                const u_int32_t jfsize_sblks, const rcvdat* const ro);

        static std::string filename(const std::string& fbasename, const u_int16_t pfid);
        void clean_file(const u_int32_t jfsize_sblks);
        void create_jfile(const u_int32_t jfsize_sblks);
    };

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_fcntl_hpp
