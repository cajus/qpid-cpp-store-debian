/**
 * \file DataTokenState.hpp
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

#ifndef mrg_journal2_DataTokenState_hpp
#define mrg_journal2_DataTokenState_hpp

#include <string>

namespace mrg
{
namespace journal2
{

    typedef enum
    {
        OP_NONE = 0,
        OP_ENQUEUE,
        OP_DEQUEUE
    } opState_t;

    typedef enum
    {
        TX_NONE = 0,
        TX_COMMIT,
        TX_ABORT
    } txnState_t;

    typedef enum
    {
        IO_NONE = 0,
        IO_PART,
        IO_COMPLETE
    } aioState_t;

    class DataTokenState
    {
        opState_t _opState;
        txnState_t _txnState;
        aioState_t _bufferWriteState;
        aioState_t _aioSubmissionState;
        aioState_t _aioCompletionState;
    public:
        DataTokenState();
        DataTokenState(const DataTokenState& s);
        DataTokenState(const opState_t o,
                       const txnState_t t,
                       const aioState_t b,
                       const aioState_t s,
                       const aioState_t c);

        // Raw state get/set functions
        inline opState_t getOpState() const { return _opState; }
        inline void setOpState(const opState_t s) { _opState = s; }
        inline txnState_t getTxnState() const { return _txnState; }
        inline void setTxnState(const txnState_t t) { _txnState = t; }
        inline aioState_t getBufferWriteState() const { return _bufferWriteState; }
        inline void setBufferWriteState(const aioState_t b) { _bufferWriteState = b; }
        inline aioState_t getAioSubmissionState() const { return _aioSubmissionState; }
        inline void setAioSubmissionState(const aioState_t s) { _aioSubmissionState = s; }
        inline aioState_t getAioCompletionState() const { return _aioCompletionState; }
        inline void setAioCompletionState(const aioState_t c) { _aioCompletionState = c; }

        // State change functions
        void reset();
        void setOpStateToEnqueue();
        void setOpStateToDequeue();
        void setTxnStateToCommit();
        void setTxnStateToAbort();
        void setBufferWriteStateToPart();
        void setBufferWriteStateToComplete();
        void setAioSubmissionStateToPart();
        void setAioSubmissionStateToComplete();
        void setAioCompletionStateToPart();
        void setAioCompletionStateToComplete();

        // State query functions
        inline bool isEnqueueable() const { return _opState == OP_NONE; }
        inline bool isDequeueable() const { return _opState == OP_ENQUEUE; }
        inline bool isTxnOpen() const { return _txnState == TX_NONE; }
        inline bool isBufferWriteStateComplete() const { return _bufferWriteState == IO_COMPLETE; }
        inline bool isAioSubmissionStateComplete() const { return _aioSubmissionState == IO_COMPLETE; }
        inline bool isAioCompletionStateComplete() const { return _aioCompletionState == IO_COMPLETE; }

        // State-to-string function(s)
        static std::string s_getStateAsString(opState_t s);
        static std::string s_getStateAsString(txnState_t s);
        static std::string s_getStateAsString(aioState_t s);
   };

} // namespace journal2
} // namespace mrg


#endif // mrg_journal2_DataTokenState_hpp

