
#ifndef _MANAGEMENT_JOURNAL_
#define _MANAGEMENT_JOURNAL_

//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//

// This source file was created by a code generator.
// Please do not edit.

#include "qpid/management/ManagementObject.h"

namespace qpid {
    namespace management {
        class ManagementAgent;
    }
}

namespace qmf {
namespace com {
namespace redhat {
namespace rhm {
namespace store {


class Journal : public ::qpid::management::ManagementObject
{
  private:

    static std::string packageName;
    static std::string className;
    static uint8_t     md5Sum[MD5_LEN];


    // Properties
    ::qpid::management::ObjectId queueRef;
    std::string name;
    std::string directory;
    std::string baseFileName;
    uint32_t writePageSize;
    uint32_t writePages;
    uint32_t readPageSize;
    uint32_t readPages;
    uint16_t initialFileCount;
    bool autoExpand;
    uint16_t currentFileCount;
    uint16_t maxFileCount;
    uint32_t dataFileSize;

    // Statistics
    uint32_t  recordDepth;
    uint32_t  recordDepthHigh;
    uint32_t  recordDepthLow;
    uint32_t  outstandingAIOs;
    uint32_t  outstandingAIOsHigh;
    uint32_t  outstandingAIOsLow;
    uint32_t  freeFileCount;
    uint32_t  freeFileCountHigh;
    uint32_t  freeFileCountLow;
    uint32_t  availableFileCount;
    uint32_t  availableFileCountHigh;
    uint32_t  availableFileCountLow;
    uint32_t  writePageCacheDepth;
    uint32_t  writePageCacheDepthHigh;
    uint32_t  writePageCacheDepthLow;
    uint32_t  readPageCacheDepth;
    uint32_t  readPageCacheDepthHigh;
    uint32_t  readPageCacheDepthLow;


    // Per-Thread Statistics
    struct PerThreadStats {
        uint64_t  enqueues;
        uint64_t  dequeues;
        uint32_t  txn;
        uint64_t  txnEnqueues;
        uint64_t  txnDequeues;
        uint64_t  txnCommits;
        uint64_t  txnAborts;
        uint64_t  writeWaitFailures;
        uint64_t  writeBusyFailures;
        uint64_t  readRecordCount;
        uint64_t  readBusyFailures;

    };

    struct PerThreadStats** perThreadStatsArray;

    inline struct PerThreadStats* getThreadStats() {
        int idx = getThreadIndex();
        struct PerThreadStats* threadStats = perThreadStatsArray[idx];
        if (threadStats == 0) {
            threadStats = new(PerThreadStats);
            perThreadStatsArray[idx] = threadStats;
            threadStats->enqueues = 0;
            threadStats->dequeues = 0;
            threadStats->txn = 0;
            threadStats->txnEnqueues = 0;
            threadStats->txnDequeues = 0;
            threadStats->txnCommits = 0;
            threadStats->txnAborts = 0;
            threadStats->writeWaitFailures = 0;
            threadStats->writeBusyFailures = 0;
            threadStats->readRecordCount = 0;
            threadStats->readBusyFailures = 0;

        }
        return threadStats;
    }

    void aggregatePerThreadStats(struct PerThreadStats*) const;

  public:
    static void writeSchema(std::string& schema);
    void mapEncodeValues(::qpid::types::Variant::Map& map,
                         bool includeProperties=true,
                         bool includeStatistics=true);
    void mapDecodeValues(const ::qpid::types::Variant::Map& map);
    void doMethod(std::string&           methodName,
                  const ::qpid::types::Variant::Map& inMap,
                  ::qpid::types::Variant::Map& outMap,
                  const std::string& userId);
    std::string getKey() const;

    uint32_t writePropertiesSize() const;
    void readProperties(const std::string& buf);
    void writeProperties(std::string& buf) const;
    void writeStatistics(std::string& buf, bool skipHeaders = false);
    void doMethod(std::string& methodName,
                  const std::string& inBuf,
                  std::string& outBuf,
                  const std::string& userId);


    writeSchemaCall_t getWriteSchemaCall() { return writeSchema; }


    Journal(::qpid::management::ManagementAgent* agent,
                            ::qpid::management::Manageable* coreObject);
    ~Journal();

    void setReference(::qpid::management::ObjectId objectId) { queueRef = objectId; }


    static void registerSelf(::qpid::management::ManagementAgent* agent);
    std::string& getPackageName() const { return packageName; }
    std::string& getClassName() const { return className; }
    uint8_t* getMd5Sum() const { return md5Sum; }

    // Method IDs
    static const uint32_t METHOD_EXPAND = 1;

    // Accessor Methods
    inline void set_queueRef (const ::qpid::management::ObjectId& val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        queueRef = val;
        configChanged = true;
    }
    inline const ::qpid::management::ObjectId& get_queueRef() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return queueRef;
    }
    inline void set_name (const std::string& val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        name = val;
        configChanged = true;
    }
    inline const std::string& get_name() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return name;
    }
    inline void set_directory (const std::string& val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        directory = val;
        configChanged = true;
    }
    inline const std::string& get_directory() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return directory;
    }
    inline void set_baseFileName (const std::string& val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        baseFileName = val;
        configChanged = true;
    }
    inline const std::string& get_baseFileName() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return baseFileName;
    }
    inline void set_writePageSize (uint32_t val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        writePageSize = val;
        configChanged = true;
    }
    inline uint32_t get_writePageSize() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return writePageSize;
    }
    inline void set_writePages (uint32_t val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        writePages = val;
        configChanged = true;
    }
    inline uint32_t get_writePages() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return writePages;
    }
    inline void set_readPageSize (uint32_t val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        readPageSize = val;
        configChanged = true;
    }
    inline uint32_t get_readPageSize() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return readPageSize;
    }
    inline void set_readPages (uint32_t val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        readPages = val;
        configChanged = true;
    }
    inline uint32_t get_readPages() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return readPages;
    }
    inline void set_initialFileCount (uint16_t val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        initialFileCount = val;
        configChanged = true;
    }
    inline uint16_t get_initialFileCount() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return initialFileCount;
    }
    inline void set_autoExpand (bool val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        autoExpand = val;
        configChanged = true;
    }
    inline bool get_autoExpand() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return autoExpand;
    }
    inline void set_currentFileCount (uint16_t val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        currentFileCount = val;
        configChanged = true;
    }
    inline uint16_t get_currentFileCount() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return currentFileCount;
    }
    inline void set_maxFileCount (uint16_t val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        maxFileCount = val;
        configChanged = true;
    }
    inline uint16_t get_maxFileCount() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return maxFileCount;
    }
    inline void set_dataFileSize (uint32_t val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        dataFileSize = val;
        configChanged = true;
    }
    inline uint32_t get_dataFileSize() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return dataFileSize;
    }
    inline void inc_recordDepth (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        recordDepth += by;
        if (recordDepthHigh < recordDepth)
            recordDepthHigh = recordDepth;
        instChanged = true;
    }
    inline void dec_recordDepth (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        recordDepth -= by;
        if (recordDepthLow > recordDepth)
            recordDepthLow = recordDepth;
        instChanged = true;
    }
    inline void inc_enqueues (uint64_t by = 1) {
        getThreadStats()->enqueues += by;
        instChanged = true;
    }
    inline void dec_enqueues (uint64_t by = 1) {
        getThreadStats()->enqueues -= by;
        instChanged = true;
    }
    inline void inc_dequeues (uint64_t by = 1) {
        getThreadStats()->dequeues += by;
        instChanged = true;
    }
    inline void dec_dequeues (uint64_t by = 1) {
        getThreadStats()->dequeues -= by;
        instChanged = true;
    }
    inline void inc_txn (uint32_t by = 1) {
        getThreadStats()->txn += by;
        instChanged = true;
    }
    inline void dec_txn (uint32_t by = 1) {
        getThreadStats()->txn -= by;
        instChanged = true;
    }
    inline void inc_txnEnqueues (uint64_t by = 1) {
        getThreadStats()->txnEnqueues += by;
        instChanged = true;
    }
    inline void dec_txnEnqueues (uint64_t by = 1) {
        getThreadStats()->txnEnqueues -= by;
        instChanged = true;
    }
    inline void inc_txnDequeues (uint64_t by = 1) {
        getThreadStats()->txnDequeues += by;
        instChanged = true;
    }
    inline void dec_txnDequeues (uint64_t by = 1) {
        getThreadStats()->txnDequeues -= by;
        instChanged = true;
    }
    inline void inc_txnCommits (uint64_t by = 1) {
        getThreadStats()->txnCommits += by;
        instChanged = true;
    }
    inline void dec_txnCommits (uint64_t by = 1) {
        getThreadStats()->txnCommits -= by;
        instChanged = true;
    }
    inline void inc_txnAborts (uint64_t by = 1) {
        getThreadStats()->txnAborts += by;
        instChanged = true;
    }
    inline void dec_txnAborts (uint64_t by = 1) {
        getThreadStats()->txnAborts -= by;
        instChanged = true;
    }
    inline void inc_outstandingAIOs (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        outstandingAIOs += by;
        if (outstandingAIOsHigh < outstandingAIOs)
            outstandingAIOsHigh = outstandingAIOs;
        instChanged = true;
    }
    inline void dec_outstandingAIOs (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        outstandingAIOs -= by;
        if (outstandingAIOsLow > outstandingAIOs)
            outstandingAIOsLow = outstandingAIOs;
        instChanged = true;
    }
    inline void inc_freeFileCount (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        freeFileCount += by;
        if (freeFileCountHigh < freeFileCount)
            freeFileCountHigh = freeFileCount;
        instChanged = true;
    }
    inline void dec_freeFileCount (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        freeFileCount -= by;
        if (freeFileCountLow > freeFileCount)
            freeFileCountLow = freeFileCount;
        instChanged = true;
    }
    inline void inc_availableFileCount (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        availableFileCount += by;
        if (availableFileCountHigh < availableFileCount)
            availableFileCountHigh = availableFileCount;
        instChanged = true;
    }
    inline void dec_availableFileCount (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        availableFileCount -= by;
        if (availableFileCountLow > availableFileCount)
            availableFileCountLow = availableFileCount;
        instChanged = true;
    }
    inline void inc_writeWaitFailures (uint64_t by = 1) {
        getThreadStats()->writeWaitFailures += by;
        instChanged = true;
    }
    inline void dec_writeWaitFailures (uint64_t by = 1) {
        getThreadStats()->writeWaitFailures -= by;
        instChanged = true;
    }
    inline void inc_writeBusyFailures (uint64_t by = 1) {
        getThreadStats()->writeBusyFailures += by;
        instChanged = true;
    }
    inline void dec_writeBusyFailures (uint64_t by = 1) {
        getThreadStats()->writeBusyFailures -= by;
        instChanged = true;
    }
    inline void inc_readRecordCount (uint64_t by = 1) {
        getThreadStats()->readRecordCount += by;
        instChanged = true;
    }
    inline void dec_readRecordCount (uint64_t by = 1) {
        getThreadStats()->readRecordCount -= by;
        instChanged = true;
    }
    inline void inc_readBusyFailures (uint64_t by = 1) {
        getThreadStats()->readBusyFailures += by;
        instChanged = true;
    }
    inline void dec_readBusyFailures (uint64_t by = 1) {
        getThreadStats()->readBusyFailures -= by;
        instChanged = true;
    }
    inline void inc_writePageCacheDepth (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        writePageCacheDepth += by;
        if (writePageCacheDepthHigh < writePageCacheDepth)
            writePageCacheDepthHigh = writePageCacheDepth;
        instChanged = true;
    }
    inline void dec_writePageCacheDepth (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        writePageCacheDepth -= by;
        if (writePageCacheDepthLow > writePageCacheDepth)
            writePageCacheDepthLow = writePageCacheDepth;
        instChanged = true;
    }
    inline void inc_readPageCacheDepth (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        readPageCacheDepth += by;
        if (readPageCacheDepthHigh < readPageCacheDepth)
            readPageCacheDepthHigh = readPageCacheDepth;
        instChanged = true;
    }
    inline void dec_readPageCacheDepth (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        readPageCacheDepth -= by;
        if (readPageCacheDepthLow > readPageCacheDepth)
            readPageCacheDepthLow = readPageCacheDepth;
        instChanged = true;
    }

};

}}}}}

#endif  /*!_MANAGEMENT_JOURNAL_*/
