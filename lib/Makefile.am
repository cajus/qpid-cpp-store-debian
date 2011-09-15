# Copyright (c) 2007, 2008, 2009 Red Hat, Inc.
#
# This file is part of the Qpid async store library msgstore.so.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
# USA
#
# The GNU Lesser General Public License is available in the file COPYING.

AM_CXXFLAGS = $(WARNING_CFLAGS) $(APR_CXXFLAGS) $(QPID_CXXFLAGS) -I$(srcdir)/gen -DRHM_CLEAN -pthread

#SUBDIRS = jrnl2

# Definitions for client and daemon plugins
PLUGINLDFLAGS=-no-undefined -module -avoid-version
dmoduledir=$(libdir)/qpid/daemon
dmodule_LTLIBRARIES = msgstore.la
 
msgstore_la_LIBADD =            \
  $(APR_LIBS)                   \
  $(LIB_DLOPEN)                 \
  $(LIB_BERKELEY_DB)            \
  $(LIB_CLOCK_GETTIME)          \
  $(QPID_LIBS)

msgstore_la_LDFLAGS =           \
  $(PLUGINLDFLAGS)

msgstore_la_SOURCES =           \
  StorePlugin.cpp               \
  BindingDbt.cpp                \
  BufferValue.cpp               \
  DataTokenImpl.cpp             \
  IdDbt.cpp                     \
  IdSequence.cpp                \
  JournalImpl.cpp               \
  MessageStoreImpl.cpp          \
  PreparedTransaction.cpp       \
  TxnCtxt.cpp                   \
  BindingDbt.h                  \
  BufferValue.h                 \
  Cursor.h                      \
  DataTokenImpl.h               \
  IdDbt.h                       \
  IdSequence.h                  \
  JournalImpl.h                 \
  MessageStoreImpl.h            \
  PreparedTransaction.h         \
  StoreException.h              \
  TxnCtxt.h                     \
  jrnl/aio.cpp                  \
  jrnl/cvar.cpp                 \
  jrnl/data_tok.cpp             \
  jrnl/deq_rec.cpp              \
  jrnl/enq_map.cpp              \
  jrnl/enq_rec.cpp              \
  jrnl/fcntl.cpp                \
  jrnl/jcntl.cpp                \
  jrnl/jdir.cpp                 \
  jrnl/jerrno.cpp               \
  jrnl/jexception.cpp           \
  jrnl/jinf.cpp                 \
  jrnl/jrec.cpp                 \
  jrnl/lp_map.cpp               \
  jrnl/lpmgr.cpp                \
  jrnl/pmgr.cpp                 \
  jrnl/rmgr.cpp                 \
  jrnl/rfc.cpp                  \
  jrnl/rrfc.cpp                 \
  jrnl/slock.cpp                \
  jrnl/smutex.cpp               \
  jrnl/time_ns.cpp              \
  jrnl/txn_map.cpp              \
  jrnl/txn_rec.cpp              \
  jrnl/wmgr.cpp                 \
  jrnl/wrfc.cpp                 \
  jrnl/aio.hpp                  \
  jrnl/aio_callback.hpp         \
  jrnl/cvar.hpp                 \
  jrnl/data_tok.hpp             \
  jrnl/deq_hdr.hpp              \
  jrnl/deq_rec.hpp              \
  jrnl/enq_hdr.hpp              \
  jrnl/enq_map.hpp              \
  jrnl/enq_rec.hpp              \
  jrnl/enums.hpp                \
  jrnl/fcntl.hpp                \
  jrnl/file_hdr.hpp             \
  jrnl/jcfg.hpp                 \
  jrnl/jcntl.hpp                \
  jrnl/jdir.hpp                 \
  jrnl/jerrno.hpp               \
  jrnl/jexception.hpp           \
  jrnl/jinf.hpp                 \
  jrnl/jrec.hpp                 \
  jrnl/lp_map.hpp               \
  jrnl/lpmgr.hpp                \
  jrnl/pmgr.hpp                 \
  jrnl/rcvdat.hpp               \
  jrnl/rec_hdr.hpp              \
  jrnl/rec_tail.hpp             \
  jrnl/rmgr.hpp                 \
  jrnl/rfc.hpp                  \
  jrnl/rrfc.hpp                 \
  jrnl/slock.hpp                \
  jrnl/smutex.hpp               \
  jrnl/time_ns.hpp              \
  jrnl/txn_hdr.hpp              \
  jrnl/txn_map.hpp              \
  jrnl/txn_rec.hpp              \
  jrnl/wmgr.hpp                 \
  jrnl/wrfc.hpp                 \
  gen/qmf/com/redhat/rhm/store/EventCreated.cpp \
  gen/qmf/com/redhat/rhm/store/EventCreated.h \
  gen/qmf/com/redhat/rhm/store/EventEnqThresholdExceeded.cpp \
  gen/qmf/com/redhat/rhm/store/EventEnqThresholdExceeded.h \
  gen/qmf/com/redhat/rhm/store/EventFull.cpp \
  gen/qmf/com/redhat/rhm/store/EventFull.h \
  gen/qmf/com/redhat/rhm/store/EventRecovered.cpp \
  gen/qmf/com/redhat/rhm/store/EventRecovered.h \
  gen/qmf/com/redhat/rhm/store/Package.cpp  \
  gen/qmf/com/redhat/rhm/store/Package.h    \
  gen/qmf/com/redhat/rhm/store/Journal.cpp  \
  gen/qmf/com/redhat/rhm/store/Journal.h    \
  gen/qmf/com/redhat/rhm/store/Store.cpp    \
  gen/qmf/com/redhat/rhm/store/Store.h      \
  gen/qmf/com/redhat/rhm/store/ArgsJournalExpand.h


BUILT_SOURCES = db-inc.h
db-inc.h: Makefile.in
	echo '#include <$(DB_CXX_HEADER_PREFIX)db_cxx.h>' > $@-t
	mv $@-t $@
