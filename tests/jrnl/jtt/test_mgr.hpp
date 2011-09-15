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

#ifndef mrg_jtt_test_mgr_hpp
#define mrg_jtt_test_mgr_hpp

#include "args.hpp"
#include <csignal>
#include <cstdlib>
#include "jrnl_instance.hpp"

namespace mrg
{
namespace jtt
{
    class test_mgr
    {
    public:
        typedef std::vector<jrnl_instance::shared_ptr> ji_list;
        typedef ji_list::iterator ji_list_itr;
        typedef ji_list::const_iterator ji_list_citr;

    private:
        ji_list _ji_list;
        args& _args;
        bool _err_flag;
        ptrdiff_t (*_random_fn_ptr)(const ptrdiff_t i);
        static volatile std::sig_atomic_t _signal;
        static volatile bool _abort;

    public:
        test_mgr(args& args);
        virtual ~test_mgr();
        void run();
        inline bool error() const { return _err_flag; }

        static void signal_handler(int signal);

    private:
        static bool exists(std::string file_name);
        void initialize_jrnls();
        void print_results(test_case::shared_ptr tcp, const bool summary);
        inline static ptrdiff_t random_fn(const ptrdiff_t i)
                { return static_cast<ptrdiff_t>(1.0 * i * std::rand() / RAND_MAX); }
    };

} // namespace jtt
} // namespace mrg

#endif // ifndef mrg_jtt_test_mgr_hpp
