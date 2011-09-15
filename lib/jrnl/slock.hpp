/**
 * \file slock.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * Messaging journal scoped lock class mrg::journal::slock and scoped try-lock
 * class mrg::journal::stlock.
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

#ifndef mrg_journal_slock_hpp
#define mrg_journal_slock_hpp

#include "jrnl/jexception.hpp"
#include "jrnl/smutex.hpp"
#include <pthread.h>

namespace mrg
{
namespace journal
{

    // Ultra-simple scoped lock class, auto-releases mutex when it goes out-of-scope
    class slock
    {
    protected:
        const smutex& _sm;
    public:
        inline slock(const smutex& sm) : _sm(sm)
        {
            PTHREAD_CHK(::pthread_mutex_lock(_sm.get()), "::pthread_mutex_lock", "slock", "slock");
        }
        inline ~slock()
        {
            PTHREAD_CHK(::pthread_mutex_unlock(_sm.get()), "::pthread_mutex_unlock", "slock", "~slock");
        }
    };

    // Ultra-simple scoped try-lock class, auto-releases mutex when it goes out-of-scope
    class stlock
    {
    protected:
        const smutex& _sm;
        bool _locked;
    public:
        inline stlock(const smutex& sm) : _sm(sm), _locked(false)
        {
            int ret = ::pthread_mutex_trylock(_sm.get());
            _locked = (ret == 0); // check if lock obtained
            if (!_locked && ret != EBUSY) PTHREAD_CHK(ret, "::pthread_mutex_trylock", "stlock", "stlock");
        }
        inline ~stlock()
        {
            if (_locked)
                PTHREAD_CHK(::pthread_mutex_unlock(_sm.get()), "::pthread_mutex_unlock", "stlock", "~stlock");
        }
        inline bool locked() const { return _locked; }
    };

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_slock_hpp
