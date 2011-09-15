/**
 * \file lp_map.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing code for class mrg::journal::lp_map (logical file map).
 * See class documentation for details.
 *
 * \author Kim van der Riet
 *
 * Copyright (c) 2008, 2009 Red Hat Inc.
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

#ifndef mrg_journal_lp_map_hpp
#define mrg_journal_lp_map_hpp

#include <map>
#include <string>
#include <sys/types.h>
#include <vector>

namespace mrg
{
namespace journal
{
    /**
    * \class lp_map
    * \brief Maps the logical file id (lfid) to the physical file id (pfid) in the journal.
    *
    * NOTE: NOT THREAD SAFE
    */
    class lp_map
    {
    public:
        typedef std::map<u_int16_t, u_int16_t> lp_map_t;
        typedef lp_map_t::const_iterator lp_map_citr_t;
        typedef lp_map_t::const_reverse_iterator lp_map_critr_t;

    private:
        typedef std::pair<u_int16_t, u_int16_t> lfpair;
        typedef std::pair<lp_map_t::iterator, bool> lfret;
        lp_map_t _map;

    public:
        lp_map();
        virtual ~lp_map();

        void insert(u_int16_t lfid, u_int16_t pfid);
        inline u_int16_t size() const { return u_int16_t(_map.size()); }
        inline bool empty() const { return _map.empty(); }
        inline lp_map_citr_t begin() { return _map.begin(); }
        inline lp_map_citr_t end() { return _map.end(); }
        inline lp_map_critr_t rbegin() { return _map.rbegin(); }
        inline lp_map_critr_t rend() { return _map.rend(); }
        void get_pfid_list(std::vector<u_int16_t>& pfid_list);

        // debug aid
        std::string to_string();
    };

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_lp_map_hpp
