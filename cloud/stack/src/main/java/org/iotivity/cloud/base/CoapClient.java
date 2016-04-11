/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
package org.iotivity.cloud.base;

import java.net.InetSocketAddress;
import java.util.ArrayList;
import java.util.List;

import org.iotivity.cloud.base.protocols.coap.CoapDecoder;
import org.iotivity.cloud.base.protocols.coap.CoapEncoder;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;

import io.netty.bootstrap.Bootstrap;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelOption;
import io.netty.channel.ChannelPipeline;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;
import io.netty.util.concurrent.GenericFutureListener;

public class CoapClient {

    private static class CoAPClientInitializer
            extends ChannelInitializer<SocketChannel> {

        private List<ChannelHandler> additionalHandlers = new ArrayList<ChannelHandler>();

        public CoAPClientInitializer() {
        }

        public void addHandler(ChannelHandler handler) {
            additionalHandlers.add(handler);
        }

        @Override
        public void initChannel(SocketChannel ch) {
            ChannelPipeline p = ch.pipeline();
            /*
             * if (sslCtx != null) { p.addLast(sslCtx.newHandler(ch.alloc())); }
             */
            p.addLast(new CoapDecoder());
            p.addLast(new CoapEncoder());
            for (ChannelHandler handler : additionalHandlers) {
                p.addLast(handler);
            }
        }
    }

    private ChannelFuture channelFuture;

    CoAPClientInitializer initializer = new CoAPClientInitializer();

    public void addHandler(ChannelHandler handler) {
        initializer.addHandler(handler);
    }

    public void startClient(final InetSocketAddress inetSocketAddress)
            throws InterruptedException {
        // Create bootstrap

        EventLoopGroup bossGroup = new NioEventLoopGroup();
        // bossGroup = new
        // EpollEventLoopGroup(Runtime.getRuntime().availableProcessors() * 2);

        try {
            Bootstrap b = new Bootstrap();
            b.group(bossGroup);
            b.channel(NioSocketChannel.class);
            b.option(ChannelOption.TCP_NODELAY, true);
            b.option(ChannelOption.SO_KEEPALIVE, true);
            b.option(ChannelOption.SO_REUSEADDR, true);

            b.handler(initializer);

            channelFuture = b.connect(inetSocketAddress).sync();

            channelFuture
                    .addListener(new GenericFutureListener<ChannelFuture>() {
                        @Override
                        public void operationComplete(ChannelFuture future)
                                throws Exception {
                            System.out.println(
                                    "Connection status of TCP CoAP CLIENT  : "
                                            + future.isSuccess());
                        }
                    });
        } finally {
        }
    }

    public ChannelFuture getChannelFuture() {
        return channelFuture;
    }

    public void sendRequest(CoapRequest request) {
        channelFuture.channel().writeAndFlush(request);
    }

    /**
     * stop connection
     */
    public void stopClient() {

        try {
            if (channelFuture != null) {
                channelFuture.channel().disconnect().sync().addListener(
                        new GenericFutureListener<ChannelFuture>() {

                            public void operationComplete(ChannelFuture future)
                                    throws Exception {
                                System.out.println(
                                        "DisConnection status of TCP CoAP CLIENT : "
                                                + future.isSuccess());
                            }
                        });
            }
        } catch (InterruptedException e1) {
            e1.printStackTrace();
        }
    }
}
