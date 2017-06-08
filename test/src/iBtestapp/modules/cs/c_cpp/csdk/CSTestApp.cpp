/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/
#include "CSCsdkCloudHelper.h"
#include "CSCsdkCloudClientHelper.h"
#include "CSCsdkUtilityHelper.h"

CSCsdkCloudHelper m_CloudAclHelper;
string m_hostAddress = COAP_HOST_ADDRESS;
string m_uidControlee = "";
string m_authTokenControlee = "";
string m_aclId = "";
string m_groupId = "";
string aceId = "";
string g_hostAddress = "";
static OCDevAddr endPoint;
static OCAccountManager::Ptr g_accountMgrControlee = nullptr;
static OCAccountManager::Ptr g_accountMgrController = nullptr;
cloudAce_t *aces;

/*
 * UI Related
 */
enum class Menu
{

    EXIT,
    START_CLOUD_DEVICE = 100,
    SIGN_UP,
    SIGN_IN,
    SIGN_OUT,
    OCCloudCertificateIssueRequest,
    OCCloudGetCRL,
    OCCloudPostCRL,

    OCCloudAclGetInvitation = 120,
    OCCloudAclJoinToInvitedGroup,
    OCCloudAclObserveGroup,
    OCCloudAclCancelInvitation,

    DISCOVER_RESOURCE = 130,
    SEND_GET_REUQUEST,
    SEND_POST_REQUEST,


    OCCloudAclPolicyCheck = 140,




    START_CLOUD_CONTROLEE = 200,
    SIGN_UP_CONTROLEE,
    SIGN_IN_CONTROLEE,
    SIGN_OUT_CONTROLEE,

    PUBLISH_RESOURCE,

    OCCloudAclIdCreate,
    OCCloudGetAclIdByDevice,
    OCCloudAclIndividualGetInfo,

    OCCloudAclIndividualAclUpdate,
    OCCloudAclIndividualAceUpdate,

    OCCloudAclIdDelete,
    OCCloudAclAcesDelete,
    OCCloudAclIndividualAceDelete,
    OCCloudAclIndividualAces,

    OCCloudAclCreateGroup = 260,
    OCCloudAclFindMyGroup,
    OCCloudAclDeleteGroup,
    OCCloudAclGroupGetInfo,



    OCCloudAclShareDeviceIntoGroup = 270,
    OCCloudAclDeleteDeviceFromGroup,

    OCCloudAclInviteUser =280,
    OCCloudAclDeleteInvitation,

};

static Menu g_userInput = Menu::EXIT;

void clearScreen()
{
    const char* CLEAR_SCREE_ANSI = "\e[1;1H\e[2J";
    ssize_t bytes = write(STDOUT_FILENO, CLEAR_SCREE_ANSI, 12);
    OC_UNUSED(bytes);
}

static void printMenu(void)
{
    clearScreen();

    printf("\n----------CONTROLLER----------\n");
    printf("%d. Start Cloud Controller\n", Menu::START_CLOUD_DEVICE);
    printf("%d. Sign Up\n", Menu::SIGN_UP);
    printf("%d. Sign In\n", Menu::SIGN_IN);
    printf("%d. Sign Out\n\n", Menu::SIGN_OUT);

    printf("%d. OCCloudCertificateIssueRequest\n", Menu::OCCloudCertificateIssueRequest);
    printf("%d. OCCloudGetCRL\n", Menu::OCCloudGetCRL);
    printf("%d. OCCloudPostCRL\n\n", Menu::OCCloudPostCRL);

    printf("%d. OCCloudAclGetInvitation\n", Menu::OCCloudAclGetInvitation);
    printf("%d. OCCloudAclJoinToInvitedGroup\n", Menu::OCCloudAclJoinToInvitedGroup);
    printf("%d. OCCloudAclObserveGroup\n", Menu::OCCloudAclObserveGroup);
    printf("%d. OCCloudAclCancelInvitation\n", Menu::OCCloudAclCancelInvitation);

    printf("\n%d. DiscoverResource\n", Menu::DISCOVER_RESOURCE);
    printf("%d. Send GET Request\n", Menu::SEND_GET_REUQUEST);
    printf("%d. Send POST Request\n", Menu::SEND_POST_REQUEST);

    printf("%d. OCCloudAclPolicyCheck\n", Menu::OCCloudAclPolicyCheck);



    printf("\n----------CONTROLLEE----------\n");
    printf("%d. Start Cloud Controller\n", Menu::START_CLOUD_CONTROLEE);
    printf("%d. Sign Up\n", Menu::SIGN_UP_CONTROLEE);
    printf("%d. Sign In\n", Menu::SIGN_IN_CONTROLEE);
    printf("%d. Sign Out\n", Menu::SIGN_OUT_CONTROLEE);

    printf("\n%d. Publish Resource\n", Menu::PUBLISH_RESOURCE);

    printf("\n%d. OCCloudAclIdCreate\n", Menu::OCCloudAclIdCreate);
    printf("%d. OCCloudGetAclIdByDevice\n", Menu::OCCloudGetAclIdByDevice);
    printf("%d. OCCloudAclIndividualGetInfo\n\n", Menu::OCCloudAclIndividualGetInfo);

    printf("%d. OCCloudAclIndividualAclUpdate\n", Menu::OCCloudAclIndividualAclUpdate);
    printf("%d. OCCloudAclIndividualAceUpdate\n\n", Menu::OCCloudAclIndividualAceUpdate);

    printf("%d. OCCloudAclIdDelete\n", Menu::OCCloudAclIdDelete);
    printf("%d. OCCloudAclAcesDelete\n", Menu::OCCloudAclAcesDelete);
    printf("%d. OCCloudAclIndividualAceDelete\n\n", Menu::OCCloudAclIndividualAceDelete);


    printf("%d. OCCloudAclCreateGroup\n", Menu::OCCloudAclCreateGroup);
    printf("%d. OCCloudAclFindMyGroup\n", Menu::OCCloudAclFindMyGroup);
    printf("%d. OCCloudAclDeleteGroup\n", Menu::OCCloudAclDeleteGroup);
    printf("%d. OCCloudAclGroupGetInfo\n", Menu::OCCloudAclGroupGetInfo);

    printf("%d. OCCloudAclShareDeviceIntoGroup\n", Menu::OCCloudAclShareDeviceIntoGroup);
    printf("%d. OCCloudAclDeleteDeviceFromGroup\n", Menu::OCCloudAclDeleteDeviceFromGroup);

    printf("%d. OCCloudAclInviteUser\n", Menu::OCCloudAclInviteUser);
    printf("%d. OCCloudAclDeleteInvitation\n", Menu::OCCloudAclDeleteInvitation);

    printf("%d. Exit\n", Menu::EXIT);
    printf("Enter Input: ");
}

static void doAction(Menu userInput)
{
    int groupNumber = 1;
    char* groupIds[] =
    { m_groupId.c_str() };
    stringArray_t *groupArray = OICCalloc(groupNumber, sizeof(stringArray_t*));
    groupArray->array = groupIds;
    groupArray->length = groupNumber;

    int memberNumber = 2;
    char* memberIDs[] =
    { GROUP_MEMBER_ID_01, GROUP_MEMBER_ID_02 };
    stringArray_t *memberArray = OICCalloc(memberNumber, sizeof(stringArray_t*));
    memberArray->array = memberIDs;
    memberArray->length = memberNumber;

    int deviceNumber = 2;
    char* deviceIDs[] =
    { GROUP_DEVICE_ID_01, GROUP_DEVICE_ID_02 };
    stringArray_t *deviceArray = OICCalloc(deviceNumber, sizeof(stringArray_t*));
    deviceArray->array = deviceIDs;
    deviceArray->length = deviceNumber;
    aces = CSCsdkUtilityHelper::createCloudAces();

    OCByteString crlData =
    { 0, 0 };

    stringArray_t serialNumbers =
    { 0, 0 };
    stringArray_t *rcsn = 0;
    OCByteString *crl = 0;
    int cnt = 1;
    int len = 1;

    char **item = (char**) OICCalloc(cnt, sizeof(char*));

    for (int i = 0; i < cnt; i++)
    {
        item[i] = (char*) OICCalloc(len, sizeof(char));
        strcpy(item[i], "1");
    }

    serialNumbers.array = item;
    serialNumbers.length = (size_t) cnt;

    rcsn = serialNumbers.array ? &serialNumbers : NULL;
    crl = crlData.bytes ? &crlData : NULL;

    std::string authcodeControlle = "";
    std::string authcodeController = "";

    switch (userInput)
    {
        case Menu::START_CLOUD_CONTROLEE:
            m_CloudAclHelper.initCloudACLClient(CLOUD_CONTROLLER);
            g_accountMgrControlee = OCPlatform::constructAccountManagerObject(g_hostAddress,
                    CT_ADAPTER_TCP);
            break;

        case Menu::SIGN_UP_CONTROLEE:
            cout << "Please Input The AuthCode" <<endl;
            cin >> authcodeController;
            CloudCommonUtil::signUp(g_accountMgrControlee, authcodeController);
            break;

        case Menu::SIGN_IN_CONTROLEE:

            CloudCommonUtil::signIn(g_accountMgrControlee);
            break;

        case Menu::SIGN_OUT_CONTROLEE:
            CloudCommonUtil::signOut(g_accountMgrControlee);
            break;

        case Menu::PUBLISH_RESOURCE:
            createAirConDevice(true);
            publishResource();
            break;

         /************************************** CONTROLEE ACL **********************************/
        case Menu::OCCloudAclIdCreate:
            m_CloudAclHelper.cloudAclIdCreate((void*) CTX_CREATE_ACL, DEFAULT_OWNER_ID,
            DEFAULT_DEV_ID_APP, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::OCCloudGetAclIdByDevice:
            m_CloudAclHelper.cloudGetAclIdByDevice((void*) CTX_GET_ACL_ID_BY_DEV,
            DEFAULT_DEV_ID_APP, &endPoint, CSCsdkCloudHelper::aclResponseCB, m_aclId, OC_STACK_OK);
            break;

        case Menu::OCCloudAclIndividualGetInfo:
            m_CloudAclHelper.cloudAclIndividualGetInfo((void*) CTX_INDIVIDUAL_GET_INFO,
                    m_aclId.c_str(), &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;


        case Menu::OCCloudAclIndividualAclUpdate:
            m_CloudAclHelper.cloudAclIndividualAclUpdate((void*) CTX_INDIVIDUAL_UPDATE_ACE,
                    m_aclId.c_str(), aces, &endPoint, CSCsdkCloudHelper::cloudResponseCB,
                    OC_STACK_OK);
            break;

        case Menu::OCCloudAclIndividualAceUpdate:
            m_CloudAclHelper.cloudAclIndividualAceUpdate((void*) CTX_INDIVIDUAL_UPDATE_ACE,
                    m_aclId.c_str(), aceId.c_str(), aces, &endPoint,
                    CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::OCCloudAclIdDelete:
            m_CloudAclHelper.cloudAclIdDelete((void*) CTX_DELETE_ACL, m_aclId.c_str(), &endPoint,
                    CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::OCCloudAclAcesDelete:
            m_CloudAclHelper.cloudAclAcesDelete((void*) CTX_INDIVIDUAL_UPDATE_ACE, m_aclId.c_str(),
                    &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::OCCloudAclIndividualAceDelete:
            m_CloudAclHelper.cloudAclIndividualAceDelete((void*) CTX_INDIVIDUAL_UPDATE_ACE,
                    m_aclId.c_str(), aceId.c_str(), &endPoint, CSCsdkCloudHelper::cloudResponseCB,
                    OC_STACK_OK);
            break;

        /************************************** CONTROLEE GROUP **********************************/
        case Menu::OCCloudAclCreateGroup:
            m_CloudAclHelper.cloudAclCreateGroup((void*) CTX_CREATE_GROUP, GROUP_TYPE_PUBLIC,
            GROUP_MASTER_ID, &endPoint, CSCsdkCloudHelper::createGroupResponseCB, m_groupId,
                    OC_STACK_OK);
            break;

        case Menu::OCCloudAclFindMyGroup:
            m_CloudAclHelper.cloudAclFindMyGroup((void*) CTX_FIND_GROUP, GROUP_MASTER_ID, &endPoint,
                    CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::OCCloudAclDeleteGroup:
            m_CloudAclHelper.cloudAclDeleteGroup((void*) CTX_DELETE_GROUP, m_groupId.c_str(),
            GROUP_MASTER_ID, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::OCCloudAclGroupGetInfo:
            m_CloudAclHelper.cloudAclGroupGetInfo((void*) CTX_GET_GROUP_INFO, m_groupId.c_str(),
            GROUP_MASTER_ID, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::OCCloudAclShareDeviceIntoGroup:
            m_CloudAclHelper.cloudAclShareDeviceIntoGroup((void*) CTX_SHARE_DEVICE,
                    m_groupId.c_str(), memberArray, deviceArray, &endPoint,
                    CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::OCCloudAclDeleteDeviceFromGroup:
            m_CloudAclHelper.cloudAclDeleteDeviceFromGroup((void*) CTX_DELETE_SHARED_DEVICE,
                    m_groupId.c_str(), memberArray, deviceArray, &endPoint,
                    CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        /************************************** CONTROLEE INVITATION **********************************/
        case Menu::OCCloudAclInviteUser:
            m_CloudAclHelper.cloudAclInviteUser((void*) CTX_INVITE_USER, NULL, groupArray,
                    memberArray, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::OCCloudAclCancelInvitation:
            m_CloudAclHelper.cloudAclCancelInvitation((void*) CTX_INVITE_USER, NULL,
                    m_groupId.c_str(),
                    GROUP_MEMBER_ID_01, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::OCCloudAclPolicyCheck:
            m_CloudAclHelper.cloudAclPolicyCheck((void*) CTX_INDIVIDUAL_GET_INFO,
                    CSCsdkCloudHelper::s_subjectuuid.c_str(), CSCsdkCloudHelper::s_deviceId.c_str(),
                    m_CloudAclHelper.GET_REQUEST.c_str(), CSCsdkCloudHelper::s_href.c_str(),
                    &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        /************************************** CONTROLLER GROUP **********************************/
        case Menu::START_CLOUD_DEVICE:
            m_CloudAclHelper.initCloudACLClient();
            g_accountMgrController = OCPlatform::constructAccountManagerObject(g_hostAddress,
                    CT_ADAPTER_TCP);
            break;

        case Menu::SIGN_UP:

            cout << "Please Input The AuthCode" <<endl;
            cin >> authcodeControlle;
            CloudCommonUtil::signUp(g_accountMgrController, authcodeControlle);
            break;

        case Menu::SIGN_IN:
            CloudCommonUtil::signIn(g_accountMgrController);
            break;

        case Menu::SIGN_OUT:
            CloudCommonUtil::signOut(g_accountMgrController);
            break;

        case Menu::OCCloudCertificateIssueRequest:
            m_CloudAclHelper.cloudCertificateIssueRequest((void*) CTX_CERT_REQ_ISSUE, &endPoint,
                    CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::OCCloudGetCRL:
            m_CloudAclHelper.cloudGetCRL((void*) CTX_GET_CRL, &endPoint,
                    CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::OCCloudPostCRL:
            m_CloudAclHelper.cloudPostCRL((void*) CTX_POST_CRL, CRL_THIS_UPDATE.c_str(),
                    CRL_NEXT_DATE.c_str(), crl, rcsn, &endPoint, CSCsdkCloudHelper::cloudResponseCB,
                    OC_STACK_INVALID_PARAM);
            break;

        case Menu::OCCloudAclGetInvitation:
            m_CloudAclHelper.cloudAclGetInvitation((void*) CTX_GET_GROUP_INVITATION, NULL,
                    &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::OCCloudAclJoinToInvitedGroup:
            m_CloudAclHelper.cloudAclJoinToInvitedGroup((void*) CTX_JOIN_GROUP, m_groupId.c_str(),
                    &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::OCCloudAclObserveGroup:
            m_CloudAclHelper.cloudAclObserveGroup((void*) CTX_OBSERVER_GROUP, m_groupId.c_str(),
                    &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::OCCloudAclDeleteInvitation:
            m_CloudAclHelper.cloudAclDeleteInvitation((void*) CTX_DELETE_INVITATION, NULL,
                    m_groupId.c_str(), &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_OK);
            break;

        case Menu::DISCOVER_RESOURCE:
            findAllResources(CloudCommonUtil::getDefaultHostAddess());
            break;

        case Menu::SEND_GET_REUQUEST:
            sendGetRequest(ResourceSelectionType.VERTICAL_RESOURCE);
            break;

        case Menu::SEND_POST_REQUEST:
            sendPostRequest(ResourceSelectionType.VERTICAL_RESOURCE);
            break;

        default:
            printf("Wrong Input, Please provide Input Again");
            return;
    }
}

void createEndPoint()
{
    memset(&endPoint, 0, sizeof(endPoint));
    strncpy(endPoint.addr, DEFAULT_HOST, sizeof(endPoint.addr));
    endPoint.port = OC_MULTICAST_PORT;
}

int main()
{
    g_hostAddress = CloudCommonUtil::getDefaultHostAddess();
    createEndPoint();
#ifdef __TLS_ON__
    enableTlsMode();
#endif
    for (;;)
    {
        printMenu();
        int input = 0;

        if (!scanf("%d", &input))
        {
            printf("Wrong Input, Please provide Input Again");
        }

        g_userInput = (Menu) input;
        doAction(g_userInput);
    }
    return 0;
}

