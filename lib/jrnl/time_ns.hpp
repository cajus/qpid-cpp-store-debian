/**
 * \file time_ns.hpp
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

#ifndef mrg_jtt_time_ns_hpp
#define mrg_jtt_time_ns_hpp

#include <cerrno>
#include <ctime>
#include <string>

namespace mrg
{
namespace journal
{

struct time_ns : public timespec
{
    inline time_ns() { tv_sec = 0; tv_nsec = 0; }
    inline time_ns(const std::time_t sec, const long nsec = 0) { tv_sec = sec; tv_nsec = nsec; }
    inline time_ns(const time_ns& t) { tv_sec = t.tv_sec; tv_nsec = t.tv_nsec; }

    inline void set_zero() { tv_sec = 0; tv_nsec = 0; }
    inline bool is_zero() const { return tv_sec == 0 && tv_nsec == 0; }
    inline int now() { if(::clock_gettime(CLOCK_REALTIME, this)) return errno; return 0; }
    const std::string str(int precision = 6) const;

    inline time_ns& operator=(const time_ns& rhs)
        { tv_sec = rhs.tv_sec; tv_nsec = rhs.tv_nsec; return *this; }
    inline time_ns& operator+=(const time_ns& rhs)
    {
        tv_nsec += rhs.tv_nsec;
        if (tv_nsec >= 1000000000L) { tv_sec++; tv_nsec -= 1000000000L; }
        tv_sec += rhs.tv_sec;
        return *this;
    }
    inline time_ns& operator+=(const long ns)
    {
        tv_nsec += ns;
        if (tv_nsec >= 1000000000L) { tv_sec++; tv_nsec -= 1000000000L; }
        return *this;
    }
    inline time_ns& operator-=(const long ns)
    {
        tv_nsec -= ns;
        if (tv_nsec < 0) { tv_sec--; tv_nsec += 1000000000L; }
        return *this;
    }
    inline time_ns& operator-=(const time_ns& rhs)
    {
        tv_nsec -= rhs.tv_nsec;
        if (tv_nsec < 0) { tv_sec--; tv_nsec += 1000000000L; }
        tv_sec -= rhs.tv_sec;
        return *this;
    }
    inline const time_ns operator+(const time_ns& rhs)
        { time_ns t(*this); t += rhs; return t; }
    inline const time_ns operator-(const time_ns& rhs)
        { time_ns t(*this); t -= rhs; return t; }
    inline bool operator==(const time_ns& rhs)
       { return tv_sec == rhs.tv_sec && tv_nsec == rhs.tv_nsec; }
    inline bool operator!=(const time_ns& rhs)
       { return tv_sec != rhs.tv_sec || tv_nsec != rhs.tv_nsec; }
    inline bool operator>(const time_ns& rhs)
       { if(tv_sec == rhs.tv_sec) return tv_nsec > rhs.tv_nsec; return tv_sec > rhs.tv_sec; }
    inline bool operator>=(const time_ns& rhs)
       { if(tv_sec == rhs.tv_sec) return tv_nsec >= rhs.tv_nsec; return tv_sec >= rhs.tv_sec; }
    inline bool operator<(const time_ns& rhs)
       { if(tv_sec == rhs.tv_sec) return tv_nsec < rhs.tv_nsec; return tv_sec < rhs.tv_sec; }
    inline bool operator<=(const time_ns& rhs)
       { if(tv_sec == rhs.tv_sec) return tv_nsec <= rhs.tv_nsec; return tv_sec <= rhs.tv_sec; }
};

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_jtt_time_ns_hpp
