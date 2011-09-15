/**
 * \file TestParameters.cpp
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

#include "TestParameters.hpp"

namespace mrg
{
namespace jtest
{

    // static declarations
    uint32_t TestParameters::_s_defaultNumMsgs = 100;
    uint32_t TestParameters::_s_defaultMsgSize = 10;
    uint16_t TestParameters::_s_defaultNumQueues = 1;
    uint16_t TestParameters::_s_defaultNumThreadPairsPerQueue = 1;
    uint16_t TestParameters::_s_defaultEnqTxnBlkSize = 0;
    uint16_t TestParameters::_s_defaultDeqTxnBlkSize = 0;

    TestParameters::TestParameters():
        Streamable(),
        _numMsgs(_s_defaultNumMsgs),
        _msgSize(_s_defaultMsgSize),
        _numQueues(_s_defaultNumQueues),
        _numThreadPairsPerQueue(_s_defaultNumThreadPairsPerQueue),
        _enqTxnBlockSize(_s_defaultEnqTxnBlkSize),
        _deqTxnBlockSize(_s_defaultDeqTxnBlkSize)
    {}

    TestParameters::TestParameters(const uint32_t numMsgs,
                                   const uint32_t msgSize,
                                   const uint16_t numQueues,
                                   const uint16_t numThreadPairsPerQueue,
                                   const uint16_t enqTxnBlockSize,
                                   const uint16_t deqTxnBlockSize) :
        Streamable(),
        _numMsgs(numMsgs),
        _msgSize(msgSize),
        _numQueues(numQueues),
        _numThreadPairsPerQueue(numThreadPairsPerQueue),
        _enqTxnBlockSize(enqTxnBlockSize),
        _deqTxnBlockSize(deqTxnBlockSize)
    {}

    TestParameters::TestParameters(const TestParameters& tp):
        Streamable(),
        _numMsgs(tp._numMsgs),
        _msgSize(tp._msgSize),
        _numQueues(tp._numQueues),
        _numThreadPairsPerQueue(tp._numThreadPairsPerQueue),
        _enqTxnBlockSize(tp._enqTxnBlockSize),
        _deqTxnBlockSize(tp._deqTxnBlockSize)
    {}

    void
    TestParameters::toStream(std::ostream& os) const
    {
        os << "Test Parameters:" << std::endl;
        os << "  num_msgs = " << _numMsgs << std::endl;
        os << "  msg_size = " << _msgSize << std::endl;
        os << "  num_queues = " << _numQueues << std::endl;
        os << "  num_thread_pairs_per_queue = " << _numThreadPairsPerQueue << std::endl;
        os << "  enq_txn_blk_size = " << _enqTxnBlockSize << std::endl;
        os << "  deq_txn_blk_size = " << _deqTxnBlockSize << std::endl;
    }

} // namespace jtest
} // namespace mrg
