/**
 * \file JournalInstance.hpp
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

#ifndef mrg_jtest_JournalInstance_hpp
#define mrg_jtest_JournalInstance_hpp

#include <cstdint>
#include <mutex>
#include <queue>

#ifdef JOURNAL2
#include "jrnl2/AioCallback.hpp"
#include "jrnl2/DataToken.hpp"
#include "jrnl2/Journal.hpp"
#else
#include "jrnl/aio_callback.hpp"
#include "jrnl/data_tok.hpp"
#include "jrnl/jcntl.hpp"
#endif

namespace mrg
{
namespace jtest
{

    /**
     * \brief Test journal instance. Each queue to be tested results in one instance of this class.
     *
     * Journal test harness which contains the journal to be tested. Each queue to be tested in the test parameters
     * results in one instance of this class being instantiated, and consequently one set of journals on disk. The
     * journal instance is provided as a pointer to the constructor.
     */
#ifdef JOURNAL2
    class JournalInstance: public mrg::journal2::AioCallback
#else
    class JournalInstance: public mrg::journal::aio_callback
#endif
    {
        const uint32_t _numMsgs;                ///< Number of messages to be processed by this journal instance
        const uint32_t _msgSize;                ///< Size of each message (in bytes)
        const char* _msgData;                   ///< Pointer to message content to be used for each message.
#ifdef JOURNAL2
        mrg::journal2::Journal* const _jrnlPtr; ///< Journal instance pointer
        std::queue<mrg::journal2::DataToken*> _unprocCallbackList; ///< Queue of unprocessed callbacks to be dequeued
#else
        mrg::journal::jcntl* const _jrnlPtr;    ///< Journal instance pointer
        std::queue<mrg::journal::data_tok*> _unprocCallbackList; ///< Queue of unprocessed callbacks to be dequeued
#endif
        std::mutex _unprocCallbackListMutex;    ///< Mutex which protects the unprocessed callback queue
        bool _threadSwitch;                     ///< A switch which alternates worker threads between enq and deq
        std::mutex _threadSwitchLock;           ///< Mutex which protects the thread switch

        /**
         * \brief Worker thread enqueue task
         *
         * This function is the worker thread enqueue task entry point. It enqueues _numMsgs onto the journal instance.
         * A data tokens is created for each record, this is the start of the data token life cycle. All possible
         * returns from the journal are handled appropriately. Since the enqueue threads also perform
         * callbacks on completed AIO operations, the data tokens from completed enqueues are placed onto the
         * unprocessed callback list (_unprocCallbackList) for dequeueing by the dequeue worker thread(s).
         *
         * This function must be thread safe.
         */
        void _doEnqueues();

        /**
         * \brief Worker thread dequeue task
         *
         * This function is the worker thread dequeue task entry point. It dequeues messages which are on the
         * unprocessed callback list (_unprocCallbackList).
         *
         * This function must be thread safe.
         */
        void _doDequeues();

    public:
        /**
         * \brief Constructor
         *
         * \param numMsgs Number of messages per thread to be enqueued then dequeued (ie both ways through broker)
         * \param msgSize Size of each message being enqueued
         * \param msgData Pointer to message content (all messages have identical content)
         * \param jrnlPtr Pinter to journal instance which is to be tested
         */
#ifdef JOURNAL2
        JournalInstance(const uint32_t numMsgs,
                        const uint32_t msgSize,
                        const char* msgData,
                        mrg::journal2::Journal* const jrnlPtr);
#else
        JournalInstance(const uint32_t numMsgs,
                        const uint32_t msgSize,
                        const char* msgData,
                        mrg::journal::jcntl* const jrnlPtr);
#endif

        /**
         * \brief virtual destructor
         */
        virtual ~JournalInstance();

        /**
         * \brief Worker thread entry point used by std::thread for classes. Threads must be provided in pairs.
         *
         * This is the entry point for worker threads. To successfully use this class, threads must be provided in
         * pairs, with alternate threads being used for enqueueing and deqeueing tasks (_doEnqueues and _doDequeues
         * respectively). See these functions for more detail on these tasks. The bool member _threadSwitch is
         * responsible for alternating threads calling this function between these two tasks.
         */
        void operator()();

        /**
         * \brief Write callback function. When AIO operations return, this function is called.
         *
         * When AIO operations return, this function will sort the enqueue ops from the rest and place the data tokens
         * of these records onto the unprocessed callback list (_unprocCallbackList) for dequeueing by another thread.
         *
         * Returning dequeue ops have their data tokens destroyed, as this is the end of the life cycle of the data
         * tokens.
         *
         * Required by all subclasses of mrg::journal::aio_callback.
         *
         * \param dataTokenList A vector of data tokens for those messages which have completed their AIO write
         *  operations
         */
#ifdef JOURNAL2
        void writeAioCompleteCallback(std::vector<mrg::journal2::DataToken*>& dataTokenList);
#else
        void wr_aio_cb(std::vector<mrg::journal::data_tok*>& dataTokenList);
#endif

        /**
         * \brief Read callback function. When read AIO operations return, this function is called.
         *
         * Not used in this test, but required by all subclasses of mrg::journal::aio_callback.
         *
         * \param buffPageCtrlBlkIndexList A vector of indices to the buffer page control blocks for completed reads
         */
#ifdef JOURNAL2
        void readAioCompleteCallback(std::vector<uint16_t>& buffPageCtrlBlkIndexList);
#else
        void rd_aio_cb(std::vector<uint16_t>& buffPageCtrlBlkIndexList);
#endif
    };

} // namespace jtest
} // namespace mrg

#endif // mrg_jtest_JournalInstance_hpp
