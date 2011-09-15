/*
 * Copyright (c) 2008 Red Hat, Inc.
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

#ifndef mrg_jtt_data_src_hpp
#define mrg_jtt_data_src_hpp

#include <cstddef>
#include "jrnl/slock.hpp"
#include "jrnl/smutex.hpp"
#include <pthread.h>
#include <string>
#include <sys/types.h>

#define DATA_SIZE 1024 * 1024
#define XID_SIZE  1024 * 1024

namespace mrg
{
namespace jtt
{
    class data_src
    {
    public:
        static const std::size_t max_dsize = DATA_SIZE;
        static const std::size_t max_xsize = XID_SIZE;

    private:
        static char _data_src[];
        static char _xid_src[];
        static u_int64_t _xid_cnt;
        static bool _initialized;
        static mrg::journal::smutex _sm;

    public:
        static const char* get_data(const std::size_t offs);
        static std::string get_xid(const std::size_t xid_size);

    private:
        data_src();
        static u_int64_t get_xid_cnt() { mrg::journal::slock s(_sm); return _xid_cnt++; }
        static const char* get_xid_content(const std::size_t offs);
        static bool __init();
    };

} // namespace jtt
} // namespace mrg

#endif // ifndef mrg_jtt_data_src_hpp
