/*
 Copyright (c) 2007, 2008, 2009 Red Hat, Inc.

 This file is part of the Qpid async store library msgstore.so.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA

 The GNU Lesser General Public License is available in the file COPYING.
 */

#include "IdSequence.h"

using namespace mrg::msgstore;
using qpid::sys::Mutex;

IdSequence::IdSequence() : id(1) {}

u_int64_t IdSequence::next()
{
    Mutex::ScopedLock guard(lock);
    if (!id) id++; // avoid 0 when folding around
    return id++;
}

void IdSequence::reset(uint64_t value)
{
    //deliberately not threadsafe, used only on recovery
    id = value;
}
