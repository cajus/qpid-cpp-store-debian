/**
 * \file DataToken.hpp
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

#ifndef mrg_journal2_DataToken_hpp
#define mrg_journal2_DataToken_hpp

#include <string>
#include <stdint.h> // uint64_t

#include "DataTokenState.hpp"
#include "ScopedLock.hpp"

namespace mrg
{
namespace journal2
{

    typedef uint64_t recordId_t;

    class AtomicRecordIdCounter
    {
        static recordId_t _recordId;
        static ScopedMutex _recordIdMutex;
    public:
        inline static recordId_t s_getNextRecordId()
        { // --- START OF CRITICAL SECTION ---
            ScopedLock l(_recordIdMutex);
            return ++_recordId;
        } // --- START OF CRITICAL SECTION ---
    };

    class DataToken
    {
        DataTokenState _dataTokenState;
        bool _transientFlag;
        bool _externalFlag;
        std::string _externalPath;
        recordId_t _recordId;
        bool _externalRecordIdFlag;
        recordId_t _dequeueRecordId;

    public:
        DataToken();
        DataToken(const recordId_t rid);

        inline DataTokenState& getDataTokenState() { return _dataTokenState; }
        inline bool isTransient() const { return _transientFlag; }
        inline bool isExternal() const { return _externalFlag; }
        inline std::string& getExternalPath() { return _externalPath; }
        void setRecordId(const recordId_t rid);
        inline recordId_t getRecordId() const { return _recordId; }
        inline bool isRecordIdExternal() { return _externalRecordIdFlag; }
        void setDequeueRecordId(const recordId_t drid);
        inline recordId_t getDequeueRecordId() const { return _dequeueRecordId; }

        // debug aids
        std::string statusStr();
    };

} // namespace journal2
} // namespace mrg


#endif // mrg_journal2_DataToken_hpp

