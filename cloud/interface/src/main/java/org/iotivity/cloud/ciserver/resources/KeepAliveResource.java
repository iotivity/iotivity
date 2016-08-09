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
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;

import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.base.exception.ServerException.BadRequestException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.base.resource.Resource;
import org.iotivity.cloud.ciserver.Constants;
import org.iotivity.cloud.util.Cbor;

/**
 *
 * This class provides a set of APIs to use KeepAlive Resource for ensuring the
 * connection.
 *
 */
public class KeepAliveResource extends Resource {
    private int[]                          mIntervals      = null;
    private Timer                          mTimer          = new Timer();
    private Cbor<HashMap<String, Integer>> mCbor           = new Cbor<>();
    private HashMap<Device, Long>          mConnectionPool = new HashMap<>();

    public KeepAliveResource(int[] intervals) {
        super(Arrays.asList(Constants.PREFIX_OIC, Constants.KEEP_ALIVE_URI));
        mIntervals = intervals;
    }

    @Override
    public void onDefaultRequestReceived(Device srcDevice, IRequest request)
            throws ServerException {

        IResponse response = null;

        switch (request.getMethod()) {
            case GET:
                response = handleGetPingConfig(request);
                break;

            case PUT:
                response = handlePutPingConfig(srcDevice, request);
                break;

            default:
                throw new BadRequestException(
                        request.getMethod() + " request type is not support");
        }

        srcDevice.sendResponse(response);
    }

    public void startSessionChecker() {
        mTimer.schedule(new KeepAliveTask(), 30000, 60000);
    }

    public void stopSessionChecker() {
        mTimer.cancel();
    }

    /**
     * API for making interval and first response to Resource
     * 
     * @param request
     *            ChannelHandlerContext of request message
     */
    private IResponse handleGetPingConfig(IRequest request) {

        HashMap<String, int[]> payloadData = new HashMap<>();
        payloadData.put("inarray", mIntervals);

        return MessageBuilder.createResponse(request, ResponseStatus.CONTENT,
                ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(payloadData));
    }

    private IResponse handlePutPingConfig(Device srcDevice, IRequest request) {

        HashMap<String, Integer> payloadData = mCbor
                .parsePayloadFromCbor(request.getPayload(), HashMap.class);

        if (payloadData.containsKey("in")) {
            mConnectionPool.put(srcDevice, System.currentTimeMillis()
                    + (payloadData.get("in") * (long) 60000));
        }

        return MessageBuilder.createResponse(request, ResponseStatus.VALID);
    }

    /**
     * API for managing session
     */
    private class KeepAliveTask extends TimerTask {

        @Override
        public void run() {
            Map<Device, Long> map = Collections
                    .synchronizedMap(mConnectionPool);

            List<Device> deleteList = new ArrayList<>();

            synchronized (map) {
                Long currentTime = System.currentTimeMillis();
                for (Device device : map.keySet()) {
                    Long lifeTime = (Long) map.get(device);
                    if (lifeTime < currentTime) {
                        deleteList.add(device);
                    }
                }
            }

            for (Device device : deleteList) {
                mConnectionPool.remove(device);
                device.getCtx().fireChannelInactive();
                device.getCtx().close();
            }
        }
    }
}
