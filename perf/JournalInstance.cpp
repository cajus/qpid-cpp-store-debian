/**
 * \file JournalInstance.cpp
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

#include "JournalInstance.hpp"

#include <iostream>

namespace mrg
{
namespace jtest
{

    JournalInstance::JournalInstance(const uint32_t numMsgs,
                                     const uint32_t msgSize,
                                     const char* msgData,
#ifdef JOURNAL2
                                     mrg::journal2::Journal* const jrnlPtr) :
#else
                                     mrg::journal::jcntl* const jrnlPtr) :
#endif
        _numMsgs(numMsgs),
        _msgSize(msgSize),
        _msgData(msgData),
        _jrnlPtr(jrnlPtr),
        _threadSwitch(false)
    {}

    JournalInstance::~JournalInstance()
    {
        delete _jrnlPtr;
    }


    // *** MUST BE THREAD-SAFE ****
    // This method will be called by multiple threads simultaneously
    // Enqueue thread entry point
    void
    JournalInstance::_doEnqueues()
    {
        bool misfireFlag = false;
        uint32_t i = 0;
        while (i < _numMsgs) {
#ifdef JOURNAL2
            mrg::journal2::DataToken* dtokPtr = new mrg::journal2::DataToken();
            mrg::journal2::ioRes jrnlIoRes = _jrnlPtr->enqueue(_msgData, _msgSize, dtokPtr);
#else
            mrg::journal::data_tok* dtokPtr = new mrg::journal::data_tok();
            mrg::journal::iores jrnlIoRes = _jrnlPtr->enqueue_data_record(_msgData, _msgSize, _msgSize, dtokPtr);
#endif
            switch (jrnlIoRes) {
#ifdef JOURNAL2
                case 0:
#else
                case mrg::journal::RHM_IORES_SUCCESS:
#endif
                    i++;
                    misfireFlag = false;
                    break;
#ifdef JOURNAL2
                case mrg::journal2::RHM_IORES_BUSY:
#else
                case mrg::journal::RHM_IORES_BUSY:
#endif
                    if (!misfireFlag) std::cout << "-" << std::flush;
                    delete dtokPtr;
                    misfireFlag = true;
                    break;
#ifdef JOURNAL2
                case mrg::journal2::RHM_IORES_ENQCAPTHRESH:
#else
                case mrg::journal::RHM_IORES_ENQCAPTHRESH:
#endif
                    //std::cout << "_doEnqueues() RHM_IORES_ENQCAPTHRESH: " << dtokPtr->status_str() << std::endl;
                    if (!misfireFlag) std::cout << "*" << std::flush;
                    //std::cout << ".";
                    delete dtokPtr;
                    misfireFlag = true;
                    ::usleep(10);
                    break;
                default:
                    delete dtokPtr;
#ifdef JOURNAL2
                    std::cerr << "enqueue_data_record FAILED with " << mrg::journal2::g_ioResAsString(jrnlIoRes) << std::endl;
#else
                    std::cerr << "enqueue_data_record FAILED with " << mrg::journal::iores_str(jrnlIoRes) << std::endl;
#endif
            }
        }
        _jrnlPtr->flush(false);
    }


    // *** MUST BE THREAD-SAFE ****
    // This method will be called by multiple threads simultaneously
    // Dequeue thread entry point
    void
    JournalInstance::_doDequeues()
    {
        uint32_t i = 0;
        while (i < _numMsgs) {
#ifdef JOURNAL2
            mrg::journal2::DataToken* dtokPtr = 0;
#else
            mrg::journal::data_tok* dtokPtr = 0;
#endif
            while (!dtokPtr) {
                bool aioEventsFlag; // thread local
                { // --- START OF CRITICAL SECTION ---
                    std::lock_guard<std::mutex> l(_unprocCallbackListMutex);
                    aioEventsFlag = _unprocCallbackList.size() == 0;
                    if (!aioEventsFlag) {
                        dtokPtr = _unprocCallbackList.front();
                        _unprocCallbackList.pop();
                    }
                } // --- END OF CRITICAL SECTION ---
                if (aioEventsFlag) {
#ifdef JOURNAL2
                    _jrnlPtr->processCompletedAioWriteEvents(0);
#else
                    _jrnlPtr->get_wr_events(0);
#endif
                    ::usleep(1);
                }
            }
            bool done = false;
            while (!done) {
#ifdef JOURNAL2
                mrg::journal2::ioRes jrnlIoRes = _jrnlPtr->dequeue(dtokPtr);
#else
                mrg::journal::iores jrnlIoRes = _jrnlPtr->dequeue_data_record(dtokPtr);
#endif
                switch (jrnlIoRes) {
#ifdef JOURNAL2
                    case 0:
#else
                    case mrg::journal::RHM_IORES_SUCCESS:
#endif
                        i ++;
                        done = true;
                        break;
#ifdef JOURNAL2
                    case mrg::journal2::RHM_IORES_BUSY:
#else
                    case mrg::journal::RHM_IORES_BUSY:
#endif
                        //::usleep(10);
                        break;
                    default:
#ifdef JOURNAL2
                        std::cerr << "dequeue_data_record FAILED with " << mrg::journal2::g_ioResAsString(jrnlIoRes) << ": "
                                  << dtokPtr->statusStr() << std::endl;
#else
                        std::cerr << "dequeue_data_record FAILED with " << mrg::journal::iores_str(jrnlIoRes) << ": "
                                  << dtokPtr->status_str() << std::endl;
#endif
                        delete dtokPtr;
                        done = true;
                }
            }
#ifdef JOURNAL2
            _jrnlPtr->processCompletedAioWriteEvents(0);
#else
            _jrnlPtr->get_wr_events(0);
#endif
        }
        _jrnlPtr->flush(true);
    }

    // *** MUST BE THREAD-SAFE ****
    // This method will be called by multiple threads simultaneously
    // Main thread entry point.
    // _threadSwitch flips on each call; this makes alternate threads run _doEnqueues() and _doDequeues()
    void
    JournalInstance::operator()()
    {
        bool localThreadSwitch; // thread local
        { // --- START OF CRITICAL SECTION ---
            std::lock_guard<std::mutex> l(_threadSwitchLock);
            localThreadSwitch = _threadSwitch;
            _threadSwitch = !_threadSwitch;
        } // --- END OF CRITICAL SECTION ---
        if (localThreadSwitch) {
            _doDequeues();
        } else {
            _doEnqueues();
        }
    }

    // *** MUST BE THREAD-SAFE ****
    // This method will be called by multiple threads simultaneously
    void
#ifdef JOURNAL2
    JournalInstance::writeAioCompleteCallback(std::vector<mrg::journal2::DataToken*>& dataTokenList)
    {
        mrg::journal2::DataToken* dtokPtr;
#else
    JournalInstance::wr_aio_cb(std::vector<mrg::journal::data_tok*>& dataTokenList)
    {
        mrg::journal::data_tok* dtokPtr;
#endif
        while (dataTokenList.size())
        {
            dtokPtr = dataTokenList.back();
            dataTokenList.pop_back();
#ifdef JOURNAL2
            switch (dtokPtr->getDataTokenState().getOpState()) {
                case mrg::journal2::OP_ENQUEUE:

#else
            switch (dtokPtr->wstate()) {
                case mrg::journal::data_tok::ENQ:
#endif
                    { // --- START OF CRITICAL SECTION ---
                        std::lock_guard<std::mutex> l(_unprocCallbackListMutex);
                        _unprocCallbackList.push(dtokPtr);
                    } // --- END OF CRITICAL SECTION ---
                break;
            default:
                delete dtokPtr;
            }
        }
    }

    // *** MUST BE THREAD-SAFE ****
    // This method will be called by multiple threads simultaneously
    void
#ifdef JOURNAL2
    JournalInstance::readAioCompleteCallback(std::vector<uint16_t>& /*buffPageCtrlBlkIndexList*/)
#else
    JournalInstance::rd_aio_cb(std::vector<uint16_t>& /*buffPageCtrlBlkIndexList*/)
#endif
    {}

} // namespace jtest
} // namespace mrg
