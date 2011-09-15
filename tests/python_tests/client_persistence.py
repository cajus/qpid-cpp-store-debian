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

from brokertest import EXPECT_EXIT_OK
from store_test import StoreTest, Qmf, store_args
from qpid.messaging import *

class ExchangeQueueTests(StoreTest):
    """
    Simple tests of the broker exchange and queue types
    """
    
    def test_direct_exchange(self):
        """Test Direct exchange."""
        broker = self.broker(store_args(), name="test_direct_exchange", expect=EXPECT_EXIT_OK)
        msg1 = Message("A_Message1", durable=True, correlation_id="Msg0001")
        msg2 = Message("B_Message1", durable=True, correlation_id="Msg0002")
        broker.send_message("a", msg1)
        broker.send_message("b", msg2)
        broker.terminate()
        
        broker = self.broker(store_args(), name="test_direct_exchange")
        self.check_message(broker, "a", msg1, True)
        self.check_message(broker, "b", msg2, True)
    
    def test_topic_exchange(self):
        """Test Topic exchange."""
        broker = self.broker(store_args(), name="test_topic_exchange", expect=EXPECT_EXIT_OK)
        ssn = broker.connect().session()
        snd1 = ssn.sender("abc/key1; {create:always, node:{type:topic, durable:True}}")
        snd2 = ssn.sender("abc/key2; {create:always, node:{type:topic, durable:True}}")
        ssn.receiver("a; {create:always, link:{x-bindings:[{exchange:abc, key:key1}]}, node:{durable:True}}")
        ssn.receiver("b; {create:always, link:{x-bindings:[{exchange:abc, key:key1}]}, node:{durable:True}}")
        ssn.receiver("c; {create:always, link:{x-bindings:[{exchange:abc, key:key1}, "
                     "{exchange:abc, key: key2}]}, node:{durable:True}}")
        ssn.receiver("d; {create:always, link:{x-bindings:[{exchange:abc, key:key2}]}, node:{durable:True}}")
        ssn.receiver("e; {create:always, link:{x-bindings:[{exchange:abc, key:key2}]}, node:{durable:True}}")
        msg1 = Message("Message1", durable=True, correlation_id="Msg0003")
        snd1.send(msg1)
        msg2 = Message("Message2", durable=True, correlation_id="Msg0004")
        snd2.send(msg2)
        broker.terminate()
        
        broker = self.broker(store_args(), name="test_topic_exchange")
        self.check_message(broker, "a", msg1, True)
        self.check_message(broker, "b", msg1, True)
        self.check_messages(broker, "c", [msg1, msg2], True)
        self.check_message(broker, "d", msg2, True)
        self.check_message(broker, "e", msg2, True)
        
    
    def test_lvq(self):
        """Test LVQ."""        
        broker = self.broker(store_args(), name="test_lvq", expect=EXPECT_EXIT_OK)
        ma1 = Message("A1", durable=True, correlation_id="Msg0005", properties={"qpid.LVQ_key":"A"})
        ma2 = Message("A2", durable=True, correlation_id="Msg0006", properties={"qpid.LVQ_key":"A"})
        mb1 = Message("B1", durable=True, correlation_id="Msg0007", properties={"qpid.LVQ_key":"B"})
        mb2 = Message("B2", durable=True, correlation_id="Msg0008", properties={"qpid.LVQ_key":"B"})
        mb3 = Message("B3", durable=True, correlation_id="Msg0009", properties={"qpid.LVQ_key":"B"})
        mc1 = Message("C1", durable=True, correlation_id="Msg0010", properties={"qpid.LVQ_key":"C"})
        broker.send_messages("lvq-test", [mb1, ma1, ma2, mb2, mb3, mc1],
                             xprops="arguments:{\"qpid.last_value_queue\":True}")
        broker.terminate()
        
        broker = self.broker(store_args(), name="test_lvq", expect=EXPECT_EXIT_OK)
        ssn = self.check_messages(broker, "lvq-test", [ma2, mb3, mc1], empty=True, ack=False)
        # Add more messages while subscriber is active (no replacement):
        ma3 = Message("A3", durable=True, correlation_id="Msg0011", properties={"qpid.LVQ_key":"A"})
        ma4 = Message("A4", durable=True, correlation_id="Msg0012", properties={"qpid.LVQ_key":"A"})
        mc2 = Message("C2", durable=True, correlation_id="Msg0013", properties={"qpid.LVQ_key":"C"})
        mc3 = Message("C3", durable=True, correlation_id="Msg0014", properties={"qpid.LVQ_key":"C"})
        mc4 = Message("C4", durable=True, correlation_id="Msg0015", properties={"qpid.LVQ_key":"C"})
        broker.send_messages("lvq-test", [mc2, mc3, ma3, ma4, mc4], session=ssn)
        ssn.acknowledge()
        broker.terminate()
        
        broker = self.broker(store_args(), name="test_lvq")
        self.check_messages(broker, "lvq-test", [mc4, ma4], True)
    
    def test_fanout_exchange(self):
        """Test Fanout Exchange"""
        broker = self.broker(store_args(), name="test_fanout_exchange", expect=EXPECT_EXIT_OK)
        ssn = broker.connect().session()
        snd = ssn.sender("TestFanoutExchange; {create: always, node: {type: topic, x-declare: {type: fanout}}}")
        ssn.receiver("TestFanoutExchange; {link: {name: \"q1\", durable: True, reliability:at-least-once}}")
        ssn.receiver("TestFanoutExchange; {link: {name: \"q2\", durable: True, reliability:at-least-once}}")
        ssn.receiver("TestFanoutExchange; {link: {name: \"q3\", durable: True, reliability:at-least-once}}")
        msg1 = Message("Msg1", durable=True, correlation_id="Msg0001")
        snd.send(msg1)
        msg2 = Message("Msg2", durable=True, correlation_id="Msg0002")
        snd.send(msg2)
        broker.terminate()
        
        broker = self.broker(store_args(), name="test_fanout_exchange")
        self.check_messages(broker, "q1", [msg1, msg2], True)
        self.check_messages(broker, "q2", [msg1, msg2], True)
        self.check_messages(broker, "q3", [msg1, msg2], True)


    def test_message_reject(self):
        broker = self.broker(store_args(), name="test_message_reject", expect=EXPECT_EXIT_OK)
        ssn = broker.connect().session()
        snd = ssn.sender("tmr; {create:always, node:{type:queue, durable:True}}")
        rcv = ssn.receiver("tmr; {create:always, node:{type:queue, durable:True}}")
        m1 = Message("test_message_reject", durable=True, correlation_id="Msg0001")
        snd.send(m1)
        m2 = rcv.fetch()
        ssn.acknowledge(message=m2, disposition=Disposition(REJECTED))
        broker.terminate()
         
        broker = self.broker(store_args(), name="test_message_reject")
        qmf = Qmf(broker)
        assert qmf.queue_message_count("tmr") == 0
        

class AlternateExchangePropertyTests(StoreTest):
    """
    Test the persistence of the Alternate Exchange property for exchanges and queues.
    """

    def test_exchange(self):
        """Exchange alternate exchange property persistence test"""
        broker = self.broker(store_args(), name="test_exchange", expect=EXPECT_EXIT_OK)
        qmf = Qmf(broker)
        qmf.add_exchange("altExch", "direct", durable=True) # Serves as alternate exchange instance
        qmf.add_exchange("testExch", "direct", durable=True, alt_exchange_name="altExch")
        qmf.close()
        broker.terminate()

        broker = self.broker(store_args(), name="test_exchange")
        qmf = Qmf(broker)
        try:
            qmf.add_exchange("altExch", "direct", passive=True)
        except Exception, error:
            self.fail("Alternate exchange (\"altExch\") instance not recovered: %s" % error)
        try:
            qmf.add_exchange("testExch", "direct", passive=True)
        except Exception, error:
            self.fail("Test exchange (\"testExch\") instance not recovered: %s" % error)
        self.assertTrue(qmf.query_exchange("testExch", alt_exchange_name = "altExch"),
                        "Alternate exchange property not found or is incorrect on exchange \"testExch\".")
        qmf.close()
        
    def test_queue(self):
        """Queue alternate exchange property persistexchangeNamece test"""
        broker = self.broker(store_args(), name="test_queue", expect=EXPECT_EXIT_OK)
        qmf = Qmf(broker)
        qmf.add_exchange("altExch", "direct", durable=True) # Serves as alternate exchange instance
        qmf.add_queue("testQueue", durable=True, alt_exchange_name="altExch")
        qmf.close()
        broker.terminate()

        broker = self.broker(store_args(), name="test_queue")
        qmf = Qmf(broker)
        try:
            qmf.add_exchange("altExch", "direct", passive=True)
        except Exception, error:
            self.fail("Alternate exchange (\"altExch\") instance not recovered: %s" % error)
        try:
            qmf.add_queue("testQueue", passive=True)
        except Exception, error:
            self.fail("Test queue (\"testQueue\") instance not recovered: %s" % error)
        self.assertTrue(qmf.query_queue("testQueue", alt_exchange_name = "altExch"),
                        "Alternate exchange property not found or is incorrect on queue \"testQueue\".")
        qmf.close()


class RedeliveredTests(StoreTest):
    """
    Test the behavior of the redelivered flag in the context of persistence
    """

    def test_broker_recovery(self):
        """Test that the redelivered flag is set on messages after recovery of broker"""
        broker = self.broker(store_args(), name="test_broker_recovery", expect=EXPECT_EXIT_OK)
        msg_content = "xyz"*100
        msg = Message(msg_content, durable=True)
        broker.send_message("testQueue", msg)
        broker.terminate()
        
        broker = self.broker(store_args(), name="test_broker_recovery")
        rcv_msg = broker.get_message("testQueue")
        self.assertEqual(msg_content, rcv_msg.content)
        self.assertTrue(rcv_msg.redelivered)

