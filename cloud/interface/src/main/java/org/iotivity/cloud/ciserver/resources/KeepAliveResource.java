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
package org.iotivity.cloud.ciserver.resources;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.Timer;
import java.util.TimerTask;

import org.iotivity.cloud.base.Resource;
import org.iotivity.cloud.base.SessionManager;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.coap.enums.CoapStatus;
import org.iotivity.cloud.ciserver.Constants;
import org.iotivity.cloud.util.Cbor;
import org.iotivity.cloud.util.Logger;

import io.netty.channel.ChannelHandlerContext;

/**
 *
 * This class provides a set of APIs to use KeepAlive Resource for ensuring the
 * connection.
 *
 */
public class KeepAliveResource extends Resource {

    private int[]                                intervals;
    private HashMap<ChannelHandlerContext, Long> connectPool;
    private Timer                                timer;
    private Cbor<HashMap<String, Integer>>       cbor;
    private SessionManager                       sessionManager = null;

    public void setIntervals(int[] intervals) {
        this.intervals = intervals;
    }

    public int[] getIntervals() {
        return this.intervals;
    }

    public KeepAliveResource(SessionManager sessionManager, int[] intervals) {
        setUri(Constants.KEEP_ALIVE_URI);
        setIntervals(intervals);
        this.sessionManager = sessionManager;
        connectPool = new HashMap<ChannelHandlerContext, Long>();
        timer = new Timer();
        cbor = new Cbor<HashMap<String, Integer>>();
    }

    public void startSessionChecker() {
        timer.schedule(new KeepAliveTask(), 30000, 60000);
    }

    public void stopSessionChecker() {
        timer.cancel();
    }

    /**
     * API for receiving message(message to keepalive resource)
     *
     * @param ctx
     *            ChannelHandlerContext of request message
     * @param request
     *            CoAP request message
     */
    @Override
    public void onRequestReceived(ChannelHandlerContext ctx,
            CoapRequest request) {

        CoapResponse response = null;

        switch (request.getRequestMethod()) {
            // First message to KeepAlive from resource
            case GET:
                if (intervals != null) {
                    response = makePingConfigMessage(request);
                    connectPool.put(ctx, System.currentTimeMillis()
                            + (intervals[0] * (long) 60000));
                }
                break;
            // interval Message to KeepAlive After receiving GET Message
            case PUT:
                HashMap<String, Integer> payloadData = null;
                payloadData = cbor.parsePayloadFromCbor(request.getPayload(),
                        HashMap.class);

                Logger.d("Receive payloadData : " + payloadData);
                if (payloadData != null) {
                    if (payloadData.containsKey("in")) {
                        Logger.d("interval : " + payloadData.get("in"));

                        connectPool.put(ctx, System.currentTimeMillis()
                                + (payloadData.get("in") * (long) 60000));
                    }
                }
                response = makeResponse(request);
                break;

            case POST:
                break;

            case DELETE:
                break;
        }

        ctx.writeAndFlush(response);
    }

    /**
     * API for making response to Resource
     *
     * @param request
     *            ChannelHandlerContext of request message
     */
    private CoapResponse makeResponse(CoapRequest request) {
        CoapResponse response = new CoapResponse(CoapStatus.VALID);
        response.setToken(request.getToken());

        return response;
    }

    /**
     * API for making interval and first response to Resource
     *
     * @param request
     *            ChannelHandlerContext of request message
     */
    private CoapResponse makePingConfigMessage(CoapRequest request) {
        CoapResponse response = new CoapResponse(CoapStatus.CONTENT);
        response.setToken(request.getToken());

        HashMap<String, int[]> payloadData = new HashMap<String, int[]>();
        payloadData.put("inarray", intervals);

        byte[] cborData = cbor.encodingPayloadToCbor(payloadData);

        response.setPayload(cborData);

        Logger.d("Send payloadData : " + payloadData);

        return response;
    }

    /**
     * API for managing session
     */
    public class KeepAliveTask extends TimerTask {

        @Override
        public void run() {
            Map<ChannelHandlerContext, Long> map = Collections
                    .synchronizedMap(connectPool);
            Set<ChannelHandlerContext> keySet = map.keySet();
            ArrayList<ChannelHandlerContext> deleteList = new ArrayList<ChannelHandlerContext>();
            Iterator<ChannelHandlerContext> iterator = null;
            synchronized (map) {
                iterator = keySet.iterator();
                Long currentTime = System.currentTimeMillis();
                // check interval
                while (iterator.hasNext()) {
                    ChannelHandlerContext key = iterator.next();
                    if (map.containsKey(key)) {
                        if (map.get(key) != null) {
                            Long lifeTime = (Long) map.get(key);
                            if (lifeTime != null) {
                                Logger.d("KeepAliveTask Operating : "
                                        + key.channel().toString() + ", Time : "
                                        + (lifeTime - currentTime));
                                if (lifeTime < currentTime) {
                                    deleteList.add(key);
                                }
                            }
                        }
                    }
                }

            }
            iterator = deleteList.iterator();
            // remove session
            while (iterator.hasNext()) {
                ChannelHandlerContext key = iterator.next();
                Logger.d("KeepAliveTask Remove");
                connectPool.remove(key);
                sessionManager.removeSessionByChannel(key);
                key.close();
            }
        }
    }
}
