//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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


/**
 * @file
 *
 * This file contains the definition, types and interfaces for server request
 *
 */

#ifndef OC_SERVER_REQUEST_H
#define OC_SERVER_REQUEST_H

/**
 * Handler function for sending a response from a single resource
 *
 * @param ehResponse   Pointer to the response from the resource.
 *
 * @return
 *     ::OCStackResult
 */
OCStackResult HandleSingleResponse(OCEntityHandlerResponse * ehResponse);

/**
 * Handler function for sending a response from multiple resources, such as a collection.
 * Aggregates responses from multiple resource until all responses are received then sends the
 * concatenated response
 *
 * TODO: Need to add a timeout in case a (remote?) resource does not respond
 *
 * @param ehResponse      Pointer to the response from the resource.
 *
 * @return
 *     ::OCStackResult
 */
OCStackResult HandleAggregateResponse(OCEntityHandlerResponse * ehResponse);

/**
 * Get a server request from the server request list using the specified token.
 *
 * @param token            Token of server request.
 * @param tokenLength      Length of token.
 *
 * @return
 *     OCServerRequest*
 */
OCRequestInfo_t *GetServerRequestUsingToken(const OCToken_t *token);

/**
 * Get a server request from the server request list using the specified handle
 *
 * @param handle    Handle of server request.
 * @return
 *     OCServerRequest*
 */
OCRequestInfo_t *GetServerRequestUsingHandle(const OCRequestInfo_t *handle);

/**
 * Get a server response from the server response list using the specified handle
 *
 * @param handle    handle of server response.
 *
 * @return
 *     OCServerResponse*
 */
OCServerResponse *GetServerResponseUsingHandle(const OCRequestInfo_t * handle);

/**
 * Fill critical values in a server request
 *
 * @param request                               Initialized server request that is created by this function.
 * @param method                                RESTful method.
 * @param observationOption                     Value of observation option.
 * @param qos                                   Request QOS.
 * @param query                                 Request query.
 * @param requestToken                          Request token.
 * @param resourceUrl                           URL of resource.
 * @param devAddr                               Device Address.
 *
 * @return
 *     ::OCStackResult
 */
void FillServerRequest(OCRequestInfo_t *request,
                       OCMethod method,
                       uint32_t observationOption,
                       OCQualityOfService qos,
                       char *query,
                       OCToken_t requestToken,
                       char *resourceUrl,
                       const OCDevAddr *devAddr);

void AddServerRequest(OCRequestInfo_t *request);

/**
 * Form the OCEntityHandlerRequest struct that is passed to a resource's entity handler
 *
 * @param entityHandlerRequest      pointer to the OCEntityHandlerRequest struct that is created.
 * @param request                   Request handle.
 * @param method                    RESTful method.
 * @param resource                  Resource handle.
 * @param queryBuf                  Resource query of request.
 * @param bufReqPayload             JSON payload of request.
 * @param numVendorOptions          Number of vendor options.
 * @param vendorOptions             Vendor options.
 * @param observeAction             Observe action flag.
 * @param observeID                 Observe ID.
 *
 * @return
 *     OCStackResult
 */

OCStackResult FormOCEntityHandlerRequest(OCEntityHandlerRequest *ehr,
                                         OCRequestInfo_t *req,
                                         OCResource *resource);

/**
 * Find a server request in the server request list and delete
 *
 * @param serverRequest       server request to find and delete.
 */
void FindAndDeleteServerRequest(OCRequestInfo_t * serverRequest);

#endif //OC_SERVER_REQUEST_H

