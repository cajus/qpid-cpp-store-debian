/**
 * \file jrnl2/JournalParameters.hpp
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

#ifndef mrg_journal2_JournalParameters_hpp
#define mrg_journal2_JournalParameters_hpp

#include <iostream>
#include <string>
#include <stdint.h> // uint16_t, uint32_t

namespace mrg
{
namespace journal2
{

    struct JournalParameters
    {
        // static default store params
        static std::string _s_defaultJrnlDir;
        static std::string _s_defaultJrnlBaseFileName;
        static uint16_t _s_defaultNumJrnlFiles;
        static uint32_t _s_defaultJrnlFileSize_sblks;
        static bool _s_defaultAutoExpand;
        static uint16_t _s_defaultAutoExpandMaxJrnlFiles;
        static uint16_t _s_defaultWriteBuffNumPgs;
        static uint32_t _s_defaultWriteBuffPgSize_sblks;

        std::string _jrnlDir;
        std::string _jrnlBaseFileName;
        uint16_t _numJrnlFiles;
        uint32_t _jrnlFileSize_sblks;
        bool _autoExpand;
        uint16_t _autoExpandMaxJrnlFiles;
        uint16_t _writeBuffNumPgs;
        uint32_t _writeBuffPgSize_sblks;

        JournalParameters();
        JournalParameters(const std::string& jrnlDir,
                          const std::string& jrnlBaseFileName,
                          const uint16_t numJrnlFiles,
                          const bool autoExpand,
                          const uint16_t autoExpandMaxJrnlFiles,
                          const uint32_t jrnlFileSize_sblks,
                          const uint16_t writeBuffNumPgs,
                          const uint32_t writeBuffPgSize_sblks);
        JournalParameters(const JournalParameters& sp);
        void toStream(std::ostream& os = std::cout) const;
        std::string toString() const;

        friend std::ostream& operator<<(std::ostream& os, const JournalParameters& jp);
        friend std::ostream& operator<<(std::ostream& os, const JournalParameters* jpPtr);
    };

} // namespace journal2
} // namespace mrg

#endif // mrg_journal2_JournalParameters_hpp
