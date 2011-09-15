/**
 * \file ScopedTimer.cpp
 *
 * Qpid asynchronous store plugin library
 *
 * This file contains performance test code for the journal.
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

#include "ScopedTimer.hpp"

namespace mrg
{
namespace jtest
{

    ScopedTimer::ScopedTimer(double& elapsed) : _elapsed(elapsed)
    {
        ::clock_gettime(CLOCK_REALTIME, &_startTime);
    }

    ScopedTimer::ScopedTimer(ScopedTimable& st) : _elapsed(st._elapsed)
    {
        ::clock_gettime(CLOCK_REALTIME, &_startTime);
    }

    ScopedTimer::~ScopedTimer()
    {
        ::timespec stopTime;
        ::clock_gettime(CLOCK_REALTIME, &stopTime);
        _elapsed = _s_getDoubleTime(stopTime) - _s_getDoubleTime(_startTime);
    }

    // static
    double ScopedTimer::_s_getDoubleTime(const ::timespec& ts)
    {
        return ts.tv_sec + (double(ts.tv_nsec) / 1e9);
    }


} // namespace jtest
} // namespace mrg
