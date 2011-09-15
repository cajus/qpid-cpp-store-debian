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

#ifndef _StoreException_
#define _StoreException_

#include "IdDbt.h"
#include <boost/format.hpp>

namespace mrg{
namespace msgstore{

class StoreException : public std::exception
{
    std::string text;
public:
    StoreException(const std::string& _text) : text(_text) {}
    StoreException(const std::string& _text, const DbException& cause) : text(_text + ": " + cause.what()) {}
    virtual ~StoreException() throw() {}
    virtual const char* what() const throw() { return text.c_str(); }
};

class StoreFullException : public StoreException
{
public:
    StoreFullException(const std::string& _text) : StoreException(_text) {}
    StoreFullException(const std::string& _text, const DbException& cause) : StoreException(_text, cause) {}
    virtual ~StoreFullException() throw() {}

};

#define THROW_STORE_EXCEPTION(MESSAGE) throw StoreException(boost::str(boost::format("%s (%s:%d)") % (MESSAGE) % __FILE__ % __LINE__))
#define THROW_STORE_EXCEPTION_2(MESSAGE, EXCEPTION) throw StoreException(boost::str(boost::format("%s (%s:%d)") % (MESSAGE) % __FILE__ % __LINE__), EXCEPTION)
#define THROW_STORE_FULL_EXCEPTION(MESSAGE) throw StoreFullException(boost::str(boost::format("%s (%s:%d)") % (MESSAGE) % __FILE__ % __LINE__))

}}

#endif
