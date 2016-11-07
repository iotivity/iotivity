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
package org.iotivity.cloud.rdserver.resources.directory.rd;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.base.exception.ServerException.BadRequestException;
import org.iotivity.cloud.base.exception.ServerException.PreconditionFailedException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.base.resource.Resource;
import org.iotivity.cloud.rdserver.Constants;
import org.iotivity.cloud.rdserver.db.DBManager;
import org.iotivity.cloud.rdserver.resources.presence.ResPresenceManager;
import org.iotivity.cloud.rdserver.resources.presence.resource.ResPresencePayload;
import org.iotivity.cloud.rdserver.util.TypeCastingManager;
import org.iotivity.cloud.util.Cbor;
import org.iotivity.cloud.util.Log;

public class ResourceDirectoryResource extends Resource {

    private Cbor<HashMap<Object, Object>>    mCbor                    = new Cbor<>();
    private TypeCastingManager<PublishTags>  mPublishTagsTypeManager  = new TypeCastingManager<>();
    private TypeCastingManager<PublishLinks> mPublishLinksTypeManager = new TypeCastingManager<>();
    private InsManager                       mInsManager              = new InsManager();
    private String                           mNotiDeviceId            = null;
    private ArrayList<ResPresencePayload>    mNotiPayloadList         = new ArrayList<ResPresencePayload>();

    public ResourceDirectoryResource() {
        super(Arrays.asList(Constants.PREFIX_OIC, Constants.RD_URI));
    }

    @Override
    public void onDefaultRequestReceived(Device srcDevice, IRequest request)
            throws ServerException {

        IResponse response = null;

        switch (request.getMethod()) {
            case POST:
                response = handlePostRequest(request);
                break;

            case DELETE:
                response = handleDeleteRequest(request);
                break;

            default:
                throw new BadRequestException(request.getMethod()
                        + " request type is not supported");
        }

        srcDevice.sendResponse(response);

        ResPresenceManager.getInstance().notifyToObservers(mNotiDeviceId,
                mNotiPayloadList);

    }

    private IResponse handlePostRequest(IRequest request)
            throws ServerException {

        HashMap<String, List<String>> queryMap = request.getUriQueryMap();

        byte[] encodedPayload = null;

        if (queryMap == null) {
            throw new PreconditionFailedException("query is null");
        }

        List<String> listRT = queryMap.get(Constants.RESOURCE_TYPE);

        if (listRT == null) {
            throw new PreconditionFailedException("rt property is not include");
        } else if (listRT.get(0).equals(Constants.RESOURCE_TYPE_RDPUBLISH)) {

            PublishPayload pubPayload = parsingPublishPayload(request
                    .getPayload());

            mNotiDeviceId = pubPayload.getTags().getDi();

            // PublishPayload copyPubPayload = pubPayload.copy();

            // ArrayList<HashMap<Object, Object>> storeResList =
            // creatDBStoreResource(changeResourceUri(copyPubPayload));

            ArrayList<HashMap<Object, Object>> storeResList = creatDBStoreResource(pubPayload);

            mNotiPayloadList = DBManager.getInstance().registerResource(
                    storeResList);

            encodedPayload = createPublishResponse(pubPayload);

        } else {
            throw new PreconditionFailedException(
                    "rt property is not rd publish");
        }

        return MessageBuilder.createResponse(request, ResponseStatus.CHANGED,
                ContentFormat.APPLICATION_CBOR, encodedPayload);
    }

    private byte[] createPublishResponse(PublishPayload pubPayload) {
        HashMap<Object, Object> responseMap = new HashMap<Object, Object>();

        PublishTags tags = pubPayload.getTags();
        responseMap.putAll(mPublishTagsTypeManager.convertObjectToMap(tags));

        ArrayList<PublishLinks> pubLinksList = pubPayload.getLinks();

        ArrayList<HashMap<Object, Object>> links = new ArrayList<HashMap<Object, Object>>();

        for (PublishLinks pubLinks : pubLinksList) {
            mPublishLinksTypeManager.callMethod("changePType", pubLinks);
            links.add(mPublishLinksTypeManager.convertObjectToMap(pubLinks));
        }
        responseMap.put(Constants.LINKS, links);

        Log.i("publish response :" + responseMap.toString());

        byte[] encodedPaylod = mCbor.encodingPayloadToCbor(responseMap);

        return encodedPaylod;
    }

    private PublishPayload parsingPublishPayload(byte[] payload)
            throws ServerException {

        HashMap<Object, Object> payloadData = mCbor.parsePayloadFromCbor(
                payload, HashMap.class);

        if (payloadData == null) {
            throw new BadRequestException("payload is null");
        } else {
            Log.i("publish payload: " + payloadData.toString());
        }

        PublishTags tags = new PublishTags();
        tags = mPublishTagsTypeManager.convertMaptoObject(payloadData, tags);

        String di = tags.getDi();

        ArrayList<HashMap<Object, Object>> linksList = (ArrayList<HashMap<Object, Object>>) payloadData
                .get(Constants.LINKS);

        ArrayList<PublishLinks> pubLinksList = new ArrayList<PublishLinks>();

        for (HashMap<Object, Object> links : linksList) {
            PublishLinks pubLinks = new PublishLinks();
            pubLinks = mPublishLinksTypeManager.convertMaptoObject(links,
                    pubLinks);
            String href = pubLinks.getHref();
            // href = "/di/" + di + href;
            int ins = pubLinks.getIns();
            ins = checkResourceIns(di, href, ins);
            if (ins == 0) {
                throw new PreconditionFailedException("ins is null");
            }
            pubLinks.setIns(ins);
            pubLinksList.add(pubLinks);
        }

        PublishPayload pubPayload = new PublishPayload();
        pubPayload.setTags(tags);
        pubPayload.setLinks(pubLinksList);

        return pubPayload;
    }

    private PublishPayload changeResourceUri(PublishPayload pubPayload) {

        String di = pubPayload.getTags().getDi();

        for (PublishLinks links : pubPayload.getLinks()) {
            String originHref = links.getHref();
            links.setHref("/di/" + di + originHref);
        }

        return pubPayload;
    }

    private int checkResourceIns(String di, String href, int ins) {
        int storedIns = mInsManager.getIns(di, href);
        if (ins == 0) {
            if (storedIns == 0) {
                ins = mInsManager.createIns(di);
            } else {
                ins = storedIns;
            }
        } else {
            if (ins != storedIns) {
                ins = 0;
            }
        }
        return ins;
    }

    private ArrayList<HashMap<Object, Object>> creatDBStoreResource(
            PublishPayload pubPayload) {
        PublishTags tags = pubPayload.getTags();
        ArrayList<PublishLinks> linksList = pubPayload.getLinks();

        ArrayList<HashMap<Object, Object>> storeResList = new ArrayList<HashMap<Object, Object>>();
        HashMap<Object, Object> storeTags = mPublishTagsTypeManager
                .convertObjectToMap(tags);

        for (PublishLinks links : linksList) {
            HashMap<Object, Object> storeRes = new HashMap<Object, Object>();
            storeRes.putAll(storeTags);
            storeRes.putAll(mPublishLinksTypeManager.convertObjectToMap(links));
            storeResList.add(storeRes);
        }
        return storeResList;
    }

    private IResponse handleDeleteRequest(IRequest request)
            throws ServerException {

        HashMap<String, List<String>> queryMap = request.getUriQueryMap();
        List<String> diList = null;
        List<String> insList = null;

        if (queryMap == null) {
            throw new PreconditionFailedException("query is null");
        } else {
            diList = queryMap.get(Constants.DEVICE_ID);
            insList = queryMap.get(Constants.INS);

            if (diList == null) {
                throw new PreconditionFailedException(
                        "di property is not include");
            } else {
                String di = diList.get(0);
                mNotiDeviceId = di;

                if (insList == null) {
                    mNotiPayloadList = DBManager.getInstance()
                            .deleteResourceAboutDi(di);

                } else {
                    String ins = insList.get(0);
                    mNotiPayloadList = DBManager.getInstance()
                            .deleteResourceAboutDiAandIns(di, ins);
                }
            }
        }

        IResponse response = MessageBuilder.createResponse(request,
                ResponseStatus.DELETED);

        return response;
    }
}
