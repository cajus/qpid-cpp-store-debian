/**
 * \file TestParameters.hpp
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

#ifndef mrg_jtest_TestParameters_hpp
#define mrg_jtest_TestParameters_hpp

#include <cstdint> // uint16_t, uint32_t
#include "Streamable.hpp"

namespace mrg
{
namespace jtest
{

    /**
     * \brief Struct for aggregating the test parameters
     *
     * This struct is used to aggregate and keep together all the test parameters. These affect the test itself, the
     * journal geometry is aggregated in class JournalParameters.
     */
    struct TestParameters : public Streamable
    {
        static uint32_t _s_defaultNumMsgs;                  ///< Default number of messages to be sent
        static uint32_t _s_defaultMsgSize;                  ///< Default message size in bytes
        static uint16_t _s_defaultNumQueues;                ///< Default number of queues to test simultaneously
        static uint16_t _s_defaultNumThreadPairsPerQueue;   ///< Default number of thread pairs (enq and deq) per queue
        static uint16_t _s_defaultEnqTxnBlkSize;            ///< Default transaction block size for enqueues
        static uint16_t _s_defaultDeqTxnBlkSize;            ///< Default transaction block size for dequeues

        uint32_t _numMsgs;                                  ///< Number of messages to be sent
        uint32_t _msgSize;                                  ///< Message size in bytes
        uint16_t _numQueues;                                ///< Number of queues to test simultaneously
        uint16_t _numThreadPairsPerQueue;                   ///< Number of thread pairs (enq and deq) per queue
        uint16_t _enqTxnBlockSize;                          ///< Transaction block size for enqueues
        uint16_t _deqTxnBlockSize;                          ///< Transaction block size for dequeues

        /**
         * \brief Defaault constructor
         *
         * Default constructor. Uses the default values for all parameters.
         */
        TestParameters();

        /**
         * \brief Constructor
         *
         * Convenience constructor.
         *
         * \param numMsgs Number of messages to be sent
         * \param msgSize Message size in bytes
         * \param numQueues Number of queues to test simultaneously
         * \param numThreadPairsPerQueue Number of thread pairs (enq and deq) per queue
         * \param enqTxnBlockSize Transaction block size for enqueues
         * \param deqTxnBlockSize Transaction block size for dequeues
         */
        TestParameters(const uint32_t numMsgs,
                       const uint32_t msgSize,
                       const uint16_t numQueues,
                       const uint16_t numThreadPairsPerQueue,
                       const uint16_t enqTxnBlockSize,
                       const uint16_t deqTxnBlockSize);

        /**
         * \brief Copy constructor
         *
         * \param tp Reference to TestParameters instance to be copied
         */
        TestParameters(const TestParameters& tp);

        /**
         * \brief Virtual destructor
         */
        virtual ~TestParameters() {}

        /***
         * \brief Stream the test parameters to an output stream
         *
         * Convenience feature which streams a multi-line representation of all the test parameters, one per line to an
         * output stream.
         *
         * \param os Output stream to which the class data is to be streamed
         */
        void toStream(std::ostream& os = std::cout) const;
    };

} // namespace jtest
} // namespace mrg

#endif // mrg_jtest_TestParameters_hpp
