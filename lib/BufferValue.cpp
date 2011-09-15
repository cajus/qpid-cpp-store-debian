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

#include "BufferValue.h"

namespace mrg {
namespace msgstore {



BufferValue::BufferValue(u_int32_t size, u_int64_t offset)
    : data(new char[size]),
      buffer(data, size)
{
    set_data(data);
    set_size(size);
    set_flags(DB_DBT_USERMEM | DB_DBT_PARTIAL);
    set_doff(offset);
    set_dlen(size);
    set_ulen(size);
}

BufferValue::BufferValue(const qpid::broker::Persistable& p)
  : data(new char[p.encodedSize()]),
    buffer(data, p.encodedSize())
{
    p.encode(buffer);

    set_data(data);
    set_size(p.encodedSize());
}

BufferValue::~BufferValue()
{
    delete [] data;
}

}}
