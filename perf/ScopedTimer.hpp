/**
 * \file ScopedTimer.hpp
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

#ifndef mrg_jtest_ScopedTimer_hpp
#define mrg_jtest_ScopedTimer_hpp

#include <ctime>


namespace mrg
{
namespace jtest
{

    class ScopedTimer;

} // namespace jtest
} // namespace mrg

#include "ScopedTimable.hpp" // circular include

namespace mrg
{
namespace jtest
{

    /**
     * \brief Scoped timer class that starts timing on construction and finishes on destruction.
     *
     * The scoped timer will take the current time on construction and again on destruction. The destructor
     * will calculate the elapsed time from the difference between these two times and write the result
     * as a double to the double ref supplied to the constructor. A second constructor will accept a class (or
     * subclass) of ScopedTimable, which contains a double to which the result may be written and accessed at a
     * later time.
     */
    class ScopedTimer
    {
        double& _elapsed;           ///< Ref to elapsed time, will be written on destruction of ScopedTimer instances
        ::timespec _startTime;   ///< Start time, set on construction

        /**
         * \brief Convert ::timespec to seconds
         *
         * Static function to convert a ::timespec struct into a double representation in seconds.
         *
         * \param ts std::timespec struct containing the time to be converted.
         * \return A double which represents the time in parameter ts in seconds.
         */
        static double _s_getDoubleTime(const ::timespec& ts);

    public:
        /**
         * \brief Constructor
         *
         * Constructor which accepts a ref to a double. Will start the time interval measurement.
         *
         * \param elapsed A ref to a double which will contain the elapsed time in seconds after this class instance
         *                is destroyed.
         */
        ScopedTimer(double& elapsed);

        /**
         * \brief Constructor
         *
         * Constructor which accepts a ref to a ScopedTimable. Will start the time interval measurement.
         *
         * \param st A ref to a ScopedTimable into which the result of the ScopedTimer can be written.
         */
        ScopedTimer(ScopedTimable& st);

        /**
         * \brief Destructor
         *
         * Destructor. Will stop the time interval measurement and write the calculated elapsed time into _elapsed.
         */
        ~ScopedTimer();
    };

} // namespace jtest
} // namespace mrg

#endif // mrg_jtest_ScopedTimer_hpp
