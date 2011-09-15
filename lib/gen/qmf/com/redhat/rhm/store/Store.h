
#ifndef _MANAGEMENT_STORE_
#define _MANAGEMENT_STORE_

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


class Store : public ::qpid::management::ManagementObject
{
  private:

    static std::string packageName;
    static std::string className;
    static uint8_t     md5Sum[MD5_LEN];


    // Properties
    ::qpid::management::ObjectId brokerRef;
    std::string location;
    uint16_t defaultInitialFileCount;
    uint32_t defaultDataFileSize;
    bool tplIsInitialized;
    std::string tplDirectory;
    uint32_t tplWritePageSize;
    uint32_t tplWritePages;
    uint16_t tplInitialFileCount;
    uint32_t tplDataFileSize;
    uint32_t tplCurrentFileCount;

    // Statistics
    uint32_t  tplTransactionDepth;
    uint32_t  tplTransactionDepthHigh;
    uint32_t  tplTransactionDepthLow;
    uint32_t  tplOutstandingAIOs;
    uint32_t  tplOutstandingAIOsHigh;
    uint32_t  tplOutstandingAIOsLow;


    // Per-Thread Statistics
    struct PerThreadStats {
        uint64_t  tplTxnPrepares;
        uint64_t  tplTxnCommits;
        uint64_t  tplTxnAborts;

    };

    struct PerThreadStats** perThreadStatsArray;

    inline struct PerThreadStats* getThreadStats() {
        int idx = getThreadIndex();
        struct PerThreadStats* threadStats = perThreadStatsArray[idx];
        if (threadStats == 0) {
            threadStats = new(PerThreadStats);
            perThreadStatsArray[idx] = threadStats;
            threadStats->tplTxnPrepares = 0;
            threadStats->tplTxnCommits = 0;
            threadStats->tplTxnAborts = 0;

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


    Store(::qpid::management::ManagementAgent* agent,
                            ::qpid::management::Manageable* coreObject, ::qpid::management::Manageable* _parent);
    ~Store();

    

    static void registerSelf(::qpid::management::ManagementAgent* agent);
    std::string& getPackageName() const { return packageName; }
    std::string& getClassName() const { return className; }
    uint8_t* getMd5Sum() const { return md5Sum; }

    // Method IDs

    // Accessor Methods
    inline void set_brokerRef (const ::qpid::management::ObjectId& val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        brokerRef = val;
        configChanged = true;
    }
    inline const ::qpid::management::ObjectId& get_brokerRef() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return brokerRef;
    }
    inline void set_location (const std::string& val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        location = val;
        configChanged = true;
    }
    inline const std::string& get_location() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return location;
    }
    inline void set_defaultInitialFileCount (uint16_t val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        defaultInitialFileCount = val;
        configChanged = true;
    }
    inline uint16_t get_defaultInitialFileCount() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return defaultInitialFileCount;
    }
    inline void set_defaultDataFileSize (uint32_t val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        defaultDataFileSize = val;
        configChanged = true;
    }
    inline uint32_t get_defaultDataFileSize() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return defaultDataFileSize;
    }
    inline void set_tplIsInitialized (bool val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        tplIsInitialized = val;
        configChanged = true;
    }
    inline bool get_tplIsInitialized() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return tplIsInitialized;
    }
    inline void set_tplDirectory (const std::string& val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        tplDirectory = val;
        configChanged = true;
    }
    inline const std::string& get_tplDirectory() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return tplDirectory;
    }
    inline void set_tplWritePageSize (uint32_t val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        tplWritePageSize = val;
        configChanged = true;
    }
    inline uint32_t get_tplWritePageSize() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return tplWritePageSize;
    }
    inline void set_tplWritePages (uint32_t val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        tplWritePages = val;
        configChanged = true;
    }
    inline uint32_t get_tplWritePages() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return tplWritePages;
    }
    inline void set_tplInitialFileCount (uint16_t val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        tplInitialFileCount = val;
        configChanged = true;
    }
    inline uint16_t get_tplInitialFileCount() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return tplInitialFileCount;
    }
    inline void set_tplDataFileSize (uint32_t val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        tplDataFileSize = val;
        configChanged = true;
    }
    inline uint32_t get_tplDataFileSize() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return tplDataFileSize;
    }
    inline void set_tplCurrentFileCount (uint32_t val) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        tplCurrentFileCount = val;
        configChanged = true;
    }
    inline uint32_t get_tplCurrentFileCount() {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        return tplCurrentFileCount;
    }
    inline void inc_tplTransactionDepth (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        tplTransactionDepth += by;
        if (tplTransactionDepthHigh < tplTransactionDepth)
            tplTransactionDepthHigh = tplTransactionDepth;
        instChanged = true;
    }
    inline void dec_tplTransactionDepth (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        tplTransactionDepth -= by;
        if (tplTransactionDepthLow > tplTransactionDepth)
            tplTransactionDepthLow = tplTransactionDepth;
        instChanged = true;
    }
    inline void inc_tplTxnPrepares (uint64_t by = 1) {
        getThreadStats()->tplTxnPrepares += by;
        instChanged = true;
    }
    inline void dec_tplTxnPrepares (uint64_t by = 1) {
        getThreadStats()->tplTxnPrepares -= by;
        instChanged = true;
    }
    inline void inc_tplTxnCommits (uint64_t by = 1) {
        getThreadStats()->tplTxnCommits += by;
        instChanged = true;
    }
    inline void dec_tplTxnCommits (uint64_t by = 1) {
        getThreadStats()->tplTxnCommits -= by;
        instChanged = true;
    }
    inline void inc_tplTxnAborts (uint64_t by = 1) {
        getThreadStats()->tplTxnAborts += by;
        instChanged = true;
    }
    inline void dec_tplTxnAborts (uint64_t by = 1) {
        getThreadStats()->tplTxnAborts -= by;
        instChanged = true;
    }
    inline void inc_tplOutstandingAIOs (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        tplOutstandingAIOs += by;
        if (tplOutstandingAIOsHigh < tplOutstandingAIOs)
            tplOutstandingAIOsHigh = tplOutstandingAIOs;
        instChanged = true;
    }
    inline void dec_tplOutstandingAIOs (uint32_t by = 1) {
        ::qpid::management::Mutex::ScopedLock mutex(accessLock);
        tplOutstandingAIOs -= by;
        if (tplOutstandingAIOsLow > tplOutstandingAIOs)
            tplOutstandingAIOsLow = tplOutstandingAIOs;
        instChanged = true;
    }

};

}}}}}

#endif  /*!_MANAGEMENT_STORE_*/
