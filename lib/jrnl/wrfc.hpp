/**
 * \file wrfc.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::wrfc (write rotating
 * file controller). See class documentation for details.
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

#ifndef mrg_journal_wrfc_hpp
#define mrg_journal_wrfc_hpp

namespace mrg
{
namespace journal
{
class wrfc;
}
}

#include <cstddef>
#include "jrnl/enums.hpp"
#include "jrnl/rrfc.hpp"

namespace mrg
{
namespace journal
{

    /**
    * \class wrfc
    * \brief Class to handle write management of a journal rotating file controller.
    */
    class wrfc : public rfc
    {
    private:
        u_int32_t _fsize_sblks;         ///< Size of journal files in sblks
        u_int32_t _fsize_dblks;         ///< Size of journal files in dblks
        u_int32_t _enq_cap_offs_dblks;  ///< Enqueue capacity offset
        u_int64_t _rid;                 ///< Master counter for record ID (rid)
        bool _reset_ok;                 ///< Flag set when reset succeeds
        bool _owi;                      ///< Overwrite indicator
        bool _frot;                     ///< Flag is true for first rotation, false otherwise

    public:
        wrfc(const lpmgr* lpmp);
        virtual ~wrfc();

        /**
        * \brief Initialize the controller.
        * \param fsize_sblks Size of each journal file in sblks.
        * \param rdp Struct carrying restore information. Optional for non-restore use, defaults to 0 (NULL).
        */
        void initialize(const u_int32_t fsize_sblks, rcvdat* rdp = 0);

        /**
        * \brief Rotate active file controller to next file in rotating file group.
        * \exception jerrno::JERR__NINIT if called before calling initialize().
        */
        iores rotate();

        /**
        * \brief Returns the index of the earliest complete file within the rotating
        *     file group. Unwritten files are excluded. The currently active file is
        *     excluded unless it is the only written file.
        */
        u_int16_t earliest_index() const;

        /**
        * \brief Determines if a proposed write would cause the enqueue threshold to be exceeded.
        *
        * The following routine finds whether the next write will take the write pointer to beyond the
        * enqueue limit threshold. The following illustrates how this is achieved.
        * <pre>
        * Current file index: 4                         +---+----------+
        * X's mark still-enqueued records               |msg| 1-thresh |
        * msg = current msg size + unwritten cache      +---+----------+
        * thresh = JRNL_ENQ_THRESHOLD as a fraction     ^              V
        *            +-------+-------+-------+-------+--+----+-------+-+-----+-------+
        * file num ->|   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |
        * enq recs ->| X  XX |XX XXX |XX XXXX|XXXXXXX|XX     |       |       |     X |
        *            +-------+-------+-------+-------+--+----+-------+-+-----+-------+
        *                                               ^        ^       ^
        *                                  subm_dblks --+        |       |
        *                                                      These files must be free of enqueues
        *                                                      If not, return true.
        * </pre>
        * \param enq_dsize_dblks Proposed size of write in dblocks
        */
        bool enq_threshold(const u_int32_t enq_dsize_dblks) const;

        inline u_int64_t rid() const { return _rid; }
        inline u_int64_t get_incr_rid() { return _rid++; }
        bool wr_reset();
        inline bool is_wr_reset() const { return _reset_ok; }
        inline bool owi() const { return _owi; }
        inline bool frot() const { return _frot; }

        // Convenience access methods to current file controller

        inline int fh() const { return _curr_fc->wr_fh(); }

        inline u_int32_t subm_cnt_dblks() const { return _curr_fc->wr_subm_cnt_dblks(); }
        inline std::size_t subm_offs() const { return _curr_fc->wr_subm_offs(); }
        inline u_int32_t add_subm_cnt_dblks(u_int32_t a) { return _curr_fc->add_wr_subm_cnt_dblks(a); }

        inline u_int32_t cmpl_cnt_dblks() const { return _curr_fc->wr_cmpl_cnt_dblks(); }
        inline std::size_t cmpl_offs() const { return _curr_fc->wr_cmpl_offs(); }
        inline u_int32_t add_cmpl_cnt_dblks(u_int32_t a) { return _curr_fc->add_wr_cmpl_cnt_dblks(a); }

        inline u_int16_t aio_cnt() const { return _curr_fc->aio_cnt(); }
        inline u_int16_t incr_aio_cnt() { return _curr_fc->incr_aio_cnt(); }
        inline u_int16_t decr_aio_cnt() { return _curr_fc->decr_aio_cnt(); }

        inline bool is_void() const { return _curr_fc->wr_void(); }
        inline bool is_empty() const { return _curr_fc->wr_empty(); }
        inline u_int32_t remaining_dblks() const { return _curr_fc->wr_remaining_dblks(); }
        inline bool is_full() const { return _curr_fc->is_wr_full(); };
        inline bool is_compl() const { return _curr_fc->is_wr_compl(); };
        inline u_int32_t aio_outstanding_dblks() const { return _curr_fc->wr_aio_outstanding_dblks(); }
        inline bool file_rotate() const { return _curr_fc->wr_file_rotate(); }

        // Debug aid
        std::string status_str() const;
    };

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_wrfc_hpp
