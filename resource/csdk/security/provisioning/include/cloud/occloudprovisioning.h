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
#ifndef OC_CLOUD_PROVISIONING_H
#define OC_CLOUD_PROVISIONING_H

#include "octypes.h"
#include "ocprovisioningmanager.h"
#include "experimental/securevirtualresourcetypes.h"
#include "auth.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef void (*OCCloudResponseCB )(void* ctx, OCClientResponse* response, void* data);

typedef struct cloudAce cloudAce_t;

struct cloudAce {
    char *aceId;
    OicUuid_t subjectuuid;
    uint16_t stype;
    uint16_t permission;
    OicSecRsrc_t *resources;
    OicSecValidity_t *validities;
    cloudAce_t *next;
};

typedef struct {
    char **array;
    size_t length;
}stringArray_t;

typedef struct {
    stringArray_t gidlist;
    stringArray_t midlist;
}stringArrayPair_t;

typedef struct {
    stringArrayPair_t invite;
    stringArrayPair_t invited;
}inviteResponse_t;

typedef struct CloudData
{
    void *ctx;                                  /**< Pointer to user context.**/
    OCProvisionDev_t *targetDev;          /**< Pointer to OCProvisionDev_t.**/
    OicSecCred_t *credInfo;                     /**< Array of pointers to OicSecCred_t.**/
    OCProvisionResultCB resultCallback;         /**< Pointer to result callback.**/
    OCProvisionResult_t *resArr;                /**< Result array.**/
    int numOfResults;                           /**< Number of results in result array.**/
    char* cloudURI;                       /**< Cloud URI.**/
    char* oauthProvider;                  /**< Authorisation provider name.**/
    char* accessToken;                    /**< Access token that can be validated by the Cloud.**/
    char* sid;                            /**< Cloud UUID.**/
} CloudData_t;

/**
 * Certificate-Issue request function
 *
 * @param[in] ctx               user-defined context
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudCertificateIssueRequest(void* ctx,
                                             const char *cloudUri,
                                             OCCloudResponseCB callback);

/**
 * CRL GET request function
 *
 * @param[in] ctx               user-defined context
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudGetCRL(void* ctx,
                            const char *cloudUri,
                            OCCloudResponseCB callback);

/**
 * CRL POST request function (with Serial Numbers list to revoke)
 *
 * @param[in] ctx               user-defined context
 * @param[in] thisUpdate        mandatory parameter thisUpdate
 * @param[in] nextUpdate        mandatory parameter nextUpdate
 * @param[in] crl               optional parameter crl
 * @param[in] serialNumbers     optional parameter serial numbers
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudPostCRL(void* ctx,
                             const char *thisUpdate,
                             const char *nextUpdate,
                             const OCByteString *crl,
                             const stringArray_t *serialNumbers,
                             const char *cloudUri,
                             OCCloudResponseCB callback);

/**
 * ACL id retrieve by device id
 *
 * @param[in] ctx               user-defined context
 * @param[in] deviceId          mandatory parameter device id
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudGetAclIdByDevice(void* ctx,
                                      const char *deviceId,
                                      const char *cloudUri,
                                      OCCloudResponseCB callback);

/**
 * ACL id create
 *
 * @param[in] ctx               user-defined context
 * @param[in] ownerId           mandatory parameter owner id
 * @param[in] deviceId          mandatory parameter device id
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclIdCreate(void* ctx,
                                 const char *ownerId,
                                 const char *deviceId,
                                 const char *cloudUri,
                                 OCCloudResponseCB callback);

/**
 * ACL id delete
 *
 * @param[in] ctx               user-defined context
 * @param[in] aclId             mandatory parameter acl id
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclIdDelete(void* ctx,
                                 const char *aclId,
                                 const char *cloudUri,
                                 OCCloudResponseCB callback);

/**
 * ACL individual get info
 *
 * @param[in] ctx               user-defined context
 * @param[in] aclId             mandatory parameter acl id
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclIndividualGetInfo(void* ctx,
                                          const char *aclId,
                                          const char *cloudUri,
                                          OCCloudResponseCB callback);

/**
 * ACL individual update ACE
 *
 * @param[in] ctx               user-defined context
 * @param[in] aclId             mandatory parameter acl id
 * @param[in] aces              mandatory parameter aces
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclIndividualAclUpdate(void* ctx,
                                            const char *aclId,
                                            const cloudAce_t *aces,
                                            const char *cloudUri,
                                            OCCloudResponseCB callback);

/**
 * ACL individual update
 *
 * @param[in] ctx               user-defined context
 * @param[in] aclId             mandatory parameter acl id
 * @param[in] aceId             mandatory parameter target ace id
 * @param[in] aces              mandatory parameter aces
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclIndividualAceUpdate(void* ctx,
                                            const char *aclId,
                                            const char *aceId,
                                            const cloudAce_t *aces,
                                            const char *cloudUri,
                                            OCCloudResponseCB callback);

/**
 * ACL individual delete - replaces ACE with given aceid with provided ACE
 *
 * @param[in] ctx               user-defined context
 * @param[in] aclId             mandatory parameter acl id
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclAcesDelete(void* ctx,
                                         const char *aclId,
                                         const char *cloudUri,
                                         OCCloudResponseCB callback);

/**
 * ACL individual delete ACE
 *
 * @param[in] ctx               user-defined context
 * @param[in] aclId             mandatory parameter acl id
 * @param[in] aceId             target ace id
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclIndividualAceDelete(void* ctx,
                                         const char *aclId,
                                         const char *aceId,
                                         const char *cloudUri,
                                         OCCloudResponseCB callback);

/**
 * ACL post group request function
 *
 * @param[in] ctx               user-defined context
 * @param[in] groupType         mandatory parameter group type
 * @param[in] groupMasterId     optional parameter group master id
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclCreateGroup(void* ctx,
                                    const char *groupType,
                                    const char *groupMasterId,
                                    const char *cloudUri,
                                    OCCloudResponseCB callback);

/**
 * ACL get group request function
 *
 * @param[in] ctx               user-defined context
 * @param[in] memberId          member id
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclFindMyGroup(void* ctx,
                                    const char *memberId,
                                    const char *cloudUri,
                                    OCCloudResponseCB callback);

/**
 * ACL delete group request function
 *
 * @param[in] ctx               user-defined context
 * @param[in] groupId           mandatory parameter group id
 * @param[in] groupMasterId     optional parameter group master id
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclDeleteGroup(void* ctx,
                                    const char *groupId,
                                    const char *groupMasterId,
                                    const char *cloudUri,
                                    OCCloudResponseCB callback);

/**
 * ACL join to invited group request function
 *
 * @param[in] ctx               user-defined context
 * @param[in] groupId           mandatory parameter group id
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclJoinToInvitedGroup(void* ctx,
                                           const char *groupId,
                                           const char *cloudUri,
                                           OCCloudResponseCB callback);

/**
 * ACL observe group request function
 *
 * @param[in] ctx               user-defined context
 * @param[in] groupId           mandatory parameter group id
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclObserveGroup(void* ctx,
                                     const char *groupId,
                                     const char *cloudUri,
                                     OCCloudResponseCB callback);

/**
 * ACL share device into group request function
 *
 * @param[in] ctx               user-defined context
 * @param[in] groupId           mandatory parameter group id
 * @param[in] memberIds         mandatory parameter members list
 * @param[in] deviceIds         mandatory parameter devices list
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclShareDeviceIntoGroup(void* ctx,
                                             const char *groupId,
                                             const stringArray_t *memberIds,
                                             const stringArray_t *deviceIds,
                                             const char *cloudUri,
                                             OCCloudResponseCB callback);

/**
 * ACL delete device from group request function
 *
 * @param[in] ctx               user-defined context
 * @param[in] groupId           mandatory parameter group id
 * @param[in] memberIds         mandatory parameter members list
 * @param[in] deviceIds         mandatory parameter devices list
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclDeleteDeviceFromGroup(void* ctx,
                                              const char *groupId,
                                              const stringArray_t *memberIds,
                                              const stringArray_t *deviceIds,
                                              const char *cloudUri,
                                              OCCloudResponseCB callback);

/**
 * ACL get group info request function
 *
 * @param[in] ctx               user-defined context
 * @param[in] groupId           mandatory parameter group id
 * @param[in] memberId          optional parameter member id
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclGroupGetInfo(void* ctx,
                                     const char *groupId,
                                     const char *memberId,
                                     const char *cloudUri,
                                     OCCloudResponseCB callback);

/**
 * ACL invite user to group request function
 *
 * @param[in] ctx               user-defined context
 * @param[in] userId            optional parameter user id
 * @param[in] groupIds          mandatory parameter groups list
 * @param[in] memberIds         mandatory parameter members list
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclInviteUser(void* ctx,
                                   const char *userId,
                                   const stringArray_t *groupIds,
                                   const stringArray_t *memberIds,
                                   const char *cloudUri,
                                   OCCloudResponseCB callback);

/**
 * ACL get invitation request function
 *
 * @param[in] ctx               user-defined context
 * @param[in] userId            optional parameter user id
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclGetInvitation(void* ctx,
                                      const char *userId,
                                      const char *cloudUri,
                                      OCCloudResponseCB callback);

/**
 * ACL delete invitation request function (by receiver)
 *
 * @param[in] ctx               user-defined context
 * @param[in] userId            optional parameter user id
 * @param[in] groupId           mandatory parameter group id
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclDeleteInvitation(void* ctx,
                                         const char *userId,
                                         const char *groupId,
                                         const char *cloudUri,
                                         OCCloudResponseCB callback);

/**
 * ACL cancel invitation request function (by sender)
 *
 * @param[in] ctx               user-defined context
 * @param[in] userId            optional parameter user id
 * @param[in] groupId           mandatory parameter group id
 * @param[in] memberId          mandatory parameter member id
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclCancelInvitation(void* ctx,
                                         const char *userId,
                                         const char *groupId,
                                         const char *memberId,
                                         const char *cloudUri,
                                         OCCloudResponseCB callback);

/**
 * ACL check that given request can be applied to resource
 *
 * @param[in] ctx               user-defined context
 * @param[in] subjectId         mandatory parameter subject id
 * @param[in] deviceId          mandatory parameter device id
 * @param[in] method            mandatory parameter method
 * @param[in] uri               mandatory parameter URI
 * @param[in] cloudUri          cloud host and port
 * @param[in] callback          optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCCloudAclPolicyCheck(void* ctx,
                                    const char *subjectId,
                                    const char *deviceId,
                                    const char *method,
                                    const char *uri,
                                    const char *cloudUri,
                                    OCCloudResponseCB callback);

/**
 * Provision cloud configuration
 *
 * @param[in] ctx               user-defined context
 * @param[in] pDev              provision device
 * @param[in] cloud             struct containing provisioned parameters
 * @param[in] resultCallback    optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCProvisionCloudConfig(void *ctx,
                                     const OCProvisionDev_t *pDev,
                                     const OicCloud_t *cloud,
                                     OCClientResponseHandler resultCallback);

/**
 * Get cloud status of the selected device
 *
 * @param[in] ctx               user-defined context
 * @param[in] pDev              provision device
 * @param[in] cloud             struct containing cloud URL
 * @param[in] resultCallback    optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCGetCloudStatusRequest(void *ctx,
                                      const OCProvisionDev_t *pDev,
                                      const OicCloud_t *cloud,
                                      OCClientResponseHandler resultCallback);

/**
 * Delete CoAPCloudConf and cloud related ACLs from the selected device by cloud id
 *
 * @param[in] ctx               user-defined context
 * @param[in] pDev              provision device
 * @param[in] cloud             struct containing cloud UUID
 * @param[in] resultCallback    optional result callback, can be NULL if not required
 * @return  OCStackResult application result
 */
OCStackResult OCRemoveCloudConfig(void *ctx,
                                  const OCProvisionDev_t *pDev,
                                  const OicCloud_t *cloud,
                                  OCProvisionResultCB resultCallback);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //OC_CLOUD_PROVISIONING_H
