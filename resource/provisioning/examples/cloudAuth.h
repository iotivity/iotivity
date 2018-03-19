/* *****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *****************************************************************/
#ifndef CLOUD_AUTH_H
#define CLOUD_AUTH_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sends Sign UP request to cloud
 *
 * @param[in] endPoint         peer endpoint
 * @param[in] authProvider     authentication provider
 * @param[in] authToken        authentication token
 * @return  OCStackResult application result
 */
OCStackResult CloudSignUp(OCDevAddr  *endPoint, const char *authProvider, const char *authToken);

/**
 * Sends Sign IN request to cloud
 *
 * @param[in] endPoint         peer endpoint
 * @return  OCStackResult application result
 */
OCStackResult CloudSignIn(OCDevAddr  *endPoint);

/**
 * Sends Sign IN request to cloud
 *
 * @param[in] endPoint         peer endpoint
 * @return  OCStackResult application result
 */
OCStackResult CloudSignOut(OCDevAddr  *endPoint);

/**
 * Sends Sign Up request to cloud
 *
 * @param[in] ctx                    context
 * @param[in] handle                 handle
 * @return  OCStackApplicationResult application result
 */
OCStackApplicationResult handleCloudSignUpResponse(void *ctx,
                                                   OCDoHandle handle,
                                                   OCClientResponse *response);

/**
 * Sends Sign In request to cloud
 *
 * @param[in] ctx                    context
 * @param[in] handle                 handle
 * @param[in] response               response from peer
 * @return  OCStackApplicationResult application result
 */
OCStackApplicationResult handleCloudSignInResponse(void *ctx,
                                                   OCDoHandle handle,
                                                   OCClientResponse *response);

/**
 * Sends Sign Out request to cloud
 *
 * @param[in] ctx                    context
 * @param[in] handle                 handle
 * @param[in] response               response from peer
 * @return  OCStackApplicationResult application result
 */
OCStackApplicationResult handleCloudSignOutResponse(void *ctx,
                                                    OCDoHandle handle,
                                                    OCClientResponse *response);


#ifdef __cplusplus
}
#endif

#endif // CLOUD_AUTH_H
