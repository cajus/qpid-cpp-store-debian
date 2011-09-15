/**
 * \file RecordHeader.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * This file contains async journal code (v.2).
 *
 * \author Kim van der Riet
 *
 * Copyright (c) 2010, 2011 Red Hat, Inc.
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
 *
 * List of structs in this file:
 *   struct DequeueHeader
 *   struct EnqueueHeader
 *   struct EventHeader
 *   struct FileHeader
 *   struct RecordHeader
 *   struct RecordTail
 *   struct TransactionHeader
 *
 * Overview of structs in this file:
 *
 * <pre>
 *  +------------+                   +--------------+
 *  | RecordTail |                   | RecordHeader |
 *  +------------+                   |  (abstract)  |
 *                                   +--------------+
 *                                           ^
 *                                           |
 *        +----------------+-----------------+-------------------+
 *        |                |                 |                   |
 *  +------------+  +-------------+  +---------------+  +-------------------+
 *  | FileHeader |  | EventHeader |  | DequeueHeader |  | TransactionHeader |
 *  +------------+  +-------------+  +---------------+  +-------------------+
 *                         ^
 *                         |
 *                 +---------------+
 *                 | EnqueueHeader |
 *                 +---------------+
 *  </pre>
 */

#ifndef mrg_journal2_RecordHeader_hpp
#define mrg_journal2_RecordHeader_hpp

#include <cstddef> // std::size_t
#include <ctime> // std::time_t
#include "Configuration.hpp"
#include "JournalException.hpp"
#include <stdint.h> // uint8_t, uint16_t, uint32_t, uint64_t

namespace mrg
{
namespace journal2
{

#pragma pack(1)

    /**
     * \brief Struct for data common to the head of all journal files and records.
     * This includes identification for the file type, the encoding version, endian
     * indicator and a record ID.
     *
     * File layout in binary format (16 bytes):
     * <pre>
     *        0x0                                       0x7
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * 0x00 |        _magic         |  v  |  e  |  _flags   |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * 0x08 |                  _recordId                    |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * </pre>
     * <table>
     * <tr>
     *     <td>v</td>
     *     <td>file version [ <code>_version</code> ] (If the format or encoding of
     *     this file changes, then this number should be incremented)</td>
     * </tr>
     * <tr>
     *     <td>e</td>
     *     <td>endian flag [ <code>_bigEndianFlag</code> ], <b>false</b> (0x00) for
     *     little endian, <b>true</b> (0x01) for big endian</td>
     * </tr>
     * </table>
     */
    struct RecordHeader
    {
        uint32_t _magic;                ///< File type identifier (magic number)
        uint8_t _version;               ///< File encoding version
        uint8_t _bigEndianFlag;         ///< Flag for determining endianness
        uint16_t _flags;                ///< User and system flags
        uint64_t _recordId;             ///< Record ID (rotating 64-bit counter)

        static const uint16_t HDR_OVERWRITE_INDICATOR_MASK = 0x1;

        /**
         * \brief Default constructor, which sets all values to 0.
         */
        inline RecordHeader() :
            _magic(0),
            _version(0),
            _bigEndianFlag(0),
            _flags(0),
            _recordId(0)
        {}

        /**
         * \brief Convenience constructor which initializes values during construction.
         *
         * \param magic Magic for this record
         * \param version Version of this record
         * \param recordId Record identifier for this record
         * \param overwriteIndicator Overwrite indicator for this record
         */
        inline RecordHeader(const uint32_t magic,
                            const uint8_t version,
                            const uint64_t recordId,
                            const bool overwriteIndicator) :
            _magic(magic),
            _version(version),
            _bigEndianFlag(Configuration::_s_endianValue),
            _flags(overwriteIndicator ? HDR_OVERWRITE_INDICATOR_MASK : 0),
            _recordId(recordId)
        {}

        /**
         * \brief Copy constructor.
         */
        inline RecordHeader(const RecordHeader& rh) :
            _magic(rh._magic),
            _version(rh._version),
            _bigEndianFlag(rh._bigEndianFlag),
            _flags(rh._flags),
            _recordId(rh._recordId)
        {}

        /**
         * \brief Destructor.
         */
        virtual inline ~RecordHeader() {}

        /**
         * \brief Convenience copy method.
         */
        virtual inline void copy(const RecordHeader& rh) {
            _magic = rh._magic;
            _version = rh._version;
            _bigEndianFlag = rh._bigEndianFlag;
            _flags = rh._flags;
            _recordId = rh._recordId;
        }

        /**
         * \brief Resets all fields to default values (mostly 0).
         */
        virtual inline void reset() {
            _magic = 0;
            _version = 0;
            _bigEndianFlag = 0;
            _flags = 0;
            _recordId = 0;
        }

        /**
         * \brief Return the value of the Overwrite Indicator for this record.
         *
         * \return true if the Overwrite Indicator flag is set, false otherwise.
         */
        inline bool getOverwriteIndicator() const { return _flags & HDR_OVERWRITE_INDICATOR_MASK; }

        /**
         * \brief Set the value of the Overwrite Indicator for this record
         */
        inline void setOverwriteIndicator(const bool owi) {
            _flags = owi ?
                     _flags | HDR_OVERWRITE_INDICATOR_MASK :
                     _flags & (~HDR_OVERWRITE_INDICATOR_MASK);
        }

        /**
         * \brief Return the header size of this record in bytes. Must be implemented by
         * subclasses.
         *
         * \return Size of record header in bytes.
         */
        static inline std::size_t getHeaderSize() { return sizeof(RecordHeader); }

        /**
         * \brief Return the body (data) size of this record in bytes. Must be implemented
         * by subclasses.
         *
         * \return Size of record body in bytes.
         */
        virtual std::size_t getBodySize() const = 0;

        /**
         * \brief Return total size of this record in bytes, being the sum of the header,
         * xid (if present), data (if present) and tail (if present). Must be implemented
         * by subclasses.
         *
         * \returns The size of the entire record, including header, body (xid and data,
         * if present) and record tail (if persent) in bytes.
         */
        virtual std::size_t getRecordSize() const = 0;

        // TODO - Is this the right place for encode/decode fns?
        ///**
        // * \brief Encode (write) this record instance into the buffer pointed to by the buffer
        // * pointer. Must be implemented by subclasses.
        // */
        //virtual std::size_t encode(char* bufferPtr,
        //                           const std::size_t bufferSize,
        //                           const std::size_t encodeOffset = 0) = 0;

        /**
         * \brief Return a uint32_t checksum for the header and body content of this record.
         *
         * \param initialValue The initial (or seed) value of the checksum.
         *
         * \return Checksum for header and body of record. Tail (if any) is excluded.
         */
        inline uint32_t getCheckSum(uint32_t initialValue = 0) const {
            uint32_t cs = initialValue;
            for (unsigned char* p = (unsigned char*)this;
                                p < (unsigned char*)this + getHeaderSize() + getBodySize();
                                p++) {
                cs ^= (uint32_t)(*p);
                bool carry = cs & uint32_t(0x80000000);
                cs <<= 1;
                if (carry) cs++;
            }
            return cs;
        }
    }; // struct RecordHeader




    /**
     * \brief Struct for data common to the tail of all records. The magic number
     * used here is the binary inverse (1's complement) of the magic used in the
     * record header; this minimizes possible confusion with other headers that may
     * be present during recovery. The tail is used with all records that have either
     * XIDs or data - ie any size-variable content. Currently the only records that
     * do NOT use the tail are non-transactional dequeues and filler records.
     *
     * Record layout in binary format (16 bytes):
     * <pre>
     *        0x0                                       0x7
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * 0x00 |       _xMagic         |       _checkSum       |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * 0x08 |                   _recordId                   |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * </pre>
     */
    struct RecordTail
    {
        uint32_t _xMagic;               ///< Binary inverse (1's complement) of hdr magic number
        uint32_t _checkSum;             ///< Checksum for header and body of record
        uint64_t _recordId;             ///< Record identifier matching that of the header for this record


        /**
         * \brief Default constructor, which sets most values to 0.
         */
        inline RecordTail() :
            _xMagic(0xffffffff),
            _checkSum(0),
            _recordId(0)
        {}

        /**
         * \brief Convenience constructor which initializes values during construction.
         *
         * \param xMagic The inverse of the record header magic (ie ~rh._magic).
         * \param checkSum The checksum for this record header and body.
         * \param recordId The record identifier matching the record header.
         */
        inline RecordTail(const uint32_t xMagic,
                          const uint32_t checkSum,
                          const uint64_t recordId) :
            _xMagic(xMagic),
            _checkSum(checkSum),
            _recordId(recordId)
        {}

        /**
         * \brief Convenience constructor which initializes values during construction from
         * existing RecordHeader instance.
         *
         * \param rh Header instance for which the RecordTail is to be created.
         */
        inline RecordTail(const RecordHeader& rh) :
            _xMagic(~rh._magic),
            _checkSum(rh.getCheckSum()),
            _recordId(rh._recordId)
        {}

        /**
         * \brief Copy constructor.
         *
         * \param rt Instance to be copied.
         */
        inline RecordTail(const RecordTail& rt) :
            _xMagic(rt._xMagic),
            _checkSum(rt._checkSum),
            _recordId(rt._recordId)
        {}

        /**
         * \brief Destructor.
         */
        virtual inline ~RecordTail() {}

        /**
         * \brief Convenience copy method.
         *
         * \param rt Instance to be copied.
         */
        inline void copy(const RecordTail& rt) {
            _xMagic = rt._xMagic;
            _checkSum = rt._checkSum;
            _recordId = rt._recordId;
        }

        /**
         * \brief Resets all fields to default values (mostly 0).
         */
        inline void reset() {
            _xMagic = 0xffffffff;
            _checkSum = 0;
            _recordId = 0;
        }

        /**
        * \brief Returns the size of the header in bytes.
        */
        inline static std::size_t getSize() { return sizeof(RecordTail); }
    }; // struct RecordTail




    /**
     * \brief Struct for data common to the head of all journal files. In addition to
     * the common data, this includes the record ID and offset of the first record in
     * the file.
     *
     * This header precedes all data in journal files and occupies the first complete
     * block in the file. The record ID and offset are updated on each overwrite of the
     * file.
     *
     * File layout in binary format (48 bytes):
     * <pre>
     *        0x0                                       0x7
     *      +-----+-----+-----+-----+-----+-----+-----+-----+  -+
     * 0x00 |        _magic         |  v  |  e  |  _flags   |   |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+   | struct RecordHeader
     * 0x08 |   _recordId (used to show first rid in file)  |   |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+  -+
     * 0x10 |    _physicalFileId    |    _logicalFileId     |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * 0x18 |              _firstRecordOffset               |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * 0x20 |              _timestampSeconds                |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * 0x28 | _timestampNanoSeconds |       _reserved       |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * </pre>
     * <table>
     * <tr>
     *     <td>v</td>
     *     <td>file version [ <code>_version</code> ] (If the format or encoding of
     *     this file changes, then this number should be incremented)</td>
     * </tr>
     * <tr>
     *     <td>e</td>
     *     <td>endian flag [ <code>_bigEndianFlag</code> ], <b>false</b> (0x00) for
     *     little endian, <b>true</b> (0x01) for big endian</td>
     * </tr>
     * </table>
     */
    struct FileHeader : public RecordHeader
    {
        uint32_t _physicalFileId;       ///< Physical file ID (pfid)
        uint32_t _logicalFileId;        ///< Logical file ID (lfid)
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
        uint32_t _filler0;              ///< Big-endian filler for 32-bit size_t
#endif
        std::size_t _firstRecordOffset; ///< First record offset
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
        uint32_t _filler0;              ///< Little-endian filler for 32-bit size_t
#endif
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
        uint32_t _filler1;              ///< Big-endian filler for 32-bit time_t
#endif
        std::time_t _timestampSeconds;  ///< Timestamp of journal initialization, seconds component
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
        uint32_t _filler1;              ///< Little-endian filler for 32-bit time_t
#endif
        uint32_t _timestampNanoSeconds; ///< Timestamp of journal initialization, nanoseconds component
        uint32_t _reserved;             ///< Little-endian filler for uint32_t

        /**
         * \brief Default constructor, which sets all values to 0.
         */
        inline FileHeader() :
            RecordHeader(),
            _physicalFileId(0),
            _logicalFileId(0),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
            _firstRecordOffset(0),
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler1(0),
#endif
            _timestampSeconds(0),
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            _filler1(0),
#endif
            _timestampNanoSeconds(0),
            _reserved(0)
        {}

        /**
         * \brief Convenience constructor which initializes values during construction.
         *
         * \param magic Magic for this record
         * \param version Version of this record
         * \param recordId RecordId for this record
         * \param overwriteIndicator Overwrite indicator for this record
         * \param physicalFileId Physical file ID (file number on disk)
         * \param logicalFileId  Logical file ID (file number as seen by circular file buffer)
         * \param firstRecordOffset First record offset in bytes from beginning of file
         * \param setTimestampFlag If true, causes the timestamp to be initialized with the current system time
         */
        inline FileHeader(const uint32_t magic,
                          const uint8_t version,
                          const uint64_t recordId,
                          const bool overwriteIndicator,
                          const uint16_t physicalFileId,
                          const uint16_t logicalFileId,
                          const std::size_t firstRecordOffset,
                          const bool setTimestampFlag = false) :
            RecordHeader(magic, version, recordId, overwriteIndicator),
            _physicalFileId(physicalFileId),
            _logicalFileId(logicalFileId),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
            _firstRecordOffset(firstRecordOffset),
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler1(0),
#endif
            _timestampSeconds(0),
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            _filler1(0),
#endif
            _timestampNanoSeconds(0),
            _reserved(0)
        {
            if (setTimestampFlag) setTimestamp();
        }

        /**
         * \brief Copy constructor.
         *
         * \param fh FileHeader instance to be copied
         */
        inline FileHeader(const FileHeader& fh) :
            RecordHeader(fh),
            _physicalFileId(fh._physicalFileId),
            _logicalFileId(fh._logicalFileId),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(fh._filler0),
#endif
            _firstRecordOffset(fh._firstRecordOffset),
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(fh._filler0),
#endif
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler1(fh._filler1),
#endif
            _timestampSeconds(fh._timestampSeconds),
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            _filler1(fh._filler1),
#endif
            _timestampNanoSeconds(fh._timestampNanoSeconds),
            _reserved(fh._reserved)
        {}

        /**
         * \brief Destructor.
         */
        virtual ~FileHeader() {}

        /**
         * \brief Convenience copy method.
         *
         * \param fh FileHeader instance to be copied
         */
        inline void copy(const FileHeader& fh) {
            RecordHeader::copy(fh);
            _physicalFileId = fh._physicalFileId;
            _logicalFileId = fh._logicalFileId;
            _firstRecordOffset = fh._firstRecordOffset;
            _timestampSeconds = fh._timestampSeconds;
            _timestampNanoSeconds = fh._timestampNanoSeconds;
            _reserved = fh._reserved;
#if defined(JRNL_32_BIT)
            _filler0 = fh._filler0;
            _filler1 = fh._filler1;
#endif
        }

        /**
         * \brief Resets all fields to default values (mostly 0).
         */
        inline void reset() {
            RecordHeader::reset();
            _physicalFileId = 0;
            _logicalFileId = 0;
            _firstRecordOffset = 0;
            _timestampSeconds = 0;
            _timestampNanoSeconds = 0;
            _reserved = 0;
#if defined(JRNL_32_BIT)
            _filler0 = 0;
            _filler1 = 0;
#endif
        }

        /**
         * \brief Return the header size of this record in bytes.
         *
         * \return Size of record header in bytes.
         */
        static inline std::size_t getHeaderSize() { return sizeof(FileHeader); }

        /**
         * \brief Return the body (data) size of this record in bytes.
         *
         * \return Size of record body in bytes. By definition, a FileHeader has no body.
         */
        inline std::size_t getBodySize() const { return 0; }

        /**
         * \brief Return total size of this record in bytes, being in the case of the
         * FileHeader the size of the header itself only.
         */
        inline std::size_t getRecordSize() const { return getHeaderSize(); }

        /**
         * \brief Gets the current time from the system clock and sets the timestamp in the struct.
         */
        inline void setTimestamp() {
            // TODO: Standardize on method for getting time that does not require a context switch.
            timespec ts;
            if (::clock_gettime(CLOCK_REALTIME, &ts)) {
                std::ostringstream oss;
                oss << FORMAT_SYSERR(errno);
                throw JournalException(JournalErrors::JERR_RTCLOCK, oss.str(), "FileHeader", "setTimestamp");
            }
            setTimestamp(ts);
        }

        /**
         * \brief Sets the timestamp in the struct to the provided value (in seconds and nanoseconds).
         *
         * \param ts Timestamp from which the file header time stamp is to be copied
         */
        inline void setTimestamp(const timespec& ts) {
            _timestampSeconds = ts.tv_sec;
            _timestampNanoSeconds = ts.tv_nsec;
        }
    }; // struct FileHeader




    /**
     * \brief Struct for event records, which can be used to record system events in the
     * store.
     *
     * The EventHeader record type may be used to store events into the journal which do
     * not constitute data content but changes of state in the broker. These can be
     * recovered and used to set appropriate state in the broker.
     *
     * This record is almost identical to the EnqueueRecord, but without the flags. I
     * am uncertain at this time whether it is necessary to set an XID on an event
     * record, but in case, I have left this feature in. In any event, there is only a
     * 1 byte size penalty in the header size for doing so.
     *
     * Record layout in binary format (32 bytes):
     * <pre>
     *        0x0                                       0x7
     *      +-----+-----+-----+-----+-----+-----+-----+-----+  -+
     * 0x00 |        _magic         |  v  |  e  |  _flags   |   |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+   | struct RecordHeader
     * 0x08 |                   _recordId                   |   |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+  -+
     * 0x10 |                   _xidSize                    |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * 0x18 |                   _dataSize                   |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * </pre>
     * <table>
     * <tr>
     *     <td>v</td>
     *     <td>file version [ <code>_version</code> ] (If the format or encoding of
     *     this file changes, then this number should be incremented)</td>
     * </tr>
     * <tr>
     *     <td>e</td>
     *     <td>endian flag [ <code>_bigEndianFlag</code> ], <b>false</b> (0x00) for
     *     little endian, <b>true</b> (0x01) for big endian</td>
     * </tr>
     * </table>
     */
    struct EventHeader : public RecordHeader
    {
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
        uint32_t _filler0;              ///< Big-endian filler for 32-bit size_t
#endif
        std::size_t _xidSize;           ///< XID size
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
        uint32_t _filler0;              ///< Little-endian filler for 32-bit size_t
#endif
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
        uint32_t _filler1;              ///< Big-endian filler for 32-bit size_t
#endif
        std::size_t _dataSize;          ///< Record data size
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
        uint32_t _filler1;              ///< Little-endian filler for 32-bit size_t
#endif

        /**
         * \brief Default constructor, which sets all values to 0.
         */
        inline EventHeader() :
            RecordHeader(),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
            _xidSize(0),
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler1(0),
#endif
            _dataSize(0)
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            , _filler1(0)
#endif
        {}

        /**
         * \brief Convenience constructor which initializes values during construction.
         *
         * \param magic The magic for this record
         * \param version Version of this record
         * \param recordId Record identifier for this record
         * \param xidSize Size of the transaction (or distributed transaction) ID for this record
         * \param dataSize Size of the opaque data block for this record
         * \param overwriteIndicator Flag indicating the present value of the overwrite indicator when writing this
         *        record
         */
        inline EventHeader(const uint32_t magic,
                           const uint8_t version,
                           const uint64_t recordId,
                           const std::size_t xidSize,
                           const std::size_t dataSize,
                           const bool overwriteIndicator) :
            RecordHeader(magic, version, recordId, overwriteIndicator),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
            _xidSize(xidSize),
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler1(0),
#endif
            _dataSize(dataSize)
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            , _filler1(0)
#endif
        {}

        /**
         * \brief Copy constructor
         *
         * \param eh Instance to be copied
         */
        inline EventHeader(const EventHeader& eh) :
            RecordHeader(eh),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(eh._filler0),
#endif
            _xidSize(eh._xidSize),
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(eh._filler0),
#endif
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler1(eh._filler1),
#endif
            _dataSize(eh._dataSize)
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
        , _filler1(eh._filler1)
#endif
        {}

        /**
         * \brief Destructor.
         */
        virtual ~EventHeader() {}

        /**
         * \brief Convenience copy method.
         */
        virtual inline void copy(const EventHeader& e) {
            RecordHeader::copy(e);
            _xidSize = e._xidSize;
            _dataSize = e._dataSize;
#if defined(JRNL_32_BIT)
            _filler0 = e._filler0;
            _filler1 = e._filler1;
#endif
        }

        /**
         * \brief Reset this record to default values (mostly 0)
         */
        virtual inline void reset() {
            RecordHeader::reset();
            _xidSize = 0;
            _dataSize = 0;
#if defined(JRNL_32_BIT)
            _filler0 = 0;
            _filler1 = 0;
#endif
        }

        /**
         * \brief Return the header size of this record in bytes.
         *
         * \return Size of record header in bytes.
         */
        static inline std::size_t getHeaderSize() { return sizeof(EventHeader); }

        /**
         * \brief Return the body (data) size of this record in bytes.
         *
         * \return Size of record body in bytes.
         */
        virtual inline std::size_t getBodySize() const { return _xidSize + _dataSize; }

        /**
         * \brief Return total size of this record in bytes, being in the case of the
         * enqueue record the size of the header, the size of the body (xid and data)
         * and the size of the tail.
         */
        virtual inline std::size_t getRecordSize() const {
            return getHeaderSize() + (getBodySize() ?
                   getBodySize() + RecordTail::getSize() :
                   0);
        }
    }; // struct EventHeader




    /**
     * \brief Struct for enqueue record.
     *
     * Struct for enqueue record. In addition to the common data, this header includes both the
     * xid and data blob sizes.
     *
     * This header precedes all enqueue data in journal files.
     *
     * Record layout in binary format (32 bytes):
     * <pre>
     *        0x0                                       0x7
     *      +-----+-----+-----+-----+-----+-----+-----+-----+  -+
     * 0x00 |        _magic         |  v  |  e  |  _flags   |   |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+   | struct RecordHeader
     * 0x08 |                   _recordId                   |   |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+  -+
     * 0x10 |                   _xidSize                    |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * 0x18 |                   _dataSize                   |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * </pre>
     * <table>
     * <tr>
     *     <td>v</td>
     *     <td>file version [ <code>_version</code> ] (If the format or encoding of
     *     this file changes, then this number should be incremented)</td>
     * </tr>
     * <tr>
     *     <td>e</td>
     *     <td>endian flag [ <code>_bigEndianFlag</code> ], <b>false</b> (0x00) for
     *     little endian, <b>true</b> (0x01) for big endian</td>
     * </tr>
     * </table>
     */
    struct EnqueueHeader : public EventHeader
    {
        static const uint16_t ENQ_HDR_TRANSIENT_MASK = 0x10;
        static const uint16_t ENQ_HDR_EXTERNAL_MASK = 0x20;

        /**
         * \brief Default constructor, which sets all values to 0.
         */
        inline EnqueueHeader() :
                EventHeader()
        {}

        /**
         * \brief Convenience constructor which initializes values during construction.
         *
         * \param magic The magic for this record
         * \param version Version of this record
         * \param recordId Record identifier for this record
         * \param xidSize Size of the transaction (or distributed transaction) ID for this record
         * \param dataSize Size of the opaque data block for this record
         * \param overwriteIndicator Flag indicating the present value of the overwrite indicator when writing this
         *        record
         * \param transient Flag indicating that this record is transient (ie to be discarded on recovery)
         * \param external Flag indicating that this record's data is stored externally to the journal, the data portion
         *        of the record identifies the storage location.
         */
        inline EnqueueHeader(const uint32_t magic,
                       const uint8_t version,
                       const uint64_t recordId,
                       const std::size_t xidSize,
                       const std::size_t dataSize,
                       const bool overwriteIndicator,
                       const bool transient = false,
                       const bool external = false) :
            EventHeader(magic, version, recordId, xidSize, dataSize, overwriteIndicator)
        {
            setTransientFlag(transient);
            setExternalFlag(external);
        }

        /**
         * \brief Copy constructor
         *
         * \param eh Instance to be copied
         */
        inline EnqueueHeader(const EnqueueHeader& eh) :
            EventHeader(eh)
        {}

        /**
         * \brief Destructor.
         */
        virtual ~EnqueueHeader() {}

        /**
         * \brief Return the value of the Transient flag for this record.
         * If set, this record is ignored during recovery.
         *
         * \return true if the Transient flag for this record is set, false otherwise.
         */
        inline bool getTransientFlag() const { return _flags & ENQ_HDR_TRANSIENT_MASK; }

        /**
         * \brief Set the value of the Transient flag for this record.
         *
         * \param transient The value to be set in the transient flag.
         */
        inline void setTransientFlag(const bool transient = true) {
            _flags = transient ?
                     _flags | ENQ_HDR_TRANSIENT_MASK :
                     _flags & (~ENQ_HDR_TRANSIENT_MASK);
        }

        /**
         * \brief Return the value of the External flag for this record. If set, this record data is not within the
         * journal but external to it. The data part of this record contains the location of the stored data.
         *
         * \return true if the Transient flag for this record is set, false otherwise.
         */
        inline bool getExternalFlag() const { return _flags & ENQ_HDR_EXTERNAL_MASK; }

        /**
         * \brief Set the value of the External flag for this record.
         *
         * \param external The value to be set in the External flag.
         */
        inline void setExternalFlag(const bool external = true) {
            _flags = external ?
                     _flags | ENQ_HDR_EXTERNAL_MASK :
                     _flags & (~ENQ_HDR_EXTERNAL_MASK);
        }

        /**
         * \brief Return the header size of this record in bytes.
         *
         * \return Size of record header in bytes.
         */
        static inline std::size_t getHeaderSize() { return sizeof(EnqueueHeader); }
    }; // struct EnqueueHeader




    /**
     * \brief Struct for dequeue record.
     *
     * Struct for dequeue record. If this record has a non-zero xidsize field (i.e., there is a
     * valid XID), then this header is followed by the XID of xidsize bytes and a rec_tail. If,
     * on the other hand, this record has a zero xidsize (i.e., there is no XID), then the rec_tail
     * is absent.
     *
     * Note that this record had its own rid distinct from the rid of the record it is dequeuing.
     * The rid field below is the rid of the dequeue record itself; the deq-rid field is the rid of a
     * previous enqueue record being dequeued by this record.
     *
     * Record layout in binary format (32 bytes):
     * <pre>
     *        0x0                                       0x7
     *      +-----+-----+-----+-----+-----+-----+-----+-----+  -+
     * 0x00 |        _magic         |  v  |  e  |  _flags   |   |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+   | struct RecordHeader
     * 0x08 |                   _recordId                   |   |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+  -+
     * 0x10 |               _dequeuedRecordId               |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * 0x18 |                   _xidSize                    |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * </pre>
     * <table>
     * <tr>
     *     <td>v</td>
     *     <td>file version [ <code>_version</code> ] (If the format or encoding of
     *     this file changes, then this number should be incremented)</td>
     * </tr>
     * <tr>
     *     <td>e</td>
     *     <td>endian flag [ <code>_bigEndianFlag</code> ], <b>false</b> (0x00) for
     *     little endian, <b>true</b> (0x01) for big endian</td>
     * </tr>
     * </table>
     */
    struct DequeueHeader : public RecordHeader
    {
        uint64_t _dequeuedRecordId;     ///< Record ID of dequeued record
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
        uint32_t _filler0;              ///< Big-endian filler for 32-bit size_t
#endif
        std::size_t _xidSize;           ///< XID size
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
        uint32_t _filler0;              ///< Little-endian filler for 32-bit size_t
#endif

        static const uint16_t DEQ_HDR_TPL_COMMIT_ON_TXN_COMPL_MASK = 0x10;

        /**
         * \brief Default constructor, which sets all values to 0.
         */
        inline DequeueHeader() :
            RecordHeader(),
            _dequeuedRecordId(0),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
            _xidSize(0)
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            , _filler0(0)
#endif
        {}

        /**
         * \brief Convenience constructor which initializes values during construction.
         *
         * \param magic The magic for this record
         * \param version Version of this record
         * \param recordId Record identifier for this record
         * \param dequeuedRecordId  Record identifier of the record being dequeued by this record
         * \param xidSize Size of the transaction (or distributed transaction) ID for this record
         * \param overwriteIndicator Flag indicating the present value of the overwrite indicator when writing this
         *        record
         * \param tplCommitOnTxnComplFlag
         */
        inline DequeueHeader(const uint32_t magic,
                             const uint8_t version,
                             const uint64_t recordId,
                             const uint64_t dequeuedRecordId,
                             const std::size_t xidSize,
                             const bool overwriteIndicator,
                             const bool tplCommitOnTxnComplFlag = false) :
            RecordHeader(magic, version, recordId, overwriteIndicator),
            _dequeuedRecordId(dequeuedRecordId),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
            _xidSize(xidSize)
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            , _filler0(0)
#endif
        {
            setTplCommitOnTxnComplFlag(tplCommitOnTxnComplFlag);
        }

        /**
         * \brief Copy constructor
         *
         * \param dh Instance to be copied
         */
        inline DequeueHeader(const DequeueHeader& dh) :
            RecordHeader(dh),
            _dequeuedRecordId(dh._dequeuedRecordId),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(dh._filler0),
#endif
            _xidSize(dh._xidSize)
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            , _filler0(dh._filler0)
#endif
        {}


        /**
         * \brief Destructor.
         */
        virtual ~DequeueHeader() {}

        /**
         * \brief Convenience copy method.
         */
        inline void copy(const DequeueHeader& dh) {
            RecordHeader::copy(dh);
            _dequeuedRecordId = dh._dequeuedRecordId;
            _xidSize = dh._xidSize;
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0 = dh._filler0;
#endif
        }

        /**
         * \brief Reset this record to default values (mostly 0)
         */
        inline void reset() {
            RecordHeader::reset();
            _dequeuedRecordId = 0;
            _xidSize = 0;
#if defined(JRNL_32_BIT)
            _filler0 = 0;
#endif
        }

        /**
         * \brief Return the value of the tplCommitOnTxnComplFlag for this record. This
         * flag is used only within the TPL, and if set, indicates that the transaction was
         * closed using a commit. If not set, the transaction was closed using an abort.
         * This is used during recovery of the transactions in the store.
         *
         * \return true if the tplCommitOnTxnComplFlag flag for this record is set, false
         * otherwise.
         */
        inline bool getTplCommitOnTxnComplFlag() const { return _flags & DEQ_HDR_TPL_COMMIT_ON_TXN_COMPL_MASK; }

        /**
         * \brief Set the value of the tplCommitOnTxnComplFlag for this record. This is
         * only used in the TPL, and is ignored elsewhere.
         *
         * \param commitOnTxnCompl The value to be set in the tplCommitOnTxnComplFlag. If
         * true, the transaction was closed with a commit; if false, with an abort.
         */
        inline void setTplCommitOnTxnComplFlag(const bool commitOnTxnCompl) {
            _flags = commitOnTxnCompl ?
                     _flags | DEQ_HDR_TPL_COMMIT_ON_TXN_COMPL_MASK :
                     _flags & (~DEQ_HDR_TPL_COMMIT_ON_TXN_COMPL_MASK);
        }

        /**
         * \brief Return the header size of this record in bytes.
         *
         * \return Size of record header in bytes.
         */
        static std::size_t getHeaderSize() { return sizeof(DequeueHeader); }

        /**
         * \brief Return the body (xid and data) size of this record in bytes.
         *
         * \return Size of record body in bytes.
         */
        std::size_t getBodySize() const { return _xidSize; }

        /**
         * \brief Return total size of this record in bytes, being in the case of the
         * dequeue record the size of the header, the size of the body (xid only) and
         * the size of the tail.
         */
        inline std::size_t getRecordSize() const {
            return getHeaderSize() + (getBodySize() ?
                   getBodySize() + RecordTail::getSize() :
                   0);
        }
    }; // struct DequeueHeader




    /**
     * \brief Struct for transaction commit and abort records.
     *
     * Struct for DTX commit and abort records. Only the magic distinguishes between them. Since
     * this record must be used in the context of a valid XID, the xidsize field must not be zero.
     * Immediately following this record is the XID itself which is xidsize bytes long, followed by
     * a rec_tail.
     *
     * Note that this record had its own rid distinct from the rids of the record(s) making up the
     * transaction it is committing or aborting.
     *
     * Record layout in binary format (24 bytes):
     * <pre>
     *        0x0                                       0x7
     *      +-----+-----+-----+-----+-----+-----+-----+-----+  -+
     * 0x00 |        _magic         |  v  |  e  |  _flags   |   |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+   | struct RecordHeader
     * 0x08 |                   _recordId                   |   |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+  -+
     * 0x10 |                   _xidSize                    |
     *      +-----+-----+-----+-----+-----+-----+-----+-----+
     * </pre>
     * <table>
     * <tr>
     *     <td>v</td>
     *     <td>file version [ <code>_version</code> ] (If the format or encoding of
     *     this file changes, then this number should be incremented)</td>
     * </tr>
     * <tr>
     *     <td>e</td>
     *     <td>endian flag [ <code>_bigEndianFlag</code> ], <b>false</b> (0x00) for
     *     little endian, <b>true</b> (0x01) for big endian</td>
     * </tr>
     * </table>
     */
    struct TransactionHeader : public RecordHeader
    {
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
        uint32_t _filler0;              ///< Big-endian filler for 32-bit size_t
#endif
        std::size_t _xidSize;           ///< XID size
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
        uint32_t _filler0;              ///< Little-endian filler for 32-bit size_t
#endif

        /**
         * \brief Default constructor, which sets all values to 0.
         */
        TransactionHeader() :
            RecordHeader(),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
            _xidSize(0)
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            , _filler0(0)
#endif
        {}

        /**
         * \brief Convenience constructor which initializes values during construction.
         *
         * \param magic The magic for this record
         * \param version Version of this record
         * \param recordId Record identifier for this record
         * \param xidSize Size of the transaction (or distributed transaction) ID for this record
         * \param overwriteIndicator Flag indicating the present value of the overwrite indicator when writing this
         *        record
         */
        TransactionHeader(const uint32_t magic,
                          const uint8_t version,
                          const uint64_t recordId,
                          const std::size_t xidSize,
                          const bool overwriteIndicator) :
            RecordHeader(magic, version, recordId, overwriteIndicator),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(0),
#endif
            _xidSize(xidSize)
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            , _filler0(0)
#endif
        {}

        /**
         * \brief Copy constructor
         *
         * \param th Instance to be copied
         */
        TransactionHeader(const TransactionHeader& th) :
            RecordHeader(th),
#if defined(JRNL_BIG_ENDIAN) && defined(JRNL_32_BIT)
            _filler0(th._filler0),
#endif
            _xidSize(th._xidSize)
#if defined(JRNL_LITTLE_ENDIAN) && defined(JRNL_32_BIT)
            , _filler0(th._filler0)
#endif
        {}


        /**
         * \brief Destructor.
         */
        virtual ~TransactionHeader() {}

        /**
         * \brief Convenience copy method.
         */
        inline void copy(const TransactionHeader& th) {
            RecordHeader::copy(th);
            _xidSize = th._xidSize;
#if defined(JRNL_32_BIT)
            _filler0 = th._filler0;
#endif
        }

        /**
         * \brief Reset this record to default values (mostly 0)
         */
        inline void reset() {
            RecordHeader::reset();
            _xidSize = 0;
#if defined(JRNL_32_BIT)
            _filler0 = 0;
#endif
        }

        /**
         * \brief Return the header size of this record in bytes.
         *
         * \return Size of record header in bytes.
         */
        static std::size_t getHeaderSize() { return sizeof(TransactionHeader); }

        /**
         * \brief Return the body (data) size of this record in bytes.
         *
         * \return Size of record body in bytes.
         */
        std::size_t getBodySize() const { return _xidSize; }

        /**
         * \brief Return total size of this record in bytes, being in the case of the
         * dequeue record the size of the header, the size of the body (xid only) and
         * the size of the tail.
         */
        inline std::size_t getRecordSize() const {
            // By definition, TransactionRecords must always have an xid, hence a record
            // tail as well. No check on body size required in this case.
            return getHeaderSize() + getBodySize() + RecordTail::getSize();
        }
    }; // struct TransactionHeader

#pragma pack()

} // namespace journal2
} // namespace mrg

#endif // ifndef mrg_journal2_RecordHeader_hpp
