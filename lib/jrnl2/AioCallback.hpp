/**
 * \file AioCallback.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * This file contains async journal code (v.2).
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

#ifndef mrg_journal2_AioCallback_hpp
#define mrg_journal2_AioCallback_hpp

#include "DataToken.hpp"
#include <stdint.h> // uint16_t
#include <vector>

namespace mrg
{
namespace journal2
{

    class AioCallback
    {
    public:
        virtual ~AioCallback() {}
        virtual void writeAioCompleteCallback(std::vector<DataToken*>& dataTokenList) = 0;
        virtual void readAioCompleteCallback(std::vector<uint16_t>& buffPageCtrlBlkIndexList) = 0;
    };

} // namespace journal2
} // namespace mrg


#endif // mrg_journal2_AioCallback_hpp

