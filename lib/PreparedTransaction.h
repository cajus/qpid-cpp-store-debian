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

#ifndef _PreparedTransaction_
#define _PreparedTransaction_

#include <list>
#include <map>
#include <set>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_list.hpp>

namespace mrg{
namespace msgstore{

typedef u_int64_t queue_id;
typedef u_int64_t message_id;

class LockedMappings
{
public:
    typedef boost::shared_ptr<LockedMappings> shared_ptr;
    typedef std::map<std::string, shared_ptr> map;
    typedef std::pair<queue_id, message_id> idpair;
    typedef std::list<idpair>::iterator iterator;

    void add(queue_id queue, message_id message);
    bool isLocked(queue_id queue, message_id message);
    std::size_t size() { return locked.size(); }
    iterator begin() { return locked.begin(); }
    iterator end() { return locked.end(); }

    static void add(LockedMappings::map& map, std::string& key, queue_id queue, message_id message);

private:
    std::list<idpair> locked;
};

struct PreparedTransaction
{
    typedef boost::ptr_list<PreparedTransaction> list;

    const std::string xid;
    const LockedMappings::shared_ptr enqueues;
    const LockedMappings::shared_ptr dequeues;

    PreparedTransaction(const std::string& xid, LockedMappings::shared_ptr enqueues, LockedMappings::shared_ptr dequeues);
    bool isLocked(queue_id queue, message_id message);
    static bool isLocked(PreparedTransaction::list& txns, queue_id queue, message_id message);
    static PreparedTransaction::list::iterator getLockedPreparedTransaction(PreparedTransaction::list& txns, queue_id queue, message_id message);
};

}}

#endif
