/**
 * \file smutex.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * Messaging journal scoped mutex class mrg::journal::smutex.
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


#ifndef mrg_journal_smutex_hpp
#define mrg_journal_smutex_hpp

#include "jrnl/jexception.hpp"
#include <pthread.h>

namespace mrg
{
namespace journal
{

    // Ultra-simple scoped mutex class that allows a posix mutex to be initialized and destroyed with error checks
    class smutex
    {
    protected:
        mutable pthread_mutex_t _m;
    public:
        inline smutex()
        {
            PTHREAD_CHK(::pthread_mutex_init(&_m, 0), "::pthread_mutex_init", "smutex", "smutex");
        }
        inline virtual ~smutex()
        {
            PTHREAD_CHK(::pthread_mutex_destroy(&_m), "::pthread_mutex_destroy", "smutex", "~smutex");
        }
        inline pthread_mutex_t* get() const { return &_m; }
    };

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_smutex_hpp
