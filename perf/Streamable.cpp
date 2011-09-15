/**
 * \file Streamable.cpp
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

#include "Streamable.hpp"

#include <sstream>

namespace mrg
{
namespace jtest
{

    std::string
    Streamable::toString() const
    {
        std::ostringstream oss;
        toStream(oss);
        return oss.str();
    }

    std::ostream&
    operator<<(std::ostream& os, const Streamable& s)
    {
        s.toStream(os);
        return os;
    }

    std::ostream&
    operator<<(std::ostream& os, const Streamable* sPtr)
    {
        sPtr->toStream(os);
        return os;
    }

} // namespace jtest
} // namespace mrg
