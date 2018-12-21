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
package org.iotivity.cloud.base.server;

import io.netty.channel.ChannelHandler;
import io.netty.handler.codec.http.HttpRequestDecoder;
import io.netty.handler.codec.http.HttpResponseEncoder;
import org.iotivity.cloud.base.healthcheck.HealthHolder;
import org.iotivity.cloud.base.protocols.http.SimpleHttpHandler;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.net.InetSocketAddress;

public class SimpleHttpServer extends Server {
    private final static Logger Log = LoggerFactory.getLogger(SimpleHttpServer.class);

    private final HealthHolder healthHolder;

    public SimpleHttpServer(InetSocketAddress inetSocketAddress, HealthHolder healthHolder) {
        super(inetSocketAddress);
        this.healthHolder = healthHolder;
        Log.info("HTTP server listening on {}",inetSocketAddress);
    }

    @Override
    protected ChannelHandler[] onQueryDefaultHandler() {

        return new ChannelHandler[] { new HttpRequestDecoder(),
                new HttpResponseEncoder(),
                new SimpleHttpHandler(healthHolder) };
    }
}
