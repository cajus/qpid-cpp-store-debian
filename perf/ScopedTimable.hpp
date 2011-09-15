/**
 * \file ScopedTimable.hpp
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

#ifndef mrg_jtest_ScopedTimable_hpp
#define mrg_jtest_ScopedTimable_hpp

namespace mrg
{
namespace jtest
{

    class ScopedTimable;

} // namespace jtest
} // namespace mrg

#include "ScopedTimer.hpp" // circular include

namespace mrg
{
namespace jtest
{

    /**
     * \brief Scoped timer class that starts timing on construction and finishes on destruction.
     *
     * This class is designed to be the parent class for a performance result class which depends on the elapsed
     * time of some process or event. By passing this (or its subclasses) to ScopedTimer (which only exists within
     * the scope of the event), the _elapsed member of this class will be written with the elapsed time when the
     * ScopedTimer object goes out of scope or is destroyed.
     *
     * Subclasses may be aware of the parameters being timed, and may thus print and/or display performance and/or
     * rate information for these parameters.
     */
    class ScopedTimable
    {
    protected:
        double _elapsed;    ///< Elapsed time, will be written on destruction of ScopedTimer instances

    public:
        /**
         * \brief Constructor
         */
        inline ScopedTimable() : _elapsed(0.0) {}

        /**
         * \brief Destructor
         */
        virtual inline ~ScopedTimable() {}

        friend class ScopedTimer;
    };

} // namespace jtest
} // namespace mrg

#endif // mrg_jtest_ScopedTimer_hpp
