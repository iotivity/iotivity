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

import java.util.*;
import java.util.stream.Collectors;

import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.base.exception.ServerException.BadRequestException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.base.resource.Resource;
import org.iotivity.cloud.ciserver.CloudInterfaceServer;
import org.iotivity.cloud.ciserver.Constants;
import org.iotivity.cloud.ciserver.DeviceServerSystem;
import org.iotivity.cloud.util.Cbor;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 *
 * This class provides a set of APIs to use KeepAlive Resource for ensuring the
 * connection.
 *
 */
public class KeepAliveResource extends Resource  implements DevicePresenter {
    private final static Logger Log = LoggerFactory.getLogger(KeepAliveResource.class);
    private int[]                         mIntervals      = null;
    private Timer                         mTimer          = new Timer();
    private Cbor<HashMap<String, Object>> mCbor           = new Cbor<>();
    private HashMap<Device, Long>         mConnectionPool = new HashMap<>();

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
                response = handlePingConfig(request);
                break;

            case PUT:
            case POST:
                response = handlePingConfig(srcDevice, request);
                break;

            default:
                throw new BadRequestException(
                        request.getMethod() + " request type is not support");
        }

        srcDevice.sendResponse(response);
    }

    public void startSessionChecker(int startTime, int intervalTime) {
        mTimer.schedule(new KeepAliveTask(), startTime, intervalTime);
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
    private IResponse handlePingConfig(IRequest request) {

        HashMap<String, int[]> payloadData = new HashMap<>();
        payloadData.put(Constants.REQ_PING_ARRAY, mIntervals);

        return MessageBuilder.createResponse(request, ResponseStatus.CONTENT,
                ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(payloadData));
    }

    private IResponse handlePingConfig(Device srcDevice, IRequest request) {

        HashMap<String, Object> payloadData = mCbor
                .parsePayloadFromCbor(request.getPayload(), HashMap.class);

        checkPayloadException(Constants.REQ_PING, payloadData);

        Long pingTime = (long) (Integer
                .valueOf(payloadData.get(Constants.REQ_PING).toString())
                * (long) 60000 * 1.1);
        Long connectionTime = System.currentTimeMillis() + pingTime;
        mConnectionPool.put(srcDevice, connectionTime);

        return MessageBuilder.createResponse(request, ResponseStatus.VALID);
    }


    @Override
    public Set<String> getDeviceIds() {
        Map<Device, Long> map = Collections
                .synchronizedMap(mConnectionPool);
        synchronized (map){
            return new HashSet<>(map.keySet().stream().map(device -> device.getDeviceId()).collect(Collectors.toSet()));
        }
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
            try {
                synchronized (map) {
                    Long currentTime = System.currentTimeMillis();
                    for (Device device : map.keySet()) {
                        Long lifeTime = (Long) map.get(device);
                        if (lifeTime != null && lifeTime < currentTime) {
                            deleteList.add(device);
                        }
                    }
                    for(final Device device : deleteList){
                        final List<Device> samedevice = map.keySet().stream().filter(d -> filter(d,device)).collect(Collectors.toList());
                        if(device != null && samedevice != null && samedevice.size() > 0){
                            Log.info("Channel for device: {} is empty", device.getDeviceId());
                            device.setParameter(DeviceServerSystem.EMPTY_CHANNEL,true);
                        }
                    }
                }

                for (Device device : deleteList) {
                    mConnectionPool.remove(device);
                    device.getCtx().fireChannelInactive();
                    device.getCtx().close();
                }
            } catch (final Exception e){
                Log.error("Unable to remove not responding device", e);
            }
        }
        
        private boolean filter(final Device listDevice, final Device device){
            if(listDevice == null || listDevice.getDeviceId() == null || device == null || device.getDeviceId() == null){
                return false;
            }
            if(listDevice.getCtx() == null && listDevice.getCtx().channel() == null && device.getCtx() == null && device.getCtx().channel() == null){
                return false;
            }
            return listDevice.getDeviceId().compareTo(device.getDeviceId()) == 0
                    && listDevice.getCtx().channel().id().asLongText().compareTo(device.getCtx().channel().id().asLongText()) != 0;
        }
    }
}
