package org.iotivity.cloud.base.protocols.http;

import io.netty.channel.*;
import io.netty.handler.codec.http.*;
import io.netty.util.CharsetUtil;
import org.iotivity.cloud.base.healthcheck.HealthHolder;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.Message;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.concurrent.ConcurrentHashMap;

public class SimpleHttpHandler extends SimpleChannelInboundHandler<HttpObject> {

    private final static Logger Log             = LoggerFactory.getLogger(SimpleHttpHandler.class);

    private final HealthHolder healthHolder;

    public SimpleHttpHandler(HealthHolder healthHolder) {
        this.healthHolder = healthHolder;
    }

    @Override
    protected void channelRead0(ChannelHandlerContext ctx, HttpObject msg) throws Exception {

        if (msg instanceof HttpRequest) {
            final HttpRequest request = (HttpRequest) msg;
            Log.debug("HTTP uri: {}",request.uri());
            if(request.uri().compareTo("/api/healthcheck") == 0){
                if(healthHolder.isHealthy()){
                    ctx.writeAndFlush(createResponse(HttpResponseStatus.OK)).addListener(ChannelFutureListener.CLOSE);
                }else {
                    ctx.writeAndFlush(createResponse(HttpResponseStatus.INTERNAL_SERVER_ERROR)).addListener(ChannelFutureListener.CLOSE);
                }
            }else {
                ctx.writeAndFlush(createResponse(HttpResponseStatus.NOT_FOUND)).addListener(ChannelFutureListener.CLOSE);
            }
        }
    }

    @Override
    public void channelReadComplete(ChannelHandlerContext ctx) throws Exception {
        ctx.flush();
    }

    private HttpResponse createResponse(final HttpResponseStatus status){
        return new DefaultFullHttpResponse(HttpVersion.HTTP_1_1, status);
    }



}
