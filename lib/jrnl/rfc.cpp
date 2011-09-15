/**
 * \file rfc.cpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::rfc (rotating
 * file controller). See comments in file rfc.hpp for details.
 *
 * \author Kim van der Riet
 *
 * Copyright (c) 2008, 2009 Red Hat, Inc.
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

#include "jrnl/rfc.hpp"

#include <cassert>

namespace mrg
{
namespace journal
{

rfc::rfc(const lpmgr* lpmp): _lpmp(lpmp), _fc_index(0), _curr_fc(0)
{}

rfc::~rfc()
{}

void
rfc::finalize()
{
    unset_findex();
}

void
rfc::set_findex(const u_int16_t fc_index)
{
    _fc_index = fc_index;
    _curr_fc = _lpmp->get_fcntlp(fc_index);
    _curr_fc->rd_reset();
}

void
rfc::unset_findex()
{
    _fc_index = 0;
    _curr_fc = 0;
}

std::string
rfc::status_str() const
{
    if (!_lpmp->is_init())
        return "state: Uninitialized";
    if (_curr_fc == 0)
        return "state: Inactive";
    std::ostringstream oss;
    oss << "state: Active";
    return oss.str();
}

} // namespace journal
} // namespace mrg
