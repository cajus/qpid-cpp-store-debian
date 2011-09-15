/**
 * \file PerformanceResult.hpp
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

#ifndef mrg_jtest_PerformanceResult_hpp
#define mrg_jtest_PerformanceResult_hpp

#include <iostream>

#include "TestParameters.hpp"
#include "ScopedTimable.hpp"
#include "Streamable.hpp"

namespace mrg
{
namespace jtest
{

    /**
     * \brief Results class that accepts an elapsed time to calculate the rate of message throughput in the journal.
     *
     * This class (being subclassed from ScopedTimable) is passed to a ScopedTimer object on construction, and the
     * inherited _elapsed member will be written with the calculated elapsed time (in seconds) on destruction of the
     * ScopedTimer object. This time (initially set to 0.0) is used to calculate message and message byte throughput.
     * The message number and size information comes from the TestParameters object passed to the constructor.
     *
     * Results are available through the use of toStream(), toString() or the << operators.
     *
     * Output is in the following format:
     * <pre>
     * TEST RESULTS:
     *     Msgs per thread: 10000
     *            Msg size: 2048
     *          No. queues: 2
     *   No. threads/queue: 2
     *          Time taken: 1.6626 sec
     *      Total no. msgs: 40000
     *      Msg throughput: 24.0587 kMsgs/sec
     *                      49.2723 MB/sec
     * </pre>
     */
    class PerformanceResult : public ScopedTimable, public Streamable
    {
        TestParameters _testParams; ///< Test parameters used for performance calculations

    public:
        /**
         * \brief Constructor
         *
         * Constructor. Will start the time interval measurement.
         *
         * \param tp Test parameter details used to calculate the performance results.
         */
        PerformanceResult(const TestParameters& tp);

        /**
         * \brief Virtual destructor
         */
        virtual ~PerformanceResult() {}

        /**
         * \brief Stream the performance test results to an output stream
         *
         * Convenience feature which streams a multi-line performance result an output stream.
         *
         * \param os Output stream to which the results are to be streamed
         */
        void toStream(std::ostream& os = std::cout) const;
    };

} // namespace jtest
} // namespace mrg

#endif // mrg_jtest_PerformanceResult_hpp
