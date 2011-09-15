/**
 * \file aio_callback.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * This file contains the definition for the AIO callback function
 * pointer.
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

#ifndef mrg_journal_aio_callback_hpp
#define mrg_journal_aio_callback_hpp

#include <vector>
#include <sys/types.h>

namespace mrg
{
namespace journal
{

    class data_tok;

    class aio_callback
    {
    public:
        virtual ~aio_callback() {}
        virtual void wr_aio_cb(std::vector<data_tok*>& dtokl) = 0;
        virtual void rd_aio_cb(std::vector<u_int16_t>& pil) = 0;
    };

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_aio_callback_hpp
