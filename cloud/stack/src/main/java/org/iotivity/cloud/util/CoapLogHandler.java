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
package org.iotivity.cloud.util;

import java.nio.charset.StandardCharsets;

import org.iotivity.cloud.base.protocols.coap.CoapMessage;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;

import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelDuplexHandler;
import io.netty.channel.ChannelHandler.Sharable;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelPromise;

/**
 *
 * This class provides a set of APIs to print out logs for CoAP request and
 * response.
 *
 */
@Sharable
public class CoapLogHandler extends ChannelDuplexHandler {

    @Override
    public void write(ChannelHandlerContext ctx, Object msg,
            ChannelPromise promise) {

        if (msg instanceof CoapMessage) {

            CoapMessage coapMessage = (CoapMessage) msg;

            StringBuilder strBuilder = new StringBuilder();
            strBuilder.append(getStringCtx(ctx));
            strBuilder.append(
                    "\n/******************************************************************************/\n");
            strBuilder.append(getStringCoapMessage(coapMessage));
            strBuilder.append(
                    "\n/******************************************************************************/\n");
            Logger.d(strBuilder.toString());

        } else if (msg instanceof String) {

            String message = (String) msg;

            StringBuilder strBuilder = new StringBuilder();
            strBuilder.append(getStringCtx(ctx));
            strBuilder.append(
                    "\n/******************************************************************************/\n");
            strBuilder.append(message);
            strBuilder.append(
                    "\n/******************************************************************************/\n");
            Logger.d(strBuilder.toString());

            ByteBuf outByteBuf = ctx.alloc().buffer();
            outByteBuf.writeBytes(message.getBytes(StandardCharsets.UTF_8));
        }

        ctx.write(msg);
    }

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg)
            throws Exception {

        if (msg instanceof CoapMessage) {

            CoapMessage coapMessage = (CoapMessage) msg;

            StringBuilder strBuilder = new StringBuilder();
            strBuilder.append(getStringCtx(ctx));
            strBuilder.append(
                    "\n/******************************************************************************/\n");
            strBuilder.append(getStringCoapMessage(coapMessage));
            strBuilder.append(
                    "\n/******************************************************************************/\n");
            Logger.d(strBuilder.toString());

        } else if (msg instanceof ByteBuf) {

            ByteBuf inByteBuf = (ByteBuf) msg;

            String message = null;
            StringBuffer strBuffer = new StringBuffer();
            while (inByteBuf.isReadable()) {
                strBuffer.append((char) inByteBuf.readByte());
            }
            message = strBuffer.toString();

            StringBuilder strBuilder = new StringBuilder();
            strBuilder.append(getStringCtx(ctx));
            strBuilder.append(
                    "\n/******************************************************************************/\n");
            strBuilder.append(message);
            strBuilder.append(
                    "\n/******************************************************************************/\n");
            Logger.d(strBuilder.toString());
        }

        ctx.fireChannelRead(msg);
    }

    private String getStringCtx(ChannelHandlerContext ctx) {

        StringBuilder strBuilder = new StringBuilder();

        strBuilder.append("\n");
        strBuilder.append("Local Address: ");
        strBuilder.append(ctx.channel().localAddress().toString());

        strBuilder.append("\n");
        strBuilder.append("Remote Address: ");
        strBuilder.append(ctx.channel().remoteAddress().toString());

        return strBuilder.toString();
    }

    private String getStringCoapMessage(CoapMessage coapMessage) {

        StringBuilder strBuilder = new StringBuilder();

        strBuilder.append("Code: ");
        strBuilder.append(coapMessage.getCode());
        strBuilder.append("\n");

        strBuilder.append("Token Length: ");
        strBuilder.append(coapMessage.getTokenLength());
        strBuilder.append("\n");

        strBuilder.append("Token Data: ");
        strBuilder.append(coapMessage.getTokenString());
        strBuilder.append("\n");

        if (coapMessage instanceof CoapRequest) {

            CoapRequest coapRequest = (CoapRequest) coapMessage;

            strBuilder.append("Option URI_PATH: ");
            strBuilder.append(coapRequest.getUriPath());
            strBuilder.append("\n");

            strBuilder.append("Option URI_QUERY: ");
            strBuilder.append(coapRequest.getUriQuery());
            strBuilder.append("\n");
        }

        strBuilder.append("Option CONTENT_FORMAT: ");
        strBuilder.append(coapMessage.getContentFormatString());
        strBuilder.append("\n");

        strBuilder.append("Payload Length: ");
        strBuilder.append(coapMessage.getPayloadSize());

        strBuilder.append("\n");
        strBuilder.append("Payload Data: ");
        strBuilder.append(coapMessage.getPayloadString());

        return strBuilder.toString();
    }
}
