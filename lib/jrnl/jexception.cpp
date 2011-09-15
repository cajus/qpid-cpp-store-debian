/**
 * \file jexception.cpp
 *
 * Qpid asynchronous store plugin library
 *
 * Generic journal exception class mrg::journal::jexception. See comments
 * in file jexception.hpp for details.
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

#include "jrnl/jexception.hpp"

#include <iomanip>
#include <sstream>
#include "jrnl/jerrno.hpp"

#define CATLEN(p) MAX_MSG_SIZE - std::strlen(p) - 1

namespace mrg
{
namespace journal
{

jexception::jexception() throw ():
        std::exception(),
        _err_code(0)
{
    format();
}

jexception::jexception(const u_int32_t err_code) throw ():
        std::exception(),
        _err_code(err_code)
{
    format();
}

jexception::jexception(const char* additional_info) throw ():
        std::exception(),
        _err_code(0),
        _additional_info(additional_info)
{
    format();
}

jexception::jexception(const std::string& additional_info) throw ():
        std::exception(),
        _err_code(0),
        _additional_info(additional_info)
{
    format();
}

jexception::jexception(const u_int32_t err_code, const char* additional_info) throw ():
        std::exception(),
        _err_code(err_code),
        _additional_info(additional_info)
{
    format();
}

jexception::jexception(const u_int32_t err_code, const std::string& additional_info) throw ():
        std::exception(),
        _err_code(err_code),
        _additional_info(additional_info)
{
    format();
}

jexception::jexception(const u_int32_t err_code, const char* throwing_class,
        const char* throwing_fn) throw ():
        std::exception(),
        _err_code(err_code),
        _throwing_class(throwing_class),
        _throwing_fn(throwing_fn)
{
    format();
}

jexception::jexception(const u_int32_t err_code, const std::string& throwing_class,
        const std::string& throwing_fn) throw ():
        std::exception(),
        _err_code(err_code),
        _throwing_class(throwing_class),
        _throwing_fn(throwing_fn)
{
    format();
}

jexception::jexception(const u_int32_t err_code, const char* additional_info,
        const char* throwing_class, const char* throwing_fn) throw ():
        std::exception(),
        _err_code(err_code),
        _additional_info(additional_info),
        _throwing_class(throwing_class),
        _throwing_fn(throwing_fn)
{
    format();
}

jexception::jexception(const u_int32_t err_code, const std::string& additional_info,
        const std::string& throwing_class, const std::string& throwing_fn) throw ():
        std::exception(),
        _err_code(err_code),
        _additional_info(additional_info),
        _throwing_class(throwing_class),
        _throwing_fn(throwing_fn)
{
    format();
}

jexception::~jexception() throw ()
{}

void
jexception::format()
{
    const bool ai = !_additional_info.empty();
    const bool tc = !_throwing_class.empty();
    const bool tf = !_throwing_fn.empty();
    std::ostringstream oss;
    oss << "jexception 0x" << std::hex << std::setfill('0') << std::setw(4) << _err_code << " ";
    if (tc)
    {
        oss << _throwing_class;
        if (tf)
            oss << "::";
        else
            oss << " ";
    }
    if (tf)
        oss << _throwing_fn << "() ";
    if (tc || tf)
        oss << "threw " << jerrno::err_msg(_err_code);
    if (ai)
        oss << " (" << _additional_info << ")";
    _what.assign(oss.str());
}

const char*
jexception::what() const throw ()
{
    return _what.c_str();
}

std::ostream&
operator<<(std::ostream& os, const jexception& je)
{
    os << je.what();
    return os;
}

std::ostream&
operator<<(std::ostream& os, const jexception* jePtr)
{
    os << jePtr->what();
    return os;
}

} // namespace journal
} // namespace mrg
