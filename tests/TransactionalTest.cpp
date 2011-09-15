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
#include "qpid/broker/Queue.h"
#include "qpid/broker/RecoveryManagerImpl.h"
#include "qpid/framing/AMQHeaderBody.h"
#include "qpid/log/Statement.h"
#include "qpid/log/Logger.h"
#include "qpid/sys/Timer.h"

qpid::sys::Timer timer;

#define SET_LOG_LEVEL(level) \
    qpid::log::Options opts(""); \
    opts.selectors.clear(); \
    opts.selectors.push_back(level); \
    qpid::log::Logger::instance().configure(opts);

using namespace mrg::msgstore;
using namespace qpid;
using namespace qpid::broker;
using namespace qpid::framing;
using namespace std;

QPID_AUTO_TEST_SUITE(TransactionalTest)

const string test_filename("TransactionalTest");
const char* tdp = getenv("TMP_DATA_DIR");
const string test_dir(tdp && strlen(tdp) > 0 ? tdp : "/tmp/TransactionalTest");

// Test txn context which has special setCompleteFailure() method which prevents entire "txn complete" process from hapenning
class TestTxnCtxt : public TxnCtxt
{
  public:
    TestTxnCtxt(IdSequence* _loggedtx) : TxnCtxt(_loggedtx) {}
    void setCompleteFailure(const unsigned num_queues_rem) {
        // Remove queue members from back of impactedQueues until queues_rem reamin.
        // to end to simulate multi-queue txn complete failure.
        while (impactedQueues.size() > num_queues_rem) impactedQueues.erase(impactedQueues.begin());
    }
    void resetPreparedXidStorePtr() { preparedXidStorePtr = 0; }
};

// Test store which has sepcial begin() which returns a TestTPCTxnCtxt, and a method to check for
// reamining open transactions
class TestMessageStore: public MessageStoreImpl
{
  public:
    TestMessageStore(qpid::sys::Timer& timer, const char* envpath = 0) : MessageStoreImpl(timer, envpath) {}
    std::auto_ptr<qpid::broker::TransactionContext> begin() {
        checkInit();
        // pass sequence number for c/a
        return auto_ptr<TransactionContext>(new TestTxnCtxt(&messageIdSequence));
    }
    void commit(TransactionContext& ctxt, const bool complete_prepared_list) {
        checkInit();
        TxnCtxt* txn(check(&ctxt));
        if (!txn->isTPC()) {
            localPrepare(dynamic_cast<TxnCtxt*>(txn));
            if (!complete_prepared_list) dynamic_cast<TestTxnCtxt*>(txn)->resetPreparedXidStorePtr();
        }
        completed(*dynamic_cast<TxnCtxt*>(txn), true);
    }
    void abort(TransactionContext& ctxt, const bool complete_prepared_list)
    {
        checkInit();
        TxnCtxt* txn(check(&ctxt));
        if (!txn->isTPC()) {
            localPrepare(dynamic_cast<TxnCtxt*>(txn));
            if (!complete_prepared_list) dynamic_cast<TestTxnCtxt*>(txn)->resetPreparedXidStorePtr();
        }
        completed(*dynamic_cast<TxnCtxt*>(txn), false);
    }
};

// === Helper fns ===

const string nameA("queueA");
const string nameB("queueB");
//const Uuid messageId(true);
std::auto_ptr<MessageStoreImpl> store;
std::auto_ptr<QueueRegistry> queues;
Queue::shared_ptr queueA;
Queue::shared_ptr queueB;

template <class T>
void setup()
{
    store = std::auto_ptr<T>(new T(timer));
    store->init(test_dir, 4, 1, true); // truncate store

    //create two queues:
    FieldTable settings;
    queueA = Queue::shared_ptr(new Queue(nameA, 0, store.get(), 0));
    queueA->create(settings);
    queueB = Queue::shared_ptr(new Queue(nameB, 0, store.get(), 0));
    queueB->create(settings);
}

template <class T>
void restart()
{
    queueA.reset();
    queueB.reset();
    queues.reset();
    store.reset();

    store = std::auto_ptr<T>(new T(timer));
    store->init(test_dir, 4, 1);
    queues = std::auto_ptr<QueueRegistry>(new QueueRegistry);
    ExchangeRegistry exchanges;
    LinkRegistry links;
    sys::Timer t;
    DtxManager mgr(t);
    mgr.setStore (store.get());
    RecoveryManagerImpl recovery(*queues, exchanges, links, mgr);
    store->recover(recovery);

    queueA = queues->find(nameA);
    queueB = queues->find(nameB);
}

boost::intrusive_ptr<Message> createMessage(const string& id, const string& exchange="exchange", const string& key="routing_key")
{
    boost::intrusive_ptr<Message> msg = MessageUtils::createMessage(exchange, key);
    msg->getProperties<MessageProperties>()->setCorrelationId(id);
    msg->getProperties<DeliveryProperties>()->setDeliveryMode(PERSISTENT);
    return msg;
}

void checkMsg(Queue::shared_ptr& queue, u_int32_t size, const string& msgid = "<none>")
{
    BOOST_REQUIRE(queue);
    BOOST_CHECK_EQUAL(size, queue->getMessageCount());
    if (size > 0) {
        boost::intrusive_ptr<Message> msg = queue->get().payload;
        BOOST_REQUIRE(msg);
        BOOST_CHECK_EQUAL(msgid, msg->getProperties<MessageProperties>()->getCorrelationId());
    }
}

void swap(bool commit)
{
    setup<MessageStoreImpl>();

    //create message and enqueue it onto first queue:
    boost::intrusive_ptr<Message> msgA = createMessage("Message", "exchange", "routing_key");
    queueA->deliver(msgA);

    boost::intrusive_ptr<Message> msgB = queueA->get().payload;
    BOOST_REQUIRE(msgB);
    //move the message from one queue to the other as a transaction
    std::auto_ptr<TransactionContext> txn = store->begin();
    queueB->enqueue(txn.get(), msgB);//note: need to enqueue it first to avoid message being deleted
    QueuedMessage qmB;
    qmB.payload = msgB;
    queueA->dequeue(txn.get(), qmB);
    if (commit) {
        store->commit(*txn);
    } else {
        store->abort(*txn);
    }

    restart<MessageStoreImpl>();

    // Check outcome
    BOOST_REQUIRE(queueA);
    BOOST_REQUIRE(queueB);

    Queue::shared_ptr x;//the queue from which the message was swapped
    Queue::shared_ptr y;//the queue on which the message is expected to be

    if (commit) {
        x = queueA;
        y = queueB;
    } else {
        x = queueB;
        y = queueA;
    }

    checkMsg(x, 0);
    checkMsg(y, 1, "Message");
    checkMsg(y, 0);
}

void testMultiQueueTxn(const unsigned num_queues_rem, const bool complete_prepared_list, const bool commit)
{
    setup<TestMessageStore>();
    TestMessageStore* tmsp = static_cast<TestMessageStore*>(store.get());
    std::auto_ptr<TransactionContext> txn(tmsp->begin());

    //create two messages and enqueue them onto both queues:
    boost::intrusive_ptr<Message> msgA = createMessage("MessageA", "exchange", "routing_key");
    queueA->enqueue(txn.get(), msgA);
    queueB->enqueue(txn.get(), msgA);
    boost::intrusive_ptr<Message> msgB = createMessage("MessageB", "exchange", "routing_key");
    queueA->enqueue(txn.get(), msgB);
    queueB->enqueue(txn.get(), msgB);

    static_cast<TestTxnCtxt*>(txn.get())->setCompleteFailure(num_queues_rem);
    if (commit)
        tmsp->commit(*txn, complete_prepared_list);
    else
        tmsp->abort(*txn, complete_prepared_list);
    restart<TestMessageStore>();

    // Check outcome
    if (commit)
    {
        checkMsg(queueA, 2, "MessageA");
        checkMsg(queueB, 2, "MessageA");
        checkMsg(queueA, 1, "MessageB");
        checkMsg(queueB, 1, "MessageB");
    }
    checkMsg(queueA, 0);
    checkMsg(queueB, 0);
}

boost::intrusive_ptr<Message> nonTxEnq(Queue::shared_ptr q)
{
    boost::intrusive_ptr<Message> msg = createMessage("Message", "exchange", "routingKey");
    q->deliver(msg);
    return msg;
}

QueuedMessage getMsg(Queue::shared_ptr q)
{
    boost::intrusive_ptr<Message> msg = q->get().payload;
    BOOST_REQUIRE(msg);
    QueuedMessage qm;
    qm.payload = msg;
    return qm;
}

void txDeq(Queue::shared_ptr q, QueuedMessage& qm, TransactionContext* tp)
{
    q->dequeue(tp, qm);
}

void testLock(Queue::shared_ptr q, QueuedMessage qm)
{
    try {
        q->dequeue(0, qm);
        BOOST_ERROR("Did not throw JERR_MAP_LOCKED exception as expected.");
    }
    catch (const mrg::msgstore::StoreException& e) {
        if (std::strstr(e.what(), "JERR_MAP_LOCKED") == 0)
            BOOST_ERROR("Unexpected StoreException: " << e.what());
    }
    catch (const std::exception& e) {
        BOOST_ERROR("Unexpected exception: " << e.what());
    }
}

// === Test suite ===

QPID_AUTO_TEST_CASE(Commit)
{
    SET_LOG_LEVEL("error+"); // This only needs to be set once.

    cout << test_filename << ".Commit: " << flush;
    swap(true);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(Abort)
{
    cout << test_filename << ".Abort: " << flush;
    swap(false);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(MultiQueueCommit)
{
    cout << test_filename << ".MultiQueueCommit: " << flush;
    testMultiQueueTxn(2, true, true);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(MultiQueueAbort)
{
    cout << test_filename << ".MultiQueueAbort: " << flush;
    testMultiQueueTxn(2, true, false);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(MultiQueueNoQueueCommitRecover)
{
    cout << test_filename << ".MultiQueueNoQueueCommitRecover: " << flush;
    testMultiQueueTxn(0, false, true);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(MultiQueueNoQueueAbortRecover)
{
    cout << test_filename << ".MultiQueueNoQueueAbortRecover: " << flush;
    testMultiQueueTxn(0, false, false);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(MultiQueueSomeQueueCommitRecover)
{
    cout << test_filename << ".MultiQueueSomeQueueCommitRecover: " << flush;
    testMultiQueueTxn(1, false, true);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(MultiQueueSomeQueueAbortRecover)
{
    cout << test_filename << ".MultiQueueSomeQueueAbortRecover: " << flush;
    testMultiQueueTxn(1, false, false);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(MultiQueueAllQueueCommitRecover)
{
    cout << test_filename << ".MultiQueueAllQueueCommitRecover: " << flush;
    testMultiQueueTxn(2, false, true);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(MultiQueueAllQueueAbortRecover)
{
    cout << test_filename << ".MultiQueueAllQueueAbortRecover: " << flush;
    testMultiQueueTxn(2, false, false);
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(LockedRecordTest)
{
    cout << test_filename << ".LockedRecordTest: " << flush;

    setup<MessageStoreImpl>();
    nonTxEnq(queueA);
    std::auto_ptr<TransactionContext> txn = store->begin();
    QueuedMessage qm = getMsg(queueA);
    txDeq(queueA, qm, txn.get());
    testLock(queueA, qm);
    store->commit(*txn);
    checkMsg(queueA, 0);

    cout << "ok" << endl;
}

QPID_AUTO_TEST_SUITE_END()
