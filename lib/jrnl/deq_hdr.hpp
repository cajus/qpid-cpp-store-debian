/**
 * \file deq_hdr.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::deq_hdr (dequeue record),
 * used to dequeue a previously enqueued record.
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

#ifndef mrg_journal_deq_hdr_hpp
#define mrg_journal_deq_hdr_hpp

#include <cstddef>
#include "jrnl/rec_hdr.hpp"

namespace mrg
{
namespace journal
{

#pragma pack(1)

    /**
    * \brief Struct for dequeue record.
    *
    * Struct for dequeue record. If this record has a non-zero xidsize field (i.e., there is a
    * valid XID), then this header is followed by the XID of xidsize bytes and a rec_tail. If,
    * on the other hand, this record has a zero xidsize (i.e., there is no XID), then the rec_tail
    * is absent.
    *
    * Note that this record had its own rid distinct from the rid of the record it is dequeueing.
    * The rid field below is the rid of the dequeue record itself; the deq-rid field is the rid of a
    * previous enqueue record being dequeued by this record.
    *
    * Record header info in binary format (32 bytes):
    * <pre>
    *   0                           7
    * +---+---+---+---+---+---+---+---+  -+
    * |     magic     | v | e | flags |   |
    * +---+---+---+---+---+---+---+---+   | struct hdr
    * |              rid              |   |
    * +---+---+---+---+---+---+---+---+  -+
    * |            deq-rid            |
    * +---+---+---+---+---+---+---+---+
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
    struct deq_hdr : rec_hdr
    {
        u_int64_t _deq_rid;     ///< Record ID of dequeued record
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
        u_int32_t _filler0;     ///< Big-endian filler for 32-bit size_t
#endif
        std::size_t _xidsize;   ///< XID size
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
        u_int32_t _filler0;     ///< Little-endian filler for 32-bit size_t
#endif
        static const u_int16_t DEQ_HDR_TXNCMPLCOMMIT_MASK = 0x10;

        /**
        * \brief Default constructor, which sets all values to 0.
        */
        inline deq_hdr(): rec_hdr(), _deq_rid(0),
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
        inline deq_hdr(const u_int32_t magic, const u_int8_t version, const u_int64_t rid,
                const u_int64_t deq_rid, const std::size_t xidsize, const bool owi,
                const bool txn_coml_commit = false):
                rec_hdr(magic, version, rid, owi), _deq_rid(deq_rid),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
            _xidsize(xidsize)
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            , _filler0(0)
#endif
        { set_txn_coml_commit(txn_coml_commit); }


        inline bool is_txn_coml_commit() const { return _uflag & DEQ_HDR_TXNCMPLCOMMIT_MASK; }

        inline void set_txn_coml_commit(const bool commit)
        {
            _uflag = commit ? _uflag | DEQ_HDR_TXNCMPLCOMMIT_MASK :
                    _uflag & (~DEQ_HDR_TXNCMPLCOMMIT_MASK);
        }

        /**
        * \brief Returns the size of the header in bytes.
        */
        inline static std::size_t size() { return sizeof(deq_hdr); }
    };

#pragma pack()

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_deq_hdr_hpp
