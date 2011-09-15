/**
 * \file enq_hdr.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::enq_hdr (enueue header),
 * used to start an enqueue record in the journal.
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

#ifndef mrg_journal_enq_hdr_hpp
#define mrg_journal_enq_hdr_hpp

#include <cstddef>
#include "jrnl/rec_hdr.hpp"

namespace mrg
{
namespace journal
{

#pragma pack(1)

    /**
    * \brief Struct for enqueue record.
    *
    * Struct for enqueue record. In addition to the common data, this header includes both the
    * xid and data blob sizes.
    *
    * This header precedes all enqueue data in journal files.
    *
    * Record header info in binary format (32 bytes):
    * <pre>
    *   0                           7
    * +---+---+---+---+---+---+---+---+  -+
    * |     magic     | v | e | flags |   |
    * +---+---+---+---+---+---+---+---+   | struct hdr
    * |              rid              |   |
    * +---+---+---+---+---+---+---+---+  -+
    * |            xidsize            |
    * +---+---+---+---+---+---+---+---+
    * |             dsize             |
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
    struct enq_hdr : rec_hdr
    {
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
        u_int32_t _filler0;     ///< Big-endian filler for 32-bit size_t
#endif
        std::size_t _xidsize;        ///< XID size
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
        u_int32_t _filler0;     ///< Little-endian filler for 32-bit size_t
#endif
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
        u_int32_t _filler1;     ///< Big-endian filler for 32-bit size_t
#endif
        std::size_t _dsize;          ///< Record data size
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
        u_int32_t _filler1;     ///< Little-endian filler for 32-bit size_t
#endif
        static const u_int16_t ENQ_HDR_TRANSIENT_MASK = 0x10;
        static const u_int16_t ENQ_HDR_EXTERNAL_MASK = 0x20;

        /**
        * \brief Default constructor, which sets all values to 0.
        */
        inline enq_hdr(): rec_hdr(),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
            _xidsize(0),
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler1(0),
#endif
            _dsize(0)
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            , _filler1(0)
#endif
        {}

        /**
        * \brief Convenience constructor which initializes values during construction.
        */
        inline enq_hdr(const u_int32_t magic, const u_int8_t version, const u_int64_t rid,
                const std::size_t xidsize, const std::size_t dsize, const bool owi,
                const bool transient = false): rec_hdr(magic, version, rid, owi),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
            _xidsize(xidsize),
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler1(0),
#endif
            _dsize(dsize)
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            , _filler1(0)
#endif
        { set_transient(transient); }


        inline bool is_transient() const { return _uflag & ENQ_HDR_TRANSIENT_MASK; }

        inline void set_transient(const bool transient)
        {
            _uflag = transient ? _uflag | ENQ_HDR_TRANSIENT_MASK :
                    _uflag & (~ENQ_HDR_TRANSIENT_MASK);
        }

        inline bool is_external() const { return _uflag & ENQ_HDR_EXTERNAL_MASK; }

        inline void set_external(const bool external)
        {
            _uflag = external ? _uflag | ENQ_HDR_EXTERNAL_MASK :
                    _uflag & (~ENQ_HDR_EXTERNAL_MASK);
        }

        /**
        * \brief Returns the size of the header in bytes.
        */
        inline static std::size_t size() { return sizeof(enq_hdr); }
    };

#pragma pack()

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_enq_hdr_hpp
