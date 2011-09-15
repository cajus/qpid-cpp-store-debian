/*
 * Copyright (c) 2008 Red Hat, Inc.
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

#ifndef mrg_jtt_jrnl_instance_hpp
#define mrg_jtt_jrnl_instance_hpp

#include "args.hpp"
#include "jrnl_init_params.hpp"
#include "test_case.hpp"

#include <boost/shared_ptr.hpp>
#include "jrnl/cvar.hpp"
#include "jrnl/data_tok.hpp"
#include "jrnl/jcntl.hpp"
#include "jrnl/slock.hpp"
#include "jrnl/smutex.hpp"
#include <list>
#include <vector>

namespace mrg
{
namespace jtt
{

    class jrnl_instance : public mrg::journal::jcntl, public virtual mrg::journal::aio_callback
    {
    public:
        typedef boost::shared_ptr<jrnl_instance> shared_ptr;
        typedef boost::shared_ptr<journal::data_tok> dtok_ptr;

    private:
        jrnl_init_params::shared_ptr _jpp;
        const args* _args_ptr;
        std::vector<dtok_ptr> _dtok_master_enq_list;
        std::vector<dtok_ptr> _dtok_master_txn_list;
        std::list<journal::data_tok*> _dtok_rd_list;
        std::list<journal::data_tok*> _dtok_deq_list;
        mrg::journal::smutex _rd_aio_mutex;     ///< Mutex for read aio wait conditions
        mrg::journal::cvar _rd_aio_cv;          ///< Condition var for read aio wait conditions
        mrg::journal::smutex _wr_full_mutex;    ///< Mutex for write full conditions
        mrg::journal::cvar _wr_full_cv;         ///< Condition var for write full conditions
        mrg::journal::smutex _rd_list_mutex;    ///< Mutex for _dtok_rd_list
        mrg::journal::cvar _rd_list_cv;         ///< Condition var for _dtok_rd_list
        mrg::journal::smutex _deq_list_mutex;   ///< Mutex for _dtok_deq_list
        mrg::journal::cvar _deq_list_cv;        ///< Condition var for _dtok_deq_list
        pthread_t _enq_thread;
        pthread_t _deq_thread;
        pthread_t _read_thread;
        test_case::shared_ptr _tcp;
        test_case_result::shared_ptr _tcrp;

    public:
        jrnl_instance(const std::string& jid, const std::string& jdir,
            const std::string& base_filename,
            const u_int16_t num_jfiles = jrnl_init_params::def_num_jfiles,
            const bool ae = jrnl_init_params::def_ae,
            const u_int16_t ae_max_jfiles = jrnl_init_params::def_ae_max_jfiles,
            const u_int32_t jfsize_sblks = jrnl_init_params::def_jfsize_sblks,
            const u_int16_t wcache_num_pages = jrnl_init_params::def_wcache_num_pages,
            const u_int32_t wcache_pgsize_sblks = jrnl_init_params::def_wcache_pgsize_sblks);
        jrnl_instance(const jrnl_init_params::shared_ptr& params);
        virtual ~jrnl_instance();

        inline const jrnl_init_params::shared_ptr& params() const { return _jpp; }
        inline const std::string& jid() const { return _jpp->jid(); }

        void init_tc(test_case::shared_ptr& tcp, const args* const args_ptr) throw ();
        void run_tc() throw ();
        void tc_wait_compl() throw ();

        // AIO callbacks
        virtual void wr_aio_cb(std::vector<journal::data_tok*>& dtokl);
        virtual void rd_aio_cb(std::vector<u_int16_t>& pil);

    private:
        void run_enq() throw ();
        inline static void* run_enq(void* p)
                { static_cast<jrnl_instance*>(p)->run_enq(); return 0; }

        void run_read() throw ();
        inline static void* run_read(void* p)
                { static_cast<jrnl_instance*>(p)->run_read(); return 0; }

        void run_deq() throw ();
        inline static void* run_deq(void* p)
                { static_cast<jrnl_instance*>(p)->run_deq(); return 0; }

        void abort(const mrg::journal::data_tok* dtokp);
        void commit(const mrg::journal::data_tok* dtokp);
        void txn(const mrg::journal::data_tok* dtokp, const bool commit);
        mrg::journal::data_tok* prep_txn_dtok(const mrg::journal::data_tok* dtokp);

        void panic();

//         // static callbacks
//         static void aio_rd_callback(jcntl* journal, std::vector<u_int16_t>& pil);
//         static void aio_wr_callback(jcntl* journal, std::vector<journal::data_tok*>& dtokl);
    };

} // namespace jtt
} // namespace mrg

#endif // ifndef mrg_jtt_jrnl_instance_hpp
