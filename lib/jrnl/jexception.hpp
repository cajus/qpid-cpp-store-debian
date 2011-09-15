/**
 * \file jexception.hpp
 *
 * Qpid asynchronous store plugin library
 *
 * Generic journal exception class mrg::journal::jexception (derived
 * from class std::exception). Intended to serve as a common exception
 * class for all more speicalized exceptions in the message journal. See
 * class documentation for details.
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

#ifndef mrg_journal_jrnl_exception_hpp
#define mrg_journal_jrnl_exception_hpp

namespace mrg
{
namespace journal
{
class jexception;
}
}

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include "jrnl/jerrno.hpp"
#include <sstream>
#include <string>
#include <sys/types.h>

// Macro for formatting commom system errors
#define FORMAT_SYSERR(errno) " errno=" << errno << " (" << std::strerror(errno) << ")"

#define MALLOC_CHK(ptr, var, cls, fn) if(ptr == 0) { \
    clean(); \
    std::ostringstream oss; \
    oss << var << ": malloc() failed: " << FORMAT_SYSERR(errno); \
    throw jexception(jerrno::JERR__MALLOC, oss.str(), cls, fn); \
    }

// TODO: The following is a temporary bug-tracking aid which forces a core.
// Replace with the commented out version below when BZ484048 is resolved.
#define PTHREAD_CHK(err, pfn, cls, fn) if(err != 0) { \
    std::ostringstream oss; \
    oss << cls << "::" << fn << "(): " << pfn; \
    errno = err; \
    ::perror(oss.str().c_str()); \
    ::abort(); \
    }
/*
#define PTHREAD_CHK(err, pfn, cls, fn) if(err != 0) { \
    std::ostringstream oss; \
    oss << pfn << " failed: " << FORMAT_SYSERR(err); \
    throw jexception(jerrno::JERR__PTHREAD, oss.str(), cls, fn); \
    }
*/

#define ASSERT(cond, msg) if(cond == 0) { \
    std::cerr << msg << std::endl; \
    ::abort(); \
    }

namespace mrg
{
namespace journal
{
    /**
    * \class jexception
    * \brief Generic journal exception class
    */
    class jexception : public std::exception
    {
    private:
        u_int32_t _err_code;
        std::string _additional_info;
        std::string _throwing_class;
        std::string _throwing_fn;
        std::string _what;
        void format();

    public:
        jexception() throw ();

        jexception(const u_int32_t err_code) throw ();

        jexception(const char* additional_info) throw ();
        jexception(const std::string& additional_info) throw ();

        jexception(const u_int32_t err_code, const char* additional_info) throw ();
        jexception(const u_int32_t err_code, const std::string& additional_info) throw ();

        jexception(const u_int32_t err_code, const char* throwing_class, const char* throwing_fn)
                throw ();
        jexception(const u_int32_t err_code, const std::string& throwing_class,
                const std::string& throwing_fn) throw ();

        jexception(const u_int32_t err_code, const char* additional_info,
                const char* throwing_class, const char* throwing_fn) throw ();
        jexception(const u_int32_t err_code, const std::string& additional_info,
                const std::string& throwing_class, const std::string& throwing_fn) throw ();

        virtual ~jexception() throw ();
        virtual const char* what() const throw (); // override std::exception::what()

        inline u_int32_t err_code() const throw () { return _err_code; }
        inline const std::string additional_info() const throw () { return _additional_info; }
        inline const std::string throwing_class() const throw () { return _throwing_class; }
        inline const std::string throwing_fn() const throw () { return _throwing_fn; }

        friend std::ostream& operator<<(std::ostream& os, const jexception& je);
        friend std::ostream& operator<<(std::ostream& os, const jexception* jePtr);
    }; // class jexception

} // namespace journal
} // namespace mrg

#endif // ifndef mrg_journal_jrnl_exception_hpp
