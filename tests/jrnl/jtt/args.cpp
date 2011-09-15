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

#include "args.hpp"

#include <cstddef>
#include <iostream>

namespace po = boost::program_options;

namespace mrg
{
namespace jtt
{

args::args(std::string opt_title):
    _options_descr(opt_title),
    format_chk(false),
    keep_jrnls(false),
    lld_rd_num(10),
    lld_skip_num(100),
    num_jrnls(1),
    pause_secs(0),
    randomize(false),
    read_mode(),
    read_prob(50),
    recover_mode(false),
    repeat_flag(false),
    reuse_instance(false),
    seed(0)
{
    _options_descr.add_options()
        ("csv-file,c",
         po::value<std::string>(&test_case_csv_file_name)->default_value("jtt.csv"),
         "CSV file containing test cases.")

        ("format-chk",
         po::value<bool>(&format_chk)->zero_tokens(),
         "Check the format of each journal file.")

        ("help,h", "This help message.")

        ("jrnl-dir",
        po::value<std::string>(&journal_dir)->default_value("/tmp/jtt"),
        "Directory in which journal files will be placed.")

        ("keep-jrnls",
         po::value<bool>(&keep_jrnls)->zero_tokens(),
         "Keep all test journals.")

        ("lld-rd-num",
         po::value<unsigned>(&lld_rd_num)->default_value(10),
         "Number of consecutive messages to read after only dequeueing lld-skip-num "
         "messages during lazy-loading. Ignored if read-mode is not set to LAZYLOAD.")

        ("lld-skip-num",
         po::value<unsigned>(&lld_skip_num)->default_value(100),
         "Number of consecutive messages to dequeue only (without reading) prior to "
         "reading lld-rd-num messages. Ignored if read-mode is not set to LAZYLOAD.")

        ("num-jrnls",
         po::value<unsigned>(&num_jrnls)->default_value(1),
         "Number of simultaneous journal instances to test.")

        ("pause",
         po::value<unsigned>(&pause_secs)->default_value(0),
         "Pause in seconds between test cases (allows disk to catch up).")

        ("randomize",
         po::value<bool>(&randomize)->zero_tokens(),
         "Randomize the order of the tests.")

        ("read-mode",
         po::value<read_arg>(&read_mode)->default_value(read_arg::NONE),
         read_arg::descr().c_str())

        ("read-prob",
         po::value<unsigned>(&read_prob)->default_value(50),
         "Read probability (percent) for each message when read-mode is set to RANDOM.")

        ("recover-mode",
         po::value<bool>(&recover_mode)->zero_tokens(),
         "Recover journal from the previous test for each test case.")

        ("repeat",
         po::value<bool>(&repeat_flag)->zero_tokens(),
         "Repeat all test cases indefinitely.")

        ("reuse-instance",
         po::value<bool>(&reuse_instance)->zero_tokens(),
         "Reuse journal instance for all test cases.")

        ("seed",
         po::value<unsigned>(&seed)->default_value(0),
         "Seed for use in random number generator.")

        ("analyzer",
        po::value<std::string>(&jfile_analyzer)->default_value("./file_chk.py"),
        "Journal file analyzer program to use when the --format-chk option is used, ignored otherwise.")

        ;
}

bool
args::parse(int argc, char** argv) // return true if error, false if ok
{
    try
    {
        po::store(po::parse_command_line(argc, argv, _options_descr), _vmap);
        po::notify(_vmap);
    }
    catch (const std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
        return usage();
    }
    if (_vmap.count("help"))
        return usage();
    if (num_jrnls == 0)
    {
        std::cout << "ERROR: num-jrnls must be 1 or more." << std::endl;
        return usage();
    }
    if (read_prob > 100) // read_prob is unsigned, so no need to check < 0
    {
        std::cout << "ERROR: read-prob must be between 0 and 100 inclusive." << std::endl;
        return usage();
    }
    if (repeat_flag && keep_jrnls)
    {
        std::string resp;
        std::cout << "WARNING: repeat and keep-jrnls: Monitor disk usage as test journals will"
                " accumulate." << std::endl;
        std::cout << "Continue? <y/n> ";
        std::cin >> resp;
        if (resp.size() == 1)
        {
            if (resp[0] != 'y' && resp[0] != 'Y')
                return true;
        }
        else if (resp.size() == 3) // any combo of lower- and upper-case
        {
            if (resp[0] != 'y' && resp[0] != 'Y')
                return true;
            if (resp[1] != 'e' && resp[1] != 'E')
                return true;
            if (resp[2] != 's' && resp[2] != 'S')
                return true;
        }
        else
            return true;
    }
    return false;
}

bool
args::usage() const
{
    std::cout << _options_descr << std::endl;
    return true;
}

void
args::print_args() const
{
    std::cout << "Number of journals: " << num_jrnls << std::endl;
    std::cout << "Read mode: " << read_mode << std::endl;
    if (read_mode.val() == read_arg::RANDOM)
        std::cout << "Read probability: " << read_prob << " %" << std::endl;
    if (read_mode.val() == read_arg::LAZYLOAD)
    {
        std::cout << "Lazy-load skips: " << lld_skip_num << std::endl;
        std::cout << "Lazy-load reads: " << lld_rd_num << std::endl;
    }
    if (pause_secs)
        std::cout << "Pause between test cases: " << pause_secs << " sec." << std::endl;
    if (seed)
        std::cout << "Randomize seed: " << seed << std::endl;
    print_flags();
}

void
args::print_flags() const
{
    if (format_chk || keep_jrnls || randomize || recover_mode || repeat_flag ||
            reuse_instance)
    {
        std::cout << "Flag options:";
        // TODO: Get flag args and their strings directly from _options_descr.
        if (format_chk)
            std::cout << " format-chk";
        if (keep_jrnls)
            std::cout << " keep-jrnls";
        if (randomize)
            std::cout << " randomize";
        if (recover_mode)
            std::cout << " recover-mode";
        if (repeat_flag)
            std::cout << " repeat-flag";
        if (reuse_instance)
            std::cout << " reuse-instance";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

} // namespace jtt
} // namespace mrg
