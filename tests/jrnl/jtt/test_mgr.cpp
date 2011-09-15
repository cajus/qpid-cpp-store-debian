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

#include "test_mgr.hpp"

#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include "test_case_set.hpp"

namespace mrg
{
namespace jtt
{

test_mgr::test_mgr(args& args):
        _ji_list(),
        _args(args),
        _err_flag(false),
        _random_fn_ptr(random_fn)
{
    if (_args.seed)
        std::srand(_args.seed);
}

test_mgr::~test_mgr()
{}

void
test_mgr::run()
{
    // TODO: complete tidy-up of non-summary (verbose) results, then pull through
    // a command-line summary to control this.
    // Idea: --summary: prints short results afterwards
    //       --verbose: prints long version as test progresses
    //       defualt: none of these, similar to current summary = true version.
    const bool summary = true;

    std::cout << "CSV file: \"" << _args.test_case_csv_file_name << "\"";
    test_case_set tcs(_args.test_case_csv_file_name, _args.recover_mode);

    if (tcs.size())
    {
        std::cout << " (found " << tcs.size() << " test case" << (tcs.size() != 1 ? "s" : "") <<
                ")" << std::endl;
        if (tcs.ignored())
            std::cout << "WARNING: " << tcs.ignored() << " test cases were ignored. (All test "
                    "cases without auto-dequeue are ignored when recover-mode is selected.)" <<
                    std::endl;
        _args.print_args();
    }
    else if(tcs.ignored())
    {
        std::cout << " WARNING: All " << tcs.ignored() << " test case(s) were ignored. (All test "
                "cases without auto-dequeue are ignored when recover-mode is selected.)" <<
                std::endl;
    }
    else
        std::cout << " (WARNING: This CSV file is empty or does not exist.)" << std::endl;

    do
    {
        unsigned u = 0;
        if (_args.randomize)
            random_shuffle(tcs.begin(), tcs.end(), _random_fn_ptr);
        for (test_case_set::tcl_itr tci = tcs.begin(); tci != tcs.end(); tci++, u++)
        {
            if (summary)
                std::cout << "Test case " << (*tci)->test_case_num() << ": \"" <<
                        (*tci)->comment() << "\"" << std::endl;
            else
                std::cout << (*tci)->str() << std::endl;
            if (!_args.reuse_instance || _ji_list.empty())
                initialize_jrnls();
            for (ji_list_citr jii=_ji_list.begin(); jii!=_ji_list.end(); jii++)
                (*jii)->init_tc(*tci, &_args);
            for (ji_list_citr jii=_ji_list.begin(); jii!=_ji_list.end(); jii++)
                (*jii)->run_tc();
            for (ji_list_citr jii=_ji_list.begin(); jii!=_ji_list.end(); jii++)
                (*jii)->tc_wait_compl();

            if (_args.format_chk)
            {
                for (ji_list_citr jii=_ji_list.begin(); jii!=_ji_list.end(); jii++)
                {
                    jrnl_init_params::shared_ptr jpp = (*jii)->params();
                    std::string ja = _args.jfile_analyzer;
                    if (ja.empty()) ja = "./jfile_chk.py";
                    if (!exists(ja))
                    {
                        std::ostringstream oss;
                        oss << "ERROR: Validation program \"" << ja << "\" does not exist" << std::endl;
                        throw std::runtime_error(oss.str());
                    }
                    std::ostringstream oss;
                    oss << ja << " -b " << jpp->base_filename();
                    // TODO: When jfile_check.py can handle previously recovered journals for
                    // specific tests, then remove this exclusion.
                    if (!_args.recover_mode)
                    {
                        oss << " -c " << _args.test_case_csv_file_name;
                        oss << " -t " << (*tci)->test_case_num();
                    }
                    oss << " -q " << jpp->jdir();
                    bool res = system(oss.str().c_str()) != 0;
                    (*tci)->set_fmt_chk_res(res, jpp->jid());
                    if (res) _err_flag = true;
                }
            }

            if (!_args.recover_mode && !_args.keep_jrnls)
                for (ji_list_citr jii=_ji_list.begin(); jii!=_ji_list.end(); jii++)
                    try { mrg::journal::jdir::delete_dir((*jii)->jrnl_dir()); }
                    catch (...) {} // TODO - work out exception strategy for failure here...

            print_results(*tci, summary);
            if ((*tci)->average().exception())
                _err_flag = true;
            if (_abort || (!_args.repeat_flag && _signal))
                break;
            if (_args.pause_secs && tci != tcs.end())
                ::usleep(_args.pause_secs * 1000000);
        }
    }
    while (_args.repeat_flag && !_signal);
}

// static fn:
void
test_mgr::signal_handler(int sig)
{
    if (_signal)
        _abort = true;
    _signal = sig;
    std::cout << std::endl;
    std::cout << "********************************" << std::endl;
    std::cout << "Caught signal " << sig << std::endl;
    if (_abort)
        std::cout << "Aborting..." << std::endl;
    else
        std::cout << "Completing current test cycle..." << std::endl;
    std::cout << "********************************" << std::endl << std::endl;
}

bool
test_mgr::exists(std::string fname)
{
    struct stat s;
    if (::stat(fname.c_str(), &s))
    {
        if (errno == ENOENT) // No such dir or file
            return false;
        // Throw for any other condition
        std::ostringstream oss;
        oss << "ERROR: test_mgr::exists(): file=\"" << fname << "\": " << FORMAT_SYSERR(errno);
        throw std::runtime_error(oss.str());
    }
    return true;
}

void
test_mgr::initialize_jrnls()
{
    _ji_list.clear();
    for (unsigned i=0; i<_args.num_jrnls; i++)
    {
        std::ostringstream jid;
        jid << std::hex << std::setfill('0');
        jid << "test_" << std::setw(4) << std::hex << i;
        std::ostringstream jdir;
        jdir << _args.journal_dir << "/" << jid.str();
        jrnl_init_params::shared_ptr jpp(new jrnl_init_params(jid.str(), jdir.str(), jid.str()));
        jrnl_instance::shared_ptr jip(new jrnl_instance(jpp));
        _ji_list.push_back(jip);
    }
}

void
test_mgr::print_results(test_case::shared_ptr tcp, const bool summary)
{
    if (!summary)
        std::cout << "  === Results ===" << std::endl;

// TODO - the reporting is broken when --repeat is used. The following commented-out
// section was an attempt to fix it, but there are too many side-effects.
//     for (test_case::res_map_citr i=tcp->jmap_begin(); i!=tcp->jmap_end(); i++)
//         std::cout << (*i).second->str(summary, summary);
//     if (tcp->num_jrnls() > 1)
    std::cout << tcp->average().str(false, summary);

    if (!summary)
        std::cout << std::endl;
}

// static instances
volatile sig_atomic_t test_mgr::_signal = 0;
volatile bool test_mgr::_abort = false;

} // namespace jtt
} // namespace mrg
