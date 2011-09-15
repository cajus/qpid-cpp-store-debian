
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
#include "EventRecovered.h"

using namespace qmf::com::redhat::rhm::store;
using           qpid::management::ManagementAgent;
using           qpid::management::Manageable;
using           qpid::management::ManagementObject;
using           qpid::management::Args;
using           qpid::management::Mutex;
using           std::string;

string  EventRecovered::packageName  = string ("com.redhat.rhm.store");
string  EventRecovered::eventName    = string ("recovered");
uint8_t EventRecovered::md5Sum[16]   =
    {0x23,0x28,0x44,0x5a,0x72,0xb0,0x1e,0xda,0x7d,0xff,0x0,0x9,0x1,0x6d,0xa8,0xd4};

EventRecovered::EventRecovered (const std::string& _jrnlId,
        const uint32_t _fileSize,
        const uint16_t _numFiles,
        const uint32_t _numEnq,
        const uint32_t _numTxn,
        const uint32_t _numTxnEnq,
        const uint32_t _numTxnDeq) :
    jrnlId(_jrnlId),
    fileSize(_fileSize),
    numFiles(_numFiles),
    numEnq(_numEnq),
    numTxn(_numTxn),
    numTxnEnq(_numTxnEnq),
    numTxnDeq(_numTxnDeq)
{}

namespace {
    const string NAME("name");
    const string TYPE("type");
    const string DESC("desc");
    const string ARGCOUNT("argCount");
    const string ARGS("args");
}

void EventRecovered::registerSelf(ManagementAgent* agent)
{
    agent->registerEvent(packageName, eventName, md5Sum, writeSchema);
}

void EventRecovered::writeSchema (std::string& schema)
{
    const int _bufSize = 65536;
    char _msgChars[_bufSize];
    ::qpid::management::Buffer buf(_msgChars, _bufSize);
    ::qpid::types::Variant::Map ft;

    // Schema class header:
    buf.putOctet       (CLASS_KIND_EVENT);
    buf.putShortString (packageName); // Package Name
    buf.putShortString (eventName);   // Event Name
    buf.putBin128      (md5Sum);      // Schema Hash
    buf.putShort       (7); // Argument Count

    // Arguments
    ft.clear();
    ft[NAME] = "jrnlId";
    ft[TYPE] = TYPE_SSTR;
    ft[DESC] = "Journal Id";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "fileSize";
    ft[TYPE] = TYPE_U32;
    ft[DESC] = "Journal file size in bytes";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "numFiles";
    ft[TYPE] = TYPE_U16;
    ft[DESC] = "Number of journal files";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "numEnq";
    ft[TYPE] = TYPE_U32;
    ft[DESC] = "Number of recovered enqueues";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "numTxn";
    ft[TYPE] = TYPE_U32;
    ft[DESC] = "Number of recovered transactions";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "numTxnEnq";
    ft[TYPE] = TYPE_U32;
    ft[DESC] = "Number of recovered transactional enqueues";
    buf.putMap(ft);

    ft.clear();
    ft[NAME] = "numTxnDeq";
    ft[TYPE] = TYPE_U32;
    ft[DESC] = "Number of recovered transactional dequeues";
    buf.putMap(ft);


    {
        uint32_t _len = buf.getPosition();
        buf.reset();
        buf.getRawData(schema, _len);
    }
}

void EventRecovered::encode(std::string& _sBuf) const
{
    const int _bufSize=65536;
    char _msgChars[_bufSize];
    ::qpid::management::Buffer buf(_msgChars, _bufSize);

    buf.putShortString(jrnlId);
    buf.putLong(fileSize);
    buf.putShort(numFiles);
    buf.putLong(numEnq);
    buf.putLong(numTxn);
    buf.putLong(numTxnEnq);
    buf.putLong(numTxnDeq);


    uint32_t _bufLen = buf.getPosition();
    buf.reset();

    buf.getRawData(_sBuf, _bufLen);
}

void EventRecovered::mapEncode(::qpid::types::Variant::Map& map) const
{
    using namespace ::qpid::types;
    map["jrnlId"] = ::qpid::types::Variant(jrnlId);
    map["fileSize"] = ::qpid::types::Variant(fileSize);
    map["numFiles"] = ::qpid::types::Variant(numFiles);
    map["numEnq"] = ::qpid::types::Variant(numEnq);
    map["numTxn"] = ::qpid::types::Variant(numTxn);
    map["numTxnEnq"] = ::qpid::types::Variant(numTxnEnq);
    map["numTxnDeq"] = ::qpid::types::Variant(numTxnDeq);

}
