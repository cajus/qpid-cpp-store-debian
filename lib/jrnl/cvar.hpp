/**
 * \file cvar.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * This file contains a posix condition variable class.
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

#ifndef mrg_journal_cvar_hpp
#define mrg_journal_cvar_hpp

#include <cstring>
#include "jrnl/jerrno.hpp"
#include "jrnl/jexception.hpp"
#include "jrnl/smutex.hpp"
#include "jrnl/time_ns.hpp"
#include <pthread.h>
#include <sstream>

namespace mrg
{
namespace journal
{

    // Ultra-simple thread condition variable class
    class cvar
    {
    private:
        const smutex& _sm;
        pthread_cond_t _c;
    public:
        inline cvar(const smutex& sm) : _sm(sm) { ::pthread_cond_init(&_c, 0); }
        inline ~cvar() { ::pthread_cond_destroy(&_c); }
        inline void wait()
        {
            PTHREAD_CHK(::pthread_cond_wait(&_c, _sm.get()), "::pthread_cond_wait", "cvar", "wait");
        }
        inline void timedwait(timespec& ts)
        {
            PTHREAD_CHK(::pthread_cond_timedwait(&_c, _sm.get(), &ts), "::pthread_cond_timedwait", "cvar", "timedwait");
        }
        inline bool waitintvl(const long intvl_ns)
        {
            time_ns t; t.now(); t+=intvl_ns;
            int ret = ::pthread_cond_timedwait(&_c, _sm.get(), &t);
            if (ret == ETIMEDOUT)
                return true;
            PTHREAD_CHK(ret, "::pthread_cond_timedwait", "cvar", "waitintvl");
            return false;
        }
        inline void signal()
        {
            PTHREAD_CHK(::pthread_cond_signal(&_c), "::pthread_cond_signal", "cvar", "notify");
        }
        inline void broadcast()
        {
            PTHREAD_CHK(::pthread_cond_broadcast(&_c), "::pthread_cond_broadcast", "cvar", "broadcast");
        }
    };

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_cvar_hpp
