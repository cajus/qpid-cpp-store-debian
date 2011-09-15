/**
 * \file JournalException.cpp
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

#include "JournalException.hpp"

#include <iomanip>

namespace mrg
{
namespace journal2
{

    JournalException::JournalException() throw ():
        std::exception(),
        _errorCode(0)
    {
        _formatWhatStr();
    }

    JournalException::JournalException(const uint32_t errorCode) throw ():
        std::exception(),
        _errorCode(errorCode)
    {
        _formatWhatStr();
    }

    JournalException::JournalException(const char* additionalInfo) throw ():
        std::exception(),
        _errorCode(0),
        _additionalInfo(additionalInfo)
    {
        _formatWhatStr();
    }

    JournalException::JournalException(const std::string& additionalInfo) throw ():
        std::exception(),
        _errorCode(0),
        _additionalInfo(additionalInfo)
    {
        _formatWhatStr();
    }

    JournalException::JournalException(const uint32_t errorCode,
                                       const char* additionalInfo) throw ():
        std::exception(),
        _errorCode(errorCode),
        _additionalInfo(additionalInfo)
    {
        _formatWhatStr();
    }

    JournalException::JournalException(const uint32_t errorCode,
                                       const std::string& additionalInfo) throw ():
        std::exception(),
        _errorCode(errorCode),
        _additionalInfo(additionalInfo)
    {
        _formatWhatStr();
    }

    JournalException::JournalException(const uint32_t errorCode,
                                       const char* throwingClass,
                                       const char* throwingFunction) throw ():
        std::exception(),
        _errorCode(errorCode),
        _throwingClass(throwingClass),
        _throwingFunction(throwingFunction)
    {
        _formatWhatStr();
    }

    JournalException::JournalException(const uint32_t errorCode,
                                       const std::string& throwingClass,
                                       const std::string& throwingFunction) throw ():
        std::exception(),
        _errorCode(errorCode),
        _throwingClass(throwingClass),
        _throwingFunction(throwingFunction)
    {
        _formatWhatStr();
    }

    JournalException::JournalException(const uint32_t errorCode,
                                       const char* additionalInfo,
                                       const char* throwingClass,
                                       const char* throwingFunction) throw ():
        std::exception(),
        _errorCode(errorCode),
        _additionalInfo(additionalInfo),
        _throwingClass(throwingClass),
        _throwingFunction(throwingFunction)
    {
        _formatWhatStr();
    }

    JournalException::JournalException(const uint32_t errorCode,
                                       const std::string& additionalInfo,
                                       const std::string& throwingClass,
                                       const std::string& throwingFunction) throw ():
        std::exception(),
        _errorCode(errorCode),
        _additionalInfo(additionalInfo),
        _throwingClass(throwingClass),
        _throwingFunction(throwingFunction)
    {
        _formatWhatStr();
    }

    void
    JournalException::_formatWhatStr() throw ()
    {
        try {
            const bool ai = !_additionalInfo.empty();
            const bool tc = !_throwingClass.empty();
            const bool tf = !_throwingFunction.empty();
            std::ostringstream oss;
            oss << "JournalException 0x" << std::hex << std::setfill('0') << std::setw(4) << _errorCode << " ";
            if (tc) {
                oss << _throwingClass;
                if (tf) {
                    oss << "::";
                } else {
                    oss << " ";
                }
            }
            if (tf) {
                oss << _throwingFunction << "() ";
            }
            if (tc || tf) {
                oss << "threw " << JournalErrors::s_errorMessage(_errorCode);
            }
            if (ai) {
                oss << " (" << _additionalInfo << ")";
            }
            _what.assign(oss.str());
        } catch (...) {}
    }

    const char*
    JournalException::what() const throw ()
    {
        return _what.c_str();
    }

    std::ostream&
    operator<<(std::ostream& os,
               const JournalException& je)
    {
        os << je.what();
        return os;
    }

    std::ostream&
    operator<<(std::ostream& os,
               const JournalException* jePtr)
    {
        os << jePtr->what();
        return os;
    }

} // namespace journal2
} // namespace mrg
