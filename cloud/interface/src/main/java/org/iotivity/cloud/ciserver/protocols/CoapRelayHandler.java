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
package org.iotivity.cloud.ciserver.protocols;

import java.net.InetSocketAddress;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.iotivity.cloud.base.CoapClient;
import org.iotivity.cloud.base.SessionManager;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.coap.enums.CoapMethod;
import org.iotivity.cloud.base.protocols.coap.enums.CoapStatus;
import org.iotivity.cloud.ciserver.Constants;
import org.iotivity.cloud.util.Logger;
import org.iotivity.cloud.util.Net;

import io.netty.channel.ChannelDuplexHandler;
import io.netty.channel.ChannelHandler.Sharable;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.util.AttributeKey;

/**
 *
 * This class is relay handler relating Cloud Interface
 *
 */
@Sharable
public class CoapRelayHandler extends ChannelDuplexHandler {

    /////////// Handler for Resource Directory
    private static final AttributeKey<ChannelHandlerContext> keyRDClient = AttributeKey
            .newInstance("rdCtx");

    private static class RDHandler
            extends SimpleChannelInboundHandler<CoapResponse> {
        @Override
        public void channelRead0(ChannelHandlerContext ctx, CoapResponse msg)
                throws Exception {
            Logger.d("Receive response from RD, forward to client");

            ChannelHandlerContext ctxToDevice = ctx.channel().attr(keyRDClient)
                    .get();
            ctxToDevice.writeAndFlush(msg);
        }

        @Override
        public void exceptionCaught(ChannelHandlerContext ctx,
                Throwable cause) {
            cause.printStackTrace();
            ctx.close();
        }
    }

    private CoapClient rdClient = new CoapClient();
    ///////////

    ////////// Handler for Account Server
    private static final AttributeKey<List<CoapRequest>> keyAccountClient = AttributeKey
            .newInstance("accountCtx");

    private class AccountHandler
            extends SimpleChannelInboundHandler<CoapResponse> {
        @Override
        public void channelRead0(ChannelHandlerContext ctx, CoapResponse msg)
                throws Exception {
            Logger.d("Receive response from AS, make request to RD");

            CoapRequest rdRequest = null;

            switch (msg.getResponseCode()) {
                case CREATED:
                    // response of POST request
                    rdRequest = ctx.channel().attr(keyAccountClient).get()
                            .remove(0);
                    rdClient.sendRequest(rdRequest);
                    break;

                case CONTENT:
                    // response of GET request, contains did list
                    rdRequest = ctx.channel().attr(keyAccountClient).get()
                            .remove(0);
                    // change uri to send RD
                    rdRequest.setUriPath(Constants.RD_URI);
                    rdRequest.setUriQuery(Constants.DEVICE_LIST);
                    rdRequest.setPayload(msg.getPayload());
                    rdClient.sendRequest(rdRequest);
                    break;

                default:
                    break;
            }
        }

        @Override
        public void exceptionCaught(ChannelHandlerContext ctx,
                Throwable cause) {
            cause.printStackTrace();
            ctx.close();
        }
    }

    private CoapClient asClient = new CoapClient();
    //////////

    private SessionManager sessionManager = null;

    public CoapRelayHandler(SessionManager sessionManager, String rdAddress,
            int rdPort, String acAddress, int acPort) {
        this.sessionManager = sessionManager;

        rdClient.addHandler(new RDHandler());

        asClient.addHandler(new AccountHandler());

        try {
            rdClient.startClient(new InetSocketAddress(rdAddress, rdPort));
            asClient.startClient(new InetSocketAddress(acAddress, acPort));
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        asClient.getChannelFuture().channel().attr(keyAccountClient)
                .set(new ArrayList<CoapRequest>());
    }

    private static final AttributeKey<ChannelHandlerContext> keyDevice = AttributeKey
            .newInstance("deviceCtx");

    private HashMap<String, CoapClient> ciRelayClients = new HashMap<String, CoapClient>();

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg)
            throws Exception {

        if (msg instanceof CoapRequest) {
            CoapRequest request = (CoapRequest) msg;
            // Parse uri, send to RD
            String uriPath = request.getUriPath();
            CoapRequest accountRequest = null;
            String userId, deviceId, authPayload;
            CoapResponse response = null;

            Logger.d("Request received, URI: " + uriPath);
            switch (uriPath) {
                case Constants.AUTH_URI:
                    // This case user wants to logout
                    if (request.getUriQuery().endsWith("logout")) {
                        ctx.channel().attr(Constants.Attribute_UserId).remove();
                        response = new CoapResponse(CoapStatus.DELETED);
                    } else {
                        response = new CoapResponse(CoapStatus.BAD_REQUEST);
                    }
                    ctx.writeAndFlush(response);
                    break;

                case Constants.RD_URI:
                    // RD POST means publish device to server
                    switch (request.getRequestMethod()) {
                        case POST:
                            userId = ctx.channel()
                                    .attr(Constants.Attribute_UserId).get();
                            deviceId = request.decodeDeviceId();
                            authPayload = String.format(
                                    "{\"userid\":\"%s\",\"deviceid\":\"%s\"}",
                                    userId, deviceId);
                            accountRequest = new CoapRequest(CoapMethod.POST);
                            accountRequest.setUriPath(Constants.ACCOUNT_URI);
                            accountRequest.setUriQuery("reqtype=publish");
                            accountRequest.setToken(request.getToken());
                            accountRequest.setPayload(authPayload.getBytes(StandardCharsets.UTF_8));

                            // TODO: deviceId must be registered after session
                            // granted
                            Logger.d("Adding deviceId to session: " + deviceId);
                            sessionManager.addSession(deviceId, ctx);
                            break;

                        default:
                            Logger.e("Unsupported request type");
                            break;
                    }

                    rdClient.getChannelFuture().channel().attr(keyRDClient)
                            .set(ctx);

                    // Add original request to list for future use
                    asClient.getChannelFuture().channel().attr(keyAccountClient)
                            .get().add(request);
                    asClient.sendRequest(accountRequest);
                    return;

                case Constants.WELL_KNOWN_URI:
                    switch (request.getRequestMethod()) {
                        case GET:
                            userId = ctx.channel()
                                    .attr(Constants.Attribute_UserId).get();
                            authPayload = String.format("{\"userid\":\"%s\"}",
                                    userId);
                            accountRequest = new CoapRequest(CoapMethod.GET);
                            accountRequest.setUriPath(Constants.ACCOUNT_URI);
                            accountRequest.setUriQuery("reqtype=find");
                            accountRequest.setToken(request.getToken());
                            accountRequest.setPayload(authPayload.getBytes());
                            break;

                        default:
                            Logger.e("Unsupported request type");
                            break;
                    }

                    rdClient.getChannelFuture().channel().attr(keyRDClient)
                            .set(ctx);

                    // Add original request to list for future use
                    asClient.getChannelFuture().channel().attr(keyAccountClient)
                            .get().add(request);
                    asClient.sendRequest(accountRequest);
                    return;

                case Constants.KEEP_ALIVE_URI:
                    break;

                default:
                    List<String> uriPathList = request.getUriPathSegments();
                    String originUriPathList = request.getUriPath();
                    Logger.i("uriPahtList: " + uriPathList.toString());
                    String ciAddress = uriPathList.get(0);
                    String did = uriPathList.get(1);

                    Logger.i("CI address: " + ciAddress);
                    Logger.i("did: " + did);

                    // TODO: getMyIP ?
                    String hostAddress = Net.getMyIpAddress().replace("/", "");
                    Logger.i("hostAddress : " + hostAddress);
                    // if published CI is mine
                    if (hostAddress.equals(ciAddress) == true) {
                        // find ctx about did, and send msg
                        Logger.d("published CI is mine");
                        String resource = new String();
                        List<String> pathSegments = uriPathList.subList(2,
                                uriPathList.size());
                        for (String path : pathSegments) {
                            resource += "/";
                            resource += path;
                        }
                        Logger.i("resource: " + resource);
                        request.setUriPath(resource);

                        ChannelHandlerContext deviceCtx = sessionManager
                                .querySession(did);
                        if (deviceCtx != null) {
                            deviceCtx.attr(keyDevice).set(ctx);
                            deviceCtx.writeAndFlush(request);
                        } else {
                            Logger.e("deviceCtx is null");
                            response = new CoapResponse(CoapStatus.FORBIDDEN);
                            response.setToken(request.getToken());
                            ctx.writeAndFlush(response);
                        }
                    } else {
                        // if CI is not connected, connect and send msg
                        CoapClient otherCI = null;
                        synchronized (ciRelayClients) {
                            otherCI = ciRelayClients.get(ciAddress);
                            if (otherCI == null) {
                                otherCI = new CoapClient();
                                otherCI.startClient(
                                        new InetSocketAddress(ciAddress, 5683));
                                ciRelayClients.put(ciAddress, otherCI);
                            }
                        }
                        request.setUriPath(originUriPathList);
                        otherCI.sendRequest(request);
                    }
                    return;
            }

        } else if (msg instanceof CoapResponse) {
            if (ctx.attr(keyDevice).get() != null) {
                Logger.i("ctx.channel : "
                        + ctx.attr(keyDevice).get().channel().toString());
                ctx.attr(keyDevice).get().writeAndFlush(msg);
                return;
            }
        }

        super.channelRead(ctx, msg);
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) {

        cause.printStackTrace();
        ctx.close();
    }
}
