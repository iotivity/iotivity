/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef WEB_UTIL_HTTP_STATUS_CODE_H_
#define WEB_UTIL_HTTP_STATUS_CODE_H_

namespace web_util
{
// HTTP status codes
    typedef enum
    {
        kHttpStatusCodeProcessing = 100,
        kHttpStatusCodeSuccessOk = 200,
        kHttpStatusCodeSuccessCreated = 201,
        kHttpStatusCodeAccepted = 202,
        kHttpStatusCodeSuccessNoContent = 204,
        kHttpStatusCodeSuccessPartialContent = 206,
        kHttpStatusCodeRedirectSpecialResponse = 300,
        kHttpStatusCodeRedirectMovedPermanently = 301,
        kHttpStatusCodeRedirectMovedTemporarily = 302,
        kHttpStatusCodeRedirectSeeOther = 303,
        kHttpStatusCodeRedirectNotModified = 304,
        kHttpStatusCodeRedirectTemporaryRedirect = 307,
        kHttpStatusCodeErrClientBadRequest = 400,
        kHttpStatusCodeErrClientUnauthorised = 401,
        kHttpStatusCodeErrClientForbidden = 403,
        kHttpStatusCodeErrClientNotFound = 404,
        kHttpStatusCodeErrClientNotAllowed = 405,
        kHttpStatusCodeErrClientNotAcceptable = 406,
        kHttpStatusCodeErrClientRequestTimeOut = 408,
        kHttpStatusCodeErrClientConflict = 409,
        kHttpStatusCodeErrClientLengthRequired = 411,
        kHttpStatusCodeErrClientPreconditionFailed = 412,
        kHttpStatusCodeErrClientRequestEntityTooLarge = 413,
        kHttpStatusCodeErrClientRequestUriTooLarge = 414,
        kHttpStatusCodeErrClientUnsupportedMediaType = 415,
        kHttpStatusCodeErrClientRangeNotSatisfiable = 416,
        kHttpStatusCodeErrServerInternalServerError = 500,
        kHttpStatusCodeErrServerNotImplemented = 501,
        kHttpStatusCodeErrServerBadGateway = 502,
        kHttpStatusCodeErrServerServiceUnavailable = 503,
        kHttpStatusCodeErrServerGatewayTimeOut = 504,
        kHttpStatusCodeErrServerInsufficientStorage = 507,
    } HttpStatusCode;

};  // namespace web_util
#endif  // WEB_UTIL_HTTP_STATUS_CODE_H_
