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
package org.iotivity.cloud.base.protocols.http;

import io.netty.channel.ChannelDuplexHandler;
import io.netty.channel.ChannelHandler.Sharable;
import io.netty.handler.codec.http.HttpContent;
import io.netty.handler.codec.http.HttpObject;
import io.netty.handler.codec.http.HttpRequest;
import io.netty.handler.codec.http.HttpResponse;
import io.netty.handler.codec.http.HttpUtil;
import io.netty.handler.codec.http.LastHttpContent;
import io.netty.util.CharsetUtil;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelPromise;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * This class provides a set of APIs to print out logs
 * for HTTP request and response.
 */
@Sharable
public class HttpLogHandler extends ChannelDuplexHandler {
    private final static Logger Log             = LoggerFactory.getLogger(HttpLogHandler.class);

    @Override
    public void channelActive(ChannelHandlerContext ctx) throws Exception {
        Log.trace("[{}] HTTP Connected, Address: {}", ctx.channel().id().asLongText().substring(26),
                ctx.channel().remoteAddress().toString());

        ctx.fireChannelActive();
    }

    @Override
    public void channelInactive(ChannelHandlerContext ctx) throws Exception {
        Log.trace("[{}] HTTP Disconnected, Address: {}", ctx.channel().id().asLongText().substring(26),
                ctx.channel().remoteAddress().toString());

        ctx.fireChannelInactive();
    }

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg)
            throws Exception {

        if (!(msg instanceof HttpObject)) {
            Log.trace("Non-HTTP message has been received to the HC proxy:\n"
                    + msg.toString());
        }

        if (msg instanceof HttpRequest) {

            StringBuilder contentStrBuilder = new StringBuilder();
            ctx.channel().attr(HCProxyProcessor.ctxStrContent)
                    .set(contentStrBuilder);

            HttpRequest httpRequest = (HttpRequest) msg;

            Log.trace(httpRequest.toString());

            if (HttpUtil.isTransferEncodingChunked(httpRequest)) {
                Log.trace("BEGINNING OF HTTP CHUNKED CONTENT");
            } else {
                Log.trace("BEGINNING OF HTTP CONTENT");
            }
        }

        if (msg instanceof HttpContent) {

            HttpContent content = (HttpContent) msg;

            StringBuilder contentStrBuilder = ctx.channel()
                    .attr(HCProxyProcessor.ctxStrContent).get();
            contentStrBuilder
                    .append(content.content().toString(CharsetUtil.UTF_8));

            if (content instanceof LastHttpContent) {

                Log.trace(contentStrBuilder.toString());

                Log.trace("END OF HTTP CONTENT");

                contentStrBuilder.setLength(0);
            }
        }

        ctx.fireChannelRead(msg);
    }

    @Override
    public void write(ChannelHandlerContext ctx, Object msg,
            ChannelPromise promise) {

        if (!(msg instanceof HttpObject)) {
            Log.trace("Non-HTTP message has been sent from the HC proxy:\n"
                    + msg.toString());
        }

        if (msg instanceof HttpResponse) {

            StringBuilder contentStrBuilder = ctx.channel()
                    .attr(HCProxyProcessor.ctxStrContent).get();

            HttpResponse httpResponse = (HttpResponse) msg;

            if (contentStrBuilder.length() > 0) {
                Log.trace(httpResponse.toString() + "\n\n"
                        + contentStrBuilder.toString());
            } else {
                Log.trace(httpResponse.toString());
            }
        }

        ctx.writeAndFlush(msg);
    }
}
