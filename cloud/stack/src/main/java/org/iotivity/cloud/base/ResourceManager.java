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
package org.iotivity.cloud.base;

import java.util.ArrayList;

import org.iotivity.cloud.base.protocols.coap.CoapRequest;

import io.netty.channel.ChannelHandler.Sharable;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

@Sharable
public class ResourceManager extends SimpleChannelInboundHandler<CoapRequest> {

    private ArrayList<Resource> resources = new ArrayList<Resource>();

    @Override
    public void channelRead0(ChannelHandlerContext ctx, CoapRequest request)
            throws Exception {

        Resource res = queryResourceHandler(request.getUriPath());

        if (res != null) {
            res.onRequestReceived(ctx, request);
        }
    }

    private Resource queryResourceHandler(String query_uri) {

        Resource res = null;

        for (Resource r : resources) {

            String uri = r.getUri();
            if (uri.contains(query_uri)) {
                res = r;
                break;
            }
        }

        return res;
    }

    public void registerResource(Resource res) {

        synchronized (this) {
            resources.add(res);
        }
    }

    public void unregisterResource(Resource res) {

        synchronized (this) {
            resources.remove(res);
        }
    }
}
