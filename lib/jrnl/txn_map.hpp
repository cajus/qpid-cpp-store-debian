/**
 * \file txn_map.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::txn_map (transaction map).
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

#ifndef mrg_journal_txn_map_hpp
#define mrg_journal_txn_map_hpp

namespace mrg
{
namespace journal
{
    class txn_map;
}
}

#include "jrnl/smutex.hpp"
#include <map>
#include <pthread.h>
#include <string>
#include <sys/types.h>
#include <vector>

namespace mrg
{
namespace journal
{

    /**
    * \struct txn_data_struct
    * \brief Struct encapsulating transaction data necessary for processing a transaction
    *     in the journal once it is closed with either a commit or abort.
    */
    struct txn_data_struct
    {
        u_int64_t _rid;     ///< Record id for this operation
        u_int64_t _drid;    ///< Dequeue record id for this operation
        u_int16_t _pfid;    ///< Physical file id, to be used when transferring to emap on commit
        bool _enq_flag;     ///< If true, enq op, otherwise deq op
        bool _commit_flag;  ///< (2PC transactions) Records 2PC complete c/a mode
        bool _aio_compl;    ///< Initially false, set to true when record AIO returns
        txn_data_struct(const u_int64_t rid, const u_int64_t drid, const u_int16_t pfid,
                const bool enq_flag, const bool commit_flag = false);
    };
    typedef txn_data_struct txn_data;
    typedef std::vector<txn_data> txn_data_list;
    typedef txn_data_list::iterator tdl_itr;

    /**
    * \class txn_map
    * \brief Class for storing transaction data for each open (ie not committed or aborted)
    *     xid in the store. If aborted, records are discarded; if committed, they are
    *     transferred to the enqueue map.
    *
    * The data is encapsulated by struct txn_data_struct. A vector containing the information
    * for each operation included as part of the same transaction is mapped against the
    * xid.
    *
    * The aio_compl flag is set true as each AIO write operation for the enqueue or dequeue
    * returns. Checking that all of these flags are true for a given xid is the mechanism
    * used to determine if the transaction is syncronized (through method is_txn_synced()).
    *
    * On transaction commit, then each operation is handled as follows:
    *
    * If an enqueue (_enq_flag is true), then the rid and pfid are transferred to the enq_map.
    * If a dequeue (_enq_flag is false), then the rid stored in the drid field is used to
    * remove the corresponding record from the enq_map.
    *
    * On transaction abort, then each operation is handled as follows:
    *
    * If an enqueue (_enq_flag is true), then the data is simply discarded.
    * If a dequeue (_enq_flag is false), then the lock for the corresponding enqueue in enq_map
    * (if not a part of the same transaction) is removed, and the data discarded.
    *
    * <pre>
    *   key      data
    *
    *   xid1 --- vector< [ rid, drid, pfid, enq_flag, commit_flag, aio_compl ] >
    *   xid2 --- vector< [ rid, drid, pfid, enq_flag, commit_flag, aio_compl ] >
    *   xid3 --- vector< [ rid, drid, pfid, enq_flag, commit_flag, aio_compl ] >
    *   ...
    * </pre>
    */
    class txn_map
    {
    public:
        // return/error codes
        static int16_t TMAP_RID_NOT_FOUND;
        static int16_t TMAP_XID_NOT_FOUND;
        static int16_t TMAP_OK;
        static int16_t TMAP_NOT_SYNCED;
        static int16_t TMAP_SYNCED;

    private:
        typedef std::pair<std::string, txn_data_list> xmap_param;
        typedef std::map<std::string, txn_data_list> xmap;
        typedef xmap::iterator xmap_itr;

        xmap _map;
        smutex _mutex;
        std::vector<u_int32_t> _pfid_txn_cnt;
        const txn_data_list _empty_data_list;

    public:
        txn_map();
        virtual ~txn_map();

        void set_num_jfiles(const u_int16_t num_jfiles);
        u_int32_t get_txn_pfid_cnt(const u_int16_t pfid) const;
        bool insert_txn_data(const std::string& xid, const txn_data& td);
        const txn_data_list get_tdata_list(const std::string& xid);
        const txn_data_list get_remove_tdata_list(const std::string& xid);
        bool in_map(const std::string& xid);
        u_int32_t enq_cnt();
        u_int32_t deq_cnt();
        int16_t is_txn_synced(const std::string& xid); // -1=xid not found; 0=not synced; 1=synced
        int16_t set_aio_compl(const std::string& xid, const u_int64_t rid); // -2=rid not found; -1=xid not found; 0=done
        bool data_exists(const std::string& xid, const u_int64_t rid);
        bool is_enq(const u_int64_t rid);
        inline void clear() { _map.clear(); }
        inline bool empty() const { return _map.empty(); }
        inline size_t size() const { return _map.size(); }
        void xid_list(std::vector<std::string>& xv);
    private:
        u_int32_t cnt(const bool enq_flag);
        const txn_data_list get_tdata_list_nolock(const std::string& xid);
    };

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_txn_map_hpp
