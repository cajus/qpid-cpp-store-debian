/**
 * \file JournalPerformanceTest.hpp
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

#ifndef mrg_jtest_JournalPerformanceTest_hpp
#define mrg_jtest_JournalPerformanceTest_hpp

#include <vector>

#include "JournalInstance.hpp"
#include "PerformanceResult.hpp"
#include "Streamable.hpp"
#include "TestParameters.hpp"

#ifdef JOURNAL2
#include "jrnl2/JournalParameters.hpp"
#else
#include "JournalParameters.hpp"
#endif



namespace mrg
{
namespace jtest
{

    /**
     * \brief Main test class; Create an instance and execute run()
     *
     * Main test class which aggregates the components of a test.
     */
    class JournalPerformanceTest : public Streamable
    {
        const TestParameters& _testParams;          ///< Ref to a struct containing test params
#ifdef JOURNAL2
        const mrg::journal2::JournalParameters& _jrnlParams; ///< Ref to a struct containing the journal parameters
#else
        const JournalParameters& _jrnlParams;       ///< Ref to a struct containing the journal parameters
#endif
        PerformanceResult _jrnlPerf;               ///< Journal performance object
        const char* msgData;                        ///< Pointer to msg data, which is the same for all messages
        std::vector<JournalInstance*> _jrnlList;    ///< List of journals (JournalInstance instances) being tested

        /**
         * \brief Creates journals and JournalInstance classes for all journals (queues) to be tested
         *
         * Creates a new journal instance and JournalInstance instance for each queue. The journals are initialized
         * which creates a new set of journal files on the local storage media (which is determined by path in
         * JournalParameters._jrnlDir). This activity is not timed, and is not a part of the performance test per se.
         */
        void _prepareJournals();

    public:
        /**
         * \brief Constructor
         *
         * \param tp Test parameters for the test
         * \param jp Journal parameters for all queues (journals) in the test
         */
#ifdef JOURNAL2
        JournalPerformanceTest(const TestParameters& tp, const mrg::journal2::JournalParameters& jp);
#else
        JournalPerformanceTest(const TestParameters& tp, const JournalParameters& jp);
#endif

        /**
         * \brief Virtual destructor
         */
        virtual ~JournalPerformanceTest();

        /**
         * \brief Runs the test and prints out the results.
         *
         * Runs the test  as set by the test parameters and journal parameters.
         */
        void run();

        /**
         * \brief Stream the test setup and results to an output stream
         *
         * Convenience feature which streams the test setup and results to an output stream.
         *
         * \param os Output stream to which the test setup and results are to be streamed.
         */
        void toStream(std::ostream& os = std::cout) const;
    };

    /**
     * \brief Print out the program arguments
     *
     * Print out the arguments to the performance program if requested by help or a parameter error.
     *
     * \param os Stream to which the arguments should be streamed.
     */
    void printArgs(std::ostream& os = std::cout);

    /**
     * \brief Process the command-line arguments
     *
     * Process the command-line arguments and populate the TestParameters and JournalParameters structs. Only the
     * arguments supplied are on the command-line are changed in these structs, the others remain unchanged. It is
     * important therefore to make sure that defaults are pre-loaded (the default behavior of the default constructors
     * for these structs).
     *
     * \param argc Number of command-line arguments.  Process directly from main().
     * \param argv Pointer to array of command-line argument pointers. Process directly from main().
     * \param tp Reference to test parameter object. Only params on the command-line are changed.
     * \param jp Reference to journal parameter object. Only params on the command-line are changed.
     */
#ifdef JOURNAL2
    bool readArgs(int argc, char** argv, TestParameters& tp, mrg::journal2::JournalParameters& jp);
#else
    bool readArgs(int argc, char** argv, TestParameters& tp, JournalParameters& jp);
#endif

} // namespace jtest
} // namespace mrg

#endif // mrg_jtest_JournalPerformanceTest_hpp
