/**
 * \file rec_tail.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::rec_tail (record tail), used to
 * finalize a persistent record. The presence of a valid tail at the expected
 * position in the journal file indicates that the record write was completed.
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

#ifndef mrg_journal_rec_tail_hpp
#define mrg_journal_rec_tail_hpp

#include <cstddef>
#include "jrnl/jcfg.hpp"

namespace mrg
{
namespace journal
{

#pragma pack(1)

    /**
    * \brief Struct for data common to the tail of all records. The magic number
    * used here is the binary inverse (1's complement) of the magic used in the
    * record header; this minimizes possible confusion with other headers that may
    * be present during recovery. The tail is used with all records that have either
    * XIDs or data - ie any size-variable content. Currently the only records that
    * do NOT use the tail are non-transactional dequeues and filler records.
    *
    * Record header info in binary format (12 bytes):
    * <pre>
    *   0                           7
    * +---+---+---+---+---+---+---+---+
    * |   ~(magic)    |      rid      |
    * +---+---+---+---+---+---+---+---+
    * |  rid (con't)  |
    * +---+---+---+---+
    * </pre>
    */
    struct rec_tail
    {
        u_int32_t _xmagic;      ///< Binary inverse (1's complement) of hdr magic number
        u_int64_t _rid;         ///< ID (rotating 64-bit counter)


        /**
        * \brief Default constructor, which sets all values to 0.
        */
        inline rec_tail(): _xmagic(0xffffffff), _rid(0) {}

        /**
        * \brief Convenience constructor which initializes values during construction from
        *     existing enq_hdr instance.
        */
        inline rec_tail(const rec_hdr& h): _xmagic(~h._magic), _rid(h._rid) {}

        /**
        * \brief Convenience constructor which initializes values during construction.
        */
        inline rec_tail(const u_int32_t xmagic, const u_int64_t rid): _xmagic(xmagic), _rid(rid) {}

        /**
        * \brief Returns the size of the header in bytes.
        */
        inline static std::size_t size() { return sizeof(rec_tail); }
    };

#pragma pack()

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_rec_tail_hpp
