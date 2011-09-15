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

#include "test_case_result_agregation.hpp"

#include <iomanip>
#include <sstream>

namespace mrg
{
namespace jtt
{

test_case_result_agregation::test_case_result_agregation():
        test_case_result("Average"),
        _tc_average(true),
        _fmt_chk_done(false),
        _fmt_chk_err(false),
        _res_list()
{
}

test_case_result_agregation::test_case_result_agregation(const std::string& jid):
        test_case_result(jid),
        _tc_average(false),
        _fmt_chk_done(false),
        _fmt_chk_err(false),
        _res_list()
{}

test_case_result_agregation::~test_case_result_agregation()
{}

void
test_case_result_agregation::add_test_result(const test_case_result::shared_ptr& tcrp)
{
    if (_tc_average || _jid.compare(tcrp->jid()) == 0)
    {
        _num_enq += tcrp->num_enq();
        _num_deq += tcrp->num_deq();
        _num_read += tcrp->num_read();
        add_test_time(tcrp->test_time());
        _exception_list.insert(_exception_list.end(), tcrp->begin(), tcrp->end());
        _res_list.push_back(tcrp);
    }
}

bool
test_case_result_agregation::exception() const
{
    for (tcrp_list_citr i = _res_list.begin(); i < _res_list.end(); i++)
        if ((*i)->exception())
            return true;
    return false;
}

unsigned
test_case_result_agregation::exception_count() const
{
    unsigned cnt = 0;
    for (tcrp_list_citr i = _res_list.begin(); i < _res_list.end(); i++)
        cnt += (*i)->exception_count();
    return cnt;
}

void
test_case_result_agregation::clear()
{
    test_case_result::clear();
    _res_list.clear();
}

const std::string
test_case_result_agregation::str(const bool last_only, const bool summary) const
{
    std::ostringstream oss;
    if (last_only)
        oss << "  " << _res_list.at(_res_list.size()-1)->str(summary);
    else
    {
        for (tcrp_list_citr i=_res_list.begin(); i!=_res_list.end(); i++)
            oss << "  " << (*i)->str(summary);
    }
    if (_res_list.size() > 1)
         oss << "  " << (summary ? str_summary(last_only) : str_full(last_only));
    return oss.str();
}

const std::string
test_case_result_agregation::str_full(const bool /*last_only*/) const
{
    std::ostringstream oss;
    oss.precision(2);
    if (_tc_average)
        oss << "Average across all journal instances:" << std::endl;
    else
        oss << "Average for jid=\"" << _jid << "\":" << std::endl;
    oss << "  total number results: " << _res_list.size() << std::endl;
    oss << "     number exceptions: " << _exception_list.size() << " (" <<
            (100.0 * _res_list.size() / _exception_list.size()) << "%)" << std::endl;

    oss << test_case_result::str_full();

    if (_exception_list.size())
    {
        unsigned n = 0;
        oss << "List of exceptions/errors:" << std::endl;
        for (elist_citr i = _exception_list.begin(); i != _exception_list.end(); i++, n++)
            oss << "  " << n << ". " << (*i) << std::endl;
    }

    if (!_tc_average && _res_list.size() > 1)
    {
        oss << "Individual results:" << std::endl;
        for (tcrp_list_citr i=_res_list.begin(); i!=_res_list.end(); i++)
            oss << "  " << (*i)->str(false) << std::endl;
        oss << std::endl;
    }

    return oss.str();
}

const std::string
test_case_result_agregation::str_summary(const bool /*last_only*/) const
{
    std::ostringstream oss;
    if (_tc_average)
        oss << "overall average [" << _res_list.size() << "]:";
    else
        oss << "average (" << _res_list.size() << "):";

    oss << test_case_result::str_summary();
    if (_fmt_chk_done)
        oss << " fmt-chk=" << (_fmt_chk_err ? "fail" : "ok");

    if (_exception_list.size())
    {
        if (_tc_average)
            oss << " fail: " << _exception_list.size() << " exception"
                << (_exception_list.size()>1?"s":"") << std::endl;
        else
        {
            if (_exception_list.size() == 1)
                oss << " fail: " << *_exception_list.begin() << std::endl;
            else
            {
                oss << std::endl;
                unsigned n = 0;
                for (elist_citr i = _exception_list.begin(); i != _exception_list.end(); i++, n++)
                    oss << "    " << n << ". " << (*i) << std::endl;
            }
        }
    }
    else
        oss << " ok" << std::endl;
    return oss.str();
}

const journal::time_ns&
test_case_result_agregation::add_test_time(const journal::time_ns& t)
{
    _test_time += t;
    return _test_time;
}

} // namespace jtt
} // namespace mrg
