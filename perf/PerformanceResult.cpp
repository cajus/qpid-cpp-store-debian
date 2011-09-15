/**
 * \file PerformanceResult.cpp
 *
 * Qpid asynchronous store plugin library
 *
 * This file contains performance test code for the journal.
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

#include "PerformanceResult.hpp"

#include <cstdint> // uint32_t

namespace mrg
{
namespace jtest
{

    PerformanceResult::PerformanceResult(const TestParameters& tp) :
        ScopedTimable(),
        Streamable(),
        _testParams(tp)
    {}

    void
    PerformanceResult::toStream(std::ostream& os) const
    {
        os << "TEST RESULTS:" << std::endl;
        os << "    Msgs per thread: " << _testParams._numMsgs << std::endl;
        os << "           Msg size: " << _testParams._msgSize << std::endl;
        os << "         No. queues: " << _testParams._numQueues << std::endl;
        os << "  No. threads/queue: " << _testParams._numThreadPairsPerQueue << std::endl;
        os << "         Time taken: " << _elapsed << " sec" << std::endl;
        uint32_t totalMsgs = _testParams._numMsgs * _testParams._numQueues * _testParams._numThreadPairsPerQueue;
        os << "     Total no. msgs: " << totalMsgs << std::endl;
        double msgsRate = double(totalMsgs) / _elapsed;
        os << "     Msg throughput: " << (msgsRate / 1e3) << " kMsgs/sec" << std::endl;
        os << "                     " << (msgsRate * _testParams._msgSize / 1e6) << " MB/sec" << std::endl;
    }

} // namespace jtest
} // namespace mrg
