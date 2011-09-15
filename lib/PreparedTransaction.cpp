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

#include "PreparedTransaction.h"
#include <algorithm>

using namespace mrg::msgstore;
using std::string;

void LockedMappings::add(queue_id queue, message_id message)
{
    locked.push_back(std::make_pair(queue, message));
}

bool LockedMappings::isLocked(queue_id queue, message_id message)
{
    idpair op( std::make_pair(queue, message) );
    return find(locked.begin(), locked.end(), op) != locked.end();
}

void LockedMappings::add(LockedMappings::map& map, std::string& key, queue_id queue, message_id message)
{
    LockedMappings::map::iterator i = map.find(key);
    if (i == map.end()) {
        LockedMappings::shared_ptr ptr(new LockedMappings());
        i = map.insert(std::make_pair(key, ptr)).first;
    }
    i->second->add(queue, message);
}

bool PreparedTransaction::isLocked(queue_id queue, message_id message)
{
    return (enqueues.get() && enqueues->isLocked(queue, message))
        || (dequeues.get() && dequeues->isLocked(queue, message));
}


bool PreparedTransaction::isLocked(PreparedTransaction::list& txns, queue_id queue, message_id message)
{
    for (PreparedTransaction::list::iterator i = txns.begin(); i != txns.end(); i++) {
        if (i->isLocked(queue, message)) {
            return true;
        }
    }
    return false;
}

PreparedTransaction::list::iterator PreparedTransaction::getLockedPreparedTransaction(PreparedTransaction::list& txns, queue_id queue, message_id message)
{
    for (PreparedTransaction::list::iterator i = txns.begin(); i != txns.end(); i++) {
        if (i->isLocked(queue, message)) {
            return i;
        }
    }
    return txns.end();
}

PreparedTransaction::PreparedTransaction(const std::string& _xid,
                                         LockedMappings::shared_ptr _enqueues,
                                         LockedMappings::shared_ptr _dequeues)

    : xid(_xid), enqueues(_enqueues), dequeues(_dequeues) {}

