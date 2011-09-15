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

#ifndef mrg_jtt_args_hpp
#define mrg_jtt_args_hpp

#include <boost/program_options.hpp>
#include "read_arg.hpp"

namespace mrg
{
namespace jtt
{

    struct args
    {
        boost::program_options::options_description _options_descr;
        boost::program_options::variables_map _vmap;

        // Add args here
        std::string jfile_analyzer;
        std::string test_case_csv_file_name;
        std::string journal_dir;
        bool format_chk;
        bool keep_jrnls;
        unsigned lld_rd_num;
        unsigned lld_skip_num;
        unsigned num_jrnls;
        unsigned pause_secs;
        bool randomize;
        read_arg read_mode;
        unsigned read_prob;
    	bool recover_mode;
        bool repeat_flag;
        bool reuse_instance;
        unsigned seed;

        args(std::string opt_title);
        bool parse(int argc, char** argv); // return true if error, false if ok
        bool usage() const; // return true
        void print_args() const;
        void print_flags() const;
    };

} // namespace jtt
} // namespace mrg

#endif // ifndef mrg_jtt_args_hpp
