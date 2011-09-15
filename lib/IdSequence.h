/*
 Copyright (c) 2007, 2008 Red Hat, Inc.

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

#ifndef _IdSequence_
#define _IdSequence_

#include <qpid/framing/amqp_types.h>
#include <qpid/sys/Mutex.h>
#include <sys/types.h>

namespace mrg{
namespace msgstore{

class IdSequence
{
    qpid::sys::Mutex lock;
    uint64_t id;
public:
    IdSequence();
    uint64_t next();
    void reset(uint64_t value);
};

}}

#endif
