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
package org.iotivity.cloud.rdserver.resources;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.StringTokenizer;

import org.iotivity.cloud.base.Resource;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.coap.enums.CoapOption;
import org.iotivity.cloud.base.protocols.coap.enums.CoapStatus;
import org.iotivity.cloud.rdserver.Constants;
import org.iotivity.cloud.rdserver.MongoDB;
import org.iotivity.cloud.util.Cbor;
import org.iotivity.cloud.util.JSONUtil;
import org.iotivity.cloud.util.Logger;

import io.netty.channel.ChannelHandlerContext;

/**
 *
 * This class provides a set of APIs relating Resource Directory
 *
 */
public class ResourceDirectoryResource extends Resource {

    private Cbor<ArrayList<Object>> cbor;
    private MongoDB                 mongoDB;

    public ResourceDirectoryResource() {
        setUri(Constants.RD_URI);
        cbor = new Cbor<ArrayList<Object>>();
        try {
            mongoDB = new MongoDB(Constants.RD_DB_NAME);
            mongoDB.createTable(Constants.RD_TABLE);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    @Override
    public void onRequestReceived(ChannelHandlerContext ctx,
            CoapRequest request) {

        Logger.d("ResourceDirectoryResource IN");

        if (ctx == null || request == null) {
            Logger.d("ctx or request msg is null");
        }

        else {
            switch (request.getRequestMethod()) {
                case GET:
                    Logger.d("Request message is GET message");
                    try {
                        handleGetRequest(ctx, request);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    break;

                case PUT:
                    Logger.d("Request message is PUT message");
                    break;

                case POST:
                    Logger.d("Request message is POST message");
                    try {
                        handlePostRequest(ctx, request);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    break;

                case DELETE:
                    Logger.d("Request message is DELETE message");
                    try {
                        handleDeleteRequest(ctx, request);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    break;
            }
        }
    }

    private CoapResponse makePublishResponse(CoapRequest request)
            throws Exception {
        CoapResponse response = new CoapResponse(CoapStatus.CREATED);
        response.setToken(request.getToken());
        return response;
    }

    private CoapResponse makeDeleteResponse(CoapRequest request)
            throws Exception {
        CoapResponse response = new CoapResponse(CoapStatus.DELETED);
        response.setToken(request.getToken());
        return response;
    }

    private CoapResponse makeDiscoveryResponse(CoapRequest request,
            ArrayList<PublishPayloadFormat> foundResource) throws Exception {

        CoapResponse response = new CoapResponse(CoapStatus.CONTENT);
        response.setToken(request.getToken());

        // make payload
        ArrayList<HashMap<Object, Object>> discoverPayload = new ArrayList<HashMap<Object, Object>>();

        for (PublishPayloadFormat pubPayload : foundResource) {

            LinksPayloadFormat links = pubPayload.links.get(0);
            LinkedHashMap<Object, Object> discoverLinks = new LinkedHashMap<Object, Object>();
            discoverLinks.put(Constants.RS_HREF, links.getHref());
            discoverLinks.put(Constants.RS_RESOURCE_TYPE, links.getRt().get(0));
            discoverLinks.put(Constants.RS_INTERFACE, links.getItf().get(0));
            HashMap<Object, Object> pres = new HashMap<Object, Object>();
            pres.put(Constants.RS_BITMAP, pubPayload.getBitmap());
            discoverLinks.put("p", pres);

            ArrayList<Object> linksArray = null;
            for (HashMap<Object, Object> segmentPayload : discoverPayload) {
                // exist di
                if (segmentPayload.get(Constants.RS_DEVICE_ID)
                        .equals(pubPayload.getDi())) {
                    linksArray = new ArrayList<Object>();
                    linksArray = (ArrayList<Object>) segmentPayload
                            .get("links");
                    linksArray.add(discoverLinks);
                    segmentPayload.put("links", linksArray);
                }
            }
            if (linksArray == null) {
                HashMap<Object, Object> discoverRes = new HashMap<Object, Object>();
                linksArray = new ArrayList<Object>();
                discoverRes.put(Constants.RS_DEVICE_ID, pubPayload.getDi());
                linksArray.add(discoverLinks);
                discoverRes.put("links", linksArray);
                discoverPayload.add(discoverRes);
            }
        }

        // TODO : device id is decoded to byte in IoTivity. So, temporarily we
        // cast the type of device id to byte.
        for (HashMap<Object, Object> segmentPayload : discoverPayload) {
            String stringDi = segmentPayload.get(Constants.RS_DEVICE_ID)
                    .toString();
            segmentPayload.put(Constants.RS_DEVICE_ID,
                    stringDi.getBytes(StandardCharsets.UTF_8));
        }

        Logger.i("discoverPayload :" + discoverPayload.toString());

        byte[] bytes = ByteBuffer.allocate(4).putInt(60).array();
        response.addOption(CoapOption.CONTENT_FORMAT.getvalue(), bytes);

        byte[] encodedPaylod = cbor.encodingPayloadToCbor(discoverPayload);
        response.setPayload(encodedPaylod);

        return response;

    }

    private HashMap<String, String> extractFiltersFromQuery(List<String> query)
            throws Exception {

        if (query == null) {
            throw new IllegalArgumentException("query is null!");
        }

        HashMap<String, String> filters = new HashMap<String, String>();
        for (String queryPara : query) {
            StringTokenizer keyValuePair = new StringTokenizer(queryPara, "=");
            if (keyValuePair.countTokens() < 2) {
                // query error
                filters = null;
            } else {
                String key = keyValuePair.nextToken();
                String value = keyValuePair.nextToken();
                if (key.equals(Constants.RS_INTERFACE)) {
                    filters.put(Constants.RS_INTERFACE, value);
                } else if (key.equals(Constants.RS_RESOURCE_TYPE)) {
                    filters.put(Constants.RS_RESOURCE_TYPE, value);
                } else if (key.equals(Constants.RS_DEVICE_ID)) {
                    filters.put(Constants.RS_DEVICE_ID, value);
                } else if (key.equals(Constants.RS_INS)) {
                    filters.put(Constants.RS_INS, value);
                } else if (key.equals(Constants.RS_SEARCH_TYPE)) {
                    filters.put(Constants.RS_SEARCH_TYPE, value);
                }
            }
        }

        return filters;
    }

    /**
     * API for handling GET message(message to discovery resources)
     *
     * @param ctx
     *            ChannelHandlerContext of request message
     * @param request
     *            CoAP request message
     * @throws Exception
     */
    public void handleGetRequest(ChannelHandlerContext ctx, CoapRequest request)
            throws Exception {
        HashMap<String, String> filters = extractFiltersFromQuery(
                request.getUriQuerySegments());

        ArrayList<PublishPayloadFormat> foundResource = null;

        if (filters == null) {
            throw new IllegalArgumentException("filters is null");
        } else if (filters.get(Constants.RS_SEARCH_TYPE) == null) {
            Logger.d("st is null, so this is the get msg about public devices");
            if (filters.get(Constants.RS_INTERFACE) != null) {
                foundResource = mongoDB.readResource(Constants.RS_INTERFACE,
                        filters.get(Constants.RS_INTERFACE),
                        Constants.RD_TABLE);
            } else if (filters.get(Constants.RS_RESOURCE_TYPE) != null) {
                foundResource = mongoDB.readResource(Constants.RS_RESOURCE_TYPE,
                        filters.get(Constants.RS_RESOURCE_TYPE),
                        Constants.RD_TABLE);
            } else {
                throw new IllegalArgumentException("rt & if is null");
            }

            CoapResponse response = makeDiscoveryResponse(request,
                    foundResource);
            ctx.writeAndFlush(response);

        } else {
            if (filters.get(Constants.RS_SEARCH_TYPE)
                    .equals(Constants.RS_SEARCH_TYPE_DEVICE_LIST)) {
                Logger.d(
                        "st is not null, so this is the get msg about private devices");
                // parse payload
                byte[] payload = request.getPayload();
                ArrayList<String> deviceList = JSONUtil.parseJSON(payload,
                        Constants.RS_DEVICE_LIST_KEY);
                if (deviceList == null) {
                    throw new IllegalArgumentException("deviceList is null");
                }

                if (filters.get(Constants.RS_INTERFACE) != null) {
                    foundResource = new ArrayList<PublishPayloadFormat>();
                    for (String deviceId : deviceList) {
                        foundResource.addAll(mongoDB.readResourceAboutDid(
                                deviceId, Constants.RS_INTERFACE,
                                filters.get(Constants.RS_INTERFACE),
                                Constants.RD_TABLE));
                    }
                } else if (filters.get(Constants.RS_RESOURCE_TYPE) != null) {
                    foundResource = new ArrayList<PublishPayloadFormat>();
                    for (String deviceId : deviceList) {
                        foundResource.addAll(mongoDB.readResourceAboutDid(
                                deviceId, Constants.RS_RESOURCE_TYPE,
                                filters.get(Constants.RS_RESOURCE_TYPE),
                                Constants.RD_TABLE));
                    }
                } else {
                    throw new IllegalArgumentException("rt & if is null");
                }

                CoapResponse response = makeDiscoveryResponse(request,
                        foundResource);
                ctx.writeAndFlush(response);
            } else {
                throw new IllegalArgumentException("value of st is not corret");
            }
        }
    }

    /**
     * API for handling POST message(message to publish or update resources)
     *
     * @param ctx
     *            ChannelHandlerContext of request message
     * @param request
     *            CoAP request message
     * @throws Exception
     */
    public void handlePostRequest(ChannelHandlerContext ctx,
            CoapRequest request) throws Exception {

        HashMap<String, String> filters = extractFiltersFromQuery(
                request.getUriQuerySegments());

        if (filters == null) {
            throw new IllegalArgumentException("filters is null");
        } else if (filters.get(Constants.RS_RESOURCE_TYPE) == null) {
            throw new IllegalArgumentException("rt is null");
        } else if (filters.get(Constants.RS_RESOURCE_TYPE)
                .equals(Constants.RS_RESOURCE_TYPE_RDPUBLISH)) {
            Logger.d("This request is publish msg!");

            PublishPayloadFormat pubPayload = new PublishPayloadFormat();

            ArrayList<Object> payloadData = cbor.parsePayloadFromCbor(
                    request.getPayload(), ArrayList.class);

            if (payloadData == null) {
                throw new IllegalArgumentException("parsed payload is null");
            } else {
                Logger.i("payloadData: " + payloadData.toString());
            }

            HashMap<Object, Object> tags = (HashMap<Object, Object>) payloadData
                    .get(0);

            if (tags == null) {
                throw new IllegalArgumentException("tags is null!");
            }

            Object di = tags.get(Constants.RS_DEVICE_ID);
            if (di != null) {
                pubPayload.setDi(di.toString());
                Logger.i("di : " + pubPayload.getDi());
            } else {
                throw new IllegalArgumentException("device id is null!");
            }

            Object deviceName = tags.get(Constants.RS_DEVICE_NAME);
            if (deviceName != null) {
                pubPayload.setDeviceName(deviceName.toString());
                Logger.i("device name : " + pubPayload.getDeviceName());
            }

            Object baseUri = tags.get(Constants.RS_BASE_URI);
            if (baseUri != null) {
                pubPayload.setBaseUri(baseUri.toString());
                Logger.i("baseURI : " + pubPayload.getBaseUri());
            }

            Object bitMap = tags.get(Constants.RS_BITMAP);
            if (bitMap != null) {
                pubPayload.setBitmap((int) bitMap);
                Logger.i("bm : " + pubPayload.getBitmap());
            }

            Object hostingPort = tags.get(Constants.RS_HOSTING_PORT);
            if (hostingPort != null) {
                pubPayload.setPort((int) hostingPort);
                Logger.i("port : " + pubPayload.getPort());
            }

            Object ins = tags.get(Constants.RS_INS);
            if (ins != null) {
                pubPayload.setIns((int) ins);
                Logger.i("ins : " + pubPayload.getIns());
            }

            Object rts = tags.get(Constants.RS_RTS);
            if (rts != null) {
                pubPayload.setRts(rts.toString());
                Logger.i("rts : " + pubPayload.getRts());
            }

            Object drel = tags.get(Constants.RS_DREL);
            if (drel != null) {
                pubPayload.setDrel(drel.toString());
                Logger.i("drel : " + pubPayload.getDrel());
            }

            // Object ttl = tags.get(Constants.RS_TTL);
            // if (ttl != null) {
            // pubPayload.setTtl((int) ttl);
            // Logger.i("ttl : " + pubPayload.getTtl());
            // }

            ArrayList<LinkedHashMap<Object, Object>> publishLinks = (ArrayList<LinkedHashMap<Object, Object>>) payloadData
                    .get(1);

            if (publishLinks == null) {
                throw new IllegalArgumentException("publishLinks is null!");
            }

            for (LinkedHashMap<Object, Object> o : publishLinks) {

                LinksPayloadFormat storeLinks = new LinksPayloadFormat();

                Object href = o.get(Constants.RS_HREF);
                if (href != null) {
                    String prefix = "/" + pubPayload.getDi();
                    storeLinks.setHref(prefix + href.toString());
                    Logger.i("href : " + storeLinks.getHref());
                }

                if (o.get(Constants.RS_RESOURCE_TYPE) != null) {
                    Object obj = o.get(Constants.RS_RESOURCE_TYPE);
                    if (obj != null) {
                        storeLinks.setRt((ArrayList<String>) obj);
                    }
                    Object rt = storeLinks.getRt();
                    if (rt != null) {
                        Logger.i("rt : " + storeLinks.getRt().toString());
                    }
                }

                if (o.get(Constants.RS_INTERFACE) != null) {
                    storeLinks.setItf(
                            (ArrayList<String>) o.get(Constants.RS_INTERFACE));
                    Object itf = storeLinks.getItf();
                    if (itf != null) {
                        Logger.i("if : " + storeLinks.getItf().toString());
                    }
                }

                Object rel = o.get(Constants.RS_REL);
                if (rel != null) {
                    storeLinks.setRel(rel.toString());
                    Logger.i("rel : " + storeLinks.getRel());
                }

                if (o.get(Constants.RS_OBS) != null) {
                    Object obj = o.get(Constants.RS_OBS);
                    if (obj != null) {
                        storeLinks.setObs((boolean) obj);
                    }
                    Logger.i("obs : " + storeLinks.isObs());
                }

                if (o.get(Constants.RS_TITLE) != null) {
                    Object obj = o.get(Constants.RS_TITLE);
                    if (obj != null) {
                        storeLinks.setTitle(obj.toString());
                    }
                    Logger.i("title : " + storeLinks.getTitle());
                }

                if (o.get(Constants.RS_URI) != null) {
                    Object obj = o.get(Constants.RS_URI);
                    if (obj != null) {
                        storeLinks.setUri(obj.toString());
                    }
                    Logger.i("uri : " + storeLinks.getUri());
                }

                if (o.get(Constants.RS_INS) != null) {
                    Object obj = o.get(Constants.RS_INS);
                    if (obj != null) {
                        storeLinks.setIns((int) obj);
                    }
                    Logger.i("ins : " + storeLinks.getIns());
                }

                if (o.get(Constants.RS_MEDIA_TYPE) != null) {
                    Object obj = o.get(Constants.RS_MEDIA_TYPE);
                    if (obj != null) {
                        storeLinks.setMt((ArrayList<String>) obj);
                    }
                    Object mt = storeLinks.getMt();
                    if (mt != null) {
                        Logger.i("mt : " + mt.toString());
                    }
                }

                pubPayload.links.add(storeLinks);
            }

            mongoDB.createResource(pubPayload, Constants.RD_TABLE);

            CoapResponse response = makePublishResponse(request);
            ctx.writeAndFlush(response);

        } else {
            throw new IllegalArgumentException("rt is not correct");
        }
    }

    /**
     * API for handling Delete message(message to delete published resources)
     *
     * @param ctx
     *            ChannelHandlerContext of request message
     * @param request
     *            CoAP request message
     * @throws Exception
     */
    public void handleDeleteRequest(ChannelHandlerContext ctx,
            CoapRequest request) throws Exception {
        HashMap<String, String> filters = extractFiltersFromQuery(
                request.getUriQuerySegments());

        if (filters == null) {
            throw new IllegalArgumentException("filters is null");
        } else if (filters.get(Constants.RS_DEVICE_ID) == null) {
            throw new IllegalArgumentException("di is null");
        } else {
            if (filters.get(Constants.RS_INS) == null) {
                mongoDB.deleteResourceAboutDid(
                        filters.get(Constants.RS_DEVICE_ID),
                        Constants.RD_TABLE);
            } else {
                mongoDB.deleteResourceAboutDidAndIns(
                        filters.get(Constants.RS_DEVICE_ID),
                        filters.get(Constants.RS_INS), Constants.RD_TABLE);
            }
            CoapResponse response = makeDeleteResponse(request);
            ctx.writeAndFlush(response);
        }
    }
}
