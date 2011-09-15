
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
#include "Journal.h"
#include "ArgsJournalExpand.h"

#include <iostream>
#include <sstream>

using namespace qmf::com::redhat::rhm::store;
using           qpid::management::ManagementAgent;
using           qpid::management::Manageable;
using           qpid::management::ManagementObject;
using           qpid::management::Args;
using           qpid::management::Mutex;
using           std::string;

string  Journal::packageName  = string ("com.redhat.rhm.store");
string  Journal::className    = string ("journal");
uint8_t Journal::md5Sum[MD5_LEN]   =
    {0x9,0xc2,0xe2,0xfc,0xcc,0x2e,0x72,0xaa,0x3f,0xce,0x36,0x28,0xce,0x81,0xc,0xd4};

Journal::Journal (ManagementAgent*, Manageable* _core) :
    ManagementObject(_core)
{
    
    queueRef = ::qpid::management::ObjectId();
    name = "";
    directory = "";
    baseFileName = "";
    writePageSize = 0;
    writePages = 0;
    readPageSize = 0;
    readPages = 0;
    initialFileCount = 0;
    autoExpand = 0;
    currentFileCount = 0;
    maxFileCount = 0;
    dataFileSize = 0;
    recordDepth = 0;
    recordDepthHigh = 0;
    recordDepthLow  = 0;
    outstandingAIOs = 0;
    outstandingAIOsHigh = 0;
    outstandingAIOsLow  = 0;
    freeFileCount = 0;
    freeFileCountHigh = 0;
    freeFileCountLow  = 0;
    availableFileCount = 0;
    availableFileCountHigh = 0;
    availableFileCountLow  = 0;
    writePageCacheDepth = 0;
    writePageCacheDepthHigh = 0;
    writePageCacheDepthLow  = 0;
    readPageCacheDepth = 0;
    readPageCacheDepthHigh = 0;
    readPageCacheDepthLow  = 0;



    perThreadStatsArray = new struct PerThreadStats*[maxThreads];
    for (int idx = 0; idx < maxThreads; idx++)
        perThreadStatsArray[idx] = 0;

}

Journal::~Journal ()
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

void Journal::registerSelf(ManagementAgent* agent)
{
    agent->registerClass(packageName, className, md5Sum, writeSchema);
}

void Journal::writeSchema (std::string& schema)
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
    buf.putShort       (13); // Config Element Count
    buf.putShort       (29); // Inst Element Count
    buf.putShort       (1); // Method Count

    // Properties
    ft.clear();
    ft[NAME] = "queueRef";
    ft[TYPE] = TYPE_REF;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "name";
    ft[TYPE] = TYPE_SSTR;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 1;
    ft[IS_OPTIONAL] = 0;
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "directory";
    ft[TYPE] = TYPE_SSTR;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[DESC] = "Directory containing journal files";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "baseFileName";
    ft[TYPE] = TYPE_SSTR;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[DESC] = "Base filename prefix for journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "writePageSize";
    ft[TYPE] = TYPE_U32;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[UNIT] = "byte";
    ft[DESC] = "Page size in write-page-cache";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "writePages";
    ft[TYPE] = TYPE_U32;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[UNIT] = "wpage";
    ft[DESC] = "Number of pages in write-page-cache";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "readPageSize";
    ft[TYPE] = TYPE_U32;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[UNIT] = "byte";
    ft[DESC] = "Page size in read-page-cache";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "readPages";
    ft[TYPE] = TYPE_U32;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[UNIT] = "rpage";
    ft[DESC] = "Number of pages in read-page-cache";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "initialFileCount";
    ft[TYPE] = TYPE_U16;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[UNIT] = "file";
    ft[DESC] = "Number of files initially allocated to this journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "autoExpand";
    ft[TYPE] = TYPE_BOOL;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[DESC] = "Auto-expand enabled";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "currentFileCount";
    ft[TYPE] = TYPE_U16;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[UNIT] = "file";
    ft[DESC] = "Number of files currently allocated to this journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "maxFileCount";
    ft[TYPE] = TYPE_U16;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[UNIT] = "file";
    ft[DESC] = "Max number of files allowed for this journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "dataFileSize";
    ft[TYPE] = TYPE_U32;
    ft[ACCESS] = ACCESS_RO;
    ft[IS_INDEX] = 0;
    ft[IS_OPTIONAL] = 0;
    ft[UNIT] = "byte";
    ft[DESC] = "Size of each journal data file";
    buf.putMap(ft);


    // Statistics
    ft.clear();
    ft[NAME] = "recordDepth";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "record";
    ft[DESC] = "Number of currently enqueued records (durable messages)";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "recordDepthHigh";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "record";
    ft[DESC] = "Number of currently enqueued records (durable messages) (High)";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "recordDepthLow";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "record";
    ft[DESC] = "Number of currently enqueued records (durable messages) (Low)";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "enqueues";
    ft[TYPE] = TYPE_U64;
    ft[UNIT] = "record";
    ft[DESC] = "Total enqueued records on journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "dequeues";
    ft[TYPE] = TYPE_U64;
    ft[UNIT] = "record";
    ft[DESC] = "Total dequeued records on journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "txn";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "record";
    ft[DESC] = "Total open transactions (xids) on journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "txnEnqueues";
    ft[TYPE] = TYPE_U64;
    ft[UNIT] = "record";
    ft[DESC] = "Total transactional enqueued records on journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "txnDequeues";
    ft[TYPE] = TYPE_U64;
    ft[UNIT] = "record";
    ft[DESC] = "Total transactional dequeued records on journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "txnCommits";
    ft[TYPE] = TYPE_U64;
    ft[UNIT] = "record";
    ft[DESC] = "Total transactional commit records on journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "txnAborts";
    ft[TYPE] = TYPE_U64;
    ft[UNIT] = "record";
    ft[DESC] = "Total transactional abort records on journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "outstandingAIOs";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "aio_op";
    ft[DESC] = "Number of currently outstanding AIO requests in Async IO system";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "outstandingAIOsHigh";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "aio_op";
    ft[DESC] = "Number of currently outstanding AIO requests in Async IO system (High)";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "outstandingAIOsLow";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "aio_op";
    ft[DESC] = "Number of currently outstanding AIO requests in Async IO system (Low)";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "freeFileCount";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "file";
    ft[DESC] = "Number of files free on this journal. Includes free files trapped in holes.";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "freeFileCountHigh";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "file";
    ft[DESC] = "Number of files free on this journal. Includes free files trapped in holes. (High)";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "freeFileCountLow";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "file";
    ft[DESC] = "Number of files free on this journal. Includes free files trapped in holes. (Low)";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "availableFileCount";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "file";
    ft[DESC] = "Number of files available to be written.  Excluding holes";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "availableFileCountHigh";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "file";
    ft[DESC] = "Number of files available to be written.  Excluding holes (High)";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "availableFileCountLow";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "file";
    ft[DESC] = "Number of files available to be written.  Excluding holes (Low)";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "writeWaitFailures";
    ft[TYPE] = TYPE_U64;
    ft[UNIT] = "record";
    ft[DESC] = "AIO Wait failures on write";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "writeBusyFailures";
    ft[TYPE] = TYPE_U64;
    ft[UNIT] = "record";
    ft[DESC] = "AIO Busy failures on write";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "readRecordCount";
    ft[TYPE] = TYPE_U64;
    ft[UNIT] = "record";
    ft[DESC] = "Records read from the journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "readBusyFailures";
    ft[TYPE] = TYPE_U64;
    ft[UNIT] = "record";
    ft[DESC] = "AIO Busy failures on read";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "writePageCacheDepth";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "wpage";
    ft[DESC] = "Current depth of write-page-cache";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "writePageCacheDepthHigh";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "wpage";
    ft[DESC] = "Current depth of write-page-cache (High)";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "writePageCacheDepthLow";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "wpage";
    ft[DESC] = "Current depth of write-page-cache (Low)";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "readPageCacheDepth";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "rpage";
    ft[DESC] = "Current depth of read-page-cache";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "readPageCacheDepthHigh";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "rpage";
    ft[DESC] = "Current depth of read-page-cache (High)";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "readPageCacheDepthLow";
    ft[TYPE] = TYPE_U32;
    ft[UNIT] = "rpage";
    ft[DESC] = "Current depth of read-page-cache (Low)";
    buf.putMap(ft);


    // Methods
    ft.clear();
    ft[NAME] =  "expand";
    ft[ARGCOUNT] = 1;
    ft[DESC] = "Increase number of files allocated for this journal";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "by";
    ft[TYPE] = TYPE_U32;
    ft[DIR] = "I";
    ft[DESC] = "Number of files to increase journal size by";
    buf.putMap(ft);


    {
        uint32_t _len = buf.getPosition();
        buf.reset();
        buf.getRawData(schema, _len);
    }
}


void Journal::aggregatePerThreadStats(struct PerThreadStats* totals) const
{
    totals->enqueues = 0;
    totals->dequeues = 0;
    totals->txn = 0;
    totals->txnEnqueues = 0;
    totals->txnDequeues = 0;
    totals->txnCommits = 0;
    totals->txnAborts = 0;
    totals->writeWaitFailures = 0;
    totals->writeBusyFailures = 0;
    totals->readRecordCount = 0;
    totals->readBusyFailures = 0;

    for (int idx = 0; idx < maxThreads; idx++) {
        struct PerThreadStats* threadStats = perThreadStatsArray[idx];
        if (threadStats != 0) {
            totals->enqueues += threadStats->enqueues;
            totals->dequeues += threadStats->dequeues;
            totals->txn += threadStats->txn;
            totals->txnEnqueues += threadStats->txnEnqueues;
            totals->txnDequeues += threadStats->txnDequeues;
            totals->txnCommits += threadStats->txnCommits;
            totals->txnAborts += threadStats->txnAborts;
            totals->writeWaitFailures += threadStats->writeWaitFailures;
            totals->writeBusyFailures += threadStats->writeBusyFailures;
            totals->readRecordCount += threadStats->readRecordCount;
            totals->readBusyFailures += threadStats->readBusyFailures;

        }
    }
}



uint32_t Journal::writePropertiesSize() const
{
    uint32_t size = writeTimestampsSize();

    size += 16;  // queueRef
    size += (1 + name.length());  // name
    size += (1 + directory.length());  // directory
    size += (1 + baseFileName.length());  // baseFileName
    size += 4;  // writePageSize
    size += 4;  // writePages
    size += 4;  // readPageSize
    size += 4;  // readPages
    size += 2;  // initialFileCount
    size += 1;  // autoExpand
    size += 2;  // currentFileCount
    size += 2;  // maxFileCount
    size += 4;  // dataFileSize

    return size;
}

void Journal::readProperties (const std::string& _sBuf)
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


    {std::string _s; buf.getRawData(_s, queueRef.encodedSize()); queueRef.decode(_s);};
    buf.getShortString(name);
    buf.getShortString(directory);
    buf.getShortString(baseFileName);
    writePageSize = buf.getLong();
    writePages = buf.getLong();
    readPageSize = buf.getLong();
    readPages = buf.getLong();
    initialFileCount = buf.getShort();
    autoExpand = buf.getOctet()==1;
    currentFileCount = buf.getShort();
    maxFileCount = buf.getShort();
    dataFileSize = buf.getLong();


    delete [] _tmpBuf;
}

void Journal::writeProperties (std::string& _sBuf) const
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



    {std::string _s; queueRef.encode(_s); buf.putRawData(_s);};
    buf.putShortString(name);
    buf.putShortString(directory);
    buf.putShortString(baseFileName);
    buf.putLong(writePageSize);
    buf.putLong(writePages);
    buf.putLong(readPageSize);
    buf.putLong(readPages);
    buf.putShort(initialFileCount);
    buf.putOctet(autoExpand?1:0);
    buf.putShort(currentFileCount);
    buf.putShort(maxFileCount);
    buf.putLong(dataFileSize);


    uint32_t _bufLen = buf.getPosition();
    buf.reset();

    buf.getRawData(_sBuf, _bufLen);
}

void Journal::writeStatistics (std::string& _sBuf, bool skipHeaders)
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

    buf.putLong(recordDepth);
    buf.putLong(recordDepthHigh);
    buf.putLong(recordDepthLow);
    buf.putLongLong(totals.enqueues);
    buf.putLongLong(totals.dequeues);
    buf.putLong(totals.txn);
    buf.putLongLong(totals.txnEnqueues);
    buf.putLongLong(totals.txnDequeues);
    buf.putLongLong(totals.txnCommits);
    buf.putLongLong(totals.txnAborts);
    buf.putLong(outstandingAIOs);
    buf.putLong(outstandingAIOsHigh);
    buf.putLong(outstandingAIOsLow);
    buf.putLong(freeFileCount);
    buf.putLong(freeFileCountHigh);
    buf.putLong(freeFileCountLow);
    buf.putLong(availableFileCount);
    buf.putLong(availableFileCountHigh);
    buf.putLong(availableFileCountLow);
    buf.putLongLong(totals.writeWaitFailures);
    buf.putLongLong(totals.writeBusyFailures);
    buf.putLongLong(totals.readRecordCount);
    buf.putLongLong(totals.readBusyFailures);
    buf.putLong(writePageCacheDepth);
    buf.putLong(writePageCacheDepthHigh);
    buf.putLong(writePageCacheDepthLow);
    buf.putLong(readPageCacheDepth);
    buf.putLong(readPageCacheDepthHigh);
    buf.putLong(readPageCacheDepthLow);


    // Maintenance of hi-lo statistics
    recordDepthHigh = recordDepth;
    recordDepthLow  = recordDepth;
    outstandingAIOsHigh = outstandingAIOs;
    outstandingAIOsLow  = outstandingAIOs;
    freeFileCountHigh = freeFileCount;
    freeFileCountLow  = freeFileCount;
    availableFileCountHigh = availableFileCount;
    availableFileCountLow  = availableFileCount;
    writePageCacheDepthHigh = writePageCacheDepth;
    writePageCacheDepthLow  = writePageCacheDepth;
    readPageCacheDepthHigh = readPageCacheDepth;
    readPageCacheDepthLow  = readPageCacheDepth;



    uint32_t _bufLen = buf.getPosition();
    buf.reset();

    buf.getRawData(_sBuf, _bufLen);
}

void Journal::doMethod (string& methodName, const string& inStr, string& outStr, const string& userId)
{
    Manageable::status_t status = Manageable::STATUS_UNKNOWN_METHOD;
    std::string          text;

    bool _matched = false;

    const int _bufSize=65536;
    char _msgChars[_bufSize];
    ::qpid::management::Buffer outBuf(_msgChars, _bufSize);


    char *_tmpBuf = new char[inStr.length()];
    memcpy(_tmpBuf, inStr.data(), inStr.length());
    ::qpid::management::Buffer inBuf(_tmpBuf, inStr.length());

    if (methodName == "expand") {
        _matched = true;
        ArgsJournalExpand ioArgs;
        ioArgs.i_by = inBuf.getLong();
        bool allow = coreObject->AuthorizeMethod(METHOD_EXPAND, ioArgs, userId);
        if (allow)
            status = coreObject->ManagementMethod (METHOD_EXPAND, ioArgs, text);
        else
            status = Manageable::STATUS_FORBIDDEN;
        outBuf.putLong        (status);
        outBuf.putMediumString(::qpid::management::Manageable::StatusText (status, text));
    }

    delete [] _tmpBuf;


    if (!_matched) {
        outBuf.putLong(status);
        outBuf.putShortString(Manageable::StatusText(status, text));
    }

    uint32_t _bufLen = outBuf.getPosition();
    outBuf.reset();

    outBuf.getRawData(outStr, _bufLen);
}

std::string Journal::getKey() const
{
    std::stringstream key;

    key << name;
    return key.str();
}



void Journal::mapEncodeValues (::qpid::types::Variant::Map& _map,
                                              bool includeProperties,
                                              bool includeStatistics)
{
    using namespace ::qpid::types;
    Mutex::ScopedLock mutex(accessLock);

    if (includeProperties) {
        configChanged = false;
    _map["queueRef"] = ::qpid::types::Variant(queueRef);
    _map["name"] = ::qpid::types::Variant(name);
    _map["directory"] = ::qpid::types::Variant(directory);
    _map["baseFileName"] = ::qpid::types::Variant(baseFileName);
    _map["writePageSize"] = ::qpid::types::Variant(writePageSize);
    _map["writePages"] = ::qpid::types::Variant(writePages);
    _map["readPageSize"] = ::qpid::types::Variant(readPageSize);
    _map["readPages"] = ::qpid::types::Variant(readPages);
    _map["initialFileCount"] = ::qpid::types::Variant(initialFileCount);
    _map["autoExpand"] = ::qpid::types::Variant(autoExpand);
    _map["currentFileCount"] = ::qpid::types::Variant(currentFileCount);
    _map["maxFileCount"] = ::qpid::types::Variant(maxFileCount);
    _map["dataFileSize"] = ::qpid::types::Variant(dataFileSize);

    }

    if (includeStatistics) {
        instChanged = false;


        struct PerThreadStats totals;
        aggregatePerThreadStats(&totals);



    _map["recordDepth"] = ::qpid::types::Variant(recordDepth);
    _map["recordDepthHigh"] = ::qpid::types::Variant(recordDepthHigh);
    _map["recordDepthLow"] = ::qpid::types::Variant(recordDepthLow);
    _map["enqueues"] = ::qpid::types::Variant(totals.enqueues);
    _map["dequeues"] = ::qpid::types::Variant(totals.dequeues);
    _map["txn"] = ::qpid::types::Variant(totals.txn);
    _map["txnEnqueues"] = ::qpid::types::Variant(totals.txnEnqueues);
    _map["txnDequeues"] = ::qpid::types::Variant(totals.txnDequeues);
    _map["txnCommits"] = ::qpid::types::Variant(totals.txnCommits);
    _map["txnAborts"] = ::qpid::types::Variant(totals.txnAborts);
    _map["outstandingAIOs"] = ::qpid::types::Variant(outstandingAIOs);
    _map["outstandingAIOsHigh"] = ::qpid::types::Variant(outstandingAIOsHigh);
    _map["outstandingAIOsLow"] = ::qpid::types::Variant(outstandingAIOsLow);
    _map["freeFileCount"] = ::qpid::types::Variant(freeFileCount);
    _map["freeFileCountHigh"] = ::qpid::types::Variant(freeFileCountHigh);
    _map["freeFileCountLow"] = ::qpid::types::Variant(freeFileCountLow);
    _map["availableFileCount"] = ::qpid::types::Variant(availableFileCount);
    _map["availableFileCountHigh"] = ::qpid::types::Variant(availableFileCountHigh);
    _map["availableFileCountLow"] = ::qpid::types::Variant(availableFileCountLow);
    _map["writeWaitFailures"] = ::qpid::types::Variant(totals.writeWaitFailures);
    _map["writeBusyFailures"] = ::qpid::types::Variant(totals.writeBusyFailures);
    _map["readRecordCount"] = ::qpid::types::Variant(totals.readRecordCount);
    _map["readBusyFailures"] = ::qpid::types::Variant(totals.readBusyFailures);
    _map["writePageCacheDepth"] = ::qpid::types::Variant(writePageCacheDepth);
    _map["writePageCacheDepthHigh"] = ::qpid::types::Variant(writePageCacheDepthHigh);
    _map["writePageCacheDepthLow"] = ::qpid::types::Variant(writePageCacheDepthLow);
    _map["readPageCacheDepth"] = ::qpid::types::Variant(readPageCacheDepth);
    _map["readPageCacheDepthHigh"] = ::qpid::types::Variant(readPageCacheDepthHigh);
    _map["readPageCacheDepthLow"] = ::qpid::types::Variant(readPageCacheDepthLow);


    // Maintenance of hi-lo statistics
    recordDepthHigh = recordDepth;
    recordDepthLow  = recordDepth;
    outstandingAIOsHigh = outstandingAIOs;
    outstandingAIOsLow  = outstandingAIOs;
    freeFileCountHigh = freeFileCount;
    freeFileCountLow  = freeFileCount;
    availableFileCountHigh = availableFileCount;
    availableFileCountLow  = availableFileCount;
    writePageCacheDepthHigh = writePageCacheDepth;
    writePageCacheDepthLow  = writePageCacheDepth;
    readPageCacheDepthHigh = readPageCacheDepth;
    readPageCacheDepthLow  = readPageCacheDepth;


    }
}

void Journal::mapDecodeValues (const ::qpid::types::Variant::Map& _map)
{
    ::qpid::types::Variant::Map::const_iterator _i;
    Mutex::ScopedLock mutex(accessLock);

    if ((_i = _map.find("queueRef")) != _map.end()) {
        queueRef = _i->second;
    }
    if ((_i = _map.find("name")) != _map.end()) {
        name = (_i->second).getString();
    }
    if ((_i = _map.find("directory")) != _map.end()) {
        directory = (_i->second).getString();
    }
    if ((_i = _map.find("baseFileName")) != _map.end()) {
        baseFileName = (_i->second).getString();
    }
    if ((_i = _map.find("writePageSize")) != _map.end()) {
        writePageSize = _i->second;
    }
    if ((_i = _map.find("writePages")) != _map.end()) {
        writePages = _i->second;
    }
    if ((_i = _map.find("readPageSize")) != _map.end()) {
        readPageSize = _i->second;
    }
    if ((_i = _map.find("readPages")) != _map.end()) {
        readPages = _i->second;
    }
    if ((_i = _map.find("initialFileCount")) != _map.end()) {
        initialFileCount = _i->second;
    }
    if ((_i = _map.find("autoExpand")) != _map.end()) {
        autoExpand = _i->second;
    }
    if ((_i = _map.find("currentFileCount")) != _map.end()) {
        currentFileCount = _i->second;
    }
    if ((_i = _map.find("maxFileCount")) != _map.end()) {
        maxFileCount = _i->second;
    }
    if ((_i = _map.find("dataFileSize")) != _map.end()) {
        dataFileSize = _i->second;
    }

}

void Journal::doMethod (string& methodName, const ::qpid::types::Variant::Map& inMap, ::qpid::types::Variant::Map& outMap, const string& userId)
{
    Manageable::status_t status = Manageable::STATUS_UNKNOWN_METHOD;
    std::string          text;


    if (methodName == "expand") {
        ArgsJournalExpand ioArgs;
        ::qpid::types::Variant::Map::const_iterator _i;
        if ((_i = inMap.find("by")) != inMap.end()) {
            ioArgs.i_by = _i->second;
        }
        bool allow = coreObject->AuthorizeMethod(METHOD_EXPAND, ioArgs, userId);
        if (allow)
            status = coreObject->ManagementMethod (METHOD_EXPAND, ioArgs, text);
        else
            status = Manageable::STATUS_FORBIDDEN;
        outMap["_status_code"] = (uint32_t) status;
        outMap["_status_text"] = ::qpid::management::Manageable::StatusText(status, text);
        return;
    }

    outMap["_status_code"] = (uint32_t) status;
    outMap["_status_text"] = Manageable::StatusText(status, text);
}
