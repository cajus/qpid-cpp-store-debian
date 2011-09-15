/**
 * \file JournalState.hpp
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

#ifndef mrg_journal2_JournalState_hpp
#define mrg_journal2_JournalState_hpp

#include <string>

namespace mrg
{
namespace journal2
{

    /**
     * Journal state machine
     *
     *           JS_NONE  *
     *                   / \
     *                  /   v
     *                 |     o JS_RECOVERING_PHASE_1
     *                 v     |
     * JS_INITIALIZING o     v
     *                 |     o JS_RECOVERING_PHASE_2
     *                  \   /
     *                   v v
     *         JS_RUNNING o
     *                    |
     *                    v
     *        JS_STOPPING o
     *                    |
     *                    v
     *         JS_STOPPED *
     */
    typedef enum
    {
        JS_NONE = 0,
        JS_RECOVERING_PHASE_1,
        JS_RECOVERING_PHASE_2,
        JS_INITIALIZING,
        JS_RUNNING,
        JS_STOPPING,
        JS_STOPPED
    } journalState_t;

    class JournalState
    {
    protected:
        journalState_t _journalState;
    public:
        JournalState();
        JournalState(const JournalState& s);
        JournalState(const journalState_t s);

        // Raw state get/set functions
        const journalState_t& get() const;
        void set(const journalState_t s);

        // State change functions
        void reset();
        void setStateInitializing();
        void setStateRecoverPhase1();
        void setStateRecoverPhase2();
        void setStateRunning();
        void setStateStopping();
        void setStateStopped();

        static std::string s_getStateAsString(journalState_t s);
    };

} // namespace journal2
} // namespace mrg


#endif // mrg_journal2_JournalState_hpp

