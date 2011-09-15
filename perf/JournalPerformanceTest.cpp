/**
 * \file JournalPerformanceTest.cpp
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

#include "JournalPerformanceTest.hpp"

#include <cstdint> // uint16_t, uint32_t
#include <cstdlib> // atof, atoi, atol
#include <deque>
#include <getopt.h> // getopt_long(), required_argument, no_argument
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#include "ScopedTimer.hpp"
#ifdef JOURNAL2
#include "jrnl2/JournalDirectory.hpp"
#else
#include "jrnl/jdir.hpp"
#endif

namespace mrg
{
namespace jtest
{

#ifdef JOURNAL2
    JournalPerformanceTest::JournalPerformanceTest(const TestParameters& tp, const mrg::journal2::JournalParameters& jp) :
#else
    JournalPerformanceTest::JournalPerformanceTest(const TestParameters& tp, const JournalParameters& jp) :
#endif
        Streamable(),
        _testParams(tp),
        _jrnlParams(jp),
        _jrnlPerf(tp),
        msgData(new char[tp._msgSize])
    {}

    JournalPerformanceTest::~JournalPerformanceTest()
    {
        while (_jrnlList.size()) {
            delete _jrnlList.back();
            _jrnlList.pop_back();
        }
        delete[] msgData;
    }

    void
    JournalPerformanceTest::_prepareJournals()
    {
#ifdef JOURNAL2
        if (mrg::journal2::JournalDirectory::s_exists(_jrnlParams._jrnlDir)) {
            mrg::journal2::JournalDirectory::s_delete(_jrnlParams._jrnlDir);
        }
        mrg::journal2::JournalDirectory::s_create(_jrnlParams._jrnlDir);
        mrg::journal2::Journal* jp;
#else
        if (mrg::journal::jdir::exists(_jrnlParams._jrnlDir)) {
            mrg::journal::jdir::delete_dir(_jrnlParams._jrnlDir);
        }
        mrg::journal::jdir::create_dir(_jrnlParams._jrnlDir);
        mrg::journal::jcntl* jp;
#endif
        JournalInstance* ptp;
        for (uint16_t j = 0; j < _testParams._numQueues; j++) {
            std::ostringstream jname;
            jname << "jrnl_" << std::setw(4) << std::setfill('0') << j;
            std::ostringstream jdir;
            jdir << _jrnlParams._jrnlDir << "/" << jname.str();
#ifdef JOURNAL2
            jp = new mrg::journal2::Journal(jname.str(), jdir.str(), _jrnlParams._jrnlBaseFileName);
#else
            jp = new mrg::journal::jcntl(jname.str(), jdir.str(), _jrnlParams._jrnlBaseFileName);
#endif
            ptp = new JournalInstance(_testParams._numMsgs, _testParams._msgSize, msgData, jp);
#ifdef JOURNAL2
            jp->initialize(&_jrnlParams, ptp);
#else
            jp->initialize(_jrnlParams._numJrnlFiles, _jrnlParams._autoExpand, _jrnlParams._autoExpandMaxJrnlFiles,
                        _jrnlParams._jrnlFileSize_sblks, _jrnlParams._writeBuffNumPgs,
                        _jrnlParams._writeBuffPgSize_sblks, ptp);
#endif

            _jrnlList.push_back(ptp);
        }
    }

    void
    JournalPerformanceTest::run()
    {
        std::deque<std::thread*> threads;
        std::thread* tp;
        _prepareJournals();
        { // --- Start of timed section ---
            ScopedTimer st(_jrnlPerf);

            for (uint16_t q = 0; q < _testParams._numQueues; q++) {
                // Launch threads in pairs
                uint16_t numThreads = _testParams._numThreadPairsPerQueue * 2;
                for (uint16_t t = 0; t < numThreads; t++) {
                    tp = new std::thread(std::ref(*_jrnlList[q]));
                    threads.push_back(tp);
                }
            }

            while (threads.size()) {
                threads.front()->join();
                delete threads.front();
                threads.pop_front();
            }
        } // --- End of timed section ---
    }

    void
    JournalPerformanceTest::toStream(std::ostream& os) const
    {
        os << _testParams << std::endl;
        os << _jrnlParams << std::endl;
        os << _jrnlPerf << std::endl;
    }

    void
    printArgs(std::ostream& os)
    {
        os << " -h --help:                       This help message" << std::endl;
        os << std::endl;
        os << "Test params:" << std::endl;
        os << " -M --num_msgs:                   Number of messages to send ["
           << TestParameters::_s_defaultNumMsgs << "]" << std::endl;
        os << " -S --msg_size:                   Size of each message to be sent ["
           << TestParameters::_s_defaultMsgSize << "]" << std::endl;
        os << " -Q --num_queues:                 Number of simultaneous queues ["
           << TestParameters::_s_defaultNumQueues << "]" << std::endl;
        os << " -T --num_thread_pairs_per_queue: Number of thread pairs per queue ["
           << TestParameters::_s_defaultNumThreadPairsPerQueue << "]" << std::endl;
        os << " -E --enq_txn_blk_size:           Enqueue transaction block size (0=non-txn) ["
           << TestParameters::_s_defaultEnqTxnBlkSize << "]" << std::endl;
        os << " -D --deq_txn_blk_size:           Dequeue transaction block size (0=non-txn) ["
           << TestParameters::_s_defaultDeqTxnBlkSize << "]" << std::endl;
        os << std::endl;
        os << "Store params:" << std::endl;
#ifdef JOURNAL2
        os << " -d --jrnl_dir:                   Store directory [\""
           << mrg::journal2::JournalParameters::_s_defaultJrnlDir << "\"]" << std::endl;
        os << " -b --jrnl_base_filename:         Base name for journal files [\""
           << mrg::journal2::JournalParameters::_s_defaultJrnlBaseFileName << "\"]" << std::endl;
        os << " -f --num_jfiles:                 Number of journal files ["
           << mrg::journal2::JournalParameters::_s_defaultNumJrnlFiles << "]" << std::endl;
        os << " -s --jfsize_sblks:               Size of each journal file in sblks (512 byte blocks) ["
           << mrg::journal2::JournalParameters::_s_defaultJrnlFileSize_sblks << "]" << std::endl;
        os << " -a --auto_expand:                Auto-expand the journal ["
           << (mrg::journal2::JournalParameters::_s_defaultAutoExpand?"T":"F") << "]" << std::endl;
        os << " -e --ae_max_jfiles:              Upper limit on number of auto-expanded journal files ["
           << mrg::journal2::JournalParameters::_s_defaultAutoExpandMaxJrnlFiles << "]" << std::endl;
        os << " -p --wcache_num_pages:           Number of write buffer pages ["
           << mrg::journal2::JournalParameters::_s_defaultWriteBuffNumPgs << "]" << std::endl;
        os << " -c --wcache_pgsize_sblks:        Size of each write buffer page in sblks (512 byte blocks) ["
           << mrg::journal2::JournalParameters::_s_defaultWriteBuffPgSize_sblks << "]" << std::endl;
#else
        os << " -d --jrnl_dir:                   Store directory [\""
           << JournalParameters::_s_defaultJrnlDir << "\"]" << std::endl;
        os << " -b --jrnl_base_filename:         Base name for journal files [\""
           << JournalParameters::_s_defaultJrnlBaseFileName << "\"]" << std::endl;
        os << " -f --num_jfiles:                 Number of journal files ["
           << JournalParameters::_s_defaultNumJrnlFiles << "]" << std::endl;
        os << " -s --jfsize_sblks:               Size of each journal file in sblks (512 byte blocks) ["
           << JournalParameters::_s_defaultJrnlFileSize_sblks << "]" << std::endl;
        os << " -a --auto_expand:                Auto-expand the journal ["
           << (JournalParameters::_s_defaultAutoExpand?"T":"F") << "]" << std::endl;
        os << " -e --ae_max_jfiles:              Upper limit on number of auto-expanded journal files ["
           << JournalParameters::_s_defaultAutoExpandMaxJrnlFiles << "]" << std::endl;
        os << " -p --wcache_num_pages:           Number of write buffer pages ["
           << JournalParameters::_s_defaultWriteBuffNumPgs << "]" << std::endl;
        os << " -c --wcache_pgsize_sblks:        Size of each write buffer page in sblks (512 byte blocks) ["
           << JournalParameters::_s_defaultWriteBuffPgSize_sblks << "]" << std::endl;
#endif
}

    bool
#ifdef JOURNAL2
    readArgs(int argc, char** argv, TestParameters& tp, mrg::journal2::JournalParameters& sp)
#else
    readArgs(int argc, char** argv, TestParameters& tp, JournalParameters& sp)
#endif
    {
        static struct option long_options[] = {
            {"help", no_argument, 0, 'h'},

            // Test params
            {"num_msgs", required_argument, 0, 'm'},
            {"msg_size", required_argument, 0, 'S'},
            {"num_queues", required_argument, 0, 'q'},
            {"num_threads_per_queue", required_argument, 0, 't'},

            // Journal params
            {"jrnl_dir", required_argument, 0, 'd'},
            {"jrnl_base_filename", required_argument, 0, 'b'},
            {"num_jfiles", required_argument, 0, 'f'},
            {"jfsize_sblks", required_argument, 0, 's'},
            {"auto_expand", no_argument, 0, 'a'},
            {"ae_max_jfiles", required_argument, 0, 'e'},
            {"wcache_num_pages", required_argument, 0, 'p'},
            {"wcache_pgsize_sblks", required_argument, 0, 'c'},

            {0, 0, 0, 0}
        };

        bool err = false;
        int c = 0;
        while (true) {
            int option_index = 0;
            c = getopt_long(argc, argv, "ab:c:d:e:f:hm:p:q:s:S:t:", long_options, &option_index);
            if (c == -1) break;
            switch (c) {
                // Test params
                case 'm':
                    tp._numMsgs = uint32_t(std::atol(optarg));
                    break;
                case 'S':
                    tp._msgSize = uint32_t(std::atol(optarg));
                    break;
                case 'q':
                    tp._numQueues = uint16_t(std::atoi(optarg));
                    break;
                case 't':
                    tp._numThreadPairsPerQueue = uint16_t(std::atoi(optarg));
                    break;

                // Store params
                case 'd':
                    sp._jrnlDir.assign(optarg);
                    break;
                case 'b':
                    sp._jrnlBaseFileName.assign(optarg);
                    break;
                case 'f':
                    sp._numJrnlFiles = uint16_t(std::atoi(optarg));
                    break;
                case 's':
                    sp._jrnlFileSize_sblks = uint32_t(std::atol(optarg));
                    break;
                case 'a':
                    sp._autoExpand = true;
                    break;
                case 'e':
                    sp._autoExpandMaxJrnlFiles = uint16_t(std::atoi(optarg));
                    break;
                case 'p':
                    sp._writeBuffNumPgs = uint16_t(std::atoi(optarg));
                    break;
                case 'c':
                    sp._writeBuffPgSize_sblks = uint32_t(std::atol(optarg));
                    break;

                // Other
                case 'h':
                default:
                    err = true;
                    printArgs();
            }
        }
        return err;
    }

} // namespace jtest
} // namespace mrg

// -----------------------------------------------------------------

int
main(int argc, char** argv)
{
    mrg::jtest::TestParameters tp;
#ifdef JOURNAL2
    mrg::journal2::JournalParameters jp;
#else
    mrg::jtest::JournalParameters jp;
#endif
    if (mrg::jtest::readArgs(argc, argv, tp, jp)) return 1;
    mrg::jtest::JournalPerformanceTest jpt(tp, jp);
    jpt.run();
    std::cout << jpt << std::endl;
    return 0;
}
