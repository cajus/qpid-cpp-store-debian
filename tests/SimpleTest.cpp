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
#include "StoreException.h"
#include <qpid/broker/Queue.h>
#include <qpid/broker/RecoveryManagerImpl.h>
#include <qpid/framing/AMQHeaderBody.h>
#include <qpid/framing/FieldTable.h>
#include "qpid/log/Logger.h"
#include "qpid/sys/Timer.h"

qpid::sys::Timer timer;

#define SET_LOG_LEVEL(level) \
    qpid::log::Options opts(""); \
    opts.selectors.clear(); \
    opts.selectors.push_back(level); \
    qpid::log::Logger::instance().configure(opts);


using boost::intrusive_ptr;
using boost::static_pointer_cast;
using namespace qpid;
using namespace qpid::broker;
using namespace qpid::framing;
using namespace mrg::msgstore;
using namespace std;

QPID_AUTO_TEST_SUITE(SimpleTest)

const string test_filename("SimpleTest");
const char* tdp = getenv("TMP_DATA_DIR");
const string test_dir(tdp && strlen(tdp) > 0 ? tdp : "/tmp/SimpleTest");

// === Helper fns ===

struct DummyHandler : OutputHandler
{
    std::vector<AMQFrame> frames;

    virtual void send(AMQFrame& frame){
        frames.push_back(frame);
    }
};

void recover(MessageStoreImpl& store, QueueRegistry& queues, ExchangeRegistry& exchanges, LinkRegistry& links)
{
    sys::Timer t;
    DtxManager mgr(t);
    mgr.setStore (&store);
    RecoveryManagerImpl recovery(queues, exchanges, links, mgr);
    store.recover(recovery);
}

void recover(MessageStoreImpl& store, ExchangeRegistry& exchanges)
{
    QueueRegistry queues;
    LinkRegistry links;
    recover(store, queues, exchanges, links);
}

void recover(MessageStoreImpl& store, QueueRegistry& queues)
{
    ExchangeRegistry exchanges;
    LinkRegistry links;
    recover(store, queues, exchanges, links);
}

void bindAndUnbind(const string& exchangeName, const string& queueName,
                   const string& key, const FieldTable& args)
{
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1, true); // truncate store
        Exchange::shared_ptr exchange(new DirectExchange(exchangeName, true, args));
        Queue::shared_ptr queue(new Queue(queueName, 0, &store, 0));
        store.create(*exchange, qpid::framing::FieldTable());
        store.create(*queue, qpid::framing::FieldTable());
        BOOST_REQUIRE(exchange->bind(queue, key, &args));
        store.bind(*exchange, *queue, key, args);
    }//db will be closed
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1);
        ExchangeRegistry exchanges;
        QueueRegistry queues;
        LinkRegistry links;

        recover(store, queues, exchanges, links);

        Exchange::shared_ptr exchange = exchanges.get(exchangeName);
        Queue::shared_ptr queue = queues.find(queueName);
        // check exchange args are still set
        for (FieldTable::ValueMap::const_iterator i = args.begin(); i!=args.end(); i++) {
            BOOST_CHECK(exchange->getArgs().get((*i).first)->getData() == (*i).second->getData());
        }
        //check it is bound by unbinding
        BOOST_REQUIRE(exchange->unbind(queue, key, &args));
        store.unbind(*exchange, *queue, key, args);
    }
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1);
        ExchangeRegistry exchanges;
        QueueRegistry queues;
        LinkRegistry links;

        recover(store, queues, exchanges, links);

        Exchange::shared_ptr exchange = exchanges.get(exchangeName);
        Queue::shared_ptr queue = queues.find(queueName);
         // check exchange args are still set
        for (FieldTable::ValueMap::const_iterator i = args.begin(); i!=args.end(); i++) {
            BOOST_CHECK(exchange->getArgs().get((*i).first)->getData() == (*i).second->getData());
        }
        //make sure it is no longer bound
        BOOST_REQUIRE(!exchange->unbind(queue, key, &args));
    }
}


// === Test suite ===

QPID_AUTO_TEST_CASE(CreateDelete)
{
    SET_LOG_LEVEL("error+"); // This only needs to be set once.

    cout << test_filename << ".CreateDelete: " << flush;
    MessageStoreImpl store(timer);
    store.init(test_dir, 4, 1, true); // truncate store
    string name("CreateDeleteQueue");
    Queue queue(name, 0, &store, 0);
    store.create(queue, qpid::framing::FieldTable());
// TODO - check dir exists
    BOOST_REQUIRE(queue.getPersistenceId());
    store.destroy(queue);
// TODO - check dir is deleted

    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(EmptyRecover)
{
    cout << test_filename << ".EmptyRecover: " << flush;
    MessageStoreImpl store(timer);
    store.init(test_dir, 4, 1, true); // truncate store
    QueueRegistry registry;
    registry.setStore (&store);
    recover(store, registry);
    //nothing to assert, just testing it doesn't blow up

    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(QueueCreate)
{
    cout << test_filename << ".QueueCreate: " << flush;

    uint64_t id(0);
    string name("MyDurableQueue");
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1, true); // truncate store
        Queue queue(name, 0, &store, 0);
        store.create(queue, qpid::framing::FieldTable());
        BOOST_REQUIRE(queue.getPersistenceId());
        id = queue.getPersistenceId();
    }//db will be closed
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1);
        QueueRegistry registry;
        registry.setStore (&store);
        recover(store, registry);
        Queue::shared_ptr queue = registry.find(name);
        BOOST_REQUIRE(queue.get());
        BOOST_CHECK_EQUAL(id, queue->getPersistenceId());
    }

    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(QueueCreateWithSettings)
{
    cout << test_filename << ".QueueCreateWithSettings: " << flush;

    std::auto_ptr<QueuePolicy> policy( QueuePolicy::createQueuePolicy(101, 202));
    string name("MyDurableQueue");
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1, true); // truncate store
        Queue queue(name, 0, &store, 0);
        FieldTable settings;
        policy->update(settings);
        queue.create(settings);
        BOOST_REQUIRE(queue.getPersistenceId());
    }//db will be closed
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1);
        QueueRegistry registry;
        registry.setStore (&store);
        recover(store, registry);
        Queue::shared_ptr queue = registry.find(name);
        BOOST_REQUIRE(queue);
        BOOST_REQUIRE(queue->getPolicy());
        BOOST_CHECK_EQUAL(policy->getMaxCount(), queue->getPolicy()->getMaxCount());
        BOOST_CHECK_EQUAL(policy->getMaxSize(), queue->getPolicy()->getMaxSize());
    }

    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(QueueDestroy)
{
    cout << test_filename << ".QueueDestroy: " << flush;

    string name("MyDurableQueue");
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1, true); // truncate store
        Queue queue(name, 0, &store, 0);
        store.create(queue, qpid::framing::FieldTable());
        store.destroy(queue);
    }//db will be closed
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1);
        QueueRegistry registry;
        registry.setStore (&store);
        recover(store, registry);
        BOOST_REQUIRE(!registry.find(name));
    }

    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(Enqueue)
{
    cout << test_filename << ".Enqueue: " << flush;

    //TODO: this is largely copy & paste'd from MessageTest in
    //qpid tree. ideally need some helper routines for reducing
    //this to a simpler less duplicated form

    string name("MyDurableQueue");
    string exchange("MyExchange");
    string routingKey("MyRoutingKey");
    Uuid messageId(true);
    string data1("abcdefg");
    string data2("hijklmn");
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1, true); // truncate store
        Queue::shared_ptr queue(new Queue(name, 0, &store, 0));
        FieldTable settings;
        queue->create(settings);

        boost::intrusive_ptr<Message> msg = MessageUtils::createMessage(exchange, routingKey, messageId, true, 14);
        MessageUtils::addContent(msg, data1);
        MessageUtils::addContent(msg, data2);

        msg->insertCustomProperty("abc", "xyz");

        queue->enqueue(0, msg);
    }//db will be closed
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1);
        QueueRegistry registry;
        registry.setStore (&store);
        recover(store, registry);
        Queue::shared_ptr queue = registry.find(name);
        BOOST_REQUIRE(queue);
        BOOST_CHECK_EQUAL((u_int32_t) 1, queue->getMessageCount());
        boost::intrusive_ptr<Message> msg = queue->get().payload;

        BOOST_CHECK_EQUAL(exchange, msg->getExchangeName());
        BOOST_CHECK_EQUAL(routingKey, msg->getRoutingKey());
        BOOST_CHECK_EQUAL(messageId, msg->getProperties<MessageProperties>()->getMessageId());
        BOOST_CHECK_EQUAL((uint8_t) PERSISTENT, msg->getProperties<DeliveryProperties>()->getDeliveryMode());
        BOOST_REQUIRE(Str16Value("xyz") == *msg->getProperties<MessageProperties>()->getApplicationHeaders().get("abc"));
        BOOST_CHECK_EQUAL((u_int64_t) 14, msg->contentSize());

        DummyHandler handler;
        QueuedMessage qm(queue.get(),msg,0);
        MessageUtils::deliver(qm, handler, 100);
        BOOST_CHECK_EQUAL((size_t) 2, handler.frames.size());
        AMQContentBody* contentBody(dynamic_cast<AMQContentBody*>(handler.frames[1].getBody()));
        BOOST_REQUIRE(contentBody);
        BOOST_CHECK_EQUAL(data1.size() + data2.size(), contentBody->getData().size());
        BOOST_CHECK_EQUAL(data1 + data2, contentBody->getData());
    }

    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(Dequeue)
{
    cout << test_filename << ".Dequeue: " << flush;

    //TODO: reduce the duplication in these tests
    string name("MyDurableQueue");
    {
        string exchange("MyExchange");
        string routingKey("MyRoutingKey");
        Uuid messageId(true);
        string data("abcdefg");
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1, true); // truncate store
        Queue::shared_ptr queue(new Queue(name, 0, &store, 0));
        FieldTable settings;
        queue->create(settings);

        boost::intrusive_ptr<Message> msg = MessageUtils::createMessage(exchange, routingKey, messageId, true, 7);
        MessageUtils::addContent(msg, data);

        QueuedMessage qm;
        qm.payload = msg;
        queue->enqueue(0, msg);
        queue->dequeue(0, qm);
    }//db will be closed
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1);
        QueueRegistry registry;
        registry.setStore (&store);
        recover(store, registry);
        Queue::shared_ptr queue = registry.find(name);
        BOOST_REQUIRE(queue);
        BOOST_CHECK_EQUAL((u_int32_t) 0, queue->getMessageCount());
    }

    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(ExchangeCreateAndDestroy)
{
    cout << test_filename << ".ExchangeCreateAndDestroy: " << flush;

    uint64_t id(0);
    string name("MyDurableExchange");
    string type("direct");
    FieldTable args;
    args.setString("a", "A");
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1, true); // truncate store
        ExchangeRegistry registry;
        Exchange::shared_ptr exchange = registry.declare(name, type, true, args).first;
        store.create(*exchange, qpid::framing::FieldTable());
        id = exchange->getPersistenceId();
        BOOST_REQUIRE(id);
    }//db will be closed
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1);
        ExchangeRegistry registry;

        recover(store, registry);

        Exchange::shared_ptr exchange = registry.get(name);
        BOOST_CHECK_EQUAL(id, exchange->getPersistenceId());
        BOOST_CHECK_EQUAL(type, exchange->getType());
        BOOST_REQUIRE(exchange->isDurable());
        BOOST_CHECK_EQUAL(*args.get("a"), *exchange->getArgs().get("a"));
        store.destroy(*exchange);
    }
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1);
        ExchangeRegistry registry;

        recover(store, registry);

        try {
            Exchange::shared_ptr exchange = registry.get(name);
            BOOST_FAIL("Expected exchange not to be found");
        } catch (const SessionException& e) {
            BOOST_CHECK_EQUAL((framing::ReplyCode) 404, e.code);
        }
    }

    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(ExchangeBindAndUnbind)
{
    cout << test_filename << ".ExchangeBindAndUnbind: " << flush;

    bindAndUnbind("MyDurableExchange", "MyDurableQueue", "my-routing-key", FieldTable());

    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(ExchangeBindAndUnbindWithArgs)
{
    cout << test_filename << ".ExchangeBindAndUnbindWithArgs: " << flush;

    FieldTable args;
    args.setString("a", "A");
    args.setString("b", "B");
    bindAndUnbind("MyDurableExchange", "MyDurableQueue", "my-routing-key", args);

    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(ExchangeImplicitUnbind)
{
    cout << test_filename << ".ExchangeImplicitUnbind: " << flush;

    string exchangeName("MyDurableExchange");
    string queueName1("MyDurableQueue1");
    string queueName2("MyDurableQueue2");
    string key("my-routing-key");
    FieldTable args;
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1, true); // truncate store
        Exchange::shared_ptr exchange(new DirectExchange(exchangeName, true, args));
        Queue::shared_ptr queue1(new Queue(queueName1, 0, &store, 0));
        Queue::shared_ptr queue2(new Queue(queueName2, 0, &store, 0));
        store.create(*exchange, qpid::framing::FieldTable());
        store.create(*queue1, qpid::framing::FieldTable());
        store.create(*queue2, qpid::framing::FieldTable());
        store.bind(*exchange, *queue1, key, args);
        store.bind(*exchange, *queue2, key, args);
        //delete queue1:
        store.destroy(*queue1);
    }//db will be closed
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1);
        ExchangeRegistry exchanges;
        QueueRegistry queues;
        LinkRegistry links;

        //ensure recovery works ok:
        recover(store, queues, exchanges, links);

        Exchange::shared_ptr exchange = exchanges.get(exchangeName);
        BOOST_REQUIRE(!queues.find(queueName1).get());
        BOOST_REQUIRE(queues.find(queueName2).get());

        //delete exchange:
        store.destroy(*exchange);
    }
    {
        MessageStoreImpl store(timer);
        store.init(test_dir, 4, 1);
        ExchangeRegistry exchanges;
        QueueRegistry queues;
        LinkRegistry links;

        //ensure recovery works ok:
        recover(store, queues, exchanges, links);

        try {
            Exchange::shared_ptr exchange = exchanges.get(exchangeName);
            BOOST_FAIL("Expected exchange not to be found");
        } catch (const SessionException& e) {
            BOOST_CHECK_EQUAL((framing::ReplyCode) 404, e.code);
        }
        Queue::shared_ptr queue = queues.find(queueName2);
        store.destroy(*queue);
    }

    cout << "ok" << endl;
}

QPID_AUTO_TEST_SUITE_END()
