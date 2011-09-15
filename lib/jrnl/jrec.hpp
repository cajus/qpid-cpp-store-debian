/**
 * \file jrec.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * File containing source code for class mrg::journal::jrec (abstract journal
 * jrecord). See class documentation for details.
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
 * You should have jreceived a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 *
 * The GNU Lesser General Public License is available in the file COPYING.
 */

#ifndef mrg_journal_jrec_hpp
#define mrg_journal_jrec_hpp

namespace mrg
{
namespace journal
{
class jrec;
}
}

#include <cstddef>
#include <fstream>
#include "jrnl/rec_hdr.hpp"
#include "jrnl/rec_tail.hpp"
#include <string>
#include <sys/types.h>

namespace mrg
{
namespace journal
{

    /**
    * \class jrec
    * \brief Abstract class for all file jrecords, both data and log. This class establishes
    *     the common data format and structure for these jrecords.
    */
    class jrec
    {
    public:
        jrec();
        virtual ~jrec();

        /**
        * \brief Encode this instance of jrec into the write buffer at the disk-block-aligned
        *   pointer wptr starting at position rec_offs_dblks in the encoded record to a
        *   maximum size of max_size_dblks.
        *
        * This call encodes the content of the data contianed in this instance of jrec into a
        * disk-softblock-aligned (defined by JRNL_SBLK_SIZE) buffer pointed to by parameter
        * wptr. No more than paramter max_size_dblks data-blocks may be written to the buffer.
        * The parameter rec_offs_dblks is the offset in data-blocks within the fully encoded
        * data block this instance represents at which to start encoding.
        *
        * Encoding entails writing the record header (struct enq_hdr), the data and the record tail
        * (struct enq_tail). The record must be data-block-aligned (defined by JRNL_DBLK_SIZE),
        * thus any remaining space in the final data-block is ignored; the returned value is the
        * number of data-blocks consumed from the page by the encode action. Provided the initial
        * alignment requirements are met, records may be of arbitrary size and may span multiple
        * data-blocks, disk-blocks and/or pages.
        *
        * Since the record size in data-blocks is known, the general usage pattern is to call
        * encode() as many times as is needed to fully encode the data. Each call to encode()
        * will encode as much of the record as it can to what remains of the current page cache,
        * and will return the number of data-blocks actually encoded.
        *
        * <b>Example:</b> Assume that record r1 was previously written to page 0, and that this
        * is an instance representing record r2. Being larger than the page size ps, r2 would span
        * multiple pages as follows:
        * <pre>
        *       |<---ps--->|
        *       +----------+----------+----------+----...
        *       |      |r2a|   r2b    |  r2c   | |
        *       |<-r1-><----------r2---------->  |
        *       +----------+----------+----------+----...
        * page:      p0         p1         p2
        * </pre>
        * Encoding record r2 will require multiple calls to encode; one for each page which
        * is involved. Record r2 is divided logically into sections r2a, r2b and r2c at the
        * points where the page boundaries intersect with the record. Assuming a page size
        * of ps, the page boundary pointers are represented by their names p0, p1... and the
        * sizes of the record segments are represented by their names r1, r2a, r2b..., the calls
        * should be as follows:
        * <pre>
        * encode(p0+r1, 0, ps-r1); (returns r2a data-blocks)
        * encode(p1, r2a, ps);     (returns r2b data-blocks which equals ps)
        * encode(p2, r2a+r2b, ps); (returns r2c data-blocks)
        * </pre>
        *
        * \param wptr Data-block-aligned pointer to position in page buffer where encoding is to
        *   take place.
        * \param rec_offs_dblks Offset in data-blocks within record from which to start encoding.
        * \param max_size_dblks Maximum number of data-blocks to write to pointer wptr.
        * \returns Number of data-blocks encoded.
        */
        virtual u_int32_t encode(void* wptr, u_int32_t rec_offs_dblks,
                u_int32_t max_size_dblks) = 0;

        /**
        * \brief Decode into this instance of jrec from the read buffer at the disk-block-aligned
        *   pointer rptr starting at position jrec_offs_dblks in the encoded record to a
        *   maximum size of max_size_blks.
        *
        * This call decodes a record in the page buffer pointed to by the data-block-aligned
        * (defined by JRNL_DBLK_SIZE) parameter rptr into this instance of jrec. No more than
        * paramter max_size_dblks data-blocks may be read from the buffer. The parameter
        * jrec_offs_dblks is the offset in data-blocks within the encoded record at which to start
        * decoding.
        *
        * Decoding entails reading the record header, the data and the tail. The record is
        * data-block-aligned (defined by JRNL_DBLK_SIZE); the returned value is the number of
        * data-blocks read from the buffer by the decode action. As the record data size is only
        * known once the header is read, the number of calls required to complete reading the
        * record will depend on the vlaues within this instance which are set when the
        * header is decoded.
        *
        * A non-zero value for jrec_offs_dblks implies that this is not the first call to
        * decode and the record data will be appended at this offset.
        *
        * \param h Reference to instance of struct hdr, already read from page buffer and used
        *   to determine record type
        * \param rptr Data-block-aligned pointer to position in page buffer where decoding is to
        *   begin.
        * \param rec_offs_dblks Offset within record from which to start appending the decoded
        *   record.
        * \param max_size_dblks Maximum number of data-blocks to read from pointer rptr.
        * \returns Number of data-blocks read (consumed).
        */
        virtual u_int32_t decode(rec_hdr& h, void* rptr, u_int32_t rec_offs_dblks,
                u_int32_t max_size_dblks) = 0;

        virtual bool rcv_decode(rec_hdr h, std::ifstream* ifsp, std::size_t& rec_offs) = 0;

        virtual std::string& str(std::string& str) const = 0;
        virtual std::size_t data_size() const = 0;
        virtual std::size_t xid_size() const = 0;
        virtual std::size_t rec_size() const = 0;
        inline virtual u_int32_t rec_size_dblks() const { return size_dblks(rec_size()); }
        static inline u_int32_t size_dblks(const std::size_t size)
                { return size_blks(size, JRNL_DBLK_SIZE); }
        static inline u_int32_t size_sblks(const std::size_t size)
                { return size_blks(size, JRNL_DBLK_SIZE * JRNL_SBLK_SIZE); }
        static inline u_int32_t size_blks(const std::size_t size, const std::size_t blksize)
                { return (size + blksize - 1)/blksize; }
        virtual u_int64_t rid() const = 0;

    protected:
        virtual void chk_hdr() const = 0;
        virtual void chk_hdr(u_int64_t rid) const = 0;
        virtual void chk_tail() const = 0;
        static void chk_hdr(const rec_hdr& hdr);
        static void chk_rid(const rec_hdr& hdr, u_int64_t rid);
        static void chk_tail(const rec_tail& tail, const rec_hdr& hdr);
        virtual void clean() = 0;
    }; // class jrec

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_jrec_hpp
