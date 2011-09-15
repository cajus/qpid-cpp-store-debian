/**
 * \file enums.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing definitions for namespace mrg::journal enums.
 *
 * \author Kim van der Riet
 *
 * Copyright (c) 2007, 2008 Red Hat, Inc.
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

#ifndef mrg_journal_enums_hpp
#define mrg_journal_enums_hpp

namespace mrg
{
namespace journal
{

    // TODO: Change this to flags, as multiple of these conditions may exist simultaneously
    /**
    * \brief Enumeration of possilbe return states from journal read and write operations.
    */
    enum _iores
    {
        RHM_IORES_SUCCESS = 0,  ///< Success: IO operation completed noramlly.
        RHM_IORES_PAGE_AIOWAIT, ///< IO operation suspended - next page is waiting for AIO.
        RHM_IORES_FILE_AIOWAIT, ///< IO operation suspended - next file is waiting for AIO.
        RHM_IORES_EMPTY,        ///< During read operations, nothing further is available to read.
        RHM_IORES_RCINVALID,    ///< Read page cache is invalid (ie obsolete or uninitialized)
        RHM_IORES_ENQCAPTHRESH, ///< Enqueue capacity threshold (limit) reached.
        RHM_IORES_FULL,         ///< During write operations, the journal files are full.
        RHM_IORES_BUSY,         ///< Another blocking operation is in progress.
        RHM_IORES_TXPENDING,    ///< Operation blocked by pending transaction.
        RHM_IORES_NOTIMPL       ///< Function is not yet implemented.
    };
    typedef _iores iores;

    static inline const char* iores_str(iores res)
    {
        switch (res)
        {
            case RHM_IORES_SUCCESS: return "RHM_IORES_SUCCESS";
            case RHM_IORES_PAGE_AIOWAIT: return "RHM_IORES_PAGE_AIOWAIT";
            case RHM_IORES_FILE_AIOWAIT: return "RHM_IORES_FILE_AIOWAIT";
            case RHM_IORES_EMPTY: return "RHM_IORES_EMPTY";
            case RHM_IORES_RCINVALID: return "RHM_IORES_RCINVALID";
            case RHM_IORES_ENQCAPTHRESH: return "RHM_IORES_ENQCAPTHRESH";
            case RHM_IORES_FULL: return "RHM_IORES_FULL";
            case RHM_IORES_BUSY: return "RHM_IORES_BUSY";
            case RHM_IORES_TXPENDING: return "RHM_IORES_TXPENDING";
            case RHM_IORES_NOTIMPL: return "RHM_IORES_NOTIMPL";
        }
        return "<iores unknown>";
    }

    enum _log_level
    {
        LOG_TRACE = 0,
        LOG_DEBUG,
        LOG_INFO,
        LOG_NOTICE,
        LOG_WARN,
        LOG_ERROR,
        LOG_CRITICAL
    };
    typedef _log_level log_level;

    static inline const char* log_level_str(log_level ll)
    {
        switch (ll)
        {
            case LOG_TRACE: return "TRACE";
            case LOG_DEBUG: return "DEBUG";
            case LOG_INFO: return "INFO";
            case LOG_NOTICE: return "NOTICE";
            case LOG_WARN: return "WARN";
            case LOG_ERROR: return "ERROR";
            case LOG_CRITICAL: return "CRITICAL";
        }
        return "<log level unknown>";
    }


} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_enums_hpp
