//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package oic.ctt.network.control;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.DatagramChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.util.concurrent.ConcurrentLinkedQueue;

import org.slf4j.Logger;

import oic.ctt.logger.CTLogger;

/**
 * UDP Socket Handler class. Facilitates message send, receive and UDP channel
 * management
 *
 * @author Tanvir Ferdousi (tanvir.f@samsung.com)
 *
 */
public class UdpSocketHandler {

    protected WorkerThread   mWorkerThread     = null;
    private DatagramChannel  mDatagramChannel  = null;
    private ControllerEntity mControllerEntity = null;
    private Logger           mlogger           = CTLogger.getInstance();
    public static final int  UDP_BUFFER_SIZE   = 66000;

    public UdpSocketHandler(int port, ControllerEntity controllerEntity) {
        if (port < 0) {
            mlogger.error("Invalid port number, using a random port");
            port = 0;
        }

        try {
            mDatagramChannel = DatagramChannel.open();
            mDatagramChannel.socket().bind(new InetSocketAddress(port));
            mDatagramChannel.configureBlocking(false);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mControllerEntity = controllerEntity;

        mWorkerThread = new WorkerThread();
        mWorkerThread.start();

    }

    public void close() {
        mWorkerThread.close();
    }

    public void sendMessage(ControlMessage controlMsg) {
        if (mWorkerThread != null) {
            mWorkerThread.addMessageToSendBUffer(controlMsg);
        }
    }

    protected class WorkerThread extends Thread {
        Selector                                     mSelector        = null;
        public ConcurrentLinkedQueue<ControlMessage> mSendBuffer      = new ConcurrentLinkedQueue<ControlMessage>();

        long                                         mStartTime;
        static final int                             POLLING_INTERVAL = 10000;
        ByteBuffer                                   mDatagramBuffer;

        public WorkerThread() {
            mDatagramBuffer = ByteBuffer.allocate(UDP_BUFFER_SIZE);
            mStartTime = System.currentTimeMillis();

            try {
                mSelector = Selector.open();
                mDatagramChannel.register(mSelector, SelectionKey.OP_READ);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        public void close() {
            try {
                mDatagramChannel.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void run() {

            long waitFor = POLLING_INTERVAL;
            InetSocketAddress address = null;

            while (mDatagramChannel != null) {

                sendBufferedMessages();

                mDatagramBuffer.clear();

                try {
                    address = (InetSocketAddress) mDatagramChannel
                            .receive(mDatagramBuffer);
                } catch (IOException e) {
                    e.printStackTrace();
                }

                if (address != null) {
                    handleIncommingMessage(mDatagramBuffer, address);
                }

                try {
                    mSelector.select(waitFor);
                } catch (IOException e) {
                    e.printStackTrace();
                }

            }
        }

        protected synchronized void addMessageToSendBUffer(
                ControlMessage controlMsg) {
            mSendBuffer.add(controlMsg);
            mSelector.wakeup();
        }

        private void sendBufferedMessages() {
            ControlMessage controlMsg = mSendBuffer.poll();
            while (controlMsg != null) {
                sendUdpMessage(controlMsg);
                controlMsg = mSendBuffer.poll();
            }
        }

        private void handleIncommingMessage(ByteBuffer buffer,
                InetSocketAddress address) {
            ControlMessage controlMsg = null;

            try {
                controlMsg = ControlMessage.parse(buffer.array(),
                        buffer.position());
            } catch (Exception e) {
                e.printStackTrace();
            }

            if (controlMsg != null)
                mControllerEntity.handleMessage(controlMsg);
        }

        private void sendUdpMessage(ControlMessage controlMsg) {
            if (controlMsg == null) {
                return;
            }

            InetAddress inetAddress = controlMsg.getRemoteAddress();
            int port = controlMsg.getRemotePort();

            ByteBuffer buffer = ByteBuffer.wrap(controlMsg.toByteData());

            try {
                mDatagramChannel.send(buffer,
                        new InetSocketAddress(inetAddress, port));
            } catch (IOException e) {
                e.printStackTrace();
            }

        }

    }

}