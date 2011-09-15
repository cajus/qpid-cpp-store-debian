/**
 * \file JournalErrors.cpp
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

#include "JournalErrors.hpp"

namespace mrg
{
namespace journal2
{

    std::map<uint32_t, const char*> JournalErrors::_s_errorMap;
    std::map<uint32_t, const char*>::iterator JournalErrors::_s_errorMapIterator;
    bool JournalErrors::_s_initializedFlag = JournalErrors::_s_initialize();

    bool
    JournalErrors::_s_initialize()
    {
        _s_errorMap[JERR_PTHREAD] = "JERR_PTHREAD: pthread operation failure";
        _s_errorMap[JERR_RTCLOCK] = "JERR_RTCLOCK: realtime clock operation failure";
        _s_errorMap[JERR_BADJRNLSTATE] = "JERR_BADJRNLSTATE: Illegal journal state";
        _s_errorMap[JERR_BADDTOKOPSTATE] = "JERR_BADDTOKOPSTATE: Illegal data token op state";
        _s_errorMap[JERR_BADDTOKTXNSTATE] = "JERR_BADDTOKTXNSTATE: Illegal data token txn state";
        _s_errorMap[JERR_BADDTOKIOSTATE] = "JERR_BADDTOKIOSTATE: Illegal data token io state";
        return true;
    }

    const char*
    JournalErrors::s_errorMessage(const uint32_t err_no) throw ()
    {
        _s_errorMapIterator = _s_errorMap.find(err_no);
        if (_s_errorMapIterator == _s_errorMap.end())
            return "<Unknown error code>";
        return _s_errorMapIterator->second;
    }

} // namespace journal2
} // namespace mrg
