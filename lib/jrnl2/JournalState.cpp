/**
 * \file JournalState.cpp
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

#include "JournalState.hpp"

#include "JournalException.hpp"

namespace mrg
{
namespace journal2
{

    JournalState::JournalState() :
        _journalState(JS_NONE)
    {}

    JournalState::JournalState(const JournalState& s) :
        _journalState(s._journalState)
    {}

    JournalState::JournalState(const journalState_t s) :
        _journalState(s)
    {}

    const journalState_t&
    JournalState::get() const
    {
        return _journalState;
    }

    void
    JournalState::set(const journalState_t s)
    {
        _journalState = s;
    }

    void
    JournalState::reset()
    {
        _journalState = JS_NONE;
    }

    void
    JournalState::setStateInitializing()
    {
        if (_journalState != JS_NONE)
            THROW_STATE_EXCEPTION(JournalErrors::JERR_BADJRNLSTATE,
                                  s_getStateAsString(JS_INITIALIZING),
                                  s_getStateAsString(_journalState),
                                  "JournalState",
                                  "setStateInitializing");
        _journalState = JS_INITIALIZING;
    }

    void
    JournalState::setStateRecoverPhase1()
    {
        if (_journalState != JS_NONE)
            THROW_STATE_EXCEPTION(JournalErrors::JERR_BADJRNLSTATE,
                                  s_getStateAsString(JS_RECOVERING_PHASE_1),
                                  s_getStateAsString(_journalState),
                                  "JournalState",
                                  "setStateRecoverPhase1");
        _journalState = JS_RECOVERING_PHASE_1;
    }

    void
    JournalState::setStateRecoverPhase2()
    {
        if (_journalState != JS_RECOVERING_PHASE_1)
            THROW_STATE_EXCEPTION(JournalErrors::JERR_BADJRNLSTATE,
                                  s_getStateAsString(JS_RECOVERING_PHASE_2),
                                  s_getStateAsString(_journalState),
                                  "JournalState",
                                  "setStateRecoverPhase2");
        _journalState = JS_RECOVERING_PHASE_2;
    }

    void
    JournalState::setStateRunning()
    {
        if (_journalState != JS_INITIALIZING ||_journalState != JS_RECOVERING_PHASE_2 )
            THROW_STATE_EXCEPTION(JournalErrors::JERR_BADJRNLSTATE,
                                  s_getStateAsString(JS_RUNNING),
                                  s_getStateAsString(_journalState),
                                  "JournalState",
                                  "setStateRunning");
        _journalState = JS_RUNNING;
    }

    void
    JournalState::setStateStopping()
    {
        if (_journalState != JS_RUNNING)
            THROW_STATE_EXCEPTION(JournalErrors::JERR_BADJRNLSTATE,
                                  s_getStateAsString(JS_STOPPING),
                                  s_getStateAsString(_journalState),
                                  "JournalState",
                                  "setStateStopping");
        _journalState = JS_STOPPING;
    }

    void
    JournalState::setStateStopped()
    {
        if (_journalState != JS_STOPPING)
            THROW_STATE_EXCEPTION(JournalErrors::JERR_BADJRNLSTATE,
                                  s_getStateAsString(JS_STOPPED),
                                  s_getStateAsString(_journalState),
                                  "JournalState",
                                  "setStateStopped");
        _journalState = JS_STOPPED;
    }

    //static
    std::string
    JournalState::s_getStateAsString(journalState_t s)
    {
        switch (s) {
        case JS_NONE:
            return "JS_NONE";
        case JS_RECOVERING_PHASE_1:
            return "JS_RECOVERING_PHASE_1";
        case JS_RECOVERING_PHASE_2:
            return "JS_RECOVERING_PHASE_2";
        case JS_INITIALIZING:
            return "JS_INITIALIZING";
        case JS_RUNNING:
            return "JS_RUNNING";
        case JS_STOPPING:
            return "JS_STOPPING";
        case JS_STOPPED:
            return "JS_STOPPED";
       default:
            std::ostringstream oss;
            oss << "<unknown state (" << "s" << ")>";
            return oss.str();
        }
    }

} // namespace journal2
} // namespace mrg

