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
package org.iotivity.cloud.ciserver;

import java.util.HashMap;

import org.iotivity.cloud.base.ServerSystem;
import org.iotivity.cloud.base.connector.ConnectorPool;
import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.device.IRequestChannel;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.base.exception.ServerException.BadRequestException;
import org.iotivity.cloud.base.exception.ServerException.InternalServerErrorException;
import org.iotivity.cloud.base.exception.ServerException.PreconditionFailedException;
import org.iotivity.cloud.base.exception.ServerException.UnAuthorizedException;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.base.server.CoapServer;
import org.iotivity.cloud.base.server.HttpServer;
import org.iotivity.cloud.base.server.Server;
import org.iotivity.cloud.util.Cbor;
import org.iotivity.cloud.util.ErrorLogger;

import io.netty.channel.ChannelDuplexHandler;
import io.netty.channel.ChannelHandler.Sharable;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelPromise;

public class DeviceServerSystem extends ServerSystem {

    public class CoapDevicePool {
        HashMap<String, Device> mMapDevice = new HashMap<>();

        public void addDevice(Device device) {
            String deviceId = ((CoapDevice) device).getDeviceId();
            synchronized (mMapDevice) {
                mMapDevice.put(deviceId, device);
            }
        }

        public void removeDevice(Device device) {
            String deviceId = ((CoapDevice) device).getDeviceId();
            synchronized (mMapDevice) {
                if (mMapDevice.get(deviceId) == device) {
                    mMapDevice.remove(deviceId);
                }
            }
        }

        public Device queryDevice(String deviceId) {
            Device device = null;
            synchronized (mMapDevice) {
                device = mMapDevice.get(deviceId);
            }
            return device;
        }
    }

    CoapDevicePool mDevicePool = new CoapDevicePool();

    @Sharable
    class CoapLifecycleHandler extends ChannelDuplexHandler {
        @Override
        public void channelRead(ChannelHandlerContext ctx, Object msg) {

            if (msg instanceof CoapRequest) {
                try {
                    CoapDevice coapDevice = (CoapDevice) ctx.channel()
                            .attr(keyDevice).get();

                    if (coapDevice.getIssuedTime() != null
                            && coapDevice.isExpiredTime()) {
                        throw new UnAuthorizedException("token is expired");
                    }

                } catch (Throwable t) {
                    ResponseStatus responseStatus = t instanceof ServerException
                            ? ((ServerException) t).getErrorResponse()
                            : ResponseStatus.BAD_REQUEST;
                    ctx.channel().writeAndFlush(MessageBuilder
                            .createResponse((CoapRequest) msg, responseStatus));
                    ErrorLogger.write(ctx.channel(), t);
                }
            }

            ctx.fireChannelRead(msg);
        }

        @Override
        public void channelActive(ChannelHandlerContext ctx) {

            // Authenticated device connected
            Device device = ctx.channel().attr(keyDevice).get();
            mDevicePool.addDevice(device);
            device.onConnected();

            sendDevicePresence(device.getDeviceId(), "on");
        }

        @Override
        public void channelInactive(ChannelHandlerContext ctx) {
            Device device = ctx.channel().attr(keyDevice).get();
            // Some cases, this event occurs after new device connected using
            // same di.
            // So compare actual value, and remove if same.
            if (device != null) {
                mDevicePool.removeDevice(device);
                device.onDisconnected();
                ctx.channel().attr(keyDevice).remove();

                sendDevicePresence(device.getDeviceId(), "off");
            }
        }

        private void sendDevicePresence(String deviceId, String state) {

            Cbor<HashMap<String, Object>> cbor = new Cbor<>();
            IRequestChannel RDServer = ConnectorPool.getConnection("rd");
            HashMap<String, Object> payload = new HashMap<String, Object>();
            payload.put(Constants.DEVICE_ID, deviceId);
            payload.put(Constants.PRESENCE_STATE, state);
            StringBuffer uriPath = new StringBuffer();
            uriPath.append("/" + Constants.PREFIX_WELL_KNOWN);
            uriPath.append("/" + Constants.PREFIX_OCF);
            uriPath.append("/" + Constants.DEVICE_PRESENCE_URI);
            RDServer.sendRequest(MessageBuilder.createRequest(
                    RequestMethod.POST, uriPath.toString(), null,
                    ContentFormat.APPLICATION_CBOR,
                    cbor.encodingPayloadToCbor(payload)), null);
        }
    }

    @Sharable
    class CoapAuthHandler extends ChannelDuplexHandler {
        private Cbor<HashMap<String, Object>> mCbor = new Cbor<HashMap<String, Object>>();

        @Override
        public void channelActive(ChannelHandlerContext ctx) {
            // Actual channel active should decided after authentication.
        }

        @Override
        public void write(ChannelHandlerContext ctx, Object msg,
                ChannelPromise promise) {

            try {
                if (!(msg instanceof CoapResponse)) {
                    throw new BadRequestException(
                            "this msg type is not CoapResponse");
                }
                // This is CoapResponse
                // Once the response is valid, add this to deviceList
                CoapResponse response = (CoapResponse) msg;

                switch (response.getStatus()) {
                    // TODO: below section is exceptional case for ping from
                    // clients
                    case CREATED:
                    case CONTENT:
                        break;

                    case VALID:
                        if (response.getPayload() != null) {
                            HashMap<String, Object> payloadData = mCbor
                                    .parsePayloadFromCbor(response.getPayload(),
                                            HashMap.class);
                            int remainTime = (int) payloadData
                                    .get(Constants.EXPIRES_IN);

                            Device device = ctx.channel().attr(keyDevice).get();
                            ((CoapDevice) device).setExpiredPolicy(remainTime);

                            // Remove current auth handler
                            ctx.channel().pipeline().remove(this);

                            // Raise event that we have Authenticated device
                            ctx.fireChannelActive();
                        }
                        break;

                    default:
                }

                ctx.writeAndFlush(msg);

            } catch (Throwable t) {
                ErrorLogger.write(ctx.channel(), t);
                ctx.writeAndFlush(msg);
                ctx.close();
            }
        }

        @Override
        public void channelRead(ChannelHandlerContext ctx, Object msg) {

            try {
                if (!(msg instanceof CoapRequest)) {
                    throw new BadRequestException(
                            "this msg type is not CoapRequest");
                }

                // And check first response is VALID then add or cut
                CoapRequest request = (CoapRequest) msg;
                // Check whether first request is about account
                if (request.getUriPathSegments().size() < 2) {
                    throw new UnAuthorizedException(
                            "first request must be about account or keepAlive");
                }

                // TODO: device sends ping whether not authorized
                if (request.getUriPathSegments().get(1)
                        .equals(Constants.KEEP_ALIVE_URI)) {
                    // Go upperlayer
                    ctx.fireChannelRead(msg);
                    return;
                }

                if (request.getUriPathSegments().size() < 3
                        || request.getUriPathSegments().get(2)
                                .equals(Constants.ACCOUNT_URI) == false) {
                    throw new UnAuthorizedException(
                            "authentication required first");
                }

                HashMap<String, Object> authPayload = mCbor
                        .parsePayloadFromCbor(request.getPayload(),
                                HashMap.class);

                if (authPayload.containsKey("di") == false) {
                    throw new PreconditionFailedException(
                            "di property is not included");
                }

                CoapDevice device = new CoapDevice(ctx,
                        (String) authPayload.get("di"),
                        (String) authPayload.get("accesstoken"));

                // Create device first and pass to upperlayer
                ctx.channel().attr(keyDevice).set(device);

                ctx.fireChannelRead(msg);

            } catch (Throwable t) {
                ResponseStatus responseStatus = t instanceof ServerException
                        ? ((ServerException) t).getErrorResponse()
                        : ResponseStatus.UNAUTHORIZED;
                ctx.channel().writeAndFlush(MessageBuilder
                        .createResponse((CoapRequest) msg, responseStatus));
                ErrorLogger.write(ctx.channel(), t);
            }
        }
    }

    @Sharable
    class HttpAuthHandler extends ChannelDuplexHandler {
        @Override
        public void channelActive(ChannelHandlerContext ctx) throws Exception {
            // After current channel authenticated, raise to upper layer
        }
    }

    @Override
    public void addServer(Server server) {
        if (server instanceof CoapServer) {
            server.addHandler(new CoapAuthHandler());
            server.addHandler(new CoapLifecycleHandler());
        }

        if (server instanceof HttpServer) {
            server.addHandler(new HttpAuthHandler());
        }

        super.addServer(server);
    }

    public CoapDevicePool getDevicePool() {
        return mDevicePool;
    }
}
