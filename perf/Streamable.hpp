/**
 * \file Streamable.hpp
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

#ifndef mrg_jtest_Streamable_hpp
#define mrg_jtest_Streamable_hpp

#include <iostream>
#include <string>

namespace mrg
{
namespace jtest
{

    /**
     * \brief Abstract class which provides the mechanisms to stream
     *
     * An abstract class which provides stream functions. The toStream() function must be implemented by subclasses,
     * and is used by the remaining functions. For convenience, toString() returns a std::string object.
     */
    struct Streamable
    {
        /**
         * \brief Virtual destructor
         */
        virtual ~Streamable() {}

        /***
         * \brief Stream some representation of the object to an output stream
         *
         * \param os Output stream to which the class data is to be streamed
         */
        virtual void toStream(std::ostream& os = std::cout) const = 0;

        /**
         * \brief Creates a string representation of the test parameters
         *
         * Convenience feature which creates and returns a std::string object containing the content of toStream().
         *
         * \return Content of toStream()
         */
         std::string toString() const;

        /**
         * \brief Stream the object to an output stream
         */
        friend std::ostream& operator<<(std::ostream& os, const Streamable& s);

        /**
         * \brief Stream the object to an output stream through an object pointer
         */
        friend std::ostream& operator<<(std::ostream& os, const Streamable* sPtr);
    };

} // namespace jtest
} // namespace mrg

#endif // mrg_jtest_Streamable_hpp
