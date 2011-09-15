/*
 Copyright (c) 2007 Red Hat, Inc.

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

#ifndef _BufferValue_
#define _BufferValue_

#include "db-inc.h"
#include <qpid/broker/Persistable.h>
#include <qpid/framing/Buffer.h>

namespace mrg{
namespace msgstore{

class BufferValue : public Dbt
{
    char* data;

public:
    qpid::framing::Buffer buffer;

    BufferValue(u_int32_t size, u_int64_t offset);
    BufferValue(const qpid::broker::Persistable& p);
    virtual ~BufferValue();
};

}}

#endif
