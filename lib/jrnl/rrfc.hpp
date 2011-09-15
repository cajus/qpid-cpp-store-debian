/**
 * \file rrfc.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::rrfc (rotating
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

#ifndef mrg_journal_rrfc_hpp
#define mrg_journal_rrfc_hpp

namespace mrg
{
namespace journal
{
class rrfc;
}
}

#include "jrnl/fcntl.hpp"
#include "jrnl/rfc.hpp"

namespace mrg
{
namespace journal
{

    /**
    * \class rrfc
    * \brief Read Rotating File Controller (rrfc) - Subclassed from pure virtual class rfc. Used to control the read
    *     pipeline in a rotating file buffer or journal. See class rfc for further details.
    *
    * The states that exist in this class are identical to class rfc from which it inherits, but in addition, the value
    * of the read file handle _fh is also considered. The calls to set_findex also opens the file handle _fh to the
    * active file for reading. Similarly, unset_findex() closes this file handle.
    *
    * <pre>
    *                                                                   is_init()  is_active()
    *                  +===+                    _lpmp.is_init() == false
    *      +---------->|   |     Uninitialized: _curr_fc == 0               F           F
    *      |       +-->+===+ --+                _fh == -1
    *      |       |           |
    *      |       |           |
    *      |   finalize()   initialize()
    *      |       |           |
    *      |       |           |
    *      |       +-- +===+<--+                _lpmp.is_init() == true
    *  finalize()      |   |     Inactive:      _curr_fc == 0               T           F
    *      |       +-->+===+ --+                _fh == -1
    *      |       |           |
    *      |       |           |
    *      | unset_findex() set_findex()
    *      |       |           |
    *      |       |           |
    *      |       +-- +===+<--+                _lpmp.is_init() == true
    *      +---------- |   |     Active:        _curr_fc != 0               T           T
    *                  +===+                    _fh >= 0
    * </pre>
    *
    * In adition to the states above, class rrfc contains a validity flag. This is operated indepenedently of the state
    * machine. This flag (_valid) indicates when the read buffers are valid for reading. This is not strictly a state,
    * but simply a flag used to keep track of the status, and is set/unset with calls to set_valid() and set_invalid()
    * respectively.
    */
    class rrfc : public rfc
    {
    protected:
        int _fh;                ///< Read file handle
        bool _valid;            ///< Flag is true when read pages contain vailid data

    public:
        rrfc(const lpmgr* lpmp);
        virtual ~rrfc();

        /**
        * \brief Initialize the controller, moving from state Uninitialized to Initialized. The main function of
        *     initialize() is to set the number of files and the pointer to the fcntl array.
        */
        inline void initialize() { rfc::initialize(); _valid = false; }

        /**
        * \brief Reset the controller to Uninitialized state, usually called when the journal is stopped. Once called,
        *     initialize() must be called to reuse an instance.
        */
        void finalize();

        /**
        * \brief Opens the file handle for reading a particular fid. Moves to state open.
        */
        void set_findex(const u_int16_t fc_index);

        /**
        * \brief Closes the read file handle and nulls the active fcntl pointer. Moves to state closed.
        */
        void unset_findex();

        /**
        * \brief Check the state: true = open; false = closed.
        */
        inline bool is_active() const { return _curr_fc != 0 && _fh >= 0; }

        /**
        * \brief Sets the validity flag which indicates that the read buffers contain valid data for reading.
        */
        inline void set_invalid() { _valid = false; }

        /**
        * \brief Resets the validity flag wich indicates that the read buffers are no longer synchronized and cannot
        *     be read whithout resynchronization.
        */
        inline void set_valid() { _valid = true; }

        /**
        * \brief Checks the read buffer validity status: true = valid, can be read; false = invalid, synchronization
        *     required.
        */
        inline bool is_valid() const { return _valid; }

        /**
        * \brief Rotate active file controller to next file in rotating file group.
        * \exception jerrno::JERR__NINIT if called before calling initialize().
        */
        iores rotate();

        inline int fh() const { return _fh; }

        inline u_int32_t subm_cnt_dblks() const { return _curr_fc->rd_subm_cnt_dblks(); }
        inline std::size_t subm_offs() const { return _curr_fc->rd_subm_offs(); }
        inline u_int32_t add_subm_cnt_dblks(u_int32_t a) { return _curr_fc->add_rd_subm_cnt_dblks(a); }

        inline u_int32_t cmpl_cnt_dblks() const { return _curr_fc->rd_cmpl_cnt_dblks(); }
        inline std::size_t cmpl_offs() const { return _curr_fc->rd_cmpl_offs(); }
        inline u_int32_t add_cmpl_cnt_dblks(u_int32_t a) { return _curr_fc->add_rd_cmpl_cnt_dblks(a); }

        inline bool is_void() const { return _curr_fc->rd_void(); }
        inline bool is_empty() const { return _curr_fc->rd_empty(); }
        inline u_int32_t remaining_dblks() const { return _curr_fc->rd_remaining_dblks(); }
        inline bool is_full() const { return _curr_fc->is_rd_full(); }
        inline bool is_compl() const { return _curr_fc->is_rd_compl(); }
        inline u_int32_t aio_outstanding_dblks() const { return _curr_fc->rd_aio_outstanding_dblks(); }
        inline bool file_rotate() const { return _curr_fc->rd_file_rotate(); }
        inline bool is_wr_aio_outstanding() const { return _curr_fc->wr_aio_outstanding_dblks() > 0; }

        // Debug aid
        std::string status_str() const;

    protected:
        void open_fh(const std::string& fn);
        void close_fh();
    }; // class rrfc

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_rrfc_hpp
