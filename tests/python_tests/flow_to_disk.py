"""
Copyright (c) 2008 Red Hat, Inc.

This file is part of the Qpid async store library msgstore.so.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
USA

The GNU Lesser General Public License is available in the file COPYING.
"""

import qpid
from brokertest import EXPECT_EXIT_OK, EXPECT_UNKNOWN
from qpid.datatypes import uuid4
from store_test import StoreTest, store_args
from qpid.messaging import Message, TargetCapacityExceeded, ServerError #SessionError, SendError

class FlowToDisk(StoreTest):
    """Tests for async store flow-to-disk"""

    def _broker_args(self):
        """ Disable flow control so we can easily overflow a queue!
        """
        fc_off = ["--default-flow-stop-threshold", "0",
                  "--default-flow-resume-threshold", "0"]
        return store_args() +  fc_off;


    @staticmethod
    def _broker_name(queue_name, txn_produce, txn_consume):
        """Create a broker name based on the queue name and the transaction parameters"""
        name = queue_name
        if txn_produce:
            name += "_TxP"
        if txn_consume:
            name += "_TxC"
        return name

    def _tx_simple_limit(self, queue_name, kwargs):
        """
        Test a simple case of message limits which will force flow-to-disk.
        * queue_args sets a limit - either max_count and/or max_size
        * messages are added. Some will flow to disk.
        * Consume all messages sent.
        * Check the broker has no messages left.
        """
        # Unpack args
        txn_produce = kwargs.get("txn_produce", False)
        txn_consume = kwargs.get("txn_consume", False)
        recover = kwargs.get("recover", False)
        max_count = kwargs.get("max_count")
        max_size = kwargs.get("max_size")
        policy = kwargs.get("policy", "flow_to_disk")
        num_msgs = kwargs.get("num_msgs", 15)
        msg_size = kwargs.get("msg_size", 10)
        msg_durable = kwargs.get("msg_durable", False)
        sync = kwargs.get("sync", False)
        browse = kwargs.get("browse", False)
        
        bname = self._broker_name(queue_name, txn_produce, txn_consume)
        if recover:
            expect = EXPECT_UNKNOWN
        else:
            expect = EXPECT_EXIT_OK
        broker = self.broker(self._broker_args(), name=bname, expect=expect, log_level="debug+")
        prod_session = broker.connect().session(transactional=txn_produce)
        sender = prod_session.sender(self.snd_addr(queue_name, auto_create=True, durable=True, ftd_count=max_count,
                                                   ftd_size=max_size, policy=policy))
        
        # Send messages
        msgs = []
        pre_recover_ftd_msgs = []  # msgs released before a recover
        post_recover_ftd_msgs = [] # msgs released after a recover
        cum_msg_size = 0
        for index in range(0, num_msgs):
            msg = Message(self.make_message(index, msg_size), durable=msg_durable, id=uuid4(),
                          correlation_id="msg-%04d"%index)
            #print "Sending msg %s" % msg.id
            msgs.append(msg)
            cum_msg_size += msg_size
            if (max_count != None and index >= max_count) or (max_size != None and cum_msg_size > max_size):
                pre_recover_ftd_msgs.append(msg)
            sender.send(msg, sync=sync)
        if not sync:
            sender.sync()
        # Close transaction (if needed)
        if txn_produce:
            prod_session.commit()
        
        # Browse messages
        if browse:
            self.check_messages(broker, queue_name, msgs, browse=True)
            
        if recover:
            broker.terminate()
            if msg_durable:
                post_recover_ftd_msgs = pre_recover_ftd_msgs
            else:
                del msgs[:] # Transient messages will be discarded on recover
            old_broker = broker # keep for log analysis
            broker = self.broker(self._broker_args(), name=bname, expect=EXPECT_EXIT_OK, log_level="debug+")
        
            # Browse messages after recover
            if browse:
                self.check_messages(broker, queue_name, msgs, browse=True)
        
        # Consume messages
        self.check_messages(broker, queue_name, msgs, transactional=txn_consume, empty=True)
        broker.terminate()

        # Check broker logs for released messages
        if recover:
            if txn_produce:
                self.check_msg_release_on_commit(old_broker, pre_recover_ftd_msgs)
            else:
                self.check_msg_release(old_broker, pre_recover_ftd_msgs)
            self.check_msg_release_on_recover(broker, post_recover_ftd_msgs)      
        else:
            if txn_produce:
                self.check_msg_release_on_commit(broker, pre_recover_ftd_msgs)
            else:
                self.check_msg_release(broker, pre_recover_ftd_msgs)
    
    def simple_limit(self, queue_name, **kwargs):
        """Adapter for adding transactions to test"""
        # Cycle through the produce/consume block transaction combinations
        for index in range(0, 4):
            kwargs["txn_produce"] = index & 1 != 0 # Transactional produce
            kwargs["txn_consume"] = index & 2 != 0 # Transactional consume
            self._tx_simple_limit(queue_name, kwargs)

class SimpleMaxCountTest(FlowToDisk):
    """Flow-to-disk tests based on setting max_count"""
    
    def test_base(self):
        """Base test"""
        self.simple_limit("SimpleMaxCount", max_count=10)
        
    def test_recover(self):
        """Recover test"""
        self.simple_limit("SimpleMaxCountRecover", max_count=10, recover=True)
        
    def test_durable(self):
        """Durable message test"""
        self.simple_limit("SimpleMaxCountDurable", max_count=10, msg_durable=True)
        
    def test_durable_recover(self):
        """Durable message recover test"""
        self.simple_limit("SimpleMaxCountDurableRecover", max_count=10, msg_durable=True, recover=True)
        
    def test_browse(self):
        """Browse test"""
        self.simple_limit("SimpleMaxCountBrowse", max_count=10, browse=True)
        
    def test_browse_recover(self):
        """Browse before and after recover test"""
        self.simple_limit("SimpleMaxCountBrowseRecover", max_count=10, browse=True, recover=True)
        
    def test_durable_browse(self):
        """Browse durable message test"""
        self.simple_limit("SimpleMaxCountDurableBrowse", max_count=10, msg_durable=True, browse=True)
        
    def test_durable_browse_recover(self):
        """Browse durable messages before and after recover"""
        self.simple_limit("SimpleMaxCountDurableBrowseRecover", max_count=10, msg_durable=True, browse=True,
                          recover=True)
       
    def test_large_msg(self):
        """Large message test"""   
        self.simple_limit("SimpleMaxCountLargeMsg", max_count=10, max_size=10000000, num_msgs=100, msg_size=10000)
       
    def test_large_msg_recover(self):
        """Large message test"""   
        self.simple_limit("SimpleMaxCountLargeMsgRecover", max_count=10, max_size=10000000, num_msgs=100,
                          msg_size=10000, recover=True)
    
    def test_large_msg_durable(self):
        """Large durable message test"""
        self.simple_limit("SimpleMaxCountLargeMsgDurable", max_count=10, max_size=10000000, msg_durable=True,
                          num_msgs=100, msg_size=10000)
     
    def test_large_msg_durable_recover(self):
        """Large durable message test"""
        self.simple_limit("SimpleMaxCountLargeMsgDurableRecover", max_count=10, max_size=10000000, msg_durable=True,
                          num_msgs=100, msg_size=10000, recover=True)
   
    def test_large_msg_browse(self):
        """Large message browse test"""
        self.simple_limit("SimpleMaxCountLargeMsgBrowse", max_count=10, max_size=10000000, browse=True, num_msgs=100,
                          msg_size=10000)
    
    def test_large_msg_browse_recover(self):
        """Large message browse test"""
        self.simple_limit("SimpleMaxCountLargeMsgBrowseRecover", max_count=10, max_size=10000000, browse=True,
                          num_msgs=100, msg_size=10000, recover=True)
   
    def test_large_msg_durable_browse(self):
        """Large durable message browse test""" 
        self.simple_limit("SimpleMaxCountLargeMsgDurableBrowse", max_count=10, max_size=10000000, msg_durable=True,
                          browse=True, num_msgs=100, msg_size=10000)
   
    def test_large_msg_durable_browse_recover(self):
        """Large durable message browse test""" 
        self.simple_limit("SimpleMaxCountLargeMsgDurableBrowseRecover", max_count=10, max_size=10000000,
                          msg_durable=True, browse=True, num_msgs=100, msg_size=10000, recover=True)

class SimpleMaxSizeTest(FlowToDisk):
    """Flow-to-disk tests based on setting max_size"""
    
    def test_base(self):
        """Base test"""
        self.simple_limit("SimpleMaxSize", max_size=100)
        
    def test_recover(self):
        """Recover test"""
        self.simple_limit("SimpleMaxSizeRecover", max_size=100, recover=True)
        
    def test_durable(self):
        """Durable message test"""
        self.simple_limit("SimpleMaxSizeDurable", max_size=100, msg_durable=True)
        
    def test_durable_recover(self):
        """Durable message recover test"""
        self.simple_limit("SimpleMaxSizeDurable", max_size=100, msg_durable=True, recover=True)
        
    def test_browse(self):
        """Browse test"""
        self.simple_limit("SimpleMaxSizeBrowse", max_size=100, browse=True)
        
    def test_browse_recover(self):
        """Browse before and after recover test"""
        self.simple_limit("SimpleMaxSizeBrowseRecover", max_size=100, browse=True, recover=True)
        
    def test_durable_browse(self):
        """Browse durable message test"""
        self.simple_limit("SimpleMaxSizeDurableBrowse", max_size=100, msg_durable=True, browse=True)
        
    def test_durable_browse_recover(self):
        """Browse durable messages before and after recover"""
        self.simple_limit("SimpleMaxSizeDurableBrowseRecover", max_size=100, msg_durable=True, browse=True,
                          recover=True)
        
    def test_large_msg(self):
        """Large message test"""   
        self.simple_limit("SimpleMaxSizeLargeMsg", max_size=100000, num_msgs=100, msg_size=10000)
       
    def test_large_msg_recover(self):
        """Large message test"""   
        self.simple_limit("SimpleMaxSizeLargeMsgRecover", max_size=100000, num_msgs=100, msg_size=10000, recover=True)
     
    def test_large_msg_durable(self):
        """Large durable message test"""
        self.simple_limit("SimpleMaxSizeLargeMsgDurable", max_size=100000, msg_durable=True, num_msgs=100,
                          msg_size=10000)
     
    def test_large_msg_durable_recover(self):
        """Large durable message test"""
        self.simple_limit("SimpleMaxSizeLargeMsgDurableRecover", max_size=100000, msg_durable=True, num_msgs=100,
                          msg_size=10000, recover=True)
   
    def test_large_msg_browse(self):
        """Large message browse test"""
        self.simple_limit("SimpleMaxSizeLargeMsgBrowse", max_size=100, browse=True, num_msgs=100, msg_size=10000)
    
    def test_large_msg_browse_recover(self):
        """Large message browse test"""
        self.simple_limit("SimpleMaxSizeLargeMsgBrowseRecover", max_size=100, browse=True, num_msgs=100, msg_size=10000,
                          recover=True)
   
    def test_large_msg_durable_browse(self):
        """Large durable message browse test""" 
        self.simple_limit("SimpleMaxSizeLargeMsgDurableBrowse", max_size=100, msg_durable=True, browse=True,
                          num_msgs=100, msg_size=10000)
   
    def test_large_msg_durable_browse_recover(self):
        """Large durable message browse test""" 
        self.simple_limit("SimpleMaxSizeLargeMsgDurableBrowseRecover", max_size=100, msg_durable=True, browse=True,
                          num_msgs=100, msg_size=10000, recover=True)

class SimpleMaxSizeCountTest(FlowToDisk):
    """Flow-to-disk tests based on setting both max_count and max_size at the same time"""
    
    def test_base(self):
        """Base test"""
        self.simple_limit("MaxSizeMaxCount", max_count=10, max_size=1000)
        
    def test_recover(self):
        """Recover test"""
        self.simple_limit("MaxSizeMaxCountRecover", max_count=10, max_size=1000, recover=True)
        
    def test_durable(self):
        """Durable message test"""
        self.simple_limit("MaxSizeMaxCountDurable", max_count=10, max_size=1000, msg_size=250, msg_durable=True)
        
    def test_durable_recover(self):
        """Durable message recover test"""
        self.simple_limit("MaxSizeMaxCountDurableRecover", max_count=10, max_size=1000, msg_size=250, msg_durable=True,
                          recover=True)
        
    def test_browse(self):
        """Browse test"""
        self.simple_limit("MaxSizeMaxCountBrowse", max_count=10, max_size=1000, browse=True)
        
    def test_browse_recover(self):
        """Browse before and after recover test"""
        self.simple_limit("MaxSizeMaxCountBrowseRecover", max_count=10, max_size=1000, browse=True, recover=True)
        
    def test_durable_browse(self):
        """Browse durable message test"""
        self.simple_limit("MaxSizeMaxCountDurableBrowse", max_count=10, max_size=1000, msg_size=250, msg_durable=True,
                          browse=True)
        
    def test_durable_browse_recover(self):
        """Browse durable messages before and after recover"""
        self.simple_limit("MaxSizeMaxCountDurableBrowseRecover", max_count=10, max_size=1000, msg_size=250,
                          msg_durable=True, browse=True, recover=True)

# ======================================================================================================================

class MultiQueueFlowToDisk(FlowToDisk):
    """Tests for async store flow-to-disk involving multiple queues"""
    
    def _multi_queue_setup(self, queue_map, broker, exchange_name, txn_produce, txn_consume, policy, exclusive = False):
        """Create the send session and receive sessions for multi-queue scenarios"""
        connection = broker.connect()
        snd_session = connection.session(transactional=txn_produce)
        addr = self.snd_addr(exchange_name, topic_flag=True, exchage_type="fanout")
        #print "snd_addr=\"%s\"" % addr
        sndr = snd_session.sender(addr)
        for queue_name, queue_properties in queue_map.iteritems():
            if "durable" in queue_properties.keys():
                durable = queue_properties["durable"]
            else:
                durable = False
            max_count = None
            if "max_count" in queue_properties.keys():
                max_count = queue_properties["max_count"]
            max_size = None
            if "max_size" in queue_properties.keys():
                max_size = queue_properties["max_size"]
            rcv_session = connection.session(transactional=txn_consume)
            addr = self.rcv_addr(exchange_name, auto_create=False, link_name=queue_name, durable=durable,
                                 exclusive=exclusive, ftd_count=max_count, ftd_size=max_size, policy=policy)
            #print "rcv_addr=\"%s\"" % addr
            rcv_session.receiver(addr)
        return snd_session, sndr

    @staticmethod
    def _make_policy_dict(src, marker, delim=";"):
        """Create a dictionary of key/value strings from a formatted string src of the form
        '... marker key1=val1, key2=val2, ..., keyN=valN delimiter ...'
        where the portion of interest starts at marker m until the following delimiter d (default: ';')."""
        pos = src.find(marker) + len(marker)
        res = []
        for index in src[pos:src.find(delim, pos)].split():
            if "=" in index:
                res.append(index.strip(",").split("="))
        if len(res) > 0:
            return dict(res)
    
    @staticmethod
    def _make_policy_val(src, marker, delim=";"):
        """Return a string value from a formatted string of the form '... marker val delimiter ...' where the value
        lies between marker and delimiter d (default: ';')"""
        pos = src.find(marker) + len(marker)
        return src[pos:src.find(delim, pos)].strip()
    
    @staticmethod
    def _check_error(error_str, fail_list=None):
        """Check a policy exception string to ensure the failure occurred on the expected queue and at the expected
        count."""
        if error_str.startswith("resource-limit-exceeded"):
            fail_policy = MultiQueueFlowToDisk._make_policy_val(error_str, "type=", delim="(")
            fail_queue_name = MultiQueueFlowToDisk._make_policy_val(error_str, "Policy exceeded on ", delim=",")
            fail_count_dict = MultiQueueFlowToDisk._make_policy_dict(error_str, "count: ")
            fail_size_dict = MultiQueueFlowToDisk._make_policy_dict(error_str, "size: ")
            if fail_list == None:
                return False # Not expected - no failure should have occurred
            for fail in fail_list:
                if fail_queue_name == fail["queue"]:
                    if fail_policy != fail["type"]:
                        return False
                    if (fail_count_dict != None and "count" in fail and \
                        int(fail_count_dict["current"]) != fail["count"]) \
                       or \
                       (fail_size_dict != None and "size" in fail and int(fail_size_dict["current"]) != fail["size"]):
                        return False
                    return True
        return False
    
    @staticmethod
    def _check_target_capacity_exceeded_error(err, fail_list=None):
        """Check that an error is a TargetCapacityExceeded."""
        if not isinstance(err, TargetCapacityExceeded):
            return False
        return MultiQueueFlowToDisk._check_error(str(err), fail_list)
    
    @staticmethod
    def _check_server_error(err, txn=False):
        """Check that an error is a ServerError."""
        if not isinstance(err, ServerError):
            return False
        if txn and str(err).startswith("internal-error: Commit failed"):
            return True
        return False
    
    @staticmethod
    def _is_queue_durable(queue_map, index):
        """Return true if the indexed queue is durable (indexed by queue_map.keys() or queue_map.values())"""
        return "durable" in queue_map.values()[index] and queue_map.values()[index]["durable"]
    
    @staticmethod
    def _expected_msg_loss(fail_list):
        """Examine the fail_list for expected failures and return a tuple containing the expected failure conditions""" 
        count_exp_loss = None
        count_exp_loss_queues = None
        size_exp_loss = None
        size_exp_loss_queues = None
        if fail_list != None:
            for fail in fail_list:
                if "count" in fail:
                    this_count = fail["count"]
                    if count_exp_loss == None:
                        count_exp_loss = this_count
                        count_exp_loss_queues = [fail["queue"]]
                    elif this_count < count_exp_loss:
                        count_exp_loss = this_count
                        count_exp_loss_queues = [fail["queue"]]
                    elif this_count == count_exp_loss:
                        count_exp_loss_queues.append(fail["queue"])
                if "size" in fail:
                    this_size = fail["size"]
                    if size_exp_loss == None:
                        size_exp_loss = this_size
                        size_exp_loss_queues = [fail["queue"]]
                    elif this_size < size_exp_loss:
                        size_exp_loss = this_size
                        size_exp_loss_queues = [fail["queue"]]
                    elif this_size == size_exp_loss:
                        size_exp_loss_queues.append(fail["queue"])
        return (count_exp_loss, count_exp_loss_queues, size_exp_loss, size_exp_loss_queues)
 
    @staticmethod
    def _expected_msg_ftd(queue_map):
        max_count = None
        max_size = None
        for queue_props in queue_map.itervalues():
            if "durable" in queue_props and queue_props["durable"]:
                if "max_count" in queue_props and queue_props["max_count"] != None and \
                   (max_count == None or queue_props["max_count"] < max_count):
                    max_count = queue_props["max_count"]
                if "max_size" in queue_props and queue_props["max_size"] != None and \
                   (max_size == None or queue_props["max_size"] < max_size):
                    max_size = queue_props["max_size"]
        return (max_count, max_size)
        
 
    def tx_multi_queue_limit(self, broker_base_name, queue_map, exchange_name, **kwargs):
        """ Test a multi-queue case
        queue_map = queue map where map is queue name (key) against queue args (value)
        """
        # Unpack args
        msg_durable = kwargs.get("msg_durable", False)
        num_msgs = kwargs.get("num_msgs", 15)
        msg_size = kwargs.get("msg_size", 10)
        txn_produce = kwargs.get("txn_produce", False)
        txn_consume = kwargs.get("txn_consume", False)
        browse = kwargs.get("browse", False)
        policy = kwargs.get("policy", "flow_to_disk")
        recover = kwargs.get("recover", False)
        sync = kwargs.get("sync", False)
        fail_list = kwargs.get("fail_list")
        
        bname = self._broker_name(broker_base_name, txn_produce, txn_consume)
        broker = self.broker(self._broker_args(), name=bname, expect=EXPECT_EXIT_OK, log_level="debug+")
        snd_session, sndr = self._multi_queue_setup(queue_map, broker, exchange_name, txn_produce, txn_consume, policy)
        
        # Find expected limits
        count_exp_loss, count_exp_loss_queues, size_exp_loss, size_exp_loss_queues = self._expected_msg_loss(fail_list)
        max_count, max_size = self._expected_msg_ftd(queue_map)
        
        # Send messages
        try: 
            msgs = []
            pre_recover_ftd_msgs = []  # msgs released before a recover
            post_recover_ftd_msgs = [] # msgs released after a recover
            cum_msg_size = 0
            target_queues = []
            for index in range(0, num_msgs):
                msg = Message(self.make_message(index, msg_size), durable=msg_durable, id=uuid4(),
                            correlation_id="msg-%04d"%index)
                #print "Sending msg %s" % msg.id
                sndr.send(msg, sync=sync)
                if msg_size != None:
                    cum_msg_size += msg_size
                if count_exp_loss != None and index >= count_exp_loss:
                    target_queues.extend(count_exp_loss_queues)
                if size_exp_loss != None and cum_msg_size > size_exp_loss:
                    target_queues.extend(size_exp_loss_queues)
                if (count_exp_loss == None or index < count_exp_loss) and \
                   (size_exp_loss == None or cum_msg_size <= size_exp_loss):
                    msgs.append(msg)
                if (max_count != None and index >= max_count) or (max_size != None and cum_msg_size > max_size):
                    pre_recover_ftd_msgs.append(msg)
            if not sync:
                sndr.sync()
            if txn_produce:
                snd_session.commit()
        except TargetCapacityExceeded, err:
            if not self._check_target_capacity_exceeded_error(err, fail_list):
                raise
        except ServerError, err:
            msgs[:] = [] # Transaction failed, all messages lost
            if not self._check_server_error(err, txn_produce):
                raise

        # Browse messages
        if browse: 
            for index in range(0, len(queue_map)):
                self.check_messages(broker, queue_map.keys()[index], msgs, browse=True)
            
        if recover:
            broker.terminate()
            if msg_durable:
                post_recover_ftd_msgs = pre_recover_ftd_msgs
            else:
                del msgs[:] # Transient messages will be discarded on recover
            old_broker = broker # keep for log analysis
            broker = self.broker(self._broker_args(), name=bname, expect=EXPECT_EXIT_OK, log_level="debug+")
            # Browse messages
            if browse: 
                for index in range(0, len(queue_map)):
                    empty = not self._is_queue_durable(queue_map, index)
                    self.check_messages(broker, queue_map.keys()[index], msgs, browse=True, emtpy_flag=empty)
        
        # Consume messages
        for index in range(0, len(queue_map)):
            empty_chk = txn_produce or queue_map.keys()[index] in target_queues
            empty = recover and not self._is_queue_durable(queue_map, index)
            self.check_messages(broker, queue_map.keys()[index], msgs, transactional=txn_consume, empty=empty_chk,
                                emtpy_flag=empty)

        broker.terminate()

        # Check broker logs for released messages
        if recover:
            if txn_produce:
                if msg_durable:
                    self.check_msg_release_on_commit(old_broker, pre_recover_ftd_msgs)
                else:
                    self.check_msg_block_on_commit(old_broker, pre_recover_ftd_msgs)
            else:
                if msg_durable:
                    self.check_msg_release(old_broker, pre_recover_ftd_msgs)
                else:
                    self.check_msg_block(old_broker, pre_recover_ftd_msgs)
            self.check_msg_release_on_recover(broker, post_recover_ftd_msgs)      
        else:
            if txn_produce:
                if msg_durable:
                    self.check_msg_release_on_commit(broker, pre_recover_ftd_msgs)
                else:
                    self.check_msg_block_on_commit(broker, pre_recover_ftd_msgs)
            else:
                if msg_durable:
                    self.check_msg_release(broker, pre_recover_ftd_msgs)
                else:
                    self.check_msg_block(broker, pre_recover_ftd_msgs)

    # --- Parameterized test methods ---
    
    def no_limit(self, num, queue_durable=False, msg_durable=False, browse=False, recover=False, txn_produce=False,
                 txn_consume=False):
        """No policy test"""
        queue_map_1 = {"a%02d" % num : {"durable":queue_durable, "max_count":None, "max_size": None},
                       "b%02d" % num : {"durable":queue_durable, "max_count":None, "max_size": None} }
        self.tx_multi_queue_limit("MultiQueue_NoLimit", queue_map_1, exchange_name="Fanout_a%02d" % num,
                                  msg_durable=msg_durable, browse=browse, recover=recover, txn_produce=txn_produce,
                                  txn_consume=txn_consume)

    def max_count(self, num, queue_durable=False, msg_durable=False, browse=False, recover=False, txn_produce=False,
                  txn_consume=False):
        """Count policy test"""
        queue_map_2 = {"c%02d" % num : {"durable":queue_durable, "max_count":None, "max_size": None}, 
                       "d%02d" % num : {"durable":queue_durable, "max_count":10,   "max_size": None} }
        fail_list = None
        if not queue_durable:
            fail_list = [{"queue":"d%02d" % num, "type":"reject", "count":10}]
        self.tx_multi_queue_limit("MultiQueue_MaxCount", queue_map_2, exchange_name="Fanout_b%02d" % num,
                                  msg_durable=msg_durable, browse=browse, recover=recover, fail_list=fail_list,
                                  txn_produce=txn_produce, txn_consume=txn_consume)

    def max_size(self, num, queue_durable=False, msg_durable=False, browse=False, recover=False, txn_produce=False,
                 txn_consume=False):
        """Size policy test"""
        queue_map_3 = {"e%02d" % num : {"durable":queue_durable, "max_count":None, "max_size": None}, 
                       "f%02d" % num : {"durable":queue_durable, "max_count":None, "max_size": 1000} }
        fail_list = None
        if not queue_durable:
            fail_list = [{"queue":"f%02d" % num, "type":"reject", "size":1000}]
        self.tx_multi_queue_limit("MultiQueue_MaxSize", queue_map_3, exchange_name="Fanout_c%02d" % num, msg_size=100,
                                  msg_durable=msg_durable, browse=browse, recover=recover, fail_list=fail_list,
                                  txn_produce=txn_produce, txn_consume=txn_consume)
    
    def dual_max_count(self, num, queue_durable=False, msg_durable=False, browse=False, recover=False,
                       txn_produce=False, txn_consume=False):
        """Multiple count policy test"""
        queue_map_4 = {"g%02d" % num : {"durable":queue_durable, "max_count":10,   "max_size": None}, 
                       "h%02d" % num : {"durable":queue_durable, "max_count":8,    "max_size": None} }
        fail_list = None
        if not queue_durable:
            fail_list = [{"queue":"h%02d" % num, "type":"reject", "count":8}]
        self.tx_multi_queue_limit("MultiQueue_DualMaxCount", queue_map_4, exchange_name="Fanout_d%02d" % num,
                                  msg_durable=msg_durable, browse=browse, recover=recover, fail_list=fail_list,
                                  txn_produce=txn_produce, txn_consume=txn_consume)
    
    def dual_max_size(self, num, queue_durable=False, msg_durable=False, browse=False, recover=False, txn_produce=False,
                      txn_consume=False):
        """Multiple size policy test"""
        queue_map_5 = {"i%02d" % num : {"durable":queue_durable, "max_count":None, "max_size": 1000}, 
                       "j%02d" % num : {"durable":queue_durable, "max_count":None, "max_size":  800} }
        fail_list = None
        if not queue_durable:
            fail_list = [{"queue":"j%02d" % num, "type":"reject", "size":800}]
        self.tx_multi_queue_limit("MultiQueue_DualMaxSize", queue_map_5, exchange_name="Fanout_e%02d" % num,
                                  msg_size=100, msg_durable=msg_durable, browse=browse, recover=recover,
                                  fail_list=fail_list, txn_produce=txn_produce, txn_consume=txn_consume)
    
    def mixed_limit_1(self, num, queue_durable=False, msg_durable=False, browse=False, recover=False, txn_produce=False,
                      txn_consume=False):
        """Both count and size polices active with the same queue having equal probabilities of triggering the
        policy"""
        queue_map_6 = {"k%02d" % num : {"durable":queue_durable, "max_count":None, "max_size": None}, 
                       "l%02d" % num : {"durable":queue_durable, "max_count":10,   "max_size": None},
                       "m%02d" % num : {"durable":queue_durable, "max_count":None, "max_size": 1000},
                       "n%02d" % num : {"durable":queue_durable, "max_count":8,    "max_size":  800} }
        fail_list = None
        if not queue_durable:
            fail_list = [{"queue":"n%02d" % num, "type":"reject", "count":8, "size":800}]
        self.tx_multi_queue_limit("MultiQueue_MixedLimit", queue_map_6, exchange_name="Fanout_f%02d" % num,
                                  msg_size=100, msg_durable=msg_durable, browse=browse, recover=recover,
                                  fail_list=fail_list, txn_produce=txn_produce, txn_consume=txn_consume)
    
    def mixed_limit_2(self, num, queue_durable=False, msg_durable=False, browse=False, recover=False, txn_produce=False,
                      txn_consume=False):
        """Both count and size polices active with different queues having equal probabilities of triggering the
        policy"""
        queue_map_7 = {"o%02d" % num : {"durable":queue_durable, "max_count":None, "max_size": None}, 
                       "p%02d" % num : {"durable":queue_durable, "max_count":10,   "max_size": None},
                       "q%02d" % num : {"durable":queue_durable, "max_count":None, "max_size": 800},
                       "r%02d" % num : {"durable":queue_durable, "max_count":8,    "max_size": 1000} }
        fail_list = None
        if not queue_durable:
            fail_list = [{"queue":"q%02d" % num, "type":"reject", "size":800},
                         {"queue":"r%02d" % num, "type":"reject", "count":8,}]
        self.tx_multi_queue_limit("MultiQueue_MixedLimit", queue_map_7, exchange_name="Fanout_g%02d" % num,
                                  msg_size=100, msg_durable=msg_durable, browse=browse, recover=recover,
                                  fail_list=fail_list, txn_produce=txn_produce, txn_consume=txn_consume)

    # --- Non-parameterized test methods - these will be run by Python test framework ---
    
    _num = None
    _queue_durable = False
    _msg_durable = False
    _browse = False
    _recover = False
    _txn_produce = False
    _txn_consume = False
    
    def test_no_limit(self):
        """No policy test (non-parameterized)"""
        self.no_limit(self._num, queue_durable=self._queue_durable, msg_durable=self._msg_durable, browse=self._browse,
                      recover=self._recover, txn_produce=self._txn_produce, txn_consume=self._txn_consume)
        
    def test_max_count(self):
        """Count policy test (non-parameterized)"""
        self.max_count(self._num, queue_durable=self._queue_durable, msg_durable=self._msg_durable, browse=self._browse,
                       recover=self._recover, txn_produce=self._txn_produce, txn_consume=self._txn_consume)
        
    def test_max_size(self):
        """Size policy test (non-parameterized)"""
        self.max_size(self._num, queue_durable=self._queue_durable, msg_durable=self._msg_durable, browse=self._browse,
                      recover=self._recover, txn_produce=self._txn_produce, txn_consume=self._txn_consume)
        
    def test_dual_max_count(self):
        """Multiple count policy test (non-parameterized)"""
        self.dual_max_count(self._num, queue_durable=self._queue_durable, msg_durable=self._msg_durable,
                            browse=self._browse, recover=self._recover, txn_produce=self._txn_produce,
                            txn_consume=self._txn_consume)
        
    def test_dual_max_size(self):
        """Multiple size policy test (non-parameterized)"""
        self.dual_max_size(self._num, queue_durable=self._queue_durable, msg_durable=self._msg_durable,
                           browse=self._browse, recover=self._recover, txn_produce=self._txn_produce,
                           txn_consume=self._txn_consume)
        
    def test_mixed_limit_1(self):
        """Both count and size polices active with the same queue having equal probabilities of triggering the
        policy (non-parameterized)"""
        self.mixed_limit_1(self._num, queue_durable=self._queue_durable, msg_durable=self._msg_durable,
                           browse=self._browse, recover=self._recover, txn_produce=self._txn_produce,
                           txn_consume=self._txn_consume)
        
    def test_mixed_limit_2(self):
        """Both count and size polices active with different queues having equal probabilities of triggering the
        policy (non-parameterized)"""
        self.mixed_limit_2(self._num, queue_durable=self._queue_durable, msg_durable=self._msg_durable,
                           browse=self._browse, recover=self._recover, txn_produce=self._txn_produce,
                           txn_consume=self._txn_consume)
    
# --- Tests ---
        
class MultiQueueTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues"""
    _num = 1
 
class MultiDurableQueueTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues"""
    _num = 2
    _queue_durable = True
 
class MultiQueueDurableMsgTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues"""
    _num = 3
    _msg_durable = True
 
class MultiDurableQueueDurableMsgTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues"""
    _num = 4
    _queue_durable = True
    _msg_durable = True
        
class MultiQueueBrowseTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues with messages browsed before being consumed"""
    _num = 5
    _browse = True
 
class MultiDurableQueueBrowseTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues with messages browsed before being consumed"""
    _num = 6
    _queue_durable = True
    _browse = True
 
class MultiQueueDurableMsgBrowseTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues with messages browsed before being consumed"""
    _num = 7
    _msg_durable = True
    _browse = True

class MultiDurableQueueDurableMsgBrowseTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues with messages browsed before being consumed"""
    _num = 8
    _queue_durable = True
    _msg_durable = True
    _browse = True
        
class MultiQueueRecoverTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues and broker terminated/recovered"""
    _num = 9
    _recover = True
 
class MultiDurableQueueRecoverTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues and broker terminated/recovered"""
    _num = 10
    _queue_durable = True
    _recover = True
 
class MultiQueueDurableMsgRecoverTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues and broker terminated/recovered"""
    _num = 11
    _msg_durable = True
    _recover = True
 
class MultiDurableQueueDurableMsgRecoverTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues and broker terminated/recovered"""
    _num = 12
    _queue_durable = True
    _msg_durable = True
    _recover = True
        
class MultiQueueBrowseRecoverTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues with messages browsed before broker terminated/recovered and
    are consumed"""
    _num = 13
    _browse = True
    _recover = True
 
class MultiDurableQueueBrowseRecoverTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues with messages browsed before broker terminated/recovered and
    are consumed"""
    _num = 14
    _queue_durable = True
    _browse = True
    _recover = True
 
class MultiQueueDurableMsgBrowseRecoverTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues with messages browsed before broker terminated/recovered and
    are consumed"""
    _num = 15
    _msg_durable = True
    _browse = True
    _recover = True

class MultiDurableQueueDurableMsgBrowseRecoverTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues with messages browsed before broker terminated/recovered and are
    consumed"""
    _num = 16
    _queue_durable = True
    _msg_durable = True
    _browse = True
    _recover = True
         
class MultiQueueTxPTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues under transactional produce"""
    _num = 17
    _txn_produce = True
 
class MultiDurableQueueTxPTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues under transactional produce"""
    _num = 18
    _queue_durable = True
    _txn_produce = True
 
class MultiQueueDurableMsgTxPTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues under transactional produce"""
    _num = 19
    _msg_durable = True
    _txn_produce = True
 
class MultiDurableQueueDurableMsgTxPTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues under transactional produce"""
    _num = 20
    _queue_durable = True
    _msg_durable = True
    _txn_produce = True
        
class MultiQueueBrowseTxPTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues under transactional produce with messages browsed before
    being consumed"""
    _num = 21
    _browse = True
    _txn_produce = True
 
class MultiDurableQueueBrowseTxPTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues under transactional produce with messages browsed before
    being consumed"""
    _num = 22
    _queue_durable = True
    _browse = True
    _txn_produce = True
 
class MultiQueueDurableMsgBrowseTxPTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues under transactional produce with messages browsed before
    being consumed"""
    _num = 23
    _msg_durable = True
    _browse = True
    _txn_produce = True

class MultiDurableQueueDurableMsgBrowseTxPTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues under transactional produce with messages browsed before being
    consumed"""
    _num = 24
    _queue_durable = True
    _msg_durable = True
    _browse = True
    _txn_produce = True
        
class MultiQueueRecoverTxPTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues under transactional produce and broker
    terminated/recovered"""
    _num = 25
    _recover = True
    _txn_produce = True
 
class MultiDurableQueueRecoverTxPTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues under transactional produce and broker terminated/recovered"""
    _num = 26
    _queue_durable = True
    _recover = True
    _txn_produce = True
 
class MultiQueueDurableMsgRecoverTxPTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues under transactional produce and broker terminated/recovered"""
    _num = 27
    _msg_durable = True
    _recover = True
    _txn_produce = True
 
class MultiDurableQueueDurableMsgRecoverTxPTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues under transactional produce and broker terminated/recovered"""
    _num = 28
    _queue_durable = True
    _msg_durable = True
    _recover = True
    _txn_produce = True
        
class MultiQueueBrowseRecoverTxPTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues under transactional produce with messages browsed before
    broker terminated/recovered and are consumed"""
    _num = 29
    _browse = True
    _recover = True
    _txn_produce = True
 
class MultiDurableQueueBrowseRecoverTxPTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues under transactional produce with messages browsed before
    broker terminated/recovered and are consumed"""
    _num = 30
    _queue_durable = True
    _browse = True
    _recover = True
    _txn_produce = True
 
class MultiQueueDurableMsgBrowseRecoverTxPTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues under transactional produce with messages browsed before
    broker terminated/recovered and are consumed"""
    _num = 31
    _msg_durable = True
    _browse = True
    _recover = True
    _txn_produce = True

class MultiDurableQueueDurableMsgBrowseRecoverTxPTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues under transactional produce with messages browsed before broker
    terminated/recovered and are consumed"""
    _num = 32
    _queue_durable = True
    _msg_durable = True
    _browse = True
    _recover = True
    _txn_produce = True
        
class MultiQueueTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues and consumed transactionally"""
    _num = 33
    _txn_consume = True
 
class MultiDurableQueueTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues and consumed transactionally"""
    _num = 34
    _queue_durable = True
    _txn_consume = True
 
class MultiQueueDurableMsgTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues and consumed transactionally"""
    _num = 35
    _msg_durable = True
    _txn_consume = True
 
class MultiDurableQueueDurableMsgTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues and consumed transactionally"""
    _num = 36
    _queue_durable = True
    _msg_durable = True
    _txn_consume = True
        
class MultiQueueBrowseTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues with messages browsed before being consumed
    transactionally"""
    _num = 37
    _browse = True
    _txn_consume = True
 
class MultiDurableQueueBrowseTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues with messages browsed before being consumed transactionally"""
    _num = 38
    _queue_durable = True
    _browse = True
    _txn_consume = True
 
class MultiQueueDurableMsgBrowseTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues with messages browsed before being consumed transactionally"""
    _num = 39
    _msg_durable = True
    _browse = True
    _txn_consume = True

class MultiDurableQueueDurableMsgBrowseTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues with messages browsed before being consumed transactionally"""
    _num = 40
    _queue_durable = True
    _msg_durable = True
    _browse = True
    _txn_consume = True
        
class MultiQueueRecoverTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues and broker terminated/recovered before being consumed
    transactionally"""
    _num = 41
    _recover = True
    _txn_consume = True
 
class MultiDurableQueueRecoverTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues and broker terminated/recovered before being consumed
    transactionally"""
    _num = 42
    _queue_durable = True
    _recover = True
    _txn_consume = True
 
class MultiQueueDurableMsgRecoverTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues and broker terminated/recovered before being consumed
    transactionally"""
    _num = 43
    _msg_durable = True
    _recover = True
    _txn_consume = True
 
class MultiDurableQueueDurableMsgRecoverTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues and broker terminated/recovered before being consumed
    transactionally"""
    _num = 44
    _queue_durable = True
    _msg_durable = True
    _recover = True
    _txn_consume = True
        
class MultiQueueBrowseRecoverTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues with messages browsed before broker terminated/recovered and
    are consumed transactionally"""
    _num = 45
    _browse = True
    _recover = True
    _txn_consume = True
 
class MultiDurableQueueBrowseRecoverTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues with messages browsed before  broker terminated/recovered and
    are consumed transactionally"""
    _num = 46
    _queue_durable = True
    _browse = True
    _recover = True
    _txn_consume = True
 
class MultiQueueDurableMsgBrowseRecoverTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues with messages browsed before broker terminated/recovered and
    are consumed transactionally"""
    _num = 47
    _msg_durable = True
    _browse = True
    _recover = True
    _txn_consume = True

class MultiDurableQueueDurableMsgBrowseRecoverTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues with messages browsed before broker terminated/recovered and
    are consumed transactionally"""
    _num = 48
    _queue_durable = True
    _msg_durable = True
    _browse = True
    _recover = True
    _txn_consume = True
         
class MultiQueueTxPTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues under transactional produce and are consumed
    transactionally"""
    _num = 49
    _txn_produce = True
    _txn_consume = True
 
class MultiDurableQueueTxPTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues under transactional produce and are consumed
    transactionally"""
    _num = 50
    _queue_durable = True
    _txn_produce = True
    _txn_consume = True
 
class MultiQueueDurableMsgTxPTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues under transactional produce and are consumed
    transactionally"""
    _num = 51
    _msg_durable = True
    _txn_produce = True
    _txn_consume = True
 
class MultiDurableQueueDurableMsgTxPTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues under transactional produce and are consumed
    transactionally"""
    _num = 52
    _queue_durable = True
    _msg_durable = True
    _txn_produce = True
    _txn_consume = True
        
class MultiQueueBrowseTxPTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues under transactional produce with messages browsed before
    being consumed transactionally"""
    _num = 53
    _browse = True
    _txn_produce = True
    _txn_consume = True
 
class MultiDurableQueueBrowseTxPTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues under transactional produce with messages browsed before
    being consumed transactionally"""
    _num = 54
    _queue_durable = True
    _browse = True
    _txn_produce = True
    _txn_consume = True
 
class MultiQueueDurableMsgBrowseTxPTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues under transactional produce with messages browsed before
    being consumed transactionally"""
    _num = 55
    _msg_durable = True
    _browse = True
    _txn_produce = True
    _txn_consume = True

class MultiDurableQueueDurableMsgBrowseTxPTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues under transactional produce with messages browsed before being
    consumed transactionally"""
    _num = 56
    _queue_durable = True
    _msg_durable = True
    _browse = True
    _txn_produce = True
    _txn_consume = True
        
class MultiQueueRecoverTxPTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues under transactional produce and broker
    terminated/recovered before they are consumed transactionally"""
    _num = 57
    _recover = True
    _txn_produce = True
    _txn_consume = True
 
class MultiDurableQueueRecoverTxPTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues under transactional produce and broker terminated/recovered
    before they are consumed transactionally"""
    _num = 58
    _queue_durable = True
    _recover = True
    _txn_produce = True
    _txn_consume = True
 
class MultiQueueDurableMsgRecoverTxPTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues under transactional produce and broker terminated/recovered
    before they are consumed transactionally"""
    _num = 59
    _msg_durable = True
    _recover = True
    _txn_produce = True
    _txn_consume = True
 
class MultiDurableQueueDurableMsgRecoverTxPTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues under transactional produce and broker terminated/recovered
    before they are consumed transactionally"""
    _num = 60
    _queue_durable = True
    _msg_durable = True
    _recover = True
    _txn_produce = True
    _txn_consume = True
        
class MultiQueueBrowseRecoverTxPTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple transient queues under transactional produce with messages browsed before
    broker terminated/recovered and are consumed transactionally"""
    _num = 61
    _browse = True
    _recover = True
    _txn_produce = True
 
class MultiDurableQueueBrowseRecoverTxPTxCTest(MultiQueueFlowToDisk):
    """Transient messages sent to multiple durable queues under transactional produce with messages browsed before
    broker terminated/recovered and are consumed transactionally"""
    _num = 62
    _queue_durable = True
    _browse = True
    _recover = True
    _txn_produce = True
    _txn_consume = True
 
class MultiQueueDurableMsgBrowseRecoverTxPTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple transient queues under transactional produce with messages browsed before
    broker terminated/recovered and are consumed transactionally"""
    _num = 63
    _msg_durable = True
    _browse = True
    _recover = True
    _txn_produce = True
    _txn_consume = True

class MultiDurableQueueDurableMsgBrowseRecoverTxPTxCTest(MultiQueueFlowToDisk):
    """Durable messages sent to multiple durable queues under transactional produce with messages browsed before broker
    terminated/recovered and are consumed transactionally"""
    _num = 64
    _queue_durable = True
    _msg_durable = True
    _browse = True
    _recover = True
    _txn_produce = True
    _txn_consume = True
  
    # --- Long and randomized tests ---
        
#    def test_12_Randomized(self):
#        """Randomized flow-to-disk tests"""
#        seed = long(1000.0 * time.time())
#        print "seed=0x%x" % seed
#        random.seed(seed)
#        for index in range(0, 10):
#            self.randomLimit(index)
