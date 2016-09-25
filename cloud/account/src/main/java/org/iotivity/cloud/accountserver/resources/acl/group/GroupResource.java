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
package org.iotivity.cloud.accountserver.resources.acl.group;

import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;

import org.iotivity.cloud.accountserver.Constants;
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
import org.iotivity.cloud.util.Cbor;

public class GroupResource extends Resource {
    private Cbor<HashMap<String, Object>> mCbor = new Cbor<>();

    public GroupResource() {
        super(Arrays.asList(Constants.PREFIX_OIC, Constants.ACL_URI,
                Constants.GROUP_URI));
    }

    @Override
    public void onDefaultRequestReceived(Device srcDevice, IRequest request)
            throws ServerException {

        IResponse response = null;

        if (request.getUriPathSegments().size() > getUriPathSegments().size()
                + 1) {
            throw new BadRequestException("uriPath is invalid");
        }

        switch (request.getMethod()) {
            case POST:
                response = handlePostRequest(request);
                break;
            case GET:
                response = handleGetRequest(srcDevice, request);
                break;
            case DELETE:
                response = handleDeleteRequest(request);
                break;
            default:
                throw new BadRequestException(
                        request.getMethod() + " request type is not support");
        }
        srcDevice.sendResponse(response);
    }

    private IResponse handlePostRequest(IRequest request)
            throws ServerException {
        HashMap<String, Object> payloadData = mCbor
                .parsePayloadFromCbor(request.getPayload(), HashMap.class);

        if (payloadData == null) {
            throw new BadRequestException("payload is null");
        }

        if (getUriPathSegments().containsAll(request.getUriPathSegments())) {
            String uuid = payloadData.get(Constants.REQ_GROUP_MASTER_ID)
                    .toString();
            String gtype = (String) payloadData.get(Constants.REQ_GROUP_TYPE)
                    .toString();
            if (uuid == null || gtype == null) {
                throw new PreconditionFailedException(
                        "value of group property is invalid");
            }
            return MessageBuilder.createResponse(request,
                    ResponseStatus.CHANGED, ContentFormat.APPLICATION_CBOR,
                    mCbor.encodingPayloadToCbor(GroupManager.getInstance()
                            .createGroup(uuid, gtype)));
        } else {
            String gid = request.getUriPathSegments()
                    .get(getUriPathSegments().size());

            if (payloadData.containsKey(Constants.REQ_MEMBER_LIST)) {
                List<String> midList = (List<String>) payloadData
                        .get(Constants.REQ_MEMBER_LIST);
                if (midList == null) {
                    throw new PreconditionFailedException(
                            "midList property is invalid");
                }
                GroupManager.getInstance().addGroupMember(gid,
                        new HashSet<String>(midList));
            }

            if (payloadData.containsKey(Constants.REQ_DEVICE_ID_LIST)) {
                List<String> diList = (List<String>) payloadData
                        .get(Constants.REQ_DEVICE_ID_LIST);
                if (diList == null) {
                    throw new PreconditionFailedException(
                            "diList property is invalid");
                }
                GroupManager.getInstance().addGroupDevice(gid,
                        new HashSet<String>(diList));
            }
        }
        return MessageBuilder.createResponse(request, ResponseStatus.CHANGED);

    }

    private IResponse handleGetRequest(Device srcDevice, IRequest request)
            throws ServerException {
        HashMap<String, Object> responsePayload = null;
        String mid = null;

        if (!request.getUriQueryMap().containsKey(Constants.REQ_MEMBER)) {
            throw new PreconditionFailedException("mid property is invalid");
        }

        mid = request.getUriQueryMap().get(Constants.REQ_MEMBER).get(0);

        if (getUriPathSegments().containsAll(request.getUriPathSegments())) {
            responsePayload = GroupManager.getInstance().getGroupList(mid);
        } else {
            String gid = request.getUriPathSegments()
                    .get(getUriPathSegments().size());
            switch (request.getObserve()) {
                case NOTHING:
                    responsePayload = GroupManager.getInstance()
                            .getGroupInfo(gid, mid);
                    break;
                case SUBSCRIBE:
                    responsePayload = GroupManager.getInstance()
                            .addGroupSubscriber(gid, mid, srcDevice, request);
                    break;
                case UNSUBSCRIBE:
                    responsePayload = GroupManager.getInstance()
                            .removeGroupSubscriber(gid, mid);
                    break;
                default:
                    throw new BadRequestException(request.getObserve()
                            + " observe type is not support");
            }
        }
        return MessageBuilder.createResponse(request, ResponseStatus.CONTENT,
                ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(responsePayload));
    }

    private IResponse handleDeleteRequest(IRequest request)
            throws ServerException {
        if (getUriPathSegments().containsAll(request.getUriPathSegments())) {

            checkQueryException(Arrays.asList(Constants.REQ_GROUP_MASTER_ID,
                    Constants.REQ_GROUP_ID), request.getUriQueryMap());

            String gmid = request.getUriQueryMap()
                    .get(Constants.REQ_GROUP_MASTER_ID).get(0);
            String gid = request.getUriQueryMap().get(Constants.REQ_GROUP_ID)
                    .get(0);

            GroupManager.getInstance().deleteGroup(gmid, gid);
        } else {
            String gid = request.getUriPathSegments()
                    .get(getUriPathSegments().size());
            if (request.getUriQueryMap()
                    .containsKey(Constants.REQ_MEMBER_LIST)) {
                List<String> midList = request.getUriQueryMap()
                        .get(Constants.REQ_MEMBER_LIST);
                if (midList == null) {
                    throw new PreconditionFailedException(
                            "midList property is invalid");
                }
                GroupManager.getInstance().removeGroupMember(gid,
                        new HashSet<String>(midList));
            }
            if (request.getUriQueryMap()
                    .containsKey(Constants.REQ_DEVICE_ID_LIST)) {
                List<String> diList = request.getUriQueryMap()
                        .get(Constants.REQ_DEVICE_ID_LIST);
                if (diList == null) {
                    throw new PreconditionFailedException(
                            "diList property is invalid");
                }
                GroupManager.getInstance().removeGroupDevice(gid,
                        new HashSet<String>(diList));
            }
        }
        return MessageBuilder.createResponse(request, ResponseStatus.DELETED);
    }
}
