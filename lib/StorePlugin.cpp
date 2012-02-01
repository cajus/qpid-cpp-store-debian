/*
 Copyright (c) 2007, 2008 Red Hat, Inc.

 This file is part of the Qpid async store library msgstore.so.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA

 The GNU Lesser General Public License is available in the file COPYING.
 */

#include "qpid/broker/Broker.h"
#include "qpid/Plugin.h"
#include "qpid/Options.h"
#include "qpid/DataDir.h"
#include "qpid/log/Statement.h"
#include "MessageStoreImpl.h"

using mrg::msgstore::MessageStoreImpl;

namespace qpid {
namespace broker {

using namespace std;

struct StorePlugin : public Plugin {

    MessageStoreImpl::StoreOptions options;
    boost::shared_ptr<MessageStoreImpl> store;

    Options* getOptions() { return &options; }

    void earlyInitialize (Plugin::Target& target)
    {
        Broker* broker = dynamic_cast<Broker*>(&target);
        if (!broker) return;
        store.reset(new MessageStoreImpl(broker->getTimer()));
        DataDir& dataDir = broker->getDataDir ();
        if (options.storeDir.empty ())
        {
            if (!dataDir.isEnabled ())
                throw Exception ("msgstore: If --data-dir is blank or --no-data-dir is specified, --store-dir must be present.");

            options.storeDir = dataDir.getPath ();
        }
        store->init(&options);
        boost::shared_ptr<qpid::broker::MessageStore> brokerStore(store);
        broker->setStore(brokerStore);
        target.addFinalizer(boost::bind(&StorePlugin::finalize, this));
    }

    void initialize(Plugin::Target& target)
    {
        Broker* broker = dynamic_cast<Broker*>(&target);
        if (!broker) return;
        if (!store) return;
        // Not done in earlyInitialize as the Broker::isInCluster test won't work there.
        if (broker->isInCluster()) {
            QPID_LOG(info, "Disabling management instrumentation for the store in a cluster.");
        } else {
            QPID_LOG(info, "Enabling management instrumentation for the store.");
            store->initManagement(broker);
        }
    }

    void finalize()
    {
        store.reset();
    }

    const char* id() {return "StorePlugin";}
};

static StorePlugin instance; // Static initialization.

}} // namespace qpid::broker
