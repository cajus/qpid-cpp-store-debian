/**
 * \file jrnl2/JournalParameters.cpp
 *
 * Qpid asynchronous store plugin library
 *
 * This file contains async journal code (v.2).
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

#include <sstream>

namespace mrg
{
namespace journal2
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
                                         const bool autoExpand,
                                         const uint16_t autoExpandMaxJrnlFiles,
                                         const uint32_t jrnlFileSize_sblks,
                                         const uint16_t writeBuffNumPgs,
                                         const uint32_t writeBuffPgSize_sblks) :
        _jrnlDir(jrnlDir),
        _jrnlBaseFileName(jrnlBaseFileName),
        _numJrnlFiles(numJrnlFiles),
        _jrnlFileSize_sblks(jrnlFileSize_sblks),
        _autoExpand(autoExpand),
        _autoExpandMaxJrnlFiles(autoExpandMaxJrnlFiles),
        _writeBuffNumPgs(writeBuffNumPgs),
        _writeBuffPgSize_sblks(writeBuffPgSize_sblks)
    {}

    JournalParameters::JournalParameters(const JournalParameters& sp) :
        _jrnlDir(sp._jrnlDir),
        _jrnlBaseFileName(sp._jrnlBaseFileName),
        _numJrnlFiles(sp._numJrnlFiles),
        _jrnlFileSize_sblks(sp._jrnlFileSize_sblks),
        _autoExpand(sp._autoExpand),
        _autoExpandMaxJrnlFiles(sp._autoExpandMaxJrnlFiles),
        _writeBuffNumPgs(sp._writeBuffNumPgs),
        _writeBuffPgSize_sblks(sp._writeBuffPgSize_sblks)
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

    std::string
    JournalParameters::toString() const
    {
        std::ostringstream oss;
        toStream(oss);
        return oss.str();
    }

    std::ostream&
    operator<<(std::ostream& os, const JournalParameters& jp)
    {
        jp.toStream(os);
        return os;
    }

    std::ostream&
    operator<<(std::ostream& os, const JournalParameters* jpPtr)
    {
        jpPtr->toStream(os);
        return os;
    }

} // namespace journal2
} // namespace mrg
