/**
 * \file rec_hdr.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::rec_hdr (record header),
 * which is a common initial header used for all journal record structures
 * except the record tail (rec_tail).
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

#ifndef mrg_journal_rec_hdr_hpp
#define mrg_journal_rec_hdr_hpp

#include <cstddef>
#include "jrnl/jcfg.hpp"
#include <sys/types.h>

namespace mrg
{
namespace journal
{

#pragma pack(1)

    /**
    * \brief Struct for data common to the head of all journal files and records.
    * This includes identification for the file type, the encoding version, endian
    * indicator and a record ID.
    *
    * File header info in binary format (16 bytes):
    * <pre>
    *   0                           7
    * +---+---+---+---+---+---+---+---+
    * |     magic     | v | e | flags |
    * +---+---+---+---+---+---+---+---+
    * |              rid              |
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
    struct rec_hdr
    {
        u_int32_t _magic;       ///< File type identifier (magic number)
        u_int8_t _version;      ///< File encoding version
        u_int8_t _eflag;        ///< Flag for determining endianness
        u_int16_t _uflag;       ///< User-defined flags
        u_int64_t _rid;         ///< Record ID (rotating 64-bit counter)

        // Global flags
        static const u_int16_t HDR_OVERWRITE_INDICATOR_MASK = 0x1;

        // Convenience constructors and methods
        /**
        * \brief Default constructor, which sets all values to 0.
        */
        inline rec_hdr(): _magic(0), _version(0), _eflag(0), _uflag(0), _rid(0) {}

        /**
        * \brief Convenience constructor which initializes values during construction.
        */
        inline rec_hdr(const u_int32_t magic, const u_int8_t version, const u_int64_t rid,
                const bool owi): _magic(magic), _version(version),
#if defined(JRNL_BIG_ENDIAN)
            _eflag(RHM_BENDIAN_FLAG),
#else
            _eflag(RHM_LENDIAN_FLAG),
#endif
            _uflag(owi ? HDR_OVERWRITE_INDICATOR_MASK : 0), _rid(rid) {}

        /**
        * \brief Convenience copy method.
        */
        inline void hdr_copy(const rec_hdr& h)
        {
            _magic = h._magic;
            _version = h._version;
            _eflag = h._eflag;
            _uflag = h._uflag;
            _rid =h._rid;
        }

        /**
        * \brief Resets all fields to 0
        */
        inline void reset()
        {
            _magic = 0;
            _version = 0;
            _eflag = 0;
            _uflag = 0;
            _rid = 0;
        }

        inline bool get_owi() const { return _uflag & HDR_OVERWRITE_INDICATOR_MASK; }

        inline void set_owi(const bool owi)
        {
            _uflag = owi ? _uflag | HDR_OVERWRITE_INDICATOR_MASK :
                    _uflag & (~HDR_OVERWRITE_INDICATOR_MASK);
        }

        /**
        * \brief Returns the size of the header in bytes.
        */
        inline static std::size_t size() { return sizeof(rec_hdr); }
    }; // struct rec_hdr

#pragma pack()

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_rec_hdr_hpp
