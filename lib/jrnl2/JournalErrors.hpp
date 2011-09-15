/**
 * \file JournalErrors.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * This file contains async journal code (v.2).
 *
 * \author Kim van der Riet
 *
 * Copyright (c) 2010 Red Hat, Inc.
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

#ifndef mrg_journal2_JournalErrors_hpp
#define mrg_journal2_JournalErrors_hpp

#include <map>
#include <stdint.h> // uint32_t

namespace mrg
{
namespace journal2
{

    class JournalErrors
    {
        static std::map<uint32_t, const char*> _s_errorMap;     ///< Map of error messages
        static std::map<uint32_t, const char*>::iterator _s_errorMapIterator; ///< Iterator
        static bool _s_initializedFlag;                         ///< Dummy flag, used to initialize map.
        static bool _s_initialize();                            ///< Static fn for initializing static data
    public:
        // generic errors
        static const uint32_t JERR_PTHREAD = 0x0001;            ///< pthread operation failure
        static const uint32_t JERR_RTCLOCK = 0x0002;            ///< realtime clock operation failure

        // illegal states
        static const uint32_t JERR_BADJRNLSTATE = 0x0101;       ///< Illegal journal state
        static const uint32_t JERR_BADDTOKOPSTATE = 0x0102;     ///< Illegal data token op state
        static const uint32_t JERR_BADDTOKTXNSTATE = 0x0103;    ///< Illegal data token txn state
        static const uint32_t JERR_BADDTOKIOSTATE = 0x0104;     ///< Illegal data token io state


        static const char* s_errorMessage(const uint32_t err_no) throw ();
    };

} // namespace journal2
} // namespace mrg

#endif // mrg_journal2_JournalErrors_hpp
