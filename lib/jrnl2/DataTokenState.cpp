/**
 * \file DataTokenState.cpp
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

#include "DataTokenState.hpp"

#include "JournalException.hpp"

namespace mrg
{
namespace journal2
{

    DataTokenState::DataTokenState() :
        _opState(OP_NONE),
        _txnState(TX_NONE),
        _bufferWriteState(IO_NONE),
        _aioSubmissionState(IO_NONE),
        _aioCompletionState(IO_NONE)
    {}

    DataTokenState::DataTokenState(const DataTokenState& s) :
        _opState(s._opState),
        _txnState(s._txnState),
        _bufferWriteState(s._bufferWriteState),
        _aioSubmissionState(s._aioSubmissionState),
        _aioCompletionState(s._aioCompletionState)
    {}

    DataTokenState::DataTokenState(const opState_t o,
                                   const txnState_t t,
                                   const aioState_t b,
                                   const aioState_t s,
                                   const aioState_t c) :
        _opState(o),
        _txnState(t),
        _bufferWriteState(b),
        _aioSubmissionState(s),
        _aioCompletionState(c)
    {}

    void
    DataTokenState::reset()
    {
        _opState = OP_NONE;
        _txnState = TX_NONE;
        _bufferWriteState = IO_NONE;
        _aioSubmissionState = IO_NONE;
        _aioCompletionState = IO_NONE;
    }

    void
    DataTokenState::setOpStateToEnqueue()
    {
        if (_opState != OP_NONE)
            THROW_STATE_EXCEPTION(JournalErrors::JERR_BADDTOKOPSTATE,
                                  s_getStateAsString(OP_ENQUEUE),
                                  s_getStateAsString(_opState),
                                  "dtok_state",
                                  "set_enqueue");
        _opState = OP_ENQUEUE;
    }

    void
    DataTokenState::setOpStateToDequeue()
    {
        if (_opState != OP_ENQUEUE)
            THROW_STATE_EXCEPTION(JournalErrors::JERR_BADDTOKOPSTATE,
                                  s_getStateAsString(OP_DEQUEUE),
                                  s_getStateAsString(_opState),
                                  "dtok_state",
                                  "set_dequeue");
        _opState = OP_DEQUEUE;
    }

    void
    DataTokenState::setTxnStateToCommit()
    {
        if (_txnState != TX_NONE)
            THROW_STATE_EXCEPTION(JournalErrors::JERR_BADDTOKTXNSTATE,
                                  s_getStateAsString(TX_COMMIT),
                                  s_getStateAsString(_txnState),
                                  "dtok_state",
                                  "set_txn_commit");
        _txnState = TX_COMMIT;
    }

    void
    DataTokenState::setTxnStateToAbort()
    {
        if (_txnState != TX_NONE)
            THROW_STATE_EXCEPTION(JournalErrors::JERR_BADDTOKTXNSTATE,
                                  s_getStateAsString(TX_ABORT),
                                  s_getStateAsString(_txnState),
                                  "dtok_state",
                                  "set_txn_abort");
        _txnState = TX_ABORT;
    }

    void
    DataTokenState::setBufferWriteStateToPart()
    {
        if (_bufferWriteState != IO_NONE)
            THROW_STATE_EXCEPTION(JournalErrors::JERR_BADDTOKIOSTATE,
                                  s_getStateAsString(IO_PART),
                                  s_getStateAsString(_bufferWriteState),
                                  "dtok_state",
                                  "set_buff_part");
        _bufferWriteState = IO_PART;
    }

    void
    DataTokenState::setBufferWriteStateToComplete()
    {
        _bufferWriteState = IO_COMPLETE;
    }

    void
    DataTokenState::setAioSubmissionStateToPart()
    {
        if (_aioSubmissionState != IO_NONE)
            THROW_STATE_EXCEPTION(JournalErrors::JERR_BADDTOKIOSTATE,
                                  s_getStateAsString(IO_PART),
                                  s_getStateAsString(_aioSubmissionState),
                                  "dtok_state",
                                  "set_iosubm_part");
        _aioSubmissionState = IO_PART;
    }

    void
    DataTokenState::setAioSubmissionStateToComplete()
    {
        _aioSubmissionState = IO_COMPLETE;
    }

    void
    DataTokenState::setAioCompletionStateToPart()
    {
        if (_aioCompletionState != IO_NONE)
            THROW_STATE_EXCEPTION(JournalErrors::JERR_BADDTOKIOSTATE,
                                  s_getStateAsString(IO_PART),
                                  s_getStateAsString(_aioCompletionState),
                                  "dtok_state",
                                  "set_iocompl_part");
        _aioCompletionState = IO_PART;
    }

    void
    DataTokenState::setAioCompletionStateToComplete()
    {
        _aioCompletionState = IO_COMPLETE;
    }

    //static
    std::string
    DataTokenState::s_getStateAsString(opState_t s)
    {
        switch (s) {
            case OP_NONE:
                return "OP_NONE";
            case OP_ENQUEUE:
                return "OP_ENQUEUE";
            case OP_DEQUEUE:
                return "OP_DEQUEUE";
            default:
                std::ostringstream oss;
                oss << "<unknown state (" << s << ")>";
                return oss.str();
        }
    }

    //static
    std::string
    DataTokenState::s_getStateAsString(txnState_t s)
    {
        switch (s) {
            case TX_NONE:
                return "TX_NONE";
            case TX_COMMIT:
                return "TX_COMMIT";
            case TX_ABORT:
                return "TX_ABORT";
            default:
                std::ostringstream oss;
                oss << "<unknown state (" << s << ")>";
                return oss.str();
        }
    }

    //static
    std::string
    DataTokenState::s_getStateAsString(aioState_t s)
    {
        switch (s) {
            case IO_NONE:
                return "IO_NONE";
            case IO_PART:
                return "IO_PART";
            case IO_COMPLETE:
                return "IO_COMPLETE";
            default:
                std::ostringstream oss;
                oss << "<unknown state (" << s << ")>";
                return oss.str();
        }
    }

} // namespace journal2
} // namespace mrg

