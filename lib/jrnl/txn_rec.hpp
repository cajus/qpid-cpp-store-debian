/**
 * \file txn_rec.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * This file contains the code for the mrg::journal::txn_rec (journal data
 * record) class. See class documentation for details.
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

#ifndef mrg_journal_txn_rec_hpp
#define mrg_journal_txn_rec_hpp

namespace mrg
{
namespace journal
{
class txn_rec;
}
}

#include <cstddef>
#include "jrnl/jrec.hpp"
#include "jrnl/txn_hdr.hpp"

namespace mrg
{
namespace journal
{

    /**
    * \class txn_rec
    * \brief Class to handle a single journal DTX commit or abort record.
    */
    class txn_rec : public jrec
    {
    private:
        txn_hdr _txn_hdr;       ///< transaction header
        const void* _xidp;      ///< xid pointer for encoding (writing to disk)
        void* _buff;            ///< Pointer to buffer to receive data read from disk
        rec_tail _txn_tail;     ///< Record tail

    public:
        // constructor used for read operations and xid must have memory allocated
        txn_rec();
        // constructor used for write operations, where xid already exists
        txn_rec(const u_int32_t magic, const u_int64_t rid, const void* const xidp,
                const std::size_t xidlen, const bool owi);
        virtual ~txn_rec();

        // Prepare instance for use in reading data from journal
        void reset(const u_int32_t magic);
        // Prepare instance for use in writing data to journal
        void reset(const u_int32_t magic, const  u_int64_t rid, const void* const xidp,
                const std::size_t xidlen, const bool owi);
        u_int32_t encode(void* wptr, u_int32_t rec_offs_dblks, u_int32_t max_size_dblks);
        u_int32_t decode(rec_hdr& h, void* rptr, u_int32_t rec_offs_dblks,
                u_int32_t max_size_dblks);
        // Decode used for recover
        bool rcv_decode(rec_hdr h, std::ifstream* ifsp, std::size_t& rec_offs);

        std::size_t get_xid(void** const xidpp);
        std::string& str(std::string& str) const;
        inline std::size_t data_size() const { return 0; } // This record never carries data
        std::size_t xid_size() const;
        std::size_t rec_size() const;
        inline u_int64_t rid() const { return _txn_hdr._rid; }

    private:
        void chk_hdr() const;
        void chk_hdr(u_int64_t rid) const;
        void chk_tail() const;
        virtual void clean();
    }; // class txn_rec

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_txn_rec_hpp
