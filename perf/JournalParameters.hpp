/**
 * \file perf/JournalParameters.hpp
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

#ifndef mrg_jtest_JournalParameters_hpp
#define mrg_jtest_JournalParameters_hpp

#include <cstdint>
#include "Streamable.hpp"

namespace mrg
{
namespace jtest
{

    /**
     * \brief Stuct for aggregating the common journal parameters
     *
     * This struct is used to aggregate and keep together all the common journal parameters. These affect the journal
     * geometry and buffers. The test parameters are aggregated in class TestParameters.
     */
    struct JournalParameters : public Streamable
    {
        // static default store params
        static std::string _s_defaultJrnlDir;               ///< Default journal directory
        static std::string _s_defaultJrnlBaseFileName;      ///< Default journal base file name
        static uint16_t _s_defaultNumJrnlFiles;             ///< Default number of journal data files
        static uint32_t _s_defaultJrnlFileSize_sblks;       ///< Default journal data file size in softblocks
        static bool _s_defaultAutoExpand;                   ///< Default auto-expand flag (allows auto-expansion of journal data files)
        static uint16_t _s_defaultAutoExpandMaxJrnlFiles;   ///< Default auto-expand file number limit (0 = no limit)
        static uint16_t _s_defaultWriteBuffNumPgs;          ///< Default number of write buffer pages
        static uint32_t _s_defaultWriteBuffPgSize_sblks;    ///< Default size of each write buffer page in softblocks

        std::string _jrnlDir;                               ///< Journal directory
        std::string _jrnlBaseFileName;                      ///< Journal base file name
        uint16_t _numJrnlFiles;                             ///< Number of journal data files
        uint32_t _jrnlFileSize_sblks;                       ///< Journal data file size in softblocks
        bool _autoExpand;                                   ///< Auto-expand flag (allows auto-expansion of journal data files)
        uint16_t _autoExpandMaxJrnlFiles;                   ///< Auto-expand file number limit (0 = no limit)
        uint16_t _writeBuffNumPgs;                          ///< Number of write buffer pages
        uint32_t _writeBuffPgSize_sblks;                    ///< Size of each write buffer page in softblocks

        /**
         * \brief Default constructor
         *
         * Default constructor. Uses the default values for all parameters.
         */
        JournalParameters();

        /**
         * \brief Constructor
         *
         * Convenience constructor.
         *
         * \param jrnlDir Journal directory
         * \param jrnlBaseFileName Journal base file name
         * \param numJrnlFiles Number of journal data files
         * \param jrnlFileSize_sblks Journal data file size in softblocks
         * \param autoExpand Auto-expand flag (allows auto-expansion of journal data files)
         * \param autoExpandMaxJrnlFiles Default auto-expand file number limit (0 = no limit)
         * \param writeBuffNumPgs Number of write buffer pages
         * \param writeBuffPgSize_sblks Size of each write buffer page in softblocks
         */
        JournalParameters(const std::string& jrnlDir,
                          const std::string& jrnlBaseFileName,
                          const uint16_t numJrnlFiles,
                          const uint32_t jrnlFileSize_sblks,
                          const bool autoExpand,
                          const uint16_t autoExpandMaxJrnlFiles,
                          const uint16_t writeBuffNumPgs,
                          const uint32_t writeBuffPgSize_sblks);

        /**
         * \brief Copy constructor
         *
         * \param jp Reference to JournalParameters instance to be copied
         */
        JournalParameters(const JournalParameters& jp);

        /**
         * \brief Virtual destructor
         */
        virtual ~JournalParameters() {}

        /***
         * \brief Stream the journal parameters to an output stream
         *
         * Convenience feature which streams a multi-line representation of all the journal parameters, one per line to
         * an output stream.
         *
         * \param os Output stream to which the class data is to be streamed
         */
        void toStream(std::ostream& os = std::cout) const;
    };

} // namespace jtest
} // namespace mrg

#endif // mrg_jtest_JournalParameters_hpp
