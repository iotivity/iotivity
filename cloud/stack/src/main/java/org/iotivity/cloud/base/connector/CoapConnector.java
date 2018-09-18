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
package org.iotivity.cloud.base.connector;

import io.netty.bootstrap.Bootstrap;
import io.netty.channel.*;
import io.netty.channel.ChannelHandler.Sharable;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;
import io.netty.handler.ssl.SslContext;
import io.netty.handler.ssl.SslContextBuilder;
import io.netty.handler.ssl.SslProvider;
import io.netty.handler.timeout.IdleState;
import io.netty.handler.timeout.IdleStateEvent;
import io.netty.handler.timeout.IdleStateHandler;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.iotivity.cloud.base.OICConstants;
import org.iotivity.cloud.base.protocols.coap.*;
import org.iotivity.cloud.base.protocols.coap.PingMessage;

import javax.net.ssl.SSLException;
import java.io.File;
import java.net.InetSocketAddress;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

public class CoapConnector {
    private final static Logger Log             = LoggerFactory.getLogger(CoapConnector.class);
    public CoapConnector() {

        mBootstrap.group(mConnectorGroup);
        mBootstrap.channel(NioSocketChannel.class);
        mBootstrap.option(ChannelOption.TCP_NODELAY, true);
        mBootstrap.option(ChannelOption.SO_KEEPALIVE, true);
        mBootstrap.option(ChannelOption.SO_REUSEADDR, true);
    }

    @Sharable
    private class CoapPacketHandler
            extends SimpleChannelInboundHandler<CoapResponse> {

        @Override
        protected void channelRead0(ChannelHandlerContext ctx, CoapResponse msg)
                throws Exception {
            mChannelMap.get(ctx.channel()).onResponseReceived(msg);
        }
    }

    public static class KeepAliveHandler extends ChannelDuplexHandler {
        @Override
        public void userEventTriggered(ChannelHandlerContext ctx, Object evt)
                throws Exception {
            if (evt instanceof IdleStateEvent) {
                IdleStateEvent event = (IdleStateEvent) evt;
                if (event.state() == IdleState.WRITER_IDLE) {
                    mChannelMap.get(ctx.channel()).sendRequest(PingMessage.build(),null);
                }
                if (event.state() == IdleState.READER_IDLE) {
                    Log.debug("Connection with" +  ctx.channel().remoteAddress().toString() + "is idle. Closing connection.");
                    ctx.close();
                }
            }
        }
    }

    public static class CoapConnectorInitializer
            extends ChannelInitializer<SocketChannel> {

        private List<ChannelHandler> additionalHandlers = new ArrayList<>();

        private Boolean              mTlsMode           = false;
        private Boolean              mKeepAlive         = false;
        InetSocketAddress            mInetSocketAddress = null;
        String                       mRootCertFiePath   = null;

        public void setTlsMode(Boolean tlsMode) {
            this.mTlsMode = tlsMode;
        }

        public void setKeepAlive(Boolean keepAlive) {
            this.mKeepAlive = keepAlive;
        }

        public void setInetSocketAddress(InetSocketAddress address) {
            this.mInetSocketAddress = address;
        }

        public void setRootCertFilePath(String path) {
            this.mRootCertFiePath = path;
        }

        public void addHandler(ChannelHandler handler) {
            additionalHandlers.add(handler);
        }

        @Override
        public void initChannel(SocketChannel ch) {
            ChannelPipeline p = ch.pipeline();

            SslContext sslContext = null;

            if (mTlsMode.equals(true)) {

                File rootCert = new File(mRootCertFiePath);

                try {
                    sslContext = SslContextBuilder.forClient()
                            .sslProvider(SslProvider.JDK).trustManager(rootCert)
                            .build();
                } catch (SSLException e) {
                    e.printStackTrace();
                }

                final SslContext sslCtx = sslContext;
                p.addLast(sslCtx.newHandler(ch.alloc(),
                        mInetSocketAddress.getHostString(),
                        mInetSocketAddress.getPort()));
            }

            p.addLast(new CoapDecoder());
            p.addLast(new CoapEncoder());
            p.addLast(new CoapLogHandler());

            if (mKeepAlive.equals(true)) {
                p.addLast(new IdleStateHandler(100, 45, 0));
                p.addLast(new KeepAliveHandler());
            }

            for (ChannelHandler handler : additionalHandlers) {
                p.addLast(handler);
            }
        }
    }

    private static ConcurrentMap<Channel, CoapClient> mChannelMap     = new ConcurrentHashMap<>();
    Bootstrap                    mBootstrap      = new Bootstrap();
    EventLoopGroup               mConnectorGroup = new NioEventLoopGroup();
    Timer                        mTimer          = new Timer();

    public void connect(final String connectionName, final InetSocketAddress inetSocketAddress,
            boolean tlsMode, boolean keepAlive, final ConnectionEstablishedListener listener) {

        CoapConnectorInitializer initializer = new CoapConnectorInitializer();

        if (tlsMode == true) {
            initializer.setTlsMode(true);
            initializer.setInetSocketAddress(inetSocketAddress);
            initializer.setRootCertFilePath(OICConstants.ROOT_CERT_FILE);
        }

        initializer.setKeepAlive(keepAlive);
        initializer.addHandler(new CoapPacketHandler());
        mBootstrap.handler(initializer);
        doConnect(connectionName, inetSocketAddress, tlsMode, listener);
    }

    private void doConnect(final String connectionName, final InetSocketAddress inetSocketAddress, final boolean tlsMode,
                           final ConnectionEstablishedListener listener) {
        mBootstrap.connect(inetSocketAddress).addListener(new ChannelFutureListener() {
                @Override public void operationComplete(ChannelFuture future) throws Exception {
                    if(!future.isSuccess()) {
                        Log.debug("Connection to " + inetSocketAddress.getHostString() + " was not successful. Retrying...");
                        future.channel().close();
                        scheduleConnect(connectionName, inetSocketAddress, tlsMode, 5000, listener);
                    } else {
                        connectionEstablished(connectionName, future.channel(), listener);
                        addCloseDetectListener(future.channel());
                    }
                }

            private void addCloseDetectListener(Channel channel) {
                channel.closeFuture().addListener((ChannelFutureListener) future -> {
                    ConnectorPool.removeConnection(connectionName);
                    Log.debug("Connection to " + inetSocketAddress.getHostString() + " was lost. Retrying...");
                    scheduleConnect(connectionName, inetSocketAddress, tlsMode, 5, listener);
                });
            }
        });
    }

    private void scheduleConnect(String connectionName, InetSocketAddress inetSocketAddress, boolean tlsMode, long millis,
                                 final ConnectionEstablishedListener listener) {
        mTimer.schedule( new TimerTask() {
            @Override
            public void run() {
                doConnect(connectionName, inetSocketAddress, tlsMode, listener);
            }
        }, millis );
    }

    public void connectionEstablished(String connectionName, Channel channel, final ConnectionEstablishedListener listener) {
        CoapClient coapClient = new CoapClient(channel);
        mChannelMap.put(channel, coapClient);
        ConnectorPool.addConnection(connectionName, coapClient);
        if(listener != null){
            listener.onConnectionEstablished(coapClient);
        }
    }

    public void disconenct() throws Exception {
        mConnectorGroup.shutdownGracefully().await();
    }
}
