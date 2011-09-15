/**
 * \file jrec.cpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing source code for class mrg::journal::jrec (abstract journal
 * jrecord). See comments in file jrec.hpp for details.
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

#include "jrnl/jrec.hpp"

#include <iomanip>
#include "jrnl/jerrno.hpp"
#include "jrnl/jexception.hpp"
#include <sstream>

namespace mrg
{
namespace journal
{

jrec::jrec() {}
jrec::~jrec() {}

void
jrec::chk_hdr(const rec_hdr& hdr)
{
    if (hdr._magic == 0)
    {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        oss << "enq magic NULL: rid=0x" << hdr._rid;
        throw jexception(jerrno::JERR_JREC_BADRECHDR, oss.str(), "jrec", "chk_hdr");
    }
    if (hdr._version != RHM_JDAT_VERSION)
    {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        oss << "version: rid=0x" << hdr._rid;
        oss << ": expected=0x" << std::setw(2) << (int)RHM_JDAT_VERSION;
        oss << " read=0x" << std::setw(2) << (int)hdr._version;
        throw jexception(jerrno::JERR_JREC_BADRECHDR, oss.str(), "jrec", "chk_hdr");
    }
#if defined (JRNL_LITTLE_ENDIAN)
    u_int8_t endian_flag = RHM_LENDIAN_FLAG;
#else
    u_int8_t endian_flag = RHM_BENDIAN_FLAG;
#endif
    if (hdr._eflag != endian_flag)
    {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        oss << "endian_flag: rid=" << hdr._rid;
        oss << ": expected=0x" << std::setw(2) << (int)endian_flag;
        oss << " read=0x" << std::setw(2) << (int)hdr._eflag;
        throw jexception(jerrno::JERR_JREC_BADRECHDR, oss.str(), "jrec", "chk_hdr");
    }
}

void
jrec::chk_rid(const rec_hdr& hdr, const u_int64_t rid)
{
    if (hdr._rid != rid)
    {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        oss << "rid mismatch: expected=0x" << rid;
        oss << " read=0x" << hdr._rid;
        throw jexception(jerrno::JERR_JREC_BADRECHDR, oss.str(), "jrec", "chk_hdr");
    }
}

void
jrec::chk_tail(const rec_tail& tail, const rec_hdr& hdr)
{
    if (tail._xmagic != ~hdr._magic)
    {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        oss << "magic: rid=0x" << hdr._rid;
        oss << ": expected=0x" << ~hdr._magic;
        oss << " read=0x" << tail._xmagic;
        throw jexception(jerrno::JERR_JREC_BADRECTAIL, oss.str(), "jrec", "chk_tail");
    }
    if (tail._rid != hdr._rid)
    {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        oss << "rid: rid=0x" << hdr._rid;
        oss << ": read=0x" << tail._rid;
        throw jexception(jerrno::JERR_JREC_BADRECTAIL, oss.str(), "jrec", "chk_tail");
    }
}

} // namespace journal
} // namespace mrg
