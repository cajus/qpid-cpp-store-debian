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

#include "test_case.hpp"

#include <cstdlib>
#include <iomanip>
#include <sstream>

namespace mrg
{
namespace jtt
{

test_case::test_case(const unsigned test_case_num, const u_int32_t num_msgs,
        const std::size_t min_data_size, const std::size_t max_data_size, const bool auto_deq,
        const std::size_t min_xid_size, const std::size_t max_xid_size, const transient_t transient,
        const external_t external, const std::string& comment):
        _test_case_num(test_case_num),
        _num_msgs(num_msgs),
        _min_data_size(min_data_size),
        _max_data_size(max_data_size),
        _auto_dequeue(auto_deq),
        _min_xid_size(min_xid_size),
        _max_xid_size(max_xid_size),
        _transient(transient),
        _external(external),
        _comment(comment),
        _result_average(),
        _result_jmap()
{}

test_case::~test_case()
{}

std::size_t
test_case::this_data_size() const
{
    if (_min_data_size == _max_data_size)
        return _max_data_size;
    std::size_t size_diff = _max_data_size - _min_data_size;
    return _min_data_size + std::size_t(1.0 * std::rand() * size_diff/(RAND_MAX + 1.0));
}

std::size_t
test_case::this_xid_size() const
{
    // TODO: rework when probabilities are introduced. Assume 50% if _min_xid_size = 0
    if (_max_xid_size == 0)
        return std::size_t(0);
    if (_min_xid_size == 0)
    {
        if (1.0 * std::rand() / RAND_MAX < 0.5)
            return std::size_t(0);
    }
    std::size_t size_diff = _max_xid_size - _min_xid_size;
    return _min_xid_size + std::size_t(1.0 * std::rand() * size_diff/(RAND_MAX + 1.0));
}

bool
test_case::this_transience() const
{
    // TODO: rework when probabilities are introduced. Assume 50% if JTT_RANDOM
    if (_transient == JTT_TRANSIENT)
        return false;
    if (_transient == JTT_PERSISTNET)
        return true;
    return 1.0 * std::rand() / RAND_MAX < 0.5;
}

bool
test_case::this_external() const
{
    // TODO: rework when probabilities are introduced. Assume 50% if JDL_RANDOM
    if (_external == JDL_INTERNAL)
        return false;
    if (_external == JDL_EXTERNAL)
        return true;
    return 1.0 * std::rand() / RAND_MAX < 0.5;
}

void
test_case::add_result(test_case_result::shared_ptr& tcrp)
{
    _result_average.add_test_result(tcrp);
    res_map_citr ari = _result_jmap.find(tcrp->jid());
    if (ari == _result_jmap.end())
    {
        test_case_result_agregation::shared_ptr p(new test_case_result_agregation(tcrp->jid()));
        p->add_test_result(tcrp);
        _result_jmap.insert(res_map_pair(tcrp->jid(), p));
    }
    else
        ari->second->add_test_result(tcrp);
}

void
test_case::set_fmt_chk_res(const bool res, const std::string& jid)
{
    _result_average.set_fmt_chk_res(res);
    res_map_citr ari = _result_jmap.find(jid);
    if (ari != _result_jmap.end())
        ari->second->set_fmt_chk_res(res);
}

const test_case_result::shared_ptr
test_case::jmap_last(std::string& jid) const
{
    res_map_citr i = _result_jmap.find(jid);
    if (i == _result_jmap.end())
        return test_case_result::shared_ptr();
    u_int32_t num_res = (*i).second->num_results();
    if (num_res)
        return (*(*i).second)[num_res - 1];
    return test_case_result::shared_ptr();
}

void
test_case::clear()
{
    _result_average.clear();
    _result_jmap.clear();
}

const std::string
test_case::str() const
{
    std::ostringstream oss;
    oss << "Test Parameters: Test case no. " << _test_case_num << ":" << std::endl;
    oss << "  Comment: " << _comment << std::endl;
    oss << "  Number of messages: " << _num_msgs << std::endl;
    oss << "  Data size: " << _min_data_size;
    if (_min_data_size == _max_data_size)
        oss << " bytes (fixed)" << std::endl;
    else
        oss << " - " << _max_data_size << " bytes" << std::endl;
    oss << "  XID size: " << _min_xid_size;
    if (_min_xid_size == _max_xid_size)
        oss << " bytes (fixed)" << std::endl;
    else
        oss << " - " <<  _max_xid_size << " bytes" << std::endl;
    oss << "  Auto-dequeue: " << (_auto_dequeue ? "true" : "false") << std::endl;
    oss << "  Persistence: ";
    switch (_transient)
    {
        case JTT_TRANSIENT: oss << "TRANSIENT" << std::endl; break;
        case JTT_PERSISTNET: oss << "PERSISTNET" << std::endl; break;
        case JTT_RANDOM: oss << "RANDOM" << std::endl; break;
    }
    oss << "  Message Data: ";
    switch (_external)
    {
        case JDL_INTERNAL: oss << "INTERNAL"; break;
        case JDL_EXTERNAL: oss << "EXTERNAL"; break;
        case JDL_RANDOM: oss << "RANDOM"; break;
    }
    return oss.str();
}

} // namespace jtt
} // namespace mrg
