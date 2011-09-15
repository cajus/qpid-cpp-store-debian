/**
 * \file lp_map.cpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::lp_map (logical file map). See
 * comments in file lp_map.hpp for details.
 *
 * \author Kim van der Riet
 *
 * Copyright (c) 2008, 2009 Red Hat Inc.
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

#include "jrnl/lp_map.hpp"

#include "jrnl/jerrno.hpp"
#include "jrnl/jexception.hpp"
#include <sstream>

namespace mrg
{
namespace journal
{
lp_map::lp_map() : _map() {}
lp_map::~lp_map() {}

void
lp_map::insert(u_int16_t lfid, u_int16_t pfid)
{
    lfpair ip = lfpair(lfid, pfid);
    lfret ret = _map.insert(ip);
    if (ret.second == false)
    {
        std::ostringstream oss;
        oss << std::hex << "lfid=0x" << lfid << " pfid=0x" << pfid;
        throw jexception(jerrno::JERR_MAP_DUPLICATE, oss.str(), "lp_map", "insert");
    }
}

void
lp_map::get_pfid_list(std::vector<u_int16_t>& pfid_list)
{
    for (lp_map_citr_t i = _map.begin(); i != _map.end(); i++)
        pfid_list.push_back(i->second);
}

// debug aid
std::string
lp_map::to_string()
{
    std::ostringstream oss;
    oss << "{lfid:pfid ";
    for (lp_map_citr_t i=_map.begin(); i!=_map.end(); i++)
    {
        if (i != _map.begin()) oss << ", ";
        oss << (*i).first << ":" << (*i).second;
    }
    oss << "}";
    return oss.str();
}

} // namespace journal
} // namespace mrg
