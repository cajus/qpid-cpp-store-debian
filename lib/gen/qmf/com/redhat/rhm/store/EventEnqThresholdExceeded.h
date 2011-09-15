
#ifndef _MANAGEMENT_ENQTHRESHOLDEXCEEDED_
#define _MANAGEMENT_ENQTHRESHOLDEXCEEDED_

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

#include "qpid/management/ManagementEvent.h"

namespace qmf {
namespace com {
namespace redhat {
namespace rhm {
namespace store {


class EventEnqThresholdExceeded : public ::qpid::management::ManagementEvent
{
  private:
    static void writeSchema (std::string& schema);
    static std::string packageName;
    static std::string eventName;
    static uint8_t md5Sum[MD5_LEN];

    const std::string& jrnlId;
    const std::string& what;


  public:
    writeSchemaCall_t getWriteSchemaCall(void) { return writeSchema; }

    EventEnqThresholdExceeded(const std::string& _jrnlId,
        const std::string& _what);
    ~EventEnqThresholdExceeded() {};

    static void registerSelf(::qpid::management::ManagementAgent* agent);
    std::string& getPackageName() const { return packageName; }
    std::string& getEventName() const { return eventName; }
    uint8_t* getMd5Sum() const { return md5Sum; }
    uint8_t getSeverity() const { return 4; }
    void encode(std::string& buffer) const;
    void mapEncode(::qpid::types::Variant::Map& map) const;
};

}}}}}

#endif  /*!_MANAGEMENT_ENQTHRESHOLDEXCEEDED_*/
