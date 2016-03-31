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
import java.util.HashMap;
import java.util.List;
import java.util.Map.Entry;
import java.util.Objects;

import io.netty.channel.ChannelHandlerContext;

public class SessionManager {

    HashMap<String, ChannelHandlerContext> sessions = new HashMap<String, ChannelHandlerContext>();

    public void addSession(String did, ChannelHandlerContext ctx) {
        synchronized (sessions) {
            sessions.put(did, ctx);
        }
    }

    public void removeSession(String did) {

        synchronized (sessions) {
            sessions.remove(did);
        }
    }

    public void removeSessionByChannel(ChannelHandlerContext ctx) {

        String did = queryDid(ctx);
        if (did != null) {
            removeSession(did);
        }
    }

    public ChannelHandlerContext querySession(String did) {
        ChannelHandlerContext ctx = null;

        synchronized (sessions) {
            ctx = sessions.get(did);
        }

        return ctx;
    }

    public String queryDid(ChannelHandlerContext ctx) {
        synchronized (sessions) {
            for (Entry<String, ChannelHandlerContext> entry : sessions
                    .entrySet()) {
                if (Objects.equals(ctx, entry.getValue())) {
                    return entry.getKey();
                }
            }
        }

        return null;
    }

    public List<String> getSessions() {
        synchronized (sessions) {
            List<String> list = new ArrayList<String>(sessions.keySet());
            return list;
        }
    }
}
