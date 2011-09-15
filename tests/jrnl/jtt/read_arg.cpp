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

#include "read_arg.hpp"

#include <cassert>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

namespace mrg
{
namespace jtt
{
std::map<std::string, read_arg::read_mode_t> read_arg::_map;
std::string read_arg::_description;
const bool read_arg::init = __init();

// static init fn
bool
read_arg::__init()
{
    // Set string versions of each enum option here
    _map["NONE"] = NONE;
    _map["ALL"] = ALL;
    _map["RANDOM"] = RANDOM;
    _map["LAZYLOAD"] = LAZYLOAD;
    _description = "Determines if and when messages will be read prior to dequeueing. "
            "Values: (NONE | ALL | RANDOM | LAZYLOAD)";
    return true;
}

void
read_arg::parse(const std::string& str)
{
    std::map<std::string, read_arg::read_mode_t>::const_iterator i = _map.find(str);
    if (i == _map.end())
        throw po::invalid_option_value(str);
    _rm = i->second;
}

// static fn
const std::string&
read_arg::str(const read_mode_t rm)
{
    std::map<std::string, read_mode_t>::const_iterator i = _map.begin();
    while (i->second != rm && i != _map.end()) i++;
    assert(i != _map.end());
    return i->first;
}

// static fn
const std::string&
read_arg::descr()
{
    return _description;
}

std::ostream&
operator<<(std::ostream& os, const read_arg& ra)
{
    os << ra.str();
    return os;
}

std::istream&
operator>>(std::istream& is, read_arg& ra)
{
    std::string s;
    is >> s;
    ra.parse(s);
    return is;
}

} // namespace jtt
} // namespace mrg
