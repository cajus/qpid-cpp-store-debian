/**
 * \file ScopedLock.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * This file contains async journal code (v.2).
 *
 * \author Kim van der Riet
 *
 * Copyright (c) 2010, 2011 Red Hat, Inc.
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

#ifndef mrg_journal2_ScopedLock_hpp
#define mrg_journal2_ScopedLock_hpp

#include <cerrno> // EBUSY
#include <pthread.h>

#include "JournalException.hpp"

namespace mrg
{
namespace journal2
{

    // Ultra-simple scoped mutex class that allows a posix mutex to be initialized and destroyed with error checks
    class ScopedMutex
    {
    protected:
        mutable ::pthread_mutex_t _m;
    public:
        inline ScopedMutex() {
            PTHREAD_CHK(::pthread_mutex_init(&_m, 0), "::pthread_mutex_init", "ScopedMutex", "ScopedMutex");
        }
        inline ~ScopedMutex() {
            PTHREAD_CHK(::pthread_mutex_destroy(&_m), "::pthread_mutex_destroy", "ScopedMutex", "~ScopedMutex");
        }
        inline ::pthread_mutex_t* get() const { return &_m; }
    };

    // Scoped-mutex (sm) container, superclass for scoped lock classes
    class ScopedMutexContainer
    {
    protected:
        const ScopedMutex& _sm;
    public:
        ScopedMutexContainer(const ScopedMutex& sm) : _sm(sm) {}
        virtual ~ScopedMutexContainer() {}
        inline ::pthread_mutex_t* get() const { return _sm.get(); }
    };

    // Ultra-simple scoped lock class, auto-releases mutex when it goes out-of-scope
    class ScopedLock : public ScopedMutexContainer
    {
    public:
        inline ScopedLock(const ScopedMutex& sm) : ScopedMutexContainer(sm) {
            PTHREAD_CHK(::pthread_mutex_lock(_sm.get()), "::pthread_mutex_lock", "ScopedLock", "ScopedLock");
        }
        virtual inline ~ScopedLock() {
            PTHREAD_CHK(::pthread_mutex_unlock(_sm.get()), "::pthread_mutex_unlock", "ScopedLock", "~ScopedLock");
        }
    };

    // Ultra-simple scoped try-lock class, auto-releases mutex when it goes out-of-scope
    class ScopedTryLock : public ScopedMutexContainer
    {
    protected:
        bool _lockedFlag;
    public:
        inline ScopedTryLock(const ScopedMutex& sm) : ScopedMutexContainer(sm), _lockedFlag(false) {
            int ret = ::pthread_mutex_trylock(_sm.get());
            _lockedFlag = (ret == 0); // check if lock obtained
            if (!_lockedFlag && ret != EBUSY)
                PTHREAD_CHK(ret, "::pthread_mutex_trylock", "ScopedTryLock", "ScopedTryLock");
        }
        virtual inline ~ScopedTryLock() {
            if (_lockedFlag)
                PTHREAD_CHK(::pthread_mutex_unlock(_sm.get()), "::pthread_mutex_unlock", "ScopedTryLock",
                                "~ScopedTryLock");
        }
        inline bool isLocked() const { return _lockedFlag; }
    };

    // Ultra-simple scoped condition variable class
    class ScopedConditionVariable
    {
    protected:
        mutable ::pthread_cond_t _cv;
    public:
        inline ScopedConditionVariable() {
            PTHREAD_CHK(::pthread_cond_init(&_cv, 0), "pthread_cond_init", "ScopedConditionVariable",
                            "ScopedConditionVariable");
        }
        inline ~ScopedConditionVariable() {
            PTHREAD_CHK(::pthread_cond_destroy(&_cv), "pthread_cond_destroy", "ScopedConditionVariable",
                            "~ScopedConditionVariable");
        }
        inline void wait(ScopedLock& sl) {
            PTHREAD_CHK(::pthread_cond_wait(&_cv, sl.get()), "pthread_cond_wait", "ScopedConditionVariable", "wait");
        }
        inline void notify_one() {
            PTHREAD_CHK(::pthread_cond_signal(&_cv), "pthread_cond_signal", "ScopedConditionVariable", "notify_one");
        }
        inline void notify_all() {
            PTHREAD_CHK(::pthread_cond_broadcast(&_cv), "pthread_cond_broadcast", "ScopedConditionVariable",
                            "notify_all");
        }
    };


} // namespace journal2
} // namespace mrg

#endif // ifndef mrg_journal2_ScopedLock_hpp
