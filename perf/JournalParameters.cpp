/**
 * \file perf/JournalParameters.cpp
 *
 * Qpid asynchronous store plugin library
 *
 * This file contains performance test code for the journal.
 *
 * \author Kim van der Riet
 *
 * Copyright (c) 2010 Red Hat, Inc.
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

#include "JournalParameters.hpp"

namespace mrg
{
namespace jtest
{

    // static declarations
    std::string JournalParameters::_s_defaultJrnlDir = "/tmp/store";
    std::string JournalParameters::_s_defaultJrnlBaseFileName = "JournalData";
    uint16_t JournalParameters::_s_defaultNumJrnlFiles = 8;
    uint32_t JournalParameters::_s_defaultJrnlFileSize_sblks = 3072;
    bool JournalParameters::_s_defaultAutoExpand = false;
    uint16_t JournalParameters::_s_defaultAutoExpandMaxJrnlFiles = 0;
    uint16_t JournalParameters::_s_defaultWriteBuffNumPgs = 32;
    uint32_t JournalParameters::_s_defaultWriteBuffPgSize_sblks = 128;

    JournalParameters::JournalParameters() :
        Streamable(),
        _jrnlDir(_s_defaultJrnlDir),
        _jrnlBaseFileName(_s_defaultJrnlBaseFileName),
        _numJrnlFiles(_s_defaultNumJrnlFiles),
        _jrnlFileSize_sblks(_s_defaultJrnlFileSize_sblks),
        _autoExpand(_s_defaultAutoExpand),
        _autoExpandMaxJrnlFiles(_s_defaultAutoExpandMaxJrnlFiles),
        _writeBuffNumPgs(_s_defaultWriteBuffNumPgs),
        _writeBuffPgSize_sblks(_s_defaultWriteBuffPgSize_sblks)
    {}

    JournalParameters::JournalParameters(const std::string& jrnlDir,
                                         const std::string& jrnlBaseFileName,
                                         const uint16_t numJrnlFiles,
                                         const uint32_t jrnlFileSize_sblks,
                                         const bool autoExpand,
                                         const uint16_t autoExpandMaxJrnlFiles,
                                         const uint16_t writeBuffNumPgs,
                                         const uint32_t writeBuffPgSize_sblks) :
        Streamable(),
        _jrnlDir(jrnlDir),
        _jrnlBaseFileName(jrnlBaseFileName),
        _numJrnlFiles(numJrnlFiles),
        _jrnlFileSize_sblks(jrnlFileSize_sblks),
        _autoExpand(autoExpand),
        _autoExpandMaxJrnlFiles(autoExpandMaxJrnlFiles),
        _writeBuffNumPgs(writeBuffNumPgs),
        _writeBuffPgSize_sblks(writeBuffPgSize_sblks)
    {}

    JournalParameters::JournalParameters(const JournalParameters& jp) :
        Streamable(),
        _jrnlDir(jp._jrnlDir),
        _jrnlBaseFileName(jp._jrnlBaseFileName),
        _numJrnlFiles(jp._numJrnlFiles),
        _jrnlFileSize_sblks(jp._jrnlFileSize_sblks),
        _autoExpand(jp._autoExpand),
        _autoExpandMaxJrnlFiles(jp._autoExpandMaxJrnlFiles),
        _writeBuffNumPgs(jp._writeBuffNumPgs),
        _writeBuffPgSize_sblks(jp._writeBuffPgSize_sblks)
    {}

    void
    JournalParameters::toStream(std::ostream& os) const
    {
        os << "Store Parameters:" << std::endl;
        os << "  jrnlDir = \"" << _jrnlDir << "\"" << std::endl;
        os << "  jrnlBaseFileName = \"" << _jrnlBaseFileName << "\"" << std::endl;
        os << "  numJrnlFiles = " << _numJrnlFiles << std::endl;
        os << "  jrnlFileSize_sblks = " << _jrnlFileSize_sblks << std::endl;
        os << "  autoExpand = " << _autoExpand << std::endl;
        os << "  autoExpandMaxJrnlFiles = " << _autoExpandMaxJrnlFiles << std::endl;
        os << "  writeBuffNumPgs = " << _writeBuffNumPgs << std::endl;
        os << "  writeBuffPgSize_sblks = " << _writeBuffPgSize_sblks << std::endl;
    }

} // namespace jtest
} // namespace mrg
