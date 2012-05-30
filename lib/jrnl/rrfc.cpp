/**
 * \file rrfc.cpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::rrfc (rotating
 * file controller). See comments in file rrfc.hpp for details.
 *
 * \author Kim van der Riet
 *
 * Copyright (c) 2007, 2008, 2009 Red Hat, Inc.
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


#include "jrnl/rrfc.hpp"

#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include "jrnl/jerrno.hpp"
#include "jrnl/jexception.hpp"

namespace mrg
{
namespace journal
{

rrfc::rrfc(const lpmgr* lpmp): rfc(lpmp), _fh(-1), _valid(false)
{}

rrfc::~rrfc()
{
    close_fh();
}

void
rrfc::finalize()
{
    unset_findex();
    rfc::finalize();
}

void
rrfc::set_findex(const u_int16_t fc_index)
{
    rfc::set_findex(fc_index);
    open_fh(_curr_fc->fname());
}

void
rrfc::unset_findex()
{
    set_invalid();
    close_fh();
    rfc::unset_findex();
}

iores
rrfc::rotate()
{
    if (!_lpmp->num_jfiles())
        throw jexception(jerrno::JERR__NINIT, "rrfc", "rotate");
    u_int16_t next_fc_index = _fc_index + 1;
    if (next_fc_index == _lpmp->num_jfiles())
        next_fc_index = 0;
    set_findex(next_fc_index);
    return RHM_IORES_SUCCESS;
}

std::string
rrfc::status_str() const
{
    std::ostringstream oss;
    oss << "rrfc: " << rfc::status_str();
    if (is_active())
        oss << " fcntl[" << _fc_index << "]: " << _curr_fc->status_str();
    return oss.str();
}

// === protected functions ===

void
rrfc::open_fh(const std::string& fn)
{
    close_fh();
    _fh = ::open(fn.c_str(), O_RDONLY | O_DIRECT);
    if (_fh < 0)
    {
        std::ostringstream oss;
        oss << "file=\"" << fn << "\"" << FORMAT_SYSERR(errno);
        throw jexception(jerrno::JERR_RRFC_OPENRD, oss.str(), "rrfc", "open_fh");
    }
}

void
rrfc::close_fh()
{
    if (_fh >= 0)
    {
        ::close(_fh);
        _fh = -1;
    }
}

} // namespace journal
} // namespace mrg
