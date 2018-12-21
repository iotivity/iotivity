package org.iotivity.cloud.base.resource;

import org.iotivity.cloud.base.OICConstants;
import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.base.healthcheck.HealthHolder;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;

import java.util.Arrays;

public class CloudPingResource extends Resource {

    private final HealthHolder healthHolder;

    public CloudPingResource() {
        super(Arrays.asList(OICConstants.PREFIX_OIC, OICConstants.KEEP_ALIVE_URI));
        this.healthHolder = null;
    }

    public CloudPingResource(final HealthHolder healthHolder) {
        super(Arrays.asList(OICConstants.PREFIX_OIC, OICConstants.KEEP_ALIVE_URI));
        this.healthHolder = healthHolder;
    }

    @Override
    public void onDefaultRequestReceived(Device srcDevice, IRequest request)
            throws ServerException {

        IResponse response;
        switch (request.getMethod()) {
            case PUT:
                response = MessageBuilder.createResponse(request, ResponseStatus.VALID);
                if(healthHolder != null){
                    healthHolder.pingAccepted();
                }
                break;

            default:
                throw new ServerException.BadRequestException(
                        request.getMethod() + " request type is not support");
        }
        srcDevice.sendResponse(response);
    }
}
