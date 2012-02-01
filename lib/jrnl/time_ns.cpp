/**
 * \file time_ns.cpp
 *
 * Qpid asynchronous store plugin library
 *
 * Messaging journal time struct mrg::journal::time_ns, derived from
 * the timespec struct and provided with helper functions.
 *
 * \author Kim van der Riet
 *
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

#include "time_ns.hpp"

#include <sstream>

namespace mrg
{
namespace journal
{

const std::string
time_ns::str(int precision) const
{
    const double t = tv_sec + (tv_nsec/1e9);
    std::ostringstream oss;
    oss.setf(std::ios::fixed, std::ios::floatfield);
    oss.precision(precision);
    oss << t;
    return oss.str();
}


} // namespace journal
} // namespace mrg
