/*
 * Copyright (c) 2008 Red Hat, Inc.
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

#include "data_src.hpp"

#include <cstddef>
#include <iomanip>
#include <sstream>

namespace mrg
{
namespace jtt
{

char data_src::_data_src[data_src::max_dsize];
char data_src::_xid_src[data_src::max_xsize];
bool data_src::_initialized = data_src::__init();
u_int64_t data_src::_xid_cnt = 0ULL;
mrg::journal::smutex data_src::_sm;

data_src::data_src()
{}

bool
data_src::__init()
{
    for (unsigned i=0; i<max_dsize; i++)
        _data_src[i] = '0' + ((i + 1) % 10); // 123456789012345...
    for (unsigned j=0; j<max_xsize; j++)
        _xid_src[j] = 'a' + (j % 26);        // abc...xyzabc...
    return true;
}

const char*
data_src::get_data(const std::size_t offs)
{
    if (offs >= max_dsize) return 0;
    return _data_src + offs;
}

std::string
data_src::get_xid(const std::size_t xid_size)
{
    if (xid_size == 0)
        return "";
    std::ostringstream oss;
    oss << std::setfill('0');
    if (xid_size < 9)
        oss << std::setw(xid_size) << get_xid_cnt();
    else if (xid_size < 13)
        oss << "xid:" << std::setw(xid_size - 4) << get_xid_cnt();
    else
    {
        oss << "xid:" << std::setw(8) << get_xid_cnt() << ":";
        oss.write(get_xid_content(13), xid_size - 13);
    }
    return oss.str();
}

const char*
data_src::get_xid_content(const std::size_t offs)
{
    if (offs >= max_xsize) return 0;
    return _xid_src + offs;
}

} // namespace jtt
} // namespace mrg

