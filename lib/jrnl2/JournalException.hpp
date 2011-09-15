/**
 * \file JournalException.hpp
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

#ifndef mrg_journal2_JournalException_hpp
#define mrg_journal2_JournalException_hpp

#include <cstring> // std::strerror
#include <sstream>
#include <string>
#include <stdint.h> // uint32_t

#include "JournalErrors.hpp"

// Macro definitions

#define FORMAT_SYSERR(errno) " errno=" << errno << " (" << std::strerror(errno) << ")"

#define PTHREAD_CHK(err, pfn, cls, fn) if(err != 0) { \
    std::ostringstream oss; \
    oss << pfn << " failed: " << FORMAT_SYSERR(err); \
    throw JournalException(JournalErrors::JERR_PTHREAD, oss.str(), cls, fn); \
    }

#define THROW_STATE_EXCEPTION(jerrno, target_st, curr_st, cls, fn) { \
    std::ostringstream oss; \
    oss << cls << "::" << fn << "() in state " << curr_st << " cannot be moved to state " << target_st << "."; \
    throw JournalException(jerrno, oss.str(), cls, fn); \
    }

namespace mrg
{
namespace journal2
{

    class JournalException : public std::exception
    {
    protected:
        uint32_t _errorCode;
        std::string _additionalInfo;
        std::string _throwingClass;
        std::string _throwingFunction;
        std::string _what;

        void _formatWhatStr() throw ();

    public:
        JournalException() throw ();

        JournalException(const uint32_t errorCode) throw ();

        JournalException(const char* additionalInfo) throw ();
        JournalException(const std::string& additionalInfo) throw ();

        JournalException(const uint32_t errorCode,
                         const char* additionalInfo) throw ();
        JournalException(const uint32_t errorCode,
                         const std::string& additionalInfo) throw ();

        JournalException(const uint32_t errorCode,
                         const char* throwingClass,
                         const char* throwingFunction) throw ();
        JournalException(const uint32_t errorCode,
                         const std::string& throwingClass,
                         const std::string& throwingFunction) throw ();

        JournalException(const uint32_t errorCode,
                         const char* additionalInfo,
                         const char* throwingClass,
                         const char* throwingFunction) throw ();
        JournalException(const uint32_t errorCode,
                         const std::string& additionalInfo,
                         const std::string& throwingClass,
                         const std::string& throwingFunction) throw ();

        virtual ~JournalException() throw () {}
        const char* what() const throw (); // override std::exception::what()

        inline uint32_t getErrorCode() const throw () { return _errorCode; }
        inline const std::string getAdditionalInfo() const throw () { return _additionalInfo; }
        inline const std::string getThrowingClass() const throw () { return _throwingClass; }
        inline const std::string getThrowingFunction() const throw () { return _throwingFunction; }

        friend std::ostream& operator<<(std::ostream& os, const JournalException& je);
        friend std::ostream& operator<<(std::ostream& os, const JournalException* jePtr);
    }; // class JournalException

} // namespace journal2
} // namespace mrg

#endif // mrg_journal2_JournalException_hpp
