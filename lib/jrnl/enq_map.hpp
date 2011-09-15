/**
 * \file enq_map.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::enq_map (enqueue map).
 * See class documentation for details.
 *
 * \author Kim van der Riet
 *
 * Copyright (c) 2007, 2008, 2009, 2010 Red Hat Inc.
 *
 * This file is part of the Qpid async store library msgstore.so.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 *
 * The GNU Lesser General Public License is available in the file COPYING.
 */

#ifndef mrg_journal_enq_map_hpp
#define mrg_journal_enq_map_hpp

namespace mrg
{
namespace journal
{
class enq_map;
}
}

#include "jrnl/jexception.hpp"
#include "jrnl/smutex.hpp"
#include <map>
#include <pthread.h>
#include <vector>

namespace mrg
{
namespace journal
{

    /**
    * \class enq_map
    * \brief Class for storing the physical file id (pfid) and a transaction locked flag for each enqueued
    *     data block using the record id (rid) as a key. This is the primary mechanism for
    *     deterimining the enqueue low water mark: if a pfid exists in this map, then there is
    *     at least one still-enqueued record in that file. (The transaction map must also be
    *     clear, however.)
    *
    * Map rids against pfid and lock status. As records are enqueued, they are added to this
    * map, and as they are dequeued, they are removed. An enqueue is locked when a transactional
    * dequeue is pending that has been neither committed nor aborted.
    * <pre>
    *   key      data
    *
    *   rid1 --- [ pfid, txn_lock ]
    *   rid2 --- [ pfid, txn_lock ]
    *   rid3 --- [ pfid, txn_lock ]
    *   ...
    * </pre>
    */
    class enq_map
    {
    public:
        // return/error codes
        static int16_t EMAP_DUP_RID;
        static int16_t EMAP_LOCKED;
        static int16_t EMAP_RID_NOT_FOUND;
        static int16_t EMAP_OK;
        static int16_t EMAP_FALSE;
        static int16_t EMAP_TRUE;

    private:

        struct emap_data_struct
        {
            u_int16_t   _pfid;
            bool        _lock;
            emap_data_struct(const u_int16_t pfid, const bool lock) : _pfid(pfid), _lock(lock) {}
        };
        typedef std::pair<u_int64_t, emap_data_struct> emap_param;
        typedef std::map<u_int64_t, emap_data_struct> emap;
        typedef emap::iterator emap_itr;

        emap _map;
        smutex _mutex;
        std::vector<u_int32_t> _pfid_enq_cnt;

    public:
        enq_map();
        virtual ~enq_map();

        void set_num_jfiles(const u_int16_t num_jfiles);
        inline u_int32_t get_enq_cnt(const u_int16_t pfid) const { return _pfid_enq_cnt.at(pfid); };

        int16_t insert_pfid(const u_int64_t rid, const u_int16_t pfid); // 0=ok; -3=duplicate rid;
        int16_t insert_pfid(const u_int64_t rid, const u_int16_t pfid, const bool locked); // 0=ok; -3=duplicate rid;
        int16_t get_pfid(const u_int64_t rid); // >=0=pfid; -1=rid not found; -2=locked
        int16_t get_remove_pfid(const u_int64_t rid, const bool txn_flag = false); // >=0=pfid; -1=rid not found; -2=locked
        bool is_enqueued(const u_int64_t rid, bool ignore_lock = false);
        int16_t lock(const u_int64_t rid); // 0=ok; -1=rid not found
        int16_t unlock(const u_int64_t rid); // 0=ok; -1=rid not found
        int16_t is_locked(const u_int64_t rid); // 1=true; 0=false; -1=rid not found
        inline void clear() { _map.clear(); }
        inline bool empty() const { return _map.empty(); }
        inline u_int32_t size() const { return u_int32_t(_map.size()); }
        void rid_list(std::vector<u_int64_t>& rv);
        void pfid_list(std::vector<u_int16_t>& fv);
    };

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_enq_map_hpp
