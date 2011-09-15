
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

#include "Package.h"
#include "Store.h"
#include "Journal.h"
#include "EventEnqThresholdExceeded.h"
#include "EventCreated.h"
#include "EventFull.h"
#include "EventRecovered.h"


using namespace qmf::com::redhat::rhm::store;

Package::Package (::qpid::management::ManagementAgent* agent)
{
    Store::registerSelf(agent);
    Journal::registerSelf(agent);
    EventEnqThresholdExceeded::registerSelf(agent);
    EventCreated::registerSelf(agent);
    EventFull::registerSelf(agent);
    EventRecovered::registerSelf(agent);

}

