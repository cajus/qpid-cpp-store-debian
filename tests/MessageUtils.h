/*
 Copyright (c) 2007, 2008, 2009 Red Hat, Inc.

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
 */

#include <qpid/broker/Message.h>
#include <qpid/framing/AMQFrame.h>
#include <qpid/framing/all_method_bodies.h>
#include <qpid/framing/Uuid.h>

using namespace qpid::broker;
using namespace qpid::framing;

struct MessageUtils
{
    static boost::intrusive_ptr<Message> createMessage(const string& exchange, const string& routingKey,
                                                       const Uuid& messageId=Uuid(), uint64_t contentSize = 0)
    {
        boost::intrusive_ptr<Message> msg(new Message());

        AMQFrame method((MessageTransferBody(ProtocolVersion(), exchange, 0, 0)));
        AMQFrame header((AMQHeaderBody()));
        header.setLastSegment(contentSize == 0);

        msg->getFrames().append(method);
        msg->getFrames().append(header);
        MessageProperties* props = msg->getFrames().getHeaders()->get<MessageProperties>(true);
        props->setContentLength(contentSize);
        props->setMessageId(messageId);
        msg->getFrames().getHeaders()->get<DeliveryProperties>(true)->setRoutingKey(routingKey);
        return msg;
    }

    static void addContent(boost::intrusive_ptr<Message> msg, const string& data)
    {
        AMQFrame content((AMQContentBody(data)));
        msg->getFrames().append(content);
    }

    static void deliver(QueuedMessage& msg, FrameHandler& h, uint16_t framesize)
    {
        msg.payload->sendHeader(h, framesize);
        msg.payload->sendContent(*(msg.queue), h, framesize);
    }

};
