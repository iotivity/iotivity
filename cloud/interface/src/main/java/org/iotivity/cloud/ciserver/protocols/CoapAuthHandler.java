package org.iotivity.cloud.ciserver.protocols;

import java.net.InetSocketAddress;
import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.Map;

import org.iotivity.cloud.base.CoapClient;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.coap.enums.CoapStatus;
import org.iotivity.cloud.ciserver.Constants;
import org.iotivity.cloud.util.Cbor;
import org.iotivity.cloud.util.JSONUtil;
import org.iotivity.cloud.util.Logger;

import io.netty.channel.ChannelDuplexHandler;
import io.netty.channel.ChannelHandler.Sharable;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.util.AttributeKey;

@Sharable
public class CoapAuthHandler extends ChannelDuplexHandler {

    private static final AttributeKey<ChannelHandlerContext> keyAuthClient = AttributeKey
            .newInstance("authCtx");

    private class AccountHandler
            extends SimpleChannelInboundHandler<CoapResponse> {

        @Override
        public void channelRead0(ChannelHandlerContext ctx, CoapResponse msg)
                throws Exception {
            Logger.d("Receive response from account, forward to client");

            ChannelHandlerContext ctxToDevice = ctx.channel()
                    .attr(keyAuthClient).get();

            if (msg.getResponseCode() == CoapStatus.CREATED) {
                Map<String, String> response = JSONUtil
                        .parseJSON(new String(msg.getPayload(), StandardCharsets.UTF_8));

                String userId = response.get("userid");
                if (userId != null) {
                    ctxToDevice.channel().attr(Constants.Attribute_UserId)
                            .set(userId);
                }
                msg.setPayload(cbor.encodingPayloadToCbor(response));

                CoapAuthHandler authHandler = ctxToDevice.channel().pipeline()
                        .get(CoapAuthHandler.class);

                ctxToDevice.channel().pipeline().remove(authHandler);
            }

            ctxToDevice.writeAndFlush(msg);

            if (msg.getResponseCode() != CoapStatus.CREATED)
                ctxToDevice.close();
        }

        @Override
        public void exceptionCaught(ChannelHandlerContext ctx,
                Throwable cause) {
            cause.printStackTrace();
            ctx.close();
        }
    }

    private CoapClient accountClient = new CoapClient();

    public CoapAuthHandler(String accountAddress, int accountPort) {

        accountClient.addHandler(new AccountHandler());
        try {
            accountClient.startClient(
                    new InetSocketAddress(accountAddress, accountPort));
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    private Cbor<HashMap<Object, Object>> cbor = new Cbor<HashMap<Object, Object>>();

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg)
            throws Exception {

        if (msg instanceof CoapRequest) {
            CoapRequest request = (CoapRequest) msg;
            switch (request.getUriPath()) {
                // This handler only used for initial handshake
                case Constants.AUTH_URI:
                    HashMap<Object, Object> payloadData = cbor
                            .parsePayloadFromCbor(request.getPayload(),
                                    HashMap.class);
                    request.setPayload(
                            JSONUtil.writeJSON(payloadData).getBytes(StandardCharsets.UTF_8));
                    accountClient.getChannelFuture().channel()
                            .attr(keyAuthClient).set(ctx);
                    accountClient.sendRequest(request);
                    return;

                case Constants.KEEP_ALIVE_URI:
                    super.channelRead(ctx, msg);
                    return;

                default:
                    CoapResponse response = new CoapResponse(
                            CoapStatus.UNAUTHORIZED);
                    Logger.e("Sending UNAUTHORIZED to client");
                    ctx.writeAndFlush(response);
                    break;
            }
        }

        Logger.d("Invalid packet for authenticating");
        ctx.close();
    }
}
