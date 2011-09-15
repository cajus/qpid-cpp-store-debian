/**
 * \file Journal.cpp
 *
 * Qpid asynchronous store plugin library
 *
 * This file contains async journal code (v.2).
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

#include "Journal.hpp"

// --- temp code ---
#include <iostream>
// --- end temp code ---

namespace mrg
{
namespace journal2
{

    std::string
    g_ioResAsString(const ioRes /*res*/)
    {
        // TODO - provide implementation
        return ".[g_ioResAsString].";
    }

    // static
    uint32_t Journal::_s_listSizeThreshold = 50;

    Journal::Journal(const std::string& jrnlId,
               const std::string& jrnlDir,
               const std::string& baseFileName) :
        _jrnlId(jrnlId),
        _jrnlDir(jrnlDir),
        _baseFileName(baseFileName),
        _jrnlParamsPtr(0),
        _aioCallbackPtr(0)
    // --- temp code ---
        , _callBackDataTokenListSwitch(false)
    // --- end temp code ---
    {}

    void
    Journal::initialize(const JournalParameters* jpPtr, AioCallback* const aiocbPtr)
    {
        _jrnlParamsPtr = jpPtr;
        _aioCallbackPtr = aiocbPtr;
    }

    ioRes
    Journal::enqueue(const void* const /*msgPtr*/, const std::size_t /*msgSize*/, DataToken* const dtokPtr)
    {
        dtokPtr->getDataTokenState().setOpStateToEnqueue();
        // --- temp code ---
        bool flushFlag;
        { // --- START OF CRITICAL SECTION ---
            ScopedLock l(_writeDataTokenListLock);
            _writeDataTokenList.push_back(dtokPtr);
            flushFlag = _writeDataTokenList.size() >= _s_listSizeThreshold;
            if (flushFlag) flushNoLock(false);
        } // --- END OF CRITICAL SECTION ---
        if (flushFlag) processCompletedAioWriteEvents(0);
        // --- end temp code ---
        return 0;
    }

    ioRes
    Journal::dequeue(DataToken* const dtokPtr)
    {
        dtokPtr->getDataTokenState().setOpStateToDequeue();
        dtokPtr->setDequeueRecordId(dtokPtr->getRecordId());
        // --- temp code ---
        bool flushFlag;
        { // --- START OF CRITICAL SECTION ---
            ScopedLock l(_writeDataTokenListLock);
            _writeDataTokenList.push_back(dtokPtr);
            flushFlag = _writeDataTokenList.size() >= _s_listSizeThreshold;
            if (flushFlag) flushNoLock(false);
        } // --- END OF CRITICAL SECTION ---
        if (flushFlag) processCompletedAioWriteEvents(0);
        // --- end temp code ---
        return 0;
    }

    ioRes
    Journal::commit()
    {
        // TODO
        return 0;
    }

    ioRes
    Journal::abort()
    {
        // TODO
        return 0;
    }

    uint32_t
    Journal::getWriteAioEventsRemaining() const
    {
        while (true) { // --- START OF CRITICAL SECTION ---
            ScopedTryLock l1(_callBackDataTokenListLock);
            ScopedTryLock l2(_writeDataTokenListLock);
            if (l1.isLocked() && l2.isLocked()) {
                return _callBackDataTokenList[0].size() + _callBackDataTokenList[1].size();
            } else {
                //::usleep(10);
            }
        }; // --- END OF CRITICAL SECTION ---
    }

    void
    Journal::flush(const bool blockTillAioCompleteFlag)
    {
        // --- temp code ---
        // --- START OF CRITICAL SECTION ---
        ScopedTryLock l(_writeDataTokenListLock);
        if (l.isLocked()) {
            flushNoLock(blockTillAioCompleteFlag);
        }
        // --- END OF CRITICAL SECTION ---
        // --- end temp code ---
    }

    // protected
    void
    Journal::flushNoLock(const bool /*blockTillAioCompleteFlag*/)
    {
        // --- temp code ---
        int i = _callBackDataTokenListSwitch ? 1 : 0;
        while (_writeDataTokenList.size()) {
            _callBackDataTokenList[i].push_back(_writeDataTokenList.back());
            _writeDataTokenList.pop_back();
        }
        // --- end temp code ---
    }

    void
    Journal::processCompletedAioWriteEvents(timespec* const /*timeout*/)
    {
        // --- temp code ---
        // --- START OF CRITICAL SECTION 1 ---
        ScopedTryLock l1(_callBackDataTokenListLock);
        if (l1.isLocked()) {
            int i = _callBackDataTokenListSwitch ? 0 : 1;
            if (_callBackDataTokenList[i].size() && _aioCallbackPtr) {
                // NOTE: Callback made with _callBackDataTokenListLock isLocked
                _aioCallbackPtr->writeAioCompleteCallback(_callBackDataTokenList[i]);
            }
            _callBackDataTokenList[i].clear();

            // take both locks before allowing switch to change
            { // --- START OF CRITICAL SECTION 2 ---
                ScopedLock l2(_writeDataTokenListLock);
                _callBackDataTokenListSwitch = !_callBackDataTokenListSwitch;
            } // --- END OF CRITICAL SECTION 2 ---
        }
        // --- END OF CRITICAL SECTION 1 ---
        // --- end temp code ---
    }

} // namespace journal2
} // namespace mrg

