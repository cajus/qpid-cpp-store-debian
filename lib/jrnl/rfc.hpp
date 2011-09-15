/**
 * \file rfc.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::rfc (rotating
 * file controller). See class documentation for details.
 *
 * \author Kim van der Riet
 *
 * Copyright (c) 2008, 2009 Red Hat, Inc.
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

#ifndef mrg_journal_rfc_hpp
#define mrg_journal_rfc_hpp

namespace mrg
{
namespace journal
{
class rfc;
}
}

#include "jrnl/lpmgr.hpp"
#include "jrnl/enums.hpp"

namespace mrg
{
namespace journal
{

    /**
    * \class rfc
    * \brief Rotating File Controller (rfc) - Class to handle the manangement of an array of file controllers (fcntl)
    *     objects for use in a circular disk buffer (journal). Each fcntl object corresponds to a file in the journal.
    *
    * The following states exist in this class:
    *
    * <pre>
    *                                                                   is_init()  is_active()
    *                  +===+                    _lpmp.is_init() == false
    *      +---------->|   |     Uninitialized: _curr_fc == 0               F           F
    *      |       +-->+===+ --+
    *      |       |           |
    *      |       |           |
    *      |   finalize()   initialize()
    *      |       |           |
    *      |       |           |
    *      |       +-- +===+<--+                _lpmp.is_init() == true
    *  finalize()      |   |     Inactive:      _curr_fc == 0               T           F
    *      |       +-->+===+ --+
    *      |       |           |
    *      |       |           |
    *      | unset_findex() set_findex()
    *      |       |           |
    *      |       |           |
    *      |       +-- +===+<--+                _lpmp.is_init() == true
    *      +---------- |   |     Active:        _curr_fc != 0               T           T
    *                  +===+
    * </pre>
    *
    * The Uninitialized state is where the class starts after construction. Once the number of files is known and
    * the array of file controllers allocated, then initialize() is called to set these, causing the state to move
    * to Inactive.
    *
    * The Inactive state has the file controllers allocated and pointing to their respective journal files, but no
    * current file controller has been selected. The pointer to the current file controller _curr_fc is null. Once the
    * index of the active file is known, then calling set_findex() will set the index and internal pointer
    * to the currently active file controller. This moves the state to Active.
    *
    * Note TODO: Comment on sync issues between change in num files in _lpmp and _fc_index/_curr_fc.
    */
    class rfc
    {
    protected:
        const lpmgr* _lpmp;     ///< Pointer to jcntl's lpmgr instance containing lfid/pfid map and fcntl objects
        u_int16_t _fc_index;    ///< Index of current file controller
        fcntl*    _curr_fc;     ///< Pointer to current file controller

    public:
        rfc(const lpmgr* lpmp);
        virtual ~rfc();

        /**
        * \brief Initialize the controller, moving from state Uninitialized to Inactive. The main function of
        *     initialize() is to set the number of files and the pointer to the fcntl array.
        */
        virtual inline void initialize() {}

        /**
        * \brief Reset the controller to Uninitialized state, usually called when the journal is stopped. Once called,
        *     initialize() must be called to reuse an instance.
        */
        virtual void finalize();

        /**
        * \brief Check initialization state: true = Not Uninitialized, ie Initialized or Active; false = Uninitialized.
        */
        virtual inline bool is_init() const { return _lpmp->is_init(); }

        /**
        * \brief Check active state: true = Initialized and _curr_fc not null; false otherwise.
        */
        virtual inline bool is_active() const { return _lpmp->is_init() && _curr_fc != 0; }

        /**
        * \brief Sets the current file index and active fcntl object. Moves to state Active.
        */
        virtual void set_findex(const u_int16_t fc_index);

        /**
        * \brief Nulls the current file index and active fcntl pointer, moves to state Inactive.
        */
        virtual void unset_findex();

        /**
        * \brief Rotate active file controller to next file in rotating file group.
        * \exception jerrno::JERR__NINIT if called before calling initialize().
        */
        virtual iores rotate() = 0;

        /**
        * \brief Returns the index of the currently active file within the rotating file group.
        */
        inline u_int16_t index() const { return _fc_index; }

        /**
        * \brief Returns the currently active journal file controller within the rotating file group.
        */
        inline fcntl* file_controller() const { return _curr_fc; }

        /**
        * \brief Returns the currently active physical file id (pfid)
        */
        inline u_int16_t pfid() const { return _curr_fc->pfid(); }

        // Convenience access methods to current file controller
        // Note: Do not call when not in active state

        inline u_int32_t enqcnt() const { return _curr_fc->enqcnt(); }
        inline u_int32_t incr_enqcnt() { return _curr_fc->incr_enqcnt(); }
        inline u_int32_t incr_enqcnt(const u_int16_t fid) { return _lpmp->get_fcntlp(fid)->incr_enqcnt(); }
        inline u_int32_t add_enqcnt(const u_int32_t a) { return _curr_fc->add_enqcnt(a); }
        inline u_int32_t add_enqcnt(const u_int16_t fid, const u_int32_t a)
                { return _lpmp->get_fcntlp(fid)->add_enqcnt(a); }
        inline u_int32_t decr_enqcnt(const u_int16_t fid) { return _lpmp->get_fcntlp(fid)->decr_enqcnt(); }
        inline u_int32_t subtr_enqcnt(const u_int16_t fid, const u_int32_t s)
                { return _lpmp->get_fcntlp(fid)->subtr_enqcnt(s); }

        virtual inline u_int32_t subm_cnt_dblks() const = 0;
        virtual inline std::size_t subm_offs() const = 0;
        virtual inline u_int32_t add_subm_cnt_dblks(u_int32_t a) = 0;

        virtual inline u_int32_t cmpl_cnt_dblks() const = 0;
        virtual inline std::size_t cmpl_offs() const = 0;
        virtual inline u_int32_t add_cmpl_cnt_dblks(u_int32_t a) = 0;

        virtual inline bool is_void() const = 0;
        virtual inline bool is_empty() const = 0;
        virtual inline u_int32_t remaining_dblks() const = 0;
        virtual inline bool is_full() const = 0;
        virtual inline bool is_compl() const = 0;
        virtual inline u_int32_t aio_outstanding_dblks() const = 0;
        virtual inline bool file_rotate() const = 0;

        // Debug aid
        virtual std::string status_str() const;
    }; // class rfc

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_rfc_hpp
