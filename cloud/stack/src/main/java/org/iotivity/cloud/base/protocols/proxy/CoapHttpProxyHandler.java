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
// package org.iotivity.cloud.base.protocols.proxy;
//
// import java.nio.charset.StandardCharsets;
// import java.util.HashMap;
// import java.util.List;
// import java.util.Map.Entry;
//
// import org.iotivity.cloud.base.SessionManager;
// import org.iotivity.cloud.base.protocols.coap.CoapRequest;
// import org.iotivity.cloud.base.protocols.coap.CoapResponse;
// import org.iotivity.cloud.base.protocols.coap.enums.CoapMethod;
// import org.iotivity.cloud.util.Cbor;
// import org.iotivity.cloud.util.Logger;
//
// import io.netty.buffer.Unpooled;
// import io.netty.channel.ChannelDuplexHandler;
// import io.netty.channel.ChannelFutureListener;
// import io.netty.channel.ChannelHandler.Sharable;
// import io.netty.channel.ChannelHandlerContext;
// import io.netty.handler.codec.http.DefaultFullHttpResponse;
// import io.netty.handler.codec.http.HttpHeaders;
// import io.netty.handler.codec.http.HttpMethod;
// import io.netty.handler.codec.http.HttpRequest;
// import io.netty.handler.codec.http.HttpResponse;
// import io.netty.handler.codec.http.HttpResponseStatus;
// import io.netty.handler.codec.http.HttpVersion;
// import io.netty.handler.codec.http.QueryStringDecoder;
// import io.netty.util.AttributeKey;
// import io.netty.util.CharsetUtil;
//
// @Sharable
// public class CoapHttpProxyHandler extends ChannelDuplexHandler {
//
// // Proxy converts http request to coaprequest and coapresponse to
// // httpresponse
// private SessionManager sessionManager = null;
//
// private static final AttributeKey<ChannelHandlerContext> keyHttpCtx =
// AttributeKey
// .newInstance("httpCtx");
//
// public CoapHttpProxyHandler(SessionManager sessionManager) {
// this.sessionManager = sessionManager;
// }
//
// @Override
// public void channelRead(ChannelHandlerContext ctx, Object msg)
// throws Exception {
//
// // in case of Receive Request from http
// if (msg instanceof HttpRequest) {
// // Check uri query param that contains coap device uuid
// // then search those and create coapRequest and send
// HttpRequest httpRequest = (HttpRequest) msg;
// QueryStringDecoder queryStringDecoder = new QueryStringDecoder(
// httpRequest.getUri());
//
// List<String> didList = queryStringDecoder.parameters().get("di");
//
// if (didList != null) {
// ChannelHandlerContext coapClient = sessionManager
// .querySession(didList.get(0));
//
// if (coapClient != null) {
// List<String> uriList = queryStringDecoder.parameters()
// .get("href");
// if (uriList != null) {
// coapClient.channel().attr(keyHttpCtx).set(ctx);
// coapClient.writeAndFlush(httpRequestToCoAPRequest(
// uriList.get(0), (HttpRequest) msg));
//
// return;
// }
// } else {
// Logger.d("Unable to find session: " + didList.get(0));
// }
// }
//
// // Prints available sessions to html
//
// ctx.writeAndFlush(printsAvailableSessions())
// .addListener(ChannelFutureListener.CLOSE);
// return;
// }
//
// if (msg instanceof CoapResponse) {
// ctx.channel().attr(keyHttpCtx).get()
// .writeAndFlush(
// coapResponseToHttpResponse((CoapResponse) msg))
// .addListener(ChannelFutureListener.CLOSE);
// return;
// }
//
// // Pass to upper-layer
// super.channelRead(ctx, msg);
// }
//
// @Override
// public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) {
// cause.printStackTrace();
// ctx.close();
// }
//
// HttpResponse printsAvailableSessions() {
//
// StringBuilder strBuilder = new StringBuilder();
// List<String> sessions = sessionManager.getSessions();
//
// strBuilder.append("<html>");
// strBuilder.append("<b>Available sessions</b><br>");
//
// for (String session : sessions) {
// strBuilder.append(session);
// strBuilder.append("<br>");
// }
//
// strBuilder.append("</html>");
//
// HttpResponse response = new DefaultFullHttpResponse(
// HttpVersion.HTTP_1_1, HttpResponseStatus.OK,
// Unpooled.copiedBuffer(strBuilder.toString(),
// CharsetUtil.UTF_8));
// response.headers().set(HttpHeaders.Names.CONTENT_TYPE,
// "text/html; charset=UTF-8");
//
// return response;
// }
//
// HttpResponse httpRequestToSendError() {
// HttpResponse response = new DefaultFullHttpResponse(
// HttpVersion.HTTP_1_1, HttpResponseStatus.NOT_FOUND,
// Unpooled.copiedBuffer(
// "Failure: " + HttpResponseStatus.NOT_FOUND + "\r\n",
// CharsetUtil.UTF_8));
// response.headers().set(HttpHeaders.Names.CONTENT_TYPE,
// "text/html; charset=UTF-8");
//
// return response;
// }
//
// CoapRequest httpRequestToCoAPRequest(String uri, HttpRequest httpRequest) {
// CoapRequest coapRequest;
//
// // TODO: coapRequest converter required
// // coapRequest.getOptions().setUriQuery();
// if (httpRequest.getMethod() == HttpMethod.GET) {
// coapRequest = new CoapRequest(CoapMethod.GET);
// } else if (httpRequest.getMethod() == HttpMethod.PUT) {
// coapRequest = new CoapRequest(CoapMethod.PUT);
// } else if (httpRequest.getMethod() == HttpMethod.POST) {
// coapRequest = new CoapRequest(CoapMethod.POST);
// } else if (httpRequest.getMethod() == HttpMethod.DELETE) {
// coapRequest = new CoapRequest(CoapMethod.DELETE);
// } else {
// throw new IllegalArgumentException();
// }
//
// coapRequest.setUriPath(uri);
//
// return coapRequest;
// }
//
// HttpResponse coapResponseToHttpResponse(CoapResponse coapResponse) {
//
// Cbor<HashMap<String, Object>> cbor = new Cbor<HashMap<String, Object>>();
//
// HashMap<String, Object> rep = cbor
// .parsePayloadFromCbor(coapResponse.getPayload(), HashMap.class);
//
// StringBuilder strBuilder = new StringBuilder();
//
// for (Entry<String, Object> entry : rep.entrySet()) {
// String key = entry.getKey();
// String value = entry.getValue().toString();
// strBuilder.append("Key: " + key + " Value: " + value + "<br>");
// }
//
// HttpResponse httpResponse = new DefaultFullHttpResponse(
// HttpVersion.HTTP_1_1, HttpResponseStatus.OK,
// Unpooled.wrappedBuffer(strBuilder.toString()
// .getBytes(StandardCharsets.UTF_8)));
//
// httpResponse.headers().set(HttpHeaders.Names.CONTENT_TYPE,
// "text/html; charset=UTF-8");
//
// // TODO: httpResponse converter required
//
// return httpResponse;
// }
// }
