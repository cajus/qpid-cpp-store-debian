/**
 * \file wrfc.cpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::rrfc (rotating
 * file controller). See comments in file rrfc.hpp for details.
 *
 * \author Kim van der Riet
 *
 * Copyright (c) 2007, 2008, 2009 Red Hat, Inc.
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

#include "jrnl/wrfc.hpp"

#include <cmath>
#include "jrnl/jerrno.hpp"
#include "jrnl/jexception.hpp"

namespace mrg
{
namespace journal
{

wrfc::wrfc(const lpmgr* lpmp):
        rfc(lpmp),
        _fsize_sblks(0),
        _fsize_dblks(0),
        _enq_cap_offs_dblks(0),
        _rid(0),
        _reset_ok(false),
        _owi(false),
        _frot(true)
{}

wrfc::~wrfc()
{}

void
wrfc::initialize(const u_int32_t fsize_sblks, rcvdat* rdp)
{
    if (rdp)
    {
        _fc_index = rdp->_lfid;
        _curr_fc = _lpmp->get_fcntlp(_fc_index);
        _curr_fc->wr_reset(rdp);
        _rid = rdp->_h_rid + 1;
        _reset_ok = true;
        _owi = rdp->_owi;
        _frot = rdp->_frot;
        if (rdp->_lffull)
            rotate();
    }
    else
    {
        rfc::initialize();
        rfc::set_findex(0);
        _rid = 0ULL;
        _reset_ok = false;
    }
    _fsize_sblks = fsize_sblks;
    _fsize_dblks = fsize_sblks * JRNL_SBLK_SIZE;
    _enq_cap_offs_dblks = (u_int32_t)std::ceil(_fsize_dblks * _lpmp->num_jfiles() * (100.0 - JRNL_ENQ_THRESHOLD) / 100);
    // Check the offset is at least one file; if not, make it so
    if (_enq_cap_offs_dblks < _fsize_dblks)
        _enq_cap_offs_dblks = _fsize_dblks;
}

iores wrfc::rotate()
{
    if (!_lpmp->num_jfiles())
        throw jexception(jerrno::JERR__NINIT, "wrfc", "rotate");
    _fc_index++;
    if (_fc_index == _lpmp->num_jfiles())
    {
        _fc_index = 0;
        _owi = !_owi;
        _frot = false;
    }
    _curr_fc = _lpmp->get_fcntlp(_fc_index);
    if (_curr_fc->aio_cnt())
        return RHM_IORES_FILE_AIOWAIT;
    if (!wr_reset()) //Checks if file is still in use (ie not fully dequeued yet)
        return RHM_IORES_FULL;
    return RHM_IORES_SUCCESS;
}

u_int16_t wrfc::earliest_index() const
{
    if (_frot)
        return 0;
    u_int16_t next_index = _fc_index + 1;
    if (next_index >= _lpmp->num_jfiles())
        next_index = 0;
    return next_index;
}

bool
wrfc::enq_threshold(const u_int32_t enq_dsize_dblks) const
{
    u_int32_t subm_dblks = subm_cnt_dblks(); // includes file hdr if > 0
    // This compensates for new files which don't have their file headers written yet,
    // as file header space cannot be included in this calculation.
    if (subm_dblks != 0)
        subm_dblks -= 4;
    u_int32_t fwd_dblks = subm_dblks + enq_dsize_dblks + _enq_cap_offs_dblks;
    u_int16_t findex = _fc_index;
    fcntl* fcp = _curr_fc;
    bool in_use = false;
    while (fwd_dblks && !(findex != _fc_index && fcp->enqcnt()))
    {
        fwd_dblks -= fwd_dblks > _fsize_dblks ? _fsize_dblks : fwd_dblks;
        if (fwd_dblks)
        {
            if (++findex == _lpmp->num_jfiles())
                findex = 0;
            fcp = _lpmp->get_fcntlp(findex);
        }
        in_use |= fcp->enqcnt() > 0;
    }
    // Return true if threshold exceeded
    return findex != _fc_index && in_use;
}

bool wrfc::wr_reset()
{
    _reset_ok = _curr_fc->reset(); // returns false if full (ie file still contains enqueued recs)
    return _reset_ok;
}

// TODO: update this to reflect all status data
std::string
wrfc::status_str() const
{
    std::ostringstream oss;
    oss << "wrfc: " << rfc::status_str();
    if (is_active())
        oss << " fcntl[" << _fc_index << "]: " << _curr_fc->status_str();
    return oss.str();
}

} // namespace journal
} // namespace mrg
