/**
 * \file Journal.hpp
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

#ifndef mrg_journal2_Journal_hpp
#define mrg_journal2_Journal_hpp

#include <string>
#include <stdint.h> // uint64_t, uint32_t, etc.
#include <time.h> // timespec

#include "AioCallback.hpp"
#include "DataToken.hpp"
#include "JournalState.hpp"
#include "JournalParameters.hpp"

// --- temp code ---
#include "ScopedLock.hpp" // ScopedMutex
#include <vector>
#include "RecordHeader.hpp"
// --- end temp code ---

namespace mrg
{
namespace journal2
{

    // TODO - decide if this is the right place to expose these codes and flags
    typedef uint64_t ioRes; // TODO - this needs to be expressed as flags
    const ioRes RHM_IORES_ENQCAPTHRESH = 0x1;
    const ioRes RHM_IORES_BUSY = 0x2;
    std::string g_ioResAsString(const ioRes /*res*/);

    class Journal
    {
    protected:
        std::string _jrnlId;
        std::string _jrnlDir;
        std::string _baseFileName;
        JournalState _jrnlState;
        const JournalParameters* _jrnlParamsPtr;
        AioCallback* _aioCallbackPtr;

        // --- temp code ---
        static uint32_t _s_listSizeThreshold;
        std::vector<DataToken*> _writeDataTokenList;
        std::vector<DataToken*> _callBackDataTokenList[2];
        bool _callBackDataTokenListSwitch;
        ScopedMutex _writeDataTokenListLock;
        ScopedMutex _callBackDataTokenListLock;
        // --- end temp code ---

        void flushNoLock(const bool blockTillAioCompleteFlag);

    public:
        Journal(const std::string& jrnlId, const std::string& jrnlDir, const std::string& baseFileName);

        // get functions
        inline std::string getJournalId() { return _jrnlId; }
        inline std::string getJournalDir() { return _jrnlDir; }
        inline std::string getBaseFileName() { return _baseFileName; }
        inline const JournalState& getJournalState() { return _jrnlState; }
        inline const JournalParameters* getJournalParameters() const { return _jrnlParamsPtr; }

        // msg ops
        void initialize(const JournalParameters* jpPtr, AioCallback* const aiocbPtr);
        ioRes enqueue(const void* const msgPtr, const std::size_t msgSize, DataToken* const dtokPtr);
        ioRes dequeue(DataToken* const dtokPtr);
        ioRes commit();
        ioRes abort();

        // aio ops and status
        // --- temp code ---
        uint32_t getWriteAioEventsRemaining() const;
        // --- end of temp code ---
        void flush(const bool blockTillAioCompleteFlag);
        void processCompletedAioWriteEvents(timespec* const timeout);
    };


} // namespace journal2
} // namespace mrg


#endif // mrg_journal2_Journal_hpp

