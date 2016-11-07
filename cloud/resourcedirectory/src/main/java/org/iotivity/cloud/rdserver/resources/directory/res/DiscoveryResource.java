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
package org.iotivity.cloud.rdserver.resources.directory.res;

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
import org.iotivity.cloud.rdserver.util.TypeCastingManager;
import org.iotivity.cloud.util.Cbor;
import org.iotivity.cloud.util.Log;

public class DiscoveryResource extends Resource {
    private Cbor<HashMap<Object, Object>>      mCbor                     = new Cbor<>();
    private TypeCastingManager<DiscoveryTags>  mDiscoveryTagsTypeManager = new TypeCastingManager<>();
    private TypeCastingManager<DiscoveryLinks> mDiscoveryLinkTypeManager = new TypeCastingManager<>();

    public DiscoveryResource() {
        super(Arrays.asList(Constants.PREFIX_OIC, Constants.WELL_KNOWN_URI));
    }

    @Override
    public void onDefaultRequestReceived(Device srcDevice, IRequest request)
            throws ServerException {

        IResponse response = null;

        switch (request.getMethod()) {
            case GET:
                response = handleGetRequest(request);
                break;

            default:
                throw new BadRequestException(
                        request.getMethod() + " request type is not supported");
        }

        srcDevice.sendResponse(response);
    }

    private IResponse handleGetRequest(IRequest request)
            throws ServerException {

        HashMap<String, List<String>> queryMap = request.getUriQueryMap();

        ArrayList<DiscoveryPayload> resourceList = new ArrayList<DiscoveryPayload>();

        if (queryMap == null) {
            throw new PreconditionFailedException("query is null");
        }

        List<String> deviceList = queryMap.get(Constants.DEVICE_ID);

        if (deviceList == null) {
            throw new PreconditionFailedException(
                    "di property is not included");
        }

        List<String> listRT = queryMap.get(Constants.RESOURCE_TYPE);
        List<String> listITF = queryMap.get(Constants.INTERFACE);
        String key = null, value = null;
        ArrayList<HashMap<Object, Object>> foundResList = null;

        // TODO: Multiple RT or ITF support required
        if (listRT != null) {
            key = Constants.RESOURCE_TYPE;
            value = listRT.get(0);
        } else if (listITF != null) {
            key = Constants.INTERFACE;
            value = listITF.get(0);
        }

        for (String deviceId : deviceList) {
            if(key != null && value != null){
                foundResList = DBManager.getInstance().findResourceAboutDiAndFilter(deviceId,
                        key, value);
            } else {
                foundResList = DBManager.getInstance().findResourceAboutDi(deviceId);
            }

            if (foundResList != null) {
                resourceList.add(makeDiscoveryPayloadSegment(foundResList));
            }
        }

        return MessageBuilder.createResponse(request, ResponseStatus.CONTENT,
                ContentFormat.APPLICATION_CBOR,
                createDiscoveryResponse(resourceList));
    }

    private DiscoveryPayload makeDiscoveryPayloadSegment(
            ArrayList<HashMap<Object, Object>> foundResList) {

        ArrayList<DiscoveryLinks> discoveryLinksList = new ArrayList<DiscoveryLinks>();

        for (HashMap<Object, Object> res : foundResList) {
            DiscoveryLinks discoveryLinksPayload = new DiscoveryLinks();
            discoveryLinksPayload = mDiscoveryLinkTypeManager
                    .convertMaptoObject(res, discoveryLinksPayload);
            discoveryLinksList.add(discoveryLinksPayload);
        }

        DiscoveryPayload discoveryPayload = new DiscoveryPayload();

        DiscoveryTags tagsPayload = new DiscoveryTags();

        tagsPayload = mDiscoveryTagsTypeManager
                .convertMaptoObject(foundResList.get(0), tagsPayload);

        discoveryPayload.setTags(tagsPayload);
        discoveryPayload.setLinks(discoveryLinksList);

        return discoveryPayload;
    }

    private byte[] createDiscoveryResponse(
            ArrayList<DiscoveryPayload> discoveryPayloadList) {
        ArrayList<HashMap<Object, Object>> responseMapList = new ArrayList<HashMap<Object, Object>>();

        for (DiscoveryPayload discoveryPayload : discoveryPayloadList) {

            DiscoveryTags tags = discoveryPayload.getTags();

            HashMap<Object, Object> responseSegment = mDiscoveryTagsTypeManager
                    .convertObjectToMap(tags);

            ArrayList<DiscoveryLinks> discoveryLinksList = discoveryPayload
                    .getLinks();

            ArrayList<HashMap<Object, Object>> links = new ArrayList<HashMap<Object, Object>>();

            for (DiscoveryLinks discoveryLinks : discoveryLinksList) {
                links.add(mDiscoveryLinkTypeManager
                        .convertObjectToMap(discoveryLinks));
            }
            responseSegment.put(Constants.LINKS, links);

            responseMapList.add(responseSegment);
        }

        Log.i("discover payload :" + responseMapList.toString());

        byte[] encodedPaylod = mCbor.encodingPayloadToCbor(responseMapList);

        return encodedPaylod;
    }
}
