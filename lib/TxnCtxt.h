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

#ifndef _TxnCtxt_
#define _TxnCtxt_

#include <boost/intrusive_ptr.hpp>
#include <db-inc.h>
#include <memory>
#include <set>
#include <string>

#include "DataTokenImpl.h"
#include "IdSequence.h"
#include "JournalImpl.h"
#include "qpid/broker/PersistableQueue.h"
#include "qpid/broker/TransactionalStore.h"
#include "qpid/sys/Mutex.h"
#include "qpid/sys/uuid.h"

namespace mrg {
namespace msgstore {

class TxnCtxt : public qpid::broker::TransactionContext
{
  protected:
    static qpid::sys::Mutex globalSerialiser;

    static uuid_t uuid;
    static IdSequence uuidSeq;
    static bool staticInit;
    static bool setUuid();

    typedef std::set<qpid::broker::ExternalQueueStore*> ipqdef;
    typedef ipqdef::iterator ipqItr;
    typedef std::auto_ptr<qpid::sys::Mutex::ScopedLock> AutoScopedLock;

    ipqdef impactedQueues; // list of Queues used in the txn
    IdSequence* loggedtx;
    boost::intrusive_ptr<DataTokenImpl> dtokp;
    AutoScopedLock globalHolder;
    JournalImpl* preparedXidStorePtr;

    /**
     * local txn id, if non XA.
     */
    std::string tid;
    DbTxn* txn;

    virtual void completeTxn(bool commit);
    void commitTxn(JournalImpl* jc, bool commit);
    void jrnl_flush(JournalImpl* jc);
    void jrnl_sync(JournalImpl* jc, timespec* timeout);

  public:
    TxnCtxt(IdSequence* _loggedtx=NULL);
    TxnCtxt(std::string _tid, IdSequence* _loggedtx);
    virtual ~TxnCtxt();

    /**
     * Call to make sure all the data for this txn is written to safe store
     *
     *@return if the data successfully synced.
     */
    void sync();
    void begin(DbEnv* env, bool sync = false);
    void commit();
    void abort();
    DbTxn* get();
    virtual bool isTPC();
    virtual const std::string& getXid();

    void addXidRecord(qpid::broker::ExternalQueueStore* queue);
    inline void prepare(JournalImpl* _preparedXidStorePtr) { preparedXidStorePtr = _preparedXidStorePtr; }
    void complete(bool commit);
    bool impactedQueuesEmpty();
    DataTokenImpl* getDtok();
    void incrDtokRef();
    void recoverDtok(const u_int64_t rid, const std::string xid);
};


class TPCTxnCtxt : public TxnCtxt, public qpid::broker::TPCTransactionContext
{
  protected:
    const std::string xid;

  public:
    TPCTxnCtxt(const std::string& _xid, IdSequence* _loggedtx);
    inline virtual bool isTPC() { return true; }
    inline virtual const std::string& getXid() { return xid; }
};

}}

#endif
