package org.iotivity.cloud.accountserver.resources.acl.id;

import java.util.Arrays;
import java.util.HashMap;
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

public class AclResource extends Resource {

    private Cbor<HashMap<String, Object>> mCbor       = new Cbor<>();
    private static AclManager             mAclManager = new AclManager();

    public AclResource() {
        super(Arrays.asList(Constants.PREFIX_OIC, Constants.ACL_URI,
                Constants.ID_URI));
    }

    public static AclManager getInstance() {
        return mAclManager;
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
            case PUT:
                response = handlePutRequest(request);
                break;
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

    private IResponse handlePutRequest(IRequest request)
            throws ServerException {

        if (getUriPathSegments().containsAll(request.getUriPathSegments())) {
            String oid = request.getUriQueryMap().get(Constants.REQ_OWNER_ID)
                    .get(0);
            String di = request.getUriQueryMap().get(Constants.REQ_DEVICE_ID)
                    .get(0);
            if (mAclManager.getAclid(di) == null) {
                return MessageBuilder.createResponse(request,
                        ResponseStatus.CREATED, ContentFormat.APPLICATION_CBOR,
                        mCbor.encodingPayloadToCbor(
                                mAclManager.createAcl(oid, di)));
            } else {
                throw new BadRequestException(
                        "aclid already exists for the given di");
            }
        }

        throw new BadRequestException("uriPath is invalid");
    }

    private IResponse handlePostRequest(IRequest request)
        throws ServerException {

            HashMap<String, Object> payloadData = mCbor
                .parsePayloadFromCbor(request.getPayload(), HashMap.class);

            if (null !=request.getUriQueryMap()) {
                String aclid = request.getUriPathSegments().get(getUriPathSegments().size());
                String aceid = request.getUriQueryMap().get(Constants.REQ_ACE_ID).get(0);
                List<HashMap<String, Object>> aclist = (List<HashMap<String, Object>>) payloadData
                    .get(Constants.REQ_ACL_LIST);
                mAclManager.updateACE(aclid, aceid, aclist.get(0));
                return MessageBuilder.createResponse(request, ResponseStatus.CHANGED);
            }
            else if (!getUriPathSegments().containsAll(request.getUriPathSegments())) {
                String aclid = request.getUriPathSegments().get(getUriPathSegments().size());

                List<HashMap<String, Object>> aclist = null;
                if (!payloadData.containsKey(Constants.REQ_ACL_LIST)) {
                    throw new BadRequestException("aclist not included in payload");
                }
                aclist = (List<HashMap<String, Object>>) payloadData
                    .get(Constants.REQ_ACL_LIST);
                aclist= mAclManager.addAclACE(aclid, aclist);
                payloadData.put(Constants.REQ_ACL_LIST, aclist);
                return MessageBuilder.createResponse(request, ResponseStatus.CHANGED,
                        ContentFormat.APPLICATION_CBOR,
                        mCbor.encodingPayloadToCbor(payloadData));
            }
            throw new BadRequestException("uriPath is invalid");
        }


    @SuppressWarnings("unchecked")

    private IResponse handleGetRequest(Device srcDevice, IRequest request)
            throws ServerException {

        HashMap<String, Object> responsePayload = null;
        String di = null;

        if (getUriPathSegments().containsAll(request.getUriPathSegments())) {
            di = request.getUriQueryMap().get(Constants.REQ_DEVICE_ID).get(0);
            if (di == null) {
                throw new PreconditionFailedException("di is invalid");
            }
            responsePayload = mAclManager.getAclid(di);
        } else {
            String aclid = request.getUriPathSegments()
                    .get(getUriPathSegments().size());
            switch (request.getObserve()) {
                case NOTHING:
                    responsePayload = mAclManager.getAclInfo(aclid);
                    break;
                case SUBSCRIBE:
                    di = request.getUriQueryMap().get(Constants.REQ_DEVICE_ID)
                            .get(0);
                    responsePayload = mAclManager.addAclSubscriber(aclid, di,
                            srcDevice, request);
                    break;
                case UNSUBSCRIBE:
                    di = request.getUriQueryMap().get(Constants.REQ_DEVICE_ID)
                            .get(0);
                    responsePayload = mAclManager.removeAclSubscriber(aclid,
                            di);
                    break;
                default:
                    throw new BadRequestException(request.getObserve()
                            + " observe type is not supported");
            }

        }

        return MessageBuilder.createResponse(request, ResponseStatus.CONTENT,
                ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(responsePayload));

    }

    private IResponse handleDeleteRequest(IRequest request)
            throws ServerException {

        String aclid = null;

        if (getUriPathSegments().containsAll(request.getUriPathSegments())) {
            aclid = request.getUriQueryMap().get(Constants.REQ_ACL_ID).get(0);
            if (aclid == null) {
                throw new PreconditionFailedException("aclid is invalid");
            }
            mAclManager.deleteAcl(aclid);
        } else {
            aclid = request.getUriPathSegments()
                    .get(getUriPathSegments().size());

            if (request.getUriQueryMap() == null)
            {
                mAclManager.deleteAclAclist(aclid);
            }
            else if (request.getUriQueryMap()
                    .containsKey(Constants.REQ_ACE_ID)) {
                String aceid = request.getUriQueryMap().get(Constants.REQ_ACE_ID)
                        .get(0);
                mAclManager.deleteAclACE(aclid, aceid);
            }
            else {
                throw new BadRequestException("uriPath is invalid");
            }
        }

        return MessageBuilder.createResponse(request, ResponseStatus.DELETED);
    }

}
