/*
 Copyright (c) 2007, 2008, 2009 Red Hat, Inc.

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

#include "unit_test.h"

#include "MessageStoreImpl.h"
#include <iostream>
#include "MessageUtils.h"
#include <qpid/broker/Queue.h>
#include <qpid/broker/RecoveryManagerImpl.h>
#include <qpid/framing/AMQHeaderBody.h>
#include "qpid/log/Logger.h"
#include "qpid/sys/Timer.h"

qpid::sys::Timer timer;

#define SET_LOG_LEVEL(level) \
    qpid::log::Options opts(""); \
    opts.selectors.clear(); \
    opts.selectors.push_back(level); \
    qpid::log::Logger::instance().configure(opts);

using namespace qpid;
using namespace qpid::broker;
using namespace qpid::framing;
using namespace mrg::msgstore;

QPID_AUTO_TEST_SUITE(OrderingTest)

const std::string test_filename("OrderingTest");
const char* tdp = getenv("TMP_DATA_DIR");
const std::string test_dir(tdp && strlen(tdp) > 0 ? tdp : "/tmp/OrderingTest");

// === Helper fns ===

const std::string name("OrderingQueue");
std::auto_ptr<MessageStoreImpl> store;
QueueRegistry queues;
Queue::shared_ptr queue;
std::queue<Uuid> ids;
int counter = 1;

void setup()
{
    store = std::auto_ptr<MessageStoreImpl>(new MessageStoreImpl(timer));
    store->init(test_dir, 4, 1, true); // truncate store

    queue = Queue::shared_ptr(new Queue(name, 0, store.get(), 0));
    FieldTable settings;
    queue->create(settings);
}

void push()
{
    Uuid messageId(true);
    ids.push(messageId);

    boost::intrusive_ptr<Message> msg = MessageUtils::createMessage("exchange", "routing_key", messageId, true, 0);

    queue->deliver(msg);
}

bool pop()
{
    boost::intrusive_ptr<Message> msg = queue->get().payload;
    if (msg) {
        QueuedMessage qm;
        qm.payload = msg;
        queue->dequeue(0, qm);
        BOOST_CHECK_EQUAL(ids.front(), msg->getProperties<MessageProperties>()->getMessageId());
        ids.pop();
        return true;
    } else {
        return false;
    }
}

void restart()
{
    queue.reset();
    store.reset();

    store = std::auto_ptr<MessageStoreImpl>(new MessageStoreImpl(timer));
    store->init(test_dir, 4, 1);
    ExchangeRegistry exchanges;
    LinkRegistry links;
    sys::Timer t;
    DtxManager mgr(t);
    mgr.setStore (store.get());
    RecoveryManagerImpl recoveryMgr(queues, exchanges, links, mgr);
    store->recover(recoveryMgr);

    queue = queues.find(name);
}

void check()
{
    BOOST_REQUIRE(queue);
    BOOST_CHECK_EQUAL((u_int32_t) ids.size(), queue->getMessageCount());
    while (pop()) ;//keeping popping 'till all messages are dequeued
    BOOST_CHECK_EQUAL((u_int32_t) 0, queue->getMessageCount());
    BOOST_CHECK_EQUAL((size_t) 0, ids.size());
}


// === Test suite ===

QPID_AUTO_TEST_CASE(Basic)
{
    SET_LOG_LEVEL("error+"); // This only needs to be set once.

    std::cout << test_filename << ".Basic: " << std::flush;
    setup();
    //push on 10 messages
    for (int i = 0; i < 10; i++) push();
    restart();
    check();
    std::cout << "ok" << std::endl;
}

QPID_AUTO_TEST_CASE(Cycle)
{
    std::cout << test_filename << ".Cycle: " << std::flush;
    setup();
    //push on 10 messages:
    for (int i = 0; i < 10; i++) push();
    //pop 5:
    for (int i = 0; i < 5; i++) pop();
    //push on another 5:
    for (int i = 0; i < 5; i++) push();
    restart();
    check();
    std::cout << "ok" << std::endl;
}

QPID_AUTO_TEST_SUITE_END()
