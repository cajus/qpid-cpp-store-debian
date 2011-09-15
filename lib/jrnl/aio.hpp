/**
 * \file aio.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * This file contains an encapsulation of the libaio interface used
 * by the journal.
 *
* \author Kim van der Riet
 *
 * Copyright (c) 2007, 2008, 2009 Red Hat, Inc.
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

#ifndef mrg_journal_aio_hpp
#define mrg_journal_aio_hpp

#include <libaio.h>
#include <cstring>
#include <sys/types.h>
#include <string.h>

namespace mrg
{
namespace journal
{

typedef iocb aio_cb;
typedef io_event aio_event;

/**
 * \brief This class is a C++ wrapper class for the libaio functions used by the journal. Note that only those
 * functions used by the journal are included here. This is not a complete implementation of all libaio functions.
 */
class aio
{
public:
    static inline int queue_init(int maxevents, io_context_t* ctxp)
    {
        return ::io_queue_init(maxevents, ctxp);
    }

    static inline int queue_release(io_context_t ctx)
    {
        return ::io_queue_release(ctx);
    }

    static inline int submit(io_context_t ctx, long nr, aio_cb* aios[])
    {
        return ::io_submit(ctx, nr, aios);
    }

    static inline int getevents(io_context_t ctx, long min_nr, long nr, aio_event* events, timespec* const timeout)
    {
        return ::io_getevents(ctx, min_nr, nr, events, timeout);
    }

    /**
     * \brief This function allows iocbs to be initialized with a pointer that can be re-used. This prepares an
     * aio_cb struct for read use. (This is a wrapper for libaio's ::io_prep_pread() function.)
     *
     * \param aiocbp Pointer to the aio_cb struct to be prepared.
     * \param fd File descriptor to be used for read.
     * \param buf Pointer to buffer in which read data is to be placed.
     * \param count Number of bytes to read - buffer must be large enough.
     * \param offset Offset within file from which data will be read.
     */
    static inline void prep_pread(aio_cb* aiocbp, int fd, void* buf, std::size_t count, int64_t offset)
    {
        ::io_prep_pread(aiocbp, fd, buf, count, offset);
    }

    /**
     * \brief Special version of libaio's io_prep_pread() which preserves the value of the data pointer. This allows
     * iocbs to be initialized with a pointer that can be re-used. This prepares a aio_cb struct for read use.
     *
     * \param aiocbp Pointer to the aio_cb struct to be prepared.
     * \param fd File descriptor to be used for read.
     * \param buf Pointer to buffer in which read data is to be placed.
     * \param count Number of bytes to read - buffer must be large enough.
     * \param offset Offset within file from which data will be read.
     */
    static inline void prep_pread_2(aio_cb* aiocbp, int fd, void* buf, std::size_t count, int64_t offset)
    {
        std::memset((void*) ((char*) aiocbp + sizeof(void*)), 0, sizeof(aio_cb) - sizeof(void*));
        aiocbp->aio_fildes = fd;
        aiocbp->aio_lio_opcode = IO_CMD_PREAD;
        aiocbp->aio_reqprio = 0;
        aiocbp->u.c.buf = buf;
        aiocbp->u.c.nbytes = count;
        aiocbp->u.c.offset = offset;
    }

    /**
     * \brief This function allows iocbs to be initialized with a pointer that can be re-used. This function prepares
     * an aio_cb struct for write use. (This is a wrapper for libaio's ::io_prep_pwrite() function.)
     *
     * \param aiocbp Pointer to the aio_cb struct to be prepared.
     * \param fd File descriptor to be used for write.
     * \param buf Pointer to buffer in which data to be written is located.
     * \param count Number of bytes to write.
     * \param offset Offset within file to which data will be written.
     */
    static inline void prep_pwrite(aio_cb* aiocbp, int fd, void* buf, std::size_t count, int64_t offset)
    {
        ::io_prep_pwrite(aiocbp, fd, buf, count, offset);
    }

    /**
     * \brief Special version of libaio's io_prep_pwrite() which preserves the value of the data pointer. This allows
     * iocbs to be initialized with a pointer that can be re-used. This function prepares an aio_cb struct for write
     * use.
     *
     * \param aiocbp Pointer to the aio_cb struct to be prepared.
     * \param fd File descriptor to be used for write.
     * \param buf Pointer to buffer in which data to be written is located.
     * \param count Number of bytes to write.
     * \param offset Offset within file to which data will be written.
     */
    static inline void prep_pwrite_2(aio_cb* aiocbp, int fd, void* buf, std::size_t count, int64_t offset)
    {
        std::memset((void*) ((char*) aiocbp + sizeof(void*)), 0, sizeof(aio_cb) - sizeof(void*));
        aiocbp->aio_fildes = fd;
        aiocbp->aio_lio_opcode = IO_CMD_PWRITE;
        aiocbp->aio_reqprio = 0;
        aiocbp->u.c.buf = buf;
        aiocbp->u.c.nbytes = count;
        aiocbp->u.c.offset = offset;
    }
};

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_aio_hpp
