/*
 * //****************************************************************** // //
 * Copyright 2016 Samsung Electronics All Rights Reserved. //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= // //
 * Licensed under the Apache License, Version 2.0 (the "License"); // you may
 * not use this file except in compliance with the License. // You may obtain a
 * copy of the License at // // http://www.apache.org/licenses/LICENSE-2.0 // //
 * Unless required by applicable law or agreed to in writing, software //
 * distributed under the License is distributed on an "AS IS" BASIS, // WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. // See the
 * License for the specific language governing permissions and // limitations
 * under the License. //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
// package org.iotivity.cloud.base;
//
// import java.net.InetSocketAddress;
// import java.security.cert.CertificateException;
// import java.util.ArrayList;
// import java.util.List;
//
// import javax.net.ssl.SSLException;
//
// import io.netty.bootstrap.ServerBootstrap;
// import io.netty.channel.ChannelFuture;
// import io.netty.channel.ChannelHandler;
// import io.netty.channel.ChannelInitializer;
// import io.netty.channel.ChannelPipeline;
// import io.netty.channel.EventLoopGroup;
// import io.netty.channel.nio.NioEventLoopGroup;
// import io.netty.channel.socket.SocketChannel;
// import io.netty.channel.socket.nio.NioServerSocketChannel;
// import io.netty.handler.codec.http.HttpRequestDecoder;
// import io.netty.handler.codec.http.HttpResponseEncoder;
// import io.netty.handler.logging.LogLevel;
// import io.netty.handler.logging.LoggingHandler;
// import io.netty.util.concurrent.GenericFutureListener;
//
// public class HttpServer {
//
// private static class HttpServerInitializer
// extends ChannelInitializer<SocketChannel> {
//
// private List<ChannelHandler> additionalHandlers = new
// ArrayList<ChannelHandler>();
//
// public HttpServerInitializer() {
// }
//
// public void addHandler(ChannelHandler handler) {
// additionalHandlers.add(handler);
// }
//
// @Override
// public void initChannel(SocketChannel ch) {
// ChannelPipeline p = ch.pipeline();
// /*
// * if (sslCtx != null) { p.addLast(sslCtx.newHandler(ch.alloc())); }
// */
// p.addLast(new HttpRequestDecoder());
// // Uncomment the following line if you don't want to handle
// // HttpChunks.
// // p.addLast(new HttpObjectAggregator(1048576));
// p.addLast(new HttpResponseEncoder());
// // Remove the following line if you don't want automatic content
// // compression.
// // p.addLast(new HttpContentCompressor());
// for (ChannelHandler handler : additionalHandlers) {
// p.addLast(handler);
// }
// }
//
// }
//
// EventLoopGroup bossGroup = new NioEventLoopGroup(1);
//
// EventLoopGroup workerGroup = new NioEventLoopGroup();
//
// HttpServerInitializer initializer = new HttpServerInitializer();
//
// public void addHandler(ChannelHandler handler) {
// initializer.addHandler(handler);
// }
//
// public void startServer(InetSocketAddress inetSocketAddress)
// throws CertificateException, SSLException, InterruptedException {
//
// try {
// ServerBootstrap b = new ServerBootstrap();
// b.group(bossGroup, workerGroup);
// b.channel(NioServerSocketChannel.class);
// b.handler(new LoggingHandler(LogLevel.INFO));
//
// b.childHandler(initializer);
//
// ChannelFuture ch = b.bind(inetSocketAddress).sync();
// ch.addListener(new GenericFutureListener<ChannelFuture>() {
//
// @Override
// public void operationComplete(ChannelFuture future)
// throws Exception {
// // TODO Auto-generated method stub
// System.out
// .println("Connection status of TCP Http SERVER : "
// + future.isSuccess());
// }
//
// });
// } finally {
// }
//
// }
//
// public void stopServer() {
// // shut down all event loops
// if (bossGroup != null) {
// bossGroup.shutdownGracefully();
//
// try {
// bossGroup.terminationFuture().sync();
// } catch (InterruptedException e) {
// // TODO Auto-generated catch block
// e.printStackTrace();
// }
// }
//
// if (workerGroup != null) {
// workerGroup.shutdownGracefully();
//
// try {
// workerGroup.terminationFuture().sync();
// } catch (InterruptedException e) {
// // TODO Auto-generated catch block
// e.printStackTrace();
// }
// }
// }
//
// }
