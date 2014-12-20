//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package gen.server.handlers;

import OC.Cm.Connector.Server.ServerSession;
import OC.Cm.Serialization.ISerializable;
import OC.Cm.Server.ResourceHandler.SyncResourceHandler;
import gen.xsd.Elements;

/**
 * This class defines methods to handle requests received corresponding to a
 * InformationResourceHandler class.
 * 
 * @since 1.0.0
 */
public class InformationResourceHandler extends SyncResourceHandler {
    /**
     * Default constructor of InformationResourceHandler class.
     * 
     * @since 1.0.0
     */
    public InformationResourceHandler() {
    }

    /******************************** NOTE : Please DO NOT modify below code ********************************/
    /**
     * This method will be invoked to distinguish the type of http request. If
     * required, validation also will be performed.
     * 
     * @param session
     *            Server session to which the request belongs
     * @param methodId
     *            Http method type that needs to be executed (GET, PUT, POST,
     *            DELETE)
     * @param request
     *            Serializable request object
     * @param response
     *            Serializable response object
     * 
     * @return statusCode Http status code to be returned to the client
     * @since 1.0.0
     */
    public int handleSyncResource(ServerSession session, String methodId,
            ISerializable request, ISerializable response) {
        int statusCode = 400;
        String allowed_methods = "GET";

        if (-1 != allowed_methods.indexOf(methodId)) {
            if (methodId.equalsIgnoreCase("GET")) {
                if (false == validateRepresentation(response,
                        Elements.EN_INFORMATION)) {
                    return 404;
                }
                statusCode = this
                        .onGET(session, (gen.xsd.Information) response);

            } else if (methodId.equalsIgnoreCase("PUT")) {
                statusCode = this.onPUT(session);

            } else if (methodId.equalsIgnoreCase("POST")) {
                statusCode = this.onPOST(session);

            } else if (methodId.equalsIgnoreCase("DELETE")) {
                statusCode = this.onDELETE(session);

            }
        } else {
            statusCode = 405;
            session.setResponseHeader("Allow", allowed_methods);
        }

        return statusCode;
    }

    /******************************** NOTE : Please DO NOT modify below code ********************************/
    /**
     * Validates the representation by matching element name. This method
     * determines whether the serializable object represented by a data argument
     * is of type specified by an argument elementName.
     * 
     * @param data
     *            Serializable object
     * @param elementName
     *            Name of the element
     * 
     * @return (True) If serializable object is matched with the given element
     *         name<br>
     *         (False) If serializable object is not matched with the given
     *         element name
     * @since 1.0.0
     */
    public boolean validateRepresentation(ISerializable data, String elementName) {
        if (null == data)
            return false;

        String element = data.getElementName();

        return elementName.equals(element);
    }

    /**
     * This method will be invoked to handle a GET request.
     * 
     * @param session
     *            Server session to which the request belongs
     * @param respData
     *            Information object to be returned to the server through the
     *            serializer. This object should not be re-initialized by the
     *            user.
     * 
     * @return Http status code to be returned to the client
     */
    int onGET(OC.Cm.Connector.Server.ServerSession session,
            gen.xsd.Information respData) {
        // Auto-generated code. Add resource implementation here.

        // TODO: Define http status code to be returned to the client
        return 501;
    }

    /**
     * This method will be invoked to handle a PUT request.
     * 
     * @param session
     *            Server session to which the request belongs
     * 
     * @return Http status code to be returned to the client
     */
    int onPUT(OC.Cm.Connector.Server.ServerSession session) {
        // Auto-generated code. Add resource implementation here.

        // TODO: Define http status code to be returned to the client
        return 501;
    }

    /**
     * This method will be invoked to handle a POST request.
     * 
     * @param session
     *            Server session to which the request belongs
     * 
     * @return Http status code to be returned to the client
     */
    int onPOST(OC.Cm.Connector.Server.ServerSession session) {
        // Auto-generated code. Add resource implementation here.

        // TODO: Define http status code to be returned to the client
        return 501;
    }

    /**
     * This method will be invoked to handle a DELETE request.
     * 
     * @param session
     *            Server session to which the request belongs
     * 
     * @return Http status code to be returned to the client
     */
    int onDELETE(OC.Cm.Connector.Server.ServerSession session) {
        // Auto-generated code. Add resource implementation here.

        // TODO: Define http status code to be returned to the client
        return 501;
    }
}
