
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

#include "qpid/management/Manageable.h"
#include "qpid/management/Buffer.h"
#include "qpid/types/Variant.h"
#include "qpid/amqp_0_10/Codecs.h"
#include "qpid/management/ManagementAgent.h"
#include "Store.h"

#include <iostream>
#include <sstream>

using namespace qmf::com::redhat::rhm::store;
using           qpid::management::ManagementAgent;
using           qpid::management::Manageable;
using           qpid::management::ManagementObject;
using           qpid::management::Args;
using           qpid::management::Mutex;
using           std::string;

string  Store::packageName  = string ("com.redhat.rhm.store");
string  Store::className    = string ("store");
uint8_t Store::md5Sum[MD5_LEN]   =
    {0x18,0xd,0xd4,0x15,0xd3,0x9a,0xf,0xbe,0x3a,0x40,0xe1,0x1b,0x9e,0x5b,0x7e,0x86};

Store::Store (ManagementAgent*, Manageable* _core, ::qpid::management::Manageable* _parent) :
    ManagementObject(_core)
{
    brokerRef = _parent->GetManagementObject ()->getObjectId ();
    location = "";
    defaultInitialFileCount = 0;
    defaultDataFileSize = 0;
    tplIsInitialized = 0;
    tplDirectory = "";
    tplWritePageSize = 0;
    tplWritePages = 0;
    tplInitialFileCount = 0;
    tplDataFileSize = 0;
    tplCurrentFileCount = 0;
    tplTransactionDepth = 0;
    tplTransactionDepthHigh = 0;
    tplTransactionDepthLow  = 0;
    tplOutstandingAIOs = 0;
    tplOutstandingAIOsHigh = 0;
    tplOutstandingAIOsLow  = 0;



    perThreadStatsArray = new struct PerThreadStats*[maxThreads];
    for (int idx = 0; idx < maxThreads; idx++)
        perThreadStatsArray[idx] = 0;

}

Store::~Store ()
{

    for (int idx = 0; idx < maxThreads; idx++)
        if (perThreadStatsArray[idx] != 0)
            delete perThreadStatsArray[idx];
    delete[] perThreadStatsArray;

}

namespace {
    const string NAME("name");
    const string TYPE("type");
    const string ACCESS("access");
    const string IS_INDEX("index");
    const string IS_OPTIONAL("optional");
    const string UNIT("unit");
    const string MIN("min");
    const string MAX("max");
    const string MAXLEN("maxlen");
    const string DESC("desc");
    const string ARGCOUNT("argCount");
    const string ARGS("args");
    const string DIR("dir");
    const string DEFAULT("default");
}

void Store::registerSelf(ManagementAgent* agent)
{
    agent->registerClass(packageName, className, md5Sum, writeSchema);
}

void Store::writeSchema (std::string& schema)
{
    const int _bufSize=65536;
    char _msgChars[_bufSize];
    ::qpid::management::Buffer buf(_msgChars, _bufSize);
    ::qpid::types::Variant::Map ft;

    // Schema class header:
    buf.putOctet       (CLASS_KIND_TABLE);
    buf.putShortString (packageName); // Package Name
    buf.putShortString (className);   // Class Name
    buf.putBin128      (md5Sum);      // Schema Hash
    buf.putShort       (11); // Config Element Count
    buf.putShort       (9); // Inst Element Count
    buf.putShort       (0); // Method Count

    // Properties
    ft.clear();
    ft[NAME] = "brokerRef";
    ft[TYPE] = TYPE_REF;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 1;
    ft[IS_OPTIONAL] = 0;
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "location";
    ft[TYPE] = TYPE_SSTR;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[DESC] = "Logical directory on disk";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "defaultInitialFileCount";
    ft[TYPE] = TYPE_U16;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[UNIT] = "file";
    ft[DESC] = "Default number of files initially allocated to each journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "defaultDataFileSize";
    ft[TYPE] = TYPE_U32;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[UNIT] = "RdPg";
    ft[DESC] = "Default size of each journal data file";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "tplIsInitialized";
    ft[TYPE] = TYPE_BOOL;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[DESC] = "Transaction prepared list has been initialized by a transactional prepare";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "tplDirectory";
    ft[TYPE] = TYPE_SSTR;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[DESC] = "Transaction prepared list directory";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "tplWritePageSize";
    ft[TYPE] = TYPE_U32;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[UNIT] = "byte";
    ft[DESC] = "Page size in transaction prepared list write-page-cache";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "tplWritePages";
    ft[TYPE] = TYPE_U32;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[UNIT] = "wpage";
    ft[DESC] = "Number of pages in transaction prepared list write-page-cache";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "tplInitialFileCount";
    ft[TYPE] = TYPE_U16;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[UNIT] = "file";
    ft[DESC] = "Number of files initially allocated to transaction prepared list journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "tplDataFileSize";
    ft[TYPE] = TYPE_U32;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[UNIT] = "byte";
    ft[DESC] = "Size of each journal data file in transaction prepared list journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "tplCurrentFileCount";
    ft[TYPE] = TYPE_U32;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[UNIT] = "file";
    ft[DESC] = "Number of files currently allocated to transaction prepared list journal";
    buf.putMap(ft);


    // Statistics
    ft.clear();
    ft[NAME] = "tplTransactionDepth";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "txn";
    ft[DESC] = "Number of currently enqueued prepared transactions";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "tplTransactionDepthHigh";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "txn";
    ft[DESC] = "Number of currently enqueued prepared transactions (High)";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "tplTransactionDepthLow";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "txn";
    ft[DESC] = "Number of currently enqueued prepared transactions (Low)";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "tplTxnPrepares";
    ft[TYPE] = TYPE_U64;
    ft[UNIT] = "record";
    ft[DESC] = "Total transaction prepares on transaction prepared list";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "tplTxnCommits";
    ft[TYPE] = TYPE_U64;
    ft[UNIT] = "record";
    ft[DESC] = "Total transaction commits on transaction prepared list";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "tplTxnAborts";
    ft[TYPE] = TYPE_U64;
    ft[UNIT] = "record";
    ft[DESC] = "Total transaction aborts on transaction prepared list";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "tplOutstandingAIOs";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "aio_op";
    ft[DESC] = "Number of currently outstanding AIO requests in Async IO system";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "tplOutstandingAIOsHigh";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "aio_op";
    ft[DESC] = "Number of currently outstanding AIO requests in Async IO system (High)";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "tplOutstandingAIOsLow";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "aio_op";
    ft[DESC] = "Number of currently outstanding AIO requests in Async IO system (Low)";
    buf.putMap(ft);


    // Methods

    {
        uint32_t _len = buf.getPosition();
        buf.reset();
        buf.getRawData(schema, _len);
    }
}


void Store::aggregatePerThreadStats(struct PerThreadStats* totals) const
{
    totals->tplTxnPrepares = 0;
    totals->tplTxnCommits = 0;
    totals->tplTxnAborts = 0;

    for (int idx = 0; idx < maxThreads; idx++) {
        struct PerThreadStats* threadStats = perThreadStatsArray[idx];
        if (threadStats != 0) {
            totals->tplTxnPrepares += threadStats->tplTxnPrepares;
            totals->tplTxnCommits += threadStats->tplTxnCommits;
            totals->tplTxnAborts += threadStats->tplTxnAborts;

        }
    }
}



uint32_t Store::writePropertiesSize() const
{
    uint32_t size = writeTimestampsSize();

    size += 16;  // brokerRef
    size += (1 + location.length());  // location
    size += 2;  // defaultInitialFileCount
    size += 4;  // defaultDataFileSize
    size += 1;  // tplIsInitialized
    size += (1 + tplDirectory.length());  // tplDirectory
    size += 4;  // tplWritePageSize
    size += 4;  // tplWritePages
    size += 2;  // tplInitialFileCount
    size += 4;  // tplDataFileSize
    size += 4;  // tplCurrentFileCount

    return size;
}

void Store::readProperties (const std::string& _sBuf)
{
    char *_tmpBuf = new char[_sBuf.length()];
    memcpy(_tmpBuf, _sBuf.data(), _sBuf.length());
    ::qpid::management::Buffer buf(_tmpBuf, _sBuf.length());
    Mutex::ScopedLock mutex(accessLock);

    {
        std::string _tbuf;
        buf.getRawData(_tbuf, writeTimestampsSize());
        readTimestamps(_tbuf);
    }


    {std::string _s; buf.getRawData(_s, brokerRef.encodedSize()); brokerRef.decode(_s);};
    buf.getShortString(location);
    defaultInitialFileCount = buf.getShort();
    defaultDataFileSize = buf.getLong();
    tplIsInitialized = buf.getOctet()==1;
    buf.getShortString(tplDirectory);
    tplWritePageSize = buf.getLong();
    tplWritePages = buf.getLong();
    tplInitialFileCount = buf.getShort();
    tplDataFileSize = buf.getLong();
    tplCurrentFileCount = buf.getLong();


    delete [] _tmpBuf;
}

void Store::writeProperties (std::string& _sBuf) const
{
    const int _bufSize=65536;
    char _msgChars[_bufSize];
    ::qpid::management::Buffer buf(_msgChars, _bufSize);

    Mutex::ScopedLock mutex(accessLock);
    configChanged = false;

    {
        std::string _tbuf;
        writeTimestamps(_tbuf);
        buf.putRawData(_tbuf);
    }



    {std::string _s; brokerRef.encode(_s); buf.putRawData(_s);};
    buf.putShortString(location);
    buf.putShort(defaultInitialFileCount);
    buf.putLong(defaultDataFileSize);
    buf.putOctet(tplIsInitialized?1:0);
    buf.putShortString(tplDirectory);
    buf.putLong(tplWritePageSize);
    buf.putLong(tplWritePages);
    buf.putShort(tplInitialFileCount);
    buf.putLong(tplDataFileSize);
    buf.putLong(tplCurrentFileCount);


    uint32_t _bufLen = buf.getPosition();
    buf.reset();

    buf.getRawData(_sBuf, _bufLen);
}

void Store::writeStatistics (std::string& _sBuf, bool skipHeaders)
{
    const int _bufSize=65536;
    char _msgChars[_bufSize];
    ::qpid::management::Buffer buf(_msgChars, _bufSize);

    Mutex::ScopedLock mutex(accessLock);
    instChanged = false;


    struct PerThreadStats totals;
    aggregatePerThreadStats(&totals);


    if (!skipHeaders) {
        std::string _tbuf;
        writeTimestamps (_tbuf);
        buf.putRawData(_tbuf);
    }

    buf.putLong(tplTransactionDepth);
    buf.putLong(tplTransactionDepthHigh);
    buf.putLong(tplTransactionDepthLow);
    buf.putLongLong(totals.tplTxnPrepares);
    buf.putLongLong(totals.tplTxnCommits);
    buf.putLongLong(totals.tplTxnAborts);
    buf.putLong(tplOutstandingAIOs);
    buf.putLong(tplOutstandingAIOsHigh);
    buf.putLong(tplOutstandingAIOsLow);


    // Maintenance of hi-lo statistics
    tplTransactionDepthHigh = tplTransactionDepth;
    tplTransactionDepthLow  = tplTransactionDepth;
    tplOutstandingAIOsHigh = tplOutstandingAIOs;
    tplOutstandingAIOsLow  = tplOutstandingAIOs;



    uint32_t _bufLen = buf.getPosition();
    buf.reset();

    buf.getRawData(_sBuf, _bufLen);
}

void Store::doMethod (string&, const string&, string& outStr, const string&)
{
    Manageable::status_t status = Manageable::STATUS_UNKNOWN_METHOD;
    std::string          text;

    bool _matched = false;

    const int _bufSize=65536;
    char _msgChars[_bufSize];
    ::qpid::management::Buffer outBuf(_msgChars, _bufSize);



    if (!_matched) {
        outBuf.putLong(status);
        outBuf.putShortString(Manageable::StatusText(status, text));
    }

    uint32_t _bufLen = outBuf.getPosition();
    outBuf.reset();

    outBuf.getRawData(outStr, _bufLen);
}

std::string Store::getKey() const
{
    std::stringstream key;

    key << brokerRef.getV2Key();
    return key.str();
}



void Store::mapEncodeValues (::qpid::types::Variant::Map& _map,
                                              bool includeProperties,
                                              bool includeStatistics)
{
    using namespace ::qpid::types;
    Mutex::ScopedLock mutex(accessLock);

    if (includeProperties) {
        configChanged = false;
    _map["brokerRef"] = ::qpid::types::Variant(brokerRef);
    _map["location"] = ::qpid::types::Variant(location);
    _map["defaultInitialFileCount"] = ::qpid::types::Variant(defaultInitialFileCount);
    _map["defaultDataFileSize"] = ::qpid::types::Variant(defaultDataFileSize);
    _map["tplIsInitialized"] = ::qpid::types::Variant(tplIsInitialized);
    _map["tplDirectory"] = ::qpid::types::Variant(tplDirectory);
    _map["tplWritePageSize"] = ::qpid::types::Variant(tplWritePageSize);
    _map["tplWritePages"] = ::qpid::types::Variant(tplWritePages);
    _map["tplInitialFileCount"] = ::qpid::types::Variant(tplInitialFileCount);
    _map["tplDataFileSize"] = ::qpid::types::Variant(tplDataFileSize);
    _map["tplCurrentFileCount"] = ::qpid::types::Variant(tplCurrentFileCount);

    }

    if (includeStatistics) {
        instChanged = false;


        struct PerThreadStats totals;
        aggregatePerThreadStats(&totals);



    _map["tplTransactionDepth"] = ::qpid::types::Variant(tplTransactionDepth);
    _map["tplTransactionDepthHigh"] = ::qpid::types::Variant(tplTransactionDepthHigh);
    _map["tplTransactionDepthLow"] = ::qpid::types::Variant(tplTransactionDepthLow);
    _map["tplTxnPrepares"] = ::qpid::types::Variant(totals.tplTxnPrepares);
    _map["tplTxnCommits"] = ::qpid::types::Variant(totals.tplTxnCommits);
    _map["tplTxnAborts"] = ::qpid::types::Variant(totals.tplTxnAborts);
    _map["tplOutstandingAIOs"] = ::qpid::types::Variant(tplOutstandingAIOs);
    _map["tplOutstandingAIOsHigh"] = ::qpid::types::Variant(tplOutstandingAIOsHigh);
    _map["tplOutstandingAIOsLow"] = ::qpid::types::Variant(tplOutstandingAIOsLow);


    // Maintenance of hi-lo statistics
    tplTransactionDepthHigh = tplTransactionDepth;
    tplTransactionDepthLow  = tplTransactionDepth;
    tplOutstandingAIOsHigh = tplOutstandingAIOs;
    tplOutstandingAIOsLow  = tplOutstandingAIOs;


    }
}

void Store::mapDecodeValues (const ::qpid::types::Variant::Map& _map)
{
    ::qpid::types::Variant::Map::const_iterator _i;
    Mutex::ScopedLock mutex(accessLock);

    if ((_i = _map.find("brokerRef")) != _map.end()) {
        brokerRef = _i->second;
    }
    if ((_i = _map.find("location")) != _map.end()) {
        location = (_i->second).getString();
    }
    if ((_i = _map.find("defaultInitialFileCount")) != _map.end()) {
        defaultInitialFileCount = _i->second;
    }
    if ((_i = _map.find("defaultDataFileSize")) != _map.end()) {
        defaultDataFileSize = _i->second;
    }
    if ((_i = _map.find("tplIsInitialized")) != _map.end()) {
        tplIsInitialized = _i->second;
    }
    if ((_i = _map.find("tplDirectory")) != _map.end()) {
        tplDirectory = (_i->second).getString();
    }
    if ((_i = _map.find("tplWritePageSize")) != _map.end()) {
        tplWritePageSize = _i->second;
    }
    if ((_i = _map.find("tplWritePages")) != _map.end()) {
        tplWritePages = _i->second;
    }
    if ((_i = _map.find("tplInitialFileCount")) != _map.end()) {
        tplInitialFileCount = _i->second;
    }
    if ((_i = _map.find("tplDataFileSize")) != _map.end()) {
        tplDataFileSize = _i->second;
    }
    if ((_i = _map.find("tplCurrentFileCount")) != _map.end()) {
        tplCurrentFileCount = _i->second;
    }

}

void Store::doMethod (string&, const ::qpid::types::Variant::Map&, ::qpid::types::Variant::Map& outMap, const string&)
{
    Manageable::status_t status = Manageable::STATUS_UNKNOWN_METHOD;
    std::string          text;


    outMap["_status_code"] = (uint32_t) status;
    outMap["_status_text"] = Manageable::StatusText(status, text);
}
