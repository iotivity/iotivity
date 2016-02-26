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
import java.util.Iterator;
import java.util.List;

import org.iotivity.cloud.util.Logger;

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
        synchronized (sessions) {
            if (!isThereCtxChannel(ctx)) {
                Logger.d("Already Session Removed : "
                        + ctx.channel().toString());
                return;
            }
            Iterator<String> iterator = sessions.keySet().iterator();
            while (iterator.hasNext()) {
                String key = (String) iterator.next();
                if (ctx.channel().toString()
                        .equals(querySession(key).channel().toString())) {
                    Logger.d("Session Remove : " + ctx.channel().toString());
                    removeSession(key);
                    break;
                }
            }
        }
    }

    public ChannelHandlerContext querySession(String did) {
        ChannelHandlerContext ctx = null;

        synchronized (sessions) {
            ctx = sessions.get(did);
        }

        return ctx;
    }

    public boolean isThereCtx(ChannelHandlerContext ctx) {

        synchronized (sessions) {
            return sessions.containsValue(ctx);
        }
    }

    public boolean isThereCtxChannel(ChannelHandlerContext ctx) {

        synchronized (sessions) {
            Iterator<String> iterator = sessions.keySet().iterator();
            while (iterator.hasNext()) {
                String key = (String) iterator.next();
                if (ctx.channel().toString()
                        .equals(querySession(key).channel().toString())) {
                    return true;
                }
            }
            return false;
        }
    }

    public List<String> getSessions() {
        return new ArrayList<String>(sessions.keySet());
    }
}
