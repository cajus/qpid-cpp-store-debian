/**
 * \file txn_hdr.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::txn_hdr (transaction
 * record header), used to start a transaction (commit or abort) record.
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

#ifndef mrg_journal_txn_hdr_hpp
#define mrg_journal_txn_hdr_hpp

#include <cstddef>
#include "jrnl/rec_hdr.hpp"

namespace mrg
{
namespace journal
{

#pragma pack(1)

    /**
    * \brief Struct for transaction commit and abort records.
    *
    * Struct for DTX commit and abort records. Only the magic distinguishes between them. Since
    * this record must be used in the context of a valid XID, the xidsize field must not be zero.
    * Immediately following this record is the XID itself which is xidsize bytes long, followed by
    * a rec_tail.
    *
    * Note that this record had its own rid distinct from the rids of the record(s) making up the
    * transaction it is committing or aborting.
    *
    * Record header info in binary format (24 bytes):
    * <pre>
    *   0                           7
    * +---+---+---+---+---+---+---+---+  -+
    * |     magic     | v | e | flags |   |
    * +---+---+---+---+---+---+---+---+   | struct hdr
    * |              rid              |   |
    * +---+---+---+---+---+---+---+---+  -+
    * |            xidsize            |
    * +---+---+---+---+---+---+---+---+
    * v = file version (If the format or encoding of this file changes, then this
    *     number should be incremented)
    * e = endian flag, false (0x00) for little endian, true (0x01) for big endian
    * </pre>
    *
    * Note that journal files should be transferable between 32- and 64-bit
    * hardware of the same endianness, but not between hardware of opposite
    * entianness without some sort of binary conversion utility. Thus buffering
    * will be needed for types that change size between 32- and 64-bit compiles.
    */
    struct txn_hdr : rec_hdr
    {
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
        u_int32_t _filler0;     ///< Big-endian filler for 32-bit size_t
#endif
        std::size_t _xidsize;        ///< XID size
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
        u_int32_t _filler0;     ///< Little-endian filler for 32-bit size_t
#endif

        /**
        * \brief Default constructor, which sets all values to 0.
        */
        txn_hdr(): rec_hdr(),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
            _xidsize(0)
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            , _filler0(0)
#endif
        {}

        /**
        * \brief Convenience constructor which initializes values during construction.
        */
        txn_hdr(const u_int32_t magic, const u_int8_t version, const u_int64_t rid,
                const std::size_t xidsize, const bool owi): rec_hdr(magic, version, rid, owi),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
            _xidsize(xidsize)
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            , _filler0(0)
#endif
        {}

        /**
        * \brief Returns the size of the header in bytes.
        */
        inline static std::size_t size() { return sizeof(txn_hdr); }
    };

#pragma pack()

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_txn_hdr_hpp
