#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#!/usr/bin/python
import sys
reload(sys)
sys.setdefaultencoding('utf-8')
import os
import java
import time
import re
import ast
import datetime

sys.path.append("IoTConformanceTestLib.jar")
from oic.ctt.network.coap import *
from oic.ctt.network import *
from oic.ctt.provision import *
from oic.ctt.devicecontroller import *
from oic.ctt import *
from oic.ctt.formatter import *
from oic.ctt.network.control import *
import oic.ctt.network.OICHelper.MessageType as MessageType
import oic.ctt.network.OICHelper.MessageParameters as MessageParameters
import oic.ctt.network.OICRequestData.Method as Method
import oic.ctt.network.OICClient.Protocol as Protocol
import oic.ctt.DUTResource.CRUDNType as CRUDNType
import oic.ctt.DUTResource.PayloadType as PayloadType
import oic.ctt.provision.SecuredDeviceInfo.OTM as OTM
from robot.api import logger
from robot.libraries.BuiltIn import BuiltIn
import java.util.ArrayList as ArrayList
import java.util.HashMap as HashMap
import java.lang.Integer as Integer
import java.lang.Float as Float
import java.lang.Boolean as Boolean
import java.io.File as File

class ConformanceKeyword(object):

    class PROTOCOL():
        global UNICAST
        global HTTP
        global COAP
        global REP
        global HREF
        global SECURITY
        global RESOURCE_DISCOVERY_URI
        global RESOURCE_DISCOVERY_RT
        global DEVICE_DISCOVERY_URI
        global DEVICE_DISCOVERY_RT
        global PLATFORM_DISCOVERY_URI
        global PLATFORM_DISCOVERY_RT
        UNICAST = 'UNICAST'
        HTTP = 'HTTP'
        COAP = 'COAP'
        REP  = 'REP'
        HREF = 'href'
        SECURITY = 'SECURITY'
        RESOURCE_DISCOVERY_URI = '/oic/res'
        RESOURCE_DISCOVERY_RT = 'oic.wk.res'
        DEVICE_DISCOVERY_URI = '/oic/d'
        DEVICE_DISCOVERY_RT = 'oic.wk.d'
        PLATFORM_DISCOVERY_URI = '/oic/p'
        PLATFORM_DISCOVERY_RT = 'oic.wk.p'
    

    class MSGSTRING():
        global DUT
        global TE
        global REQUEST_CODE
        global RESPONSE_CODE
        global REQUEST_TYPE
        global SOURCE_ADDRESS
        global SOURCE_PORT
        global SOURCE_HEADER
        global REQUEST_HEADER
        global PROTOCOL_TYPE
        global DEFAULT_URI
        global DESTINATION_URI
        global CONFIRMABLE_TYPE
        global PAYLOAD
        global QUERY
        global CREATED_PAYLOAD
        global OTM_RDP
        global OTM_JW
        global CORE_PROVISION_URI
        global CORE_ACL_URI
        global ACL_PAYLOAD
        global CLIENT_DEVICE_ID
        global OIC_ID
        global OIC_ID_PATH

        DUT = 'DUT'
        TE = 'TE'
        REQUEST_CODE = '   Request Code : '
        RESPONSE_CODE = 'Response Code : '
        REQUEST_TYPE = 'Request Type : '
        SOURCE_ADDRESS = 'Source Address : '
        SOURCE_PORT = 'Source Port : '
        SOURCE_HEADER = 'Source Header : '
        REQUEST_HEADER = 'Request Header : '
        PROTOCOL_TYPE = 'Protocol Type : '
        DEFAULT_URI = 'Default URI : '
        DESTINATION_URI = 'Destination URI : '
        CONFIRMABLE_TYPE = 'Confirmable Type : '
        PAYLOAD = 'Payload : \n'
        QUERY = 'Query : '
        CREATED_PAYLOAD = 'Created payload'
        OTM_RDP = "oic.sec.doxm.rdp"
        OTM_JW = "oic.sec.doxm.jw"
        CORE_PROVISION_URI = "/oic/sec/doxm"
        CORE_ACL_URI = "/oic/sec/acl"
        ACL_PAYLOAD = "{\"acl\":[{\"sub\":\"ClientDeviceId\",\"rsrc\":[\"/test/test\"],\"perms\":31,\"ownrs\":[\"OwnerDeviceId\"]}]}"
        CLIENT_DEVICE_ID = "adminDeviceUUID0"
        RELAY_CONTROL_PORT = 12345
        RELAY_DATA_PORT = 12346
        OIC_ID = "OIC_DEFINED_RESOURCE"
        OIC_ID_PATH = "libs/OICDescriptor.json"

    class DUTDESCRIPTORKEY:
        global ATTRIBUTE_KEY
        global ATTRIBUTE_VALUE_TYPE
        global ATTRIBUTE_TEST_DATA
        global ATTRIBUTE_TEST_DATA2
        global ATTRIBUTE_ALTERNATE_DATA
        global ATTRIBUTE_ACCESSMODE
        global ATTRIBUTE_TYPE_INT
        global ATTRIBUTE_TYPE_FLOAT
        global ATTRIBUTE_TYPE_BOOL
        global ATTRIBUTE_TYPE_STRING
        global ATTRIBUTE_TYPE_ARRAY

        ATTRIBUTE_KEY = 'key'
        ATTRIBUTE_VALUE_TYPE = 'type'
        ATTRIBUTE_TEST_DATA = 'testdata'
        ATTRIBUTE_TEST_DATA2 = 'testdata2'
        ATTRIBUTE_ALTERNATE_DATA = 'alternatedata'
        ATTRIBUTE_ACCESSMODE = 'accessmode'
        ATTRIBUTE_TYPE_INT = 'integer'
        ATTRIBUTE_TYPE_FLOAT = 'float'
        ATTRIBUTE_TYPE_BOOL = 'boolean'
        ATTRIBUTE_TYPE_STRING = 'string'
        ATTRIBUTE_TYPE_ARRAY = 'array'

    class PACKETKEY():
        global REQUESTCODE
        global RESPONSECODE
        global PAYLOAD
        global SRCADDRESS
        global SRCPORT
        global MSGID
        global TOKEN

        REQUESTCODE = 'requestCode'
        RESPONSECODE = 'responseCode'
        SRCADDRESS = 'srcAddress'
        SRCPORT = 'srcPort'
        MSGID = 'msgID'
        TOKEN = 'token'

    RESPONSE_MAP = {}
    TOTAL_RESPONSE = 0

    def __init__(self):
        self.dic_get_response = {}
        self._result = ''
        self.dut_information_manager = DUTInformationManager.getInstance()
        self.dut_id = ''
        self.json_representation = ''
        self.prop_map = ''
        self.rep_map = ''
        self.resource_href = ''
        self.resource_type_array = ''
        self.resource_type = ''
        self.resource_dis = 1
        self.resource_interface_array = ''
        self.resource_map = {}
        self.json_representation = ''
        self.prop_map = ''
        self.rep_map = ''
        self.resource_href = ''
        self.resource_type_array = ''
        self.oic_client = OICClient()
        self.oic_server = OICCoapServer()
        self.provision_manager = ProvisionManager()
        self.provisioning_client = ProvisioningClient()
        self.bwt_href = "/device/test-tool-bwt"

        self.oic_resource_light_bwt = OICCoapResource(self.bwt_href, "core.light", "oic.if.baseline", True)
        self.oic_resource_light_bwt.addResourceAttribute("power", "off")
        self.oic_resource_light_bwt.addResourceAttribute("intensity", 100)
        self.oic_resource_light_bwt.addResourceAttribute("manufacturer", "Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_End_of_BWT")
        self.request = OICRequestData()
        self.rep_key_list = ArrayList()
        self.ui = UserInterface()
        self.control_client = ControlClient()
        self.built_in = BuiltIn()
        self.hrefs = []
        self.options = []

    # Documentation for a function
    # @brief print log to console
    # @param arg(log)
    # @return void
    def log_to_console(self, arg):
        "Print log at robotframework console"
        logger.info('\n%s' % arg, also_console=True)

    # Documentation for a function
    # @brief Returns true if ran from GUI, else false
    # @return boolean
    def is_cli_enabled(self):
        "Returns true if ran from GUI, else false"
        if os.environ.get('CLI_ENABLED') == "true":
            return True
        else:
            return False

    # Documentation for a function
    # @brief print send log to console
    # @param sender(string name), receiver(string name), do(message type), what(payload)
    # @return void
    def print_testcase_do(self, sender, receiver, do, what=None):
        "Print log for procedure of sending"
        sentence = [sender, ' --------> ', \
            do.center(40), ' --------> ', receiver]
        sentence2 = ['@@-> ',receiver,'\\n @@',do,' ']
        if what is not None:
            what2 = what.replace('\n ','\\n')
            sentence2.append(what2)
            sentence.append('\n   ')
            sentence.append(what)
        logger.debug("".join(sentence2))
        self.log_to_console("".join(sentence))

    # Documentation for a function
    # @brief print received log to console
    # @param sender(string name), receiver(string name), get(message type), what(payload)
    # @return void
    def print_testcase_get(self, sender, receiver, get, what=None):
        "Print log for procedure of receiving"
        sentence = [receiver, ' <-------- ', \
            get.center(40), ' <-------- ', sender]
        sentence2 = ['@@<- ',sender,'\\n @@',get,' ']
        if what is not None:
            sentence.append('\n   ')
            sentence.append(what)
            what2 = what.lstrip()
            sentence2.append(what2)
        logger.debug("".join(sentence2))
        self.log_to_console("".join(sentence))

## Linux Device Management Keywords ##

    def initialize_device_controller(self, device_id):
        self.deviceController = LinuxDeviceController(str(device_id))
        self.logManager = LogManager.getInstance()

    def change_working_directory(self, path):
        self.deviceController.getFileManager().changeWorkingDir(str(path))

    def execute_application(self, process_name, command):
        self.deviceController.getProcessManager().createProcess(str(process_name), str(command))

    def start_application_log(self, device_id, process_name):
        self.logManager.registerProcessLogger(str(device_id), self.deviceController, str(process_name), True)

    def input_command_to_application(self, process_name, command):
        self.deviceController.getProcessManager().inputProcessCommand(str(process_name), str(command))

    def get_application_log(self, device_id, process_name):
        return self.logManager.getProcessLogger(str(device_id), str(process_name)).getLogs()

    def terminate_application(self, process_name):
        self.deviceController.getProcessManager().removeProcess(str(process_name))

    def check_keyword(self,keyword,logs):
        if re.search(str(keyword),str(logs)):
            return True
        else:
            return False

    # Documentation for a function
    # @brief start server at Conformance test tool
    # @param is_confirmable(bool)
    # @return void
    def get_space_seperated_string_from_list(self, list):
        items = ""
        for item in list:
            items = items + item + " "
        items = items.strip()
        return items

    # Documentation for a function
    # @brief start server at Conformance test tool
    # @param is_confirmable(bool)
    # @return void
    def start_server(self, is_confirmable=True):
        "Run server for test"
        self.dut_information = self.dut_information_manager.getDUTInformation(self.dut_id)
        href_list = self.get_dut_info_value(IotivityKey.HREF.toString(), self.dut_id, IotivityKey.DIS.toString()+'=1')
        for href in href_list:
            resource_type = self.get_space_seperated_string_from_list(self.dut_information.getResourceTypes(href) )
            resource_interface = self.get_space_seperated_string_from_list(self.dut_information.getInterfaceList(href) )

            self.resource_map[href] = OICCoapResource(href, resource_type, resource_interface, is_confirmable)
            rep_list = self.get_dut_resource_value(self.dut_id, HREF + '=' + href, REP, "R")
            self.rep_map = HashMap()

            for item in rep_list:
                self.add_into_json_representation(item[0], item[2], item[1])

            self.resource_map[href].addResourceAttributes(self.rep_map)
            self.oic_server.addResource(self.resource_map[href])

        self.resource_map[self.bwt_href] = self.oic_resource_light_bwt
        self.oic_server.addResource(self.resource_map[self.bwt_href])
        self.oic_server.clearDiscoveryRequests()
        self.oic_server.start()

    # Documentation for a function
    # @brief get received request from server of Conformance test tool
    # @param none
    # @return request
    def get_request(self, href, desired_code):
        "Return request when tool receive it"
        self.clear_request_list()
        self.oic_server.clearDiscoveryRequests()
        self.resource_map[href].clearRequestList()
        self.print_testcase_get(DUT, TE, 'Waiting for Request from DUT')
        for sec in range(1, 60):
            BuiltIn().sleep(1)
            request_list = self.resource_map[href].getRequestList()

            if request_list.size() > 0:
                self.request = request_list.get(0)
                request_code = self.request.getRequestValue(MessageParameters.requestCode)
                if request_code == desired_code:
                    result = self.get_request_result(self.request)
                    to_show = self.get_request_description(self.request)
                    self.print_testcase_get(DUT, TE, ' Request Message from DUT', to_show)
                    return result
                else:
                    self.resource_map[href].removeRequestFromList(self.request)
        return None

    # Documentation for a function
    # @brief clear request list
    # @param none
    # @return void
    def clear_request_list(self):
        "Clear request list"
        self.oic_server.clearDiscoveryRequests()

    # Documentation for a function
    # @brief get request code
    # @param none
    # @return reset flag
    def get_reset_state(self, href):
        "Return True if a RST have arrived, otherwise false"
        #self.oic_resource_light.updateLocalResourceResetFlag()
        return self.resource_map[href].hasReset()

    # Documentation for a function
    # @brief clear reset state
    # @param none
    # @return void
    def clear_reset_state(self, href):
        "Clear rest flag"
        self.resource_map[href].setResetReceived(False)

    # Documentation for a function
    # @brief get payload
    # @param request
    # @return request code (string)
    #def get_payload(self, request):
    #    "Return payload"
    #    payload = request.getRequestValue(OICHelper.MessageParameters.payload)
    #    return payload

    # Documentation for a function
    # @brief stop server at Conformance Test Tool
    # @param none
    # @return void
    def stop_server(self):
        "Stop server"
        self.oic_server.removeResource("/device/test-tool")
#        self.oic_server.destroy()

    # Documentation for a function
    # @brief load DUTDescriptor.json and make dut instance
    # @param dut_id, dut_info_path, dut_type
    # @return bool
    def define_dut_information(self, dut_id, dut_info_path, dut_type):
        "Make dut instance load dut json file"
        self.dut_id = dut_id
        self.dut_information_manager.createDUTInformation(OIC_ID, OIC_ID_PATH)
        return self.dut_information_manager.createDUTInformation(
            dut_id, dut_info_path)

    # Documentation for a function
    # @brief remove dut instance
    # @param dut_id
    # @return void
    def release_dut_information(self, dut_id):
        "Release dut instance"
        self.dut_information_manager.removeDUTInformation(OIC_ID)
        self.dut_information_manager.removeDUTInformation(dut_id)

    # Documentation for a function
    # @brief set directory path to log, currently this api is not available
    # @param path
    # @return bool
    def set_log_file_directory(self, path):
        return True

#Misc

    def generate_message_id(self):
        message_id = OICHelper.getRandomMessageIdString()
        return message_id

    def generate_token(self):
        token = OICHelper.createTokenString()
        return token

    def clear_response_map(self):
        self.RESPONSE_MAP = {}
        self.TOTAL_RESPONSE = 0

    def add_into_response_map(self, request, response):
        response_request_pair = [request, response]
        self.RESPONSE_MAP[self.TOTAL_RESPONSE] = response_request_pair
        self.TOTAL_RESPONSE += 1

    def get_request_from_response_map(self, key):
        if key < self.TOTAL_RESPONSE:
            response_request_pair = self.RESPONSE_MAP[key]
            return self.get_request_result(response_request_pair[0])
        else:
            return ''

    def get_response_from_response_map(self, key):
        if key < self.TOTAL_RESPONSE:
            response_request_pair = self.RESPONSE_MAP[key]
            return self.get_response_result(response_request_pair[1])
        else:
            return ''

    def get_response_map_size(self):
        return self.TOTAL_RESPONSE

    def get_cached_response_map(self):
        return self.RESPONSE_MAP

    def set_cached_response_map(self, cached_map):
        self.RESPONSE_MAP = cached_map
        self.TOTAL_RESPONSE = len(self.RESPONSE_MAP)

    def define_cached_map_globally(self):
        BuiltIn.set_suite_variable(self.TOTAL_RESPONSE, self.get_response_map_size())
        BuiltIn.set_suite_variable(self.RESPONSE_MAP, self.get_cached_response_map())

#Server - Receiving
    # Documentation for a function
    # @brief get discovery request
    # @param none
    # @return request
    def get_discovery_requests(self):
        "Return discovery request"
        self.print_testcase_do(TE, DUT, 'Waiting Discovery from DUT')
        for sec in range(1, 60):
            BuiltIn().sleep(1)
            request_list = self.oic_server.getDiscoveryRequests()
            if request_list.size() > 0:
                self.request = request_list.get(0)
#                what = REQUEST_CODE + \
#                    self.get_request_value(self.request, OICHelper.MessageParameters.requestCode) + '\n   '
#                what += SOURCE_ADDRESS + \
#                    self.get_request_value(self.request, OICHelper.MessageParameters.srcAddress) + '\n   '
#                what += SOURCE_PORT + \
#                    self.get_request_value(self.request, OICHelper.MessageParameters.srcPort) + '\n   '
#                what += SOURCE_HEADER + \
#                    self.get_request_header(self.request)
                result = self.get_request_result(self.request)
                self.print_testcase_get(DUT, TE, 'Discovery Message from DUT')
                return result
        return None

    def destroy_te_server(self):
        "Destory te server"
        return self.oic_server.destroy()

#Client - Messaging
    # Documentation for a function
    # @brief send multicast discovery message 
    # @param resuest_type, protocol_type, default_uri
    # @return response string
    def discover_all_resources(self, request_type, protocol_type, default_uri):
        "Send multicast discovery request and return response about multicast discovery request "
        result = ''
        resultList = ArrayList()
        client = OICClient()
        responseList = ArrayList()
        self.print_testcase_do(TE, DUT, 'Multicast Discover')
        dut_list = self.get_dut_info_value(
            IotivityKey.HREF.toString(), self.dut_id, IotivityKey.DIS.toString()+'=1')  # temp code
#        self.log_to_console(dut_list)
#        self.log_to_console("Line 252")
        responseList = client.discoverResource(
            self.get_protocol(protocol_type), default_uri)
        target_request = client.getLastRequest(self.get_protocol(protocol_type))
        target_response = None
#        self.log_to_console("Line 255")

        if responseList is None:
#            self.log_to_console("Line 258")
            self.print_testcase_get(DUT, TE, 'No Response')
#            self.log_to_console("Line 259")
            result = ""
        elif responseList.size() > 0:
#            self.log_to_console("Line 263")
            for response in responseList:
                result_temp = self.get_response_result(response)
                self.log_to_console("After get_response_result Call")
                result_json = self.get_response_json(result_temp)
                check = True
                for dut in dut_list:
                     if str(dut) not in str(result_json):
                         check = False
                self.print_testcase_get(
                DUT,
                TE,
                'Response for Discover',
                self.get_response_description(response))
                if check is True:
                    result = result_temp
                    target_response = response
#                    break

        self.add_into_response_map(target_request, target_response)
        return result
#                self.log_to_console("After get_response_json Call")
#                json_analyzer = JsonAnalyzer(result_json)
#                href = json_analyzer.getAllValueFromJson(IotivityKey.HREF.toString())
#                href = self.get_json_value(IotivityKey.HREF.toString(), result_json)
#                self.log_to_console(href.size())
#                di = self.get_json_value(IotivityKey.DI.toString(), result_json)
#                self.log_to_console("After get_json_value Call")
#                if (href.size() > 0 and href[0] in dut_list) or (di.size() > 0 ):
#                    self.log_to_console("After href[0]")
#                    result = result_temp
#                    self.print_testcase_get(
#                        DUT,
#                        TE,
#                        'Response for Discover',
#                        self.get_response_description(response))
#                    if "oic/res" in default_uri:
#                        self.log_to_console("inside /oic/res")
#                        self.log_to_console(dut_list)
#                        self.log_to_console(href)
#                        total_href_found = 0
#                        for dut_href in dut_list:
#                            is_exist = False
#                            for each_href in href:
#                                if dut_href is each_href:
#                                    is_exist = True
#                                    break
#                            if is_exist is True:
#                                total_href_found = total_href_found + 1
#                        if total_href_found is dut_list.size():
#                            desired_result = result
#                        self.log_to_console(href.size())
#                        if href[0] in dut_list:
#                            desired_result = result
#        self.log_to_console("Line 274")

    # Documentation for a function
    # @brief send multicast discovery message with query
    # @param request_type, protocol_type, default_uri, query, is_confirmable(bool)
    # @return response string
    def discover_resource_with_query(
            self,
            request_type,
            protocol_type,
            default_uri,
            query,
            is_confirmable, ip):
        "Send multicast discovery request with query and return response about discovery request"
        result = ""
        client = OICClient()
        responseList = ArrayList()
        self.print_testcase_do(
            TE,
            DUT,
            'Multicast Discover with query',
            QUERY + query)
        responseList = ArrayList()
        responseList = client.discoverResource(
            self.get_protocol(protocol_type), default_uri, query)
        target_request = client.getLastRequest(self.get_protocol(protocol_type))
        target_response = None
        dut_list = self.get_dut_info_value(
            IotivityKey.HREF.toString(), self.dut_id, query)  # temp code

        if responseList is None:
            self.print_testcase_get(DUT, TE, 'No Response')
            self.add_into_response_map(target_request, target_response)
            return ""
        elif responseList.size() > 0:
            for response in responseList:
                self.log_to_console(str(response))
                result_temp = self.get_response_result(response)
                if(result_temp is ''):
                    continue
                result_json = self.get_response_json(result_temp)
                href = self.get_json_value(IotivityKey.HREF.toString(), result_json)
                di = self.get_json_value(IotivityKey.DI.toString(), result_json)
#                if(href.size() < 1):
#                    continue
                
#                if (href.size() > 0 and href[0] in dut_list) or (di.size() > 0 ) or responseList.size() == 1:
                _ip = self.get_response_ip(result_temp)
                if ip == _ip:
                    result = result_temp
                    target_response = response
                    self.print_testcase_get(
                        DUT,
                        TE,
                        'Response for Discover',
                        self.get_response_description(response))
#        response = responseList.get(0)
#        result = self.get_response_result(response)
#        self.print_testcase_get(DUT,TE,'Response for Discover',self.get_response_description(response))

        self.add_into_response_map(target_request, target_response)
        return result

    # Documentation for a function
    # @brief send unicast discovery
    # @param request_type, protocol_type, ip, port, default_uri, is_confirmable, query
    # @return response string
    def discover_resource_using_unicast(
            self,
            request_type,
            protocol_type,
            message_id,
            token,
            ip,
            port,
            default_uri,
            is_confirmable,
            query=""):
        "Send unicast discovery request and return response"
        result = ""
        client = OICClient()
        #self.print_testcase_do(TE, DUT, 'Hello TEST 1', query)
        messageType = OICRequestData.Method.GET
        #self.print_testcase_do(TE, DUT, 'Hello TEST 2', query)
        protocol = self.get_protocol(protocol_type)
        #self.print_testcase_do(TE, DUT, 'Hello TEST 3', query)
        #self.print_testcase_do(TE, DUT, 'Unicast Discover', query)
        what = PROTOCOL_TYPE + protocol_type + '\n   '
        what += DEFAULT_URI + default_uri + '\n   '
        what += CONFIRMABLE_TYPE + str(is_confirmable) + '\n   '
        what += QUERY + query
        self.print_testcase_do(TE, DUT, 'Unicast Discover', what)
        response = client.sendRequest(
            protocol,
            self.get_coap_type(is_confirmable),
            messageType,
            message_id,
            token,
            ip,
            int(port),
            default_uri,
            query)
        target_request = client.getLastRequest(protocol)
        target_response = response
        self.add_into_response_map(target_request, target_response)

        if response is None:
            self.print_testcase_get(DUT, TE, 'No Response')
            return ""
        else:
            result = self.get_response_result(response)
            self.print_testcase_get(
                DUT,
                TE,
                'Response for Discover',
                self.get_response_description(response))

        return result

    # Documentation for a function
    # @brief send out unicast reuquest message with PUT method
    # @param request_type, protocol_type, ip, port, uri, is_confirmable(bool), paypload, payload_type=none
    # @return received response as  string
    def send_put_request(
            self,
            request_type,
            protocol_type,
            message_id,
            token,
            ip,
            port,
            uri,
            is_confirmable,
            payload,
            payload_type=None):
        "Send put request and return response"
        result = ""
        client = OICClient()
        messageType = OICRequestData.Method.PUT
        query = ""
        query_index = uri.find('?')
        if query_index != -1:
            query = uri[query_index+1:]
            uri = uri[:query_index]
        protocol = self.get_protocol(protocol_type)
        what = PROTOCOL_TYPE + protocol_type + '\n   '
        what += DESTINATION_URI + ip + ':' + str(port) + uri + '\n   '
        what += CONFIRMABLE_TYPE + str(is_confirmable) + '\n   '
        what += QUERY + query + '\n   '
        what += PAYLOAD + OICHelper.getPrettyJson(payload) + '\n   '
        self.print_testcase_do(TE, DUT, 'Put Request', what)

        if payload_type == None:
            payload_type = REP
        response = client.sendRequest(
            self.get_protocol(protocol_type),
            self.get_coap_type(is_confirmable),
            messageType,
            message_id,
            token,
            ip,
            int(port),
            uri,
            query,
            payload,
            self.get_payload_type(payload_type))

        target_request = client.getLastRequest(self.get_protocol(protocol_type))
        target_response = response
        self.add_into_response_map(target_request, target_response)

        if response is None:
            self.print_testcase_get(DUT, TE, 'No Response')
            return ""
        else:
            result = self.get_response_result(response)
            self.print_testcase_get(
                DUT,
                TE,
                'Response for Put',
                self.get_response_description(response))
        return result

    # Documentation for a function
    # @brief send out unicast reuquest message with POST method
    # @param request_type, protocol_type, ip, port, uri, is_confirmable(bool), paypload
    # @return response as  string
    def send_post_request(
            self,
            request_type,
            protocol_type,
            message_id,
            token,
            ip,
            port,
            uri,
            is_confirmable,
            payload):
        "Send post request and return response"
        result = ""
        client = OICClient()
        messageType = OICRequestData.Method.POST
        query = ""
        query_index = uri.find('?')
        if query_index != -1:
            query = uri[query_index+1:]
            uri = uri[:query_index]
        protocol = self.get_protocol(protocol_type)
        what = PROTOCOL_TYPE + protocol_type + '\n   '
        what += DESTINATION_URI + ip + ':' + str(port) + uri + '\n   '
        what += CONFIRMABLE_TYPE + str(is_confirmable) + '\n   '
        what += QUERY + query + '\n   '
        what += PAYLOAD + OICHelper.getPrettyJson(payload) + '\n   '
        self.print_testcase_do(TE, DUT, 'Post Request', what)
        response = client.sendRequest(
            self.get_protocol(protocol_type),
            self.get_coap_type(is_confirmable),
            messageType,
            message_id,
            token,
            ip,
            int(port),
            uri,
            query,
            payload)

        target_request = client.getLastRequest(self.get_protocol(protocol_type))
        target_response = response
        self.add_into_response_map(target_request, target_response)

        if response is None:
            # self.print_testcastuet@135e_get(DUT,TE,'No Response')
            return ""
        else:
            result = self.get_response_result(response)
            self.print_testcase_get(
                DUT,
                TE,
                'Response for Post',
                self.get_response_description(response))
        return result

    # Documentation for a function
    # @brief send out unicast reuquest message with GET method
    # @param request_type, protocol_type, ip, port, uri, is_confirmable(bool)
    # @return received response as  string
    def send_get_request(
            self,
            request_type,
            protocol_type,
            message_id,
            token,
            ip,
            port,
            uri,
            is_confirmable):
        "Send get request and return response"
        result = ""
        client = OICClient()
        messageType = OICRequestData.Method.GET
        query = ""
        query_index = uri.find('?')
        if query_index != -1:
            query = uri[query_index+1:]
            uri = uri[:query_index]
        what = PROTOCOL_TYPE + protocol_type + '\n   '
        what += DESTINATION_URI + ip + ':' + str(port) + uri + '\n   '
        what += CONFIRMABLE_TYPE + str(is_confirmable) + '\n   '
        what += QUERY + query
        self.print_testcase_do(TE, DUT, 'Get Request', what)
        response = client.sendRequest(
            self.get_protocol(protocol_type),
            self.get_coap_type(is_confirmable),
            messageType,
            message_id,
            token,
            ip,
            int(port),
            uri,
            query)

        target_request = client.getLastRequest(self.get_protocol(protocol_type))
        target_response = response
        self.add_into_response_map(target_request, target_response)

        if response is None:
            self.print_testcase_get(DUT, TE, 'No Response')
            return ""
        else:
            result = self.get_response_result(response)
            self.print_testcase_get(
                DUT,
                TE,
                'Response for Get',
                self.get_response_description(response))
        return result

    # Documentation for a function
    # @brief send out unicast reuquest message with DELETE method
    # @param request_type, protocol_type, ip, port, uri, is_confirmable(bool)
    # @return response for delete as  string
    def send_delete_request(
            self,
            request_type,
            protocol_type,
            message_id,
            token,
            ip,
            port,
            uri,
            is_confirmable):
        "Send delete request and return response"
        result = ""
        client = OICClient()
        query = ""
        query_index = uri.find('?')
        if query_index != -1:
            query = uri[query_index+1:]
            uri = uri[:query_index]
        messageType = OICRequestData.Method.DELETE
        what = PROTOCOL_TYPE + protocol_type + '\n   '
        what += DESTINATION_URI + ip + ':' + str(port) + uri + '\n   '
        what += CONFIRMABLE_TYPE + str(is_confirmable) + '\n   '
        what += QUERY + query
        self.print_testcase_do(TE, DUT, 'Delete Request', what)

        response = client.sendRequest(
            self.get_protocol(protocol_type),
            self.get_coap_type(is_confirmable),
            messageType,
            message_id,
            token,
            ip,
            int(port),
            uri,
            query)

        target_request = client.getLastRequest(self.get_protocol(protocol_type))
        target_response = response
        self.add_into_response_map(target_request, target_response)

        if response is None:
            self.print_testcase_get(DUT, TE, 'No Response')
            return ""
        else:
            result = self.get_response_result(response)
            self.print_testcase_get(
                DUT,
                TE,
                'Response for Delete',
                self.get_response_description(response))
        return result

    # Documentation for a function
    # @brief sends observer request
    # @param request_type, protocol_type, ip, port, uri, is_confirmable(bool)
    # @return void
    def send_observe_request(
            self,
            protocol_type,
            message_id,
            token,
            ip,
            port,
            uri,
            is_confirmable):
        "Send observe request and return response"
        what = PROTOCOL_TYPE + protocol_type + '\n   '
        what += DESTINATION_URI + ip + ':' + str(port) + uri + '\n   '
        what += CONFIRMABLE_TYPE + str(is_confirmable) + '\n   '
        self.print_testcase_do(TE, DUT, 'Observe Request', what)
        response = self.oic_client.observeResource(
            self.get_protocol(protocol_type),
            self.get_coap_type(is_confirmable),
            message_id,
            token,
            ip,
            port,
            uri)

        target_request = self.oic_client.getLastRequest(self.get_protocol(protocol_type))
        target_response = response
        self.add_into_response_map(target_request, target_response)

        if response is None:
            self.print_testcase_get(DUT, TE, 'No Observe Response')
            return ""
        else:
            result = self.get_response_result(response)
            self.oic_client.clearNotifications(self.get_protocol(protocol_type))
            self.print_testcase_get(
                DUT,
                TE,
                'Response for Observe Request',
                self.get_response_description(response))
        return result

    # Documentation for a function
    # @brief sends cancel observer request using get
    # @param request_type, protocol_type, ip, port, uri, is_confirmable(bool)
    # @return response as string
    def cancel_observe_with_get_message(
            self,
            protocol_type,
            message_id,
            ip,
            port,
            uri,
            is_confirmable):
        "Send cancel observe request using get and return response"
        result = ""
        what = PROTOCOL_TYPE + protocol_type + '\n   '
        what += DESTINATION_URI + ip + ':' + str(port) + uri + '\n   '
        what += CONFIRMABLE_TYPE + str(is_confirmable) + '\n   '
        self.print_testcase_do(TE, DUT, 'Cancel Observe Request', what)
        response = self.oic_client.cancelObserveWithGetMessage(self.get_protocol(protocol_type),
            self.get_coap_type(is_confirmable), message_id, ip, port, uri)
        self.oic_client.clearNotifications(self.get_protocol(protocol_type))

        target_request = self.oic_client.getLastRequest(self.get_protocol(protocol_type))
        target_response = response
        self.add_into_response_map(target_request, target_response)

        if response is None:
            self.print_testcase_get(DUT, TE, 'No Response for Observe Cancellation')
            return ""
        else:
            result = self.get_response_result(response)
            self.print_testcase_get(
                DUT,
                TE,
                'Response for Observation cancel',
                self.get_response_description(response))
        return result

    # Documentation for a function
    # @brief sends cancel observer in a passive manner (by not sending any ACK for Confirmable type notification)
    # @param protocol_type
    # @return void
    def cancel_observe_passively(self,protocol_type, href):
        self.print_testcase_do(TE, DUT, 'Cancelling Observe by not sending ACK')
        self.oic_client.cancelObservePassively(self.get_protocol(protocol_type), href)
        self.oic_client.clearNotifications(self.get_protocol(protocol_type))

    # Documentation for a function
    # @brief Cancels observe in an active manner (by sending RST message in response * to Notifications)
    # @param protocol_type
    # @return void
    def cancel_observe_reset(self,protocol_type, href):
        self.print_testcase_do(TE, DUT, 'Cancelling Observe by sending RESET Message')
        self.oic_client.cancelObserveWithReset(self.get_protocol(protocol_type), href)
        self.oic_client.clearNotifications(self.get_protocol(protocol_type))

    # Documentation for a function
    # @brief Discards the notifications in the local buffer
    # @param protocol_type
    # @return void
    def clear_notifications(self,protocol_type):
        self.oic_client.clearNotifications(self.get_protocol(protocol_type))

    # Parsing
    def get_request_value(self, request, key):
        if request is not None:
            return request.getRequestValue(key)

    def get_response_value(self, response, key):
        if response is not None:
            return response.getResponseValue(key)

    def check_type_in_list(self, to_check, type_list):
        result = False
        for resource_type in type_list:
            if resource_type == to_check:
                result = True
        return result

    # Documentation for a function
    # @brief get observe response
    # @param protocol_type
    # @return response observe as string
    def get_observe_response(self, protocol_type, href):
        notification_list = self.oic_client.getNotifications(self.get_protocol(protocol_type), href)
        result = ''
        if notification_list.size() > 0:
            for notification in notification_list:
                response_observe = self.get_response_result(notification)
                self.print_testcase_get(
                    DUT, TE, 'Response for Observation', self.get_response_description(notification))
                result = result + response_observe
        else:
            result = ''
        return result

    # Documentation for a function
    # @brief get description of response
    # @param input_response
    # @return response_value as log string
    def get_response_description(self, input_response):
        logs = RESPONSE_CODE + \
            self.get_response_value(input_response, OICHelper.MessageParameters.responseCode) + '\n   '
        logs += SOURCE_ADDRESS + self.get_response_value(
            input_response, OICHelper.MessageParameters.srcAddress) + ':' + self.get_response_value(
            input_response, OICHelper.MessageParameters.srcPort) + '\n   '
        logs += PAYLOAD + \
            OICHelper.getPrettyJson(self.get_response_value(input_response, OICHelper.MessageParameters.payload)) + '\n   '
        return logs

    # Documentation for a function
    # @brief get description of request
    # @param input_response
    # @return request_value as log string
    def get_request_description(self, input_request):
        logs = REQUEST_CODE + \
            self.get_request_value(input_request, OICHelper.MessageParameters.requestCode) + '\n   '
        logs += SOURCE_ADDRESS + self.get_request_value(
            input_request, OICHelper.MessageParameters.srcAddress) + ':' + self.get_request_value(
            input_request, OICHelper.MessageParameters.srcPort) + '\n   '
        logs += PAYLOAD + \
            OICHelper.getPrettyJson(self.get_request_value(input_request, OICHelper.MessageParameters.payload)) + '\n   '
        return logs

    # Documentation for a function
    # @brief get response code
    # @param input_response
    # @return response_code as result
    def get_response_code(self, input_response):
        self.result = input_response.split("\n", 9)
        return self.result[0]

    # Documentation for a function
    # @brief get response json
    # @param input_response
    # @return response json
    def get_response_json(self, input_response):
        self.result = input_response.split("\n", 9)
        return self.result[1]

    # Documentation for a function
    # @brief get response ip
    # @param input_response
    # @return response ip as result
    def get_response_ip(self, input_response):
        self.result = input_response.split("\n", 9)
        return self.result[2]

    # Documentation for a function
    # @brief get response port
    # @param input_response
    # @return response port as result
    def get_response_port(self, input_response):
        self.result = input_response.split("\n", 9)
        return self.result[3]

    # Documentation for a function
    # @brief get response token
    # @param input_response
    # @return response token as result
    def get_response_token(self, input_response):
        self.result = input_response.split("\n", 9)
        return self.result[5]

    # Documentation for a function
    # @brief get response msgid
    # @param input_response
    # @return msgid as result
    def get_response_msgid(self, input_response):
        self.result = input_response.split("\n", 9)
        return self.result[4]

    # Documentation for a function
    # @brief get response type
    # @param input_response
    # @return type of response as result
    def get_response_type(self, input_response):
        self.result = input_response.split("\n", 9)
        return self.result[6]

    # Documentation for a function
    # @brief get response type
    # @param input_response
    # @return type of response as result
    def get_response_secured_port(self, input_response):
        self.result = input_response.split("\n", 9)
        return self.result[7]

    # Documentation for a function
    # @brief get content format
    # @param input_response
    # @return type of response as result
    def get_response_content_format(self, input_response):
        self.result = input_response.split("\n", 9)
        #self.log_to_console("Content format" + self.result[8])
        return self.result[8]

    # Documentation for a function
    # @brief get response code
    # @param input_response
    # @return type of response as result

    def get_request_code(self, input_request):
        self.result = input_request.split("\n", 12)
        return self.result[0]

    # Documentation for a function
    # @brief get request json
    # @param input_request
    # @return request json as result
    def get_request_json(self, input_request):
        self.result = input_request.split("\n", 12)
        return self.result[1]

    # Documentation for a function
    # @brief get request ip
    # @param input_request
    # @return request ip as result
    def get_request_ip(self, input_request):
        self.result = input_request.split("\n", 12)
        return self.result[2]

    # Documentation for a function
    # @brief get request port
    # @param input_request
    # @return request port as result
    def get_request_port(self, input_request):
        self.result = input_request.split("\n", 12)
        return self.result[3]

    # Documentation for a function
    # @brief get request token
    # @param input_request
    # @return request token as result
    def get_request_token(self, input_request):
        self.result = input_request.split("\n", 12)
        return self.result[5]

    # Documentation for a function
    # @brief get request msgid
    # @param input_request
    # @return request msgid as result
    def get_request_msgid(self, input_request):
        self.result = input_request.split("\n", 12)
        return self.result[4]

    # Documentation for a function
    # @brief get request observe
    # @param input_request
    # @return request observe as result
    def get_request_observe(self, input_request):
        self.result = input_request.split("\n", 12)
        return self.result[6]

    # Documentation for a function
    # @brief get request type
    # @param input_request
    # @return request type as result
    def get_request_type(self, input_request):
        self.result = input_request.split("\n", 12)
        return self.result[7]

    # Documentation for a function
    # @brief get request type
    # @param input_request
    # @return request type as result
    def get_request_href(self, input_request):
        self.result = input_request.split("\n", 12)
        return self.result[8]

    # Documentation for a function
    # @brief get request type
    # @param input_request
    # @return request type as result
    def get_request_query(self, input_request):
        self.result = input_request.split("\n", 12)
        return self.result[9]

    # Documentation for a function
    # @brief get request content-format
    # @param input_request
    # @return content format as result
    def get_request_content_format(self, input_request):
        self.result = input_request.split("\n", 12)
        return self.result[10]

    # Documentation for a function
    # @brief get request accept
    # @param input_request
    # @return accept as result
    def get_request_accept(self, input_request):
        self.result = input_request.split("\n", 12)
        return self.result[11]

    # Documentation for a function
    # @brief get dut description
    # @param dut_id
    # @return request DUTInformation as result
    def get_dut_description(self, dut_id):
        return self.dut_information_manager.getDUTInformation(dut_id)

    # Documentation for a function
    # @brief get dut description
    # @param dut_id
    # @return request DUTInformation as result
    def get_dut_info_value(self, key, dut_id, query=""):
        if not dut_id:
            dut_id = self.dut_id
        self.dut_information = self.dut_information_manager.getDUTInformation(dut_id)
        if key == IotivityKey.DI.toString():
            return self.dut_information.getDi()
        if self.dut_information is not None:
            self.result = self.dut_information.getResourceValue(key, str(query))
        else:
            self.result = "DUT Information is NULL"
            self.log_to_console(self.result)
        return self.result


    # Documentation for a function
    # @brief get dut description
    # @param dut_id
    # @return request DUTInformation as result
    def check_all_attributes_exist(self, href, dut_id, payload):
        error_msg = ''
        key = REP.lower()
        query = HREF + "=" + href
        payload_json = JsonAnalyzer(payload)
        dut_representation = self.get_dut_info_value(key, dut_id, query)
        if dut_representation is None:
            error_msg = "Resource representation with uri: " + href + "does not exist!!"
            return error_msg
        
        for attribute_unicode in dut_representation:
            attribute_unicode = attribute_unicode.replace('true', 'True')
            attribute_unicode = attribute_unicode.replace('false', 'False')
            attribute = ast.literal_eval(attribute_unicode)
            attribute_key = attribute[ATTRIBUTE_KEY]
            attribute_type = attribute[ATTRIBUTE_VALUE_TYPE]
            value_list = payload_json.getValue(attribute_key)
            if len(value_list) is 0:
                error_msg = error_msg + "Attribute " + attribute_key + " is not present in resource with href: " + href + ". "
            else:
                attribute_value = value_list[0]
                if attribute_type == ATTRIBUTE_TYPE_INT and type(attribute_value) is not int:
                    error_msg = attribute_key+" attribute " + attribute_key + " mismatches. Expected type was integer "
                elif attribute_type == ATTRIBUTE_TYPE_FLOAT and type(attribute_value) is not float:
                    error_msg = attribute_key+" attribute " + attribute_key + " mismatches. Expected type was float "
                elif attribute_type == ATTRIBUTE_TYPE_BOOL and type(attribute_value) is not bool:
                    error_msg = attribute_key+" attribute " + attribute_key + " mismatches. Expected type was bool "
                elif attribute_type == ATTRIBUTE_TYPE_STRING and type(attribute_value) is not str:
                    error_msg = attribute_key+" the attribute " + attribute_key + " mismatches. Expected type was string "
                elif attribute_type == ATTRIBUTE_TYPE_ARRAY and type(attribute_value) is not list:
                    error_msg = attribute_key+" the attribute " + attribute_key + " mismatches. Expected type was array "

        return error_msg

    # Documentation for a function
    # @brief get dut description
    # @param dut_id
    # @return request DUTInformation as result
    def check_str_attributes_length(self, href, dut_id, payload):
        error_msg = ''
        key = REP.lower()
        query = HREF + "=" + href
        payload_json = JsonAnalyzer(payload)
        dut_representation = self.get_dut_info_value(key, dut_id, query)
        if dut_representation is None:
            error_msg = "Resource representation with uri: " + href + "does not exist!!"
            return error_msg

        for attribute_unicode in dut_representation:
            attribute_unicode = attribute_unicode.replace('true', 'True')
            attribute_unicode = attribute_unicode.replace('false', 'False')
            attribute = ast.literal_eval(attribute_unicode)
            attribute_key = attribute[ATTRIBUTE_KEY]
            attribute_type = attribute[ATTRIBUTE_VALUE_TYPE]
            value_list = payload_json.getValue(attribute_key)
            if attribute_type is ATTRIBUTE_TYPE_STRING :
                if len(value_list) is 0:
                    error_msg = error_msg + "Attribute " + attribute_key + " is not present in resource with href: " + href + ". "
                else:
                    attribute_value = value_list[0]
                    if type(attribute_value) is not str:
                        error_msg = "Value of the attribute " + attribute_key + " mismatches. Expected type was string, Actual type is: " + type(attribute_value) + ". "
                    if len(attribute_value.encode('utf-8')) > 64 :
                        error_msg = "length of the attribute " + attribute_key + " is over than 64. "
        return error_msg


    # Documentation for a function
    # @brief get dut resource value
    # @param dut_id, href_query, key, accessmode
    # @return DUTResourceValue
    def get_dut_resource_value(self, dut_id, href_query, key, accessmode):
        self.dut_information = self.dut_information_manager.getDUTInformation(
            dut_id)
        dut_resource = self.dut_information.getResourceWithQuery(href_query)
#        print "Tuhin"
#        print href_query
        if key == "rep" or key == REP:
            rep_list = dut_resource.get(0).getDUTResourceValue("rep")
	    dict_list =[]
	    for rep in rep_list:
	        #print rep
	        rep = str(rep)
                json_analyzer = JsonAnalyzer(rep)

                tmp_list = re.findall(r"[\w'.]+",rep)
	        key_value=[]
#                if accessmode == 'W':
#                    accessmode = 'RW'
                if accessmode == json_analyzer.getValue("accessmode").get(0):
                    key_value.append(json_analyzer.getValue("key").get(0))
	            key_value.append(json_analyzer.getValue("type").get(0))
                    if json_analyzer.getValue("testdata").size() >1 :
                        key_value.append(json_analyzer.getValue("testdata").toString())
                    else:
                        key_value.append(json_analyzer.getValue("testdata").get(0))

#	        print "Key value pair"
#                print key_value
	            dict_list.append(key_value)
	    	#print "dict_list"
	#print dict_list
	    return dict_list
        else:
	    return dut_resource.get(0).getDUTResourceValue(key)


    # Documentation for a function
    # @brief get dut resource value
    # @param dut_id, href_query, key, accessmode
    # @return DUTResourceValue
    def get_dut_resource_value2(self, dut_id, href_query, key, accessmode):
        self.dut_information = self.dut_information_manager.getDUTInformation(
            dut_id)
        dut_resource = self.dut_information.getResourceWithQuery(href_query)
#        print "Tuhin"
#        print href_query
        if key == "rep" or key == REP:
            rep_list = dut_resource.get(0).getDUTResourceValue("rep")
            dict_list =[]
            for rep in rep_list:
                #print rep
                rep = str(rep)
                json_analyzer = JsonAnalyzer(rep)

                tmp_list = re.findall(r"[\w'.]+",rep)
                key_value=[]
#                if accessmode == 'W':
#                    accessmode = 'RW'
                if accessmode == json_analyzer.getValue("accessmode").get(0):
                    key_value.append(json_analyzer.getValue("key").get(0))
                    key_value.append(json_analyzer.getValue("type").get(0))
                    if json_analyzer.getValue("testdata").size() >1 :
                        key_value.append(json_analyzer.getValue("testdata").toString())
                    else:
                        key_value.append(json_analyzer.getValue("testdata").get(0))

                    if json_analyzer.getValue("testdata2").size() >1 :
                        key_value.append(json_analyzer.getValue("testdata2").toString())
                    else:
                        key_value.append(json_analyzer.getValue("testdata2").get(0))


#               print "Key value pair"
#                print key_value
                    dict_list.append(key_value)
                #print "dict_list"
        #print dict_list
            return dict_list
        else:
            return dut_resource.get(0).getDUTResourceValue(key)




    # Documentation for a function
    # @brief get dut resource value
    # @param dut_id, href_query, key, accessmode
    # @return DUTResourceValue
    def get_rep_from_dut(self, dut_id, href_query):
        self.dut_information = self.dut_information_manager.getDUTInformation(
            dut_id)
        dut_resource = self.dut_information.getResourceWithQuery(href_query)

        rep_list = dut_resource.get(0).getDUTResourceValue("rep")
        self.log_to_console("rep_list: " + str(rep_list))
        dict_list = {}
        if rep_list is not None:
            for rep in rep_list:
                #print rep
                rep = str(rep)
                json_analyzer = JsonAnalyzer(rep)

                attr_key = json_analyzer.getValue("key").get(0)
                if json_analyzer.getValue("testdata").size() >1 :
                    attr_value = json_analyzer.getValue("testdata").toString()
                else:
                    attr_value = json_analyzer.getValue("testdata").get(0)
                attr_value = json_analyzer.getValue("testdata").get(0)

                dict_list[attr_key] = attr_value

        return dict_list

    def check_discovery_response(
            self,
            key,
            dut_id,
            response_string,
            response_format,
            query="",
            false=True):
        self.dut_information = self.dut_information_manager.getDUTInformation(
            dut_id)
            
#        self.log_to_console(" key= " + str(key)+ " dui_id="+str(dut_id)+" response_string="+str(response_string)+" response_format="+str(response_format))
        dut_resources = self.dut_information.getResourceWithQuery(query)
        resourceList = ArrayList()
        for dut_resource in dut_resources:
#            self.log_to_console(" resource found = " + str(dut_resource.getDUTResourceValue(key)))
            resourceList.add(dut_resource)

        for dut_resource in dut_resources:
            is_equal = self.is_equal_json_value(
                dut_resource.getDUTResourceValue(key), self.get_json_value(
                    key, response_string, dut_resource.getDUTResourceValue(IotivityKey.HREF.toString()).get(0)))

            if is_equal:
                resourceList.remove(dut_resource)
                if not false:
                    BuiltIn().fail("Invalid resource is found")

        if resourceList.size() != 0:
            not_found_resource_list = ""
            for not_found_resource in resourceList:
                not_found_resource_list += str(
                    not_found_resource.getDUTResourceValue(IotivityKey.HREF.toString()).toString()) + " ,"
            if false:
                BuiltIn().fail(not_found_resource_list + " is not found")


    def check_oic_defined_resource_discovery(
            self,
            key,
            dut_id,
            response_string,
            response_format,
            query="",
            false=True):
        self.oic_information = self.dut_information_manager.getDUTInformation(OIC_ID)
            
#        self.log_to_console(" key= " + str(key)+ " dui_id="+str(dut_id)+" response_string="+str(response_string)+" response_format="+str(response_format))
        oic_resources = self.oic_information.getResourceWithQuery(query)
        resourceList = ArrayList()
        for oic_resource in oic_resources:
#            self.log_to_console(" resource found = " + str(dut_resource.getDUTResourceValue(key)))
            resourceList.add(oic_resource)

        for oic_resource in oic_resources:
            is_equal = self.is_equal_json_value(
                oic_resource.getDUTResourceValue(key), self.get_json_value(
                    key, response_string, oic_resource.getDUTResourceValue(IotivityKey.HREF.toString()).get(0)))

            if is_equal:
                resourceList.remove(oic_resource)
                if not false:
                    BuiltIn().fail("Invalid resource is found")

        if resourceList.size() != 0:
            not_found_resource_list = ""
            for not_found_resource in resourceList:
                not_found_resource_list += str(
                    not_found_resource.getDUTResourceValue(IotivityKey.HREF.toString()).toString()) + " ,"
            if false:
                BuiltIn().fail(not_found_resource_list + " is not found")


    # Documentation for a function
    # @brief get json value
    # @param key, json_string, href=none
    # @return json_value as string
    def get_json_value(self, key, json_string, href=None, is_value_string=None):
        json_analyzer = JsonAnalyzer(json_string)
 #       print "json_string"
 #       print json_string
 #       print "key"
 #       print key
        if is_value_string is not None:
            return json_analyzer.getValue(key).get(0)
        if href is None:
            return json_analyzer.getValue(key)
        else:
            return json_analyzer.getValueWithHref(href, key)

    # Documentation for a function
    # @brief compare json two value
    # @param value1, value2
    # @return bool(true/false)
    def is_equal_json_value(self, values1, values2):
        clone_values1 = values1.clone()
        if clone_values1.size() == values2.size():
            for value1 in values1:
                if values2.contains(value1):
                    clone_values1.remove(value1)
        if clone_values1.size() == 0:
            return True
        else:
            return False

    # Documentation for a function
    # @brief validates json format
    # @param json_string
    # @return boolean
    def validate_json_format(self, json_string):
        if json_string == '' :
            json_string = "{}"
        json_analyzer = JsonAnalyzer(json_string)
        return json_analyzer.validate()


    # Documentation for a function
    # @brief validates json type
    # @param json_string, resource_href
    # @return boolean
    def validate_payload_type(self, payload_string, href):
        self.dut_information = self.dut_information_manager.getDUTInformation(self.dut_id)
        paylaod_object_type = self.dut_information.getPayloadType(href)
        first_char = payload_string[0]
        last_char = payload_string[len(payload_string) - 1]
        if paylaod_object_type == PayloadType.OBJECT and first_char == '{' and last_char == '}':
            return True
        elif paylaod_object_type == PayloadType.ARRAY and first_char == '[' and last_char == ']':
            return True
        else:
            return False

    # Documentation for a function
    # @brief validates json type
    # @param json_string, resource_href
    # @return boolean
    def validate_discovery_payload_type(self, payload_string, href):
        errorMsg = ''
        self.oicInformation = self.dut_information_manager.getDUTInformation(OIC_ID)
        paylaod_object_type = self.oicInformation.getPayloadType(href)
        first_char = payload_string[0]
        last_char = payload_string[len(payload_string) - 1]
        if paylaod_object_type == PayloadType.OBJECT and first_char == '[' and last_char == ']':
            errorMsg = 'Payload format mismatch. Expected Object type payload, Received Array type payload.'
        elif paylaod_object_type == PayloadType.ARRAY and first_char == '{' and last_char == '}':
            errorMsg = 'Payload format mismatch. Expected Array type payload, Received Object type payload.'
        return errorMsg

    # Documentation for a function
    # @brief gets protocol
    # @param protocol_type
    # @return protocol(enum)
    def get_protocol(self, protocol_type):
        if protocol_type == COAP:
            protocol = OICClient.Protocol.COAP
        else:
            protocol = OICClient.Protocol.HTTP
        return protocol

    # Documentation for a function
    # @brief gets payload type
    # @param payload_type
    # @return payload_type(enum)        
    def get_payload_type(self, payload_type):
        if payload_type == REP:
            return OCPayloadType.PAYLOAD_TYPE_REPRESENTATION
        elif payload_type == SECURITY:
            return OCPayloadType.PAYLOAD_TYPE_SECURITY
        
    def get_coap_type(self, is_confirmable):
        if is_confirmable:
            return OICHelper.MessageType.CON
        else:
            return OICHelper.MessageType.NON

    def get_response_result(self, response):
        if response is None:
            response_result = ''
        else:
            response_result = self.get_response_value(response, OICHelper.MessageParameters.responseCode)
            response_result = response_result + "\n" + \
                self.get_response_value(response, OICHelper.MessageParameters.payload)
            response_result = response_result + "\n" + \
                self.get_response_value(response, OICHelper.MessageParameters.srcAddress)
            response_result = response_result + "\n" + \
                self.get_response_value(response, OICHelper.MessageParameters.srcPort)
            response_result = response_result + "\n" + \
                self.get_response_value(response, OICHelper.MessageParameters.mId)
            response_result = response_result + "\n" + \
                self.get_response_value(response, OICHelper.MessageParameters.token)
            response_result = response_result + "\n" + \
                self.get_response_value(response, OICHelper.MessageParameters.msgType)
            response_result = response_result + "\n" + \
                self.get_response_value(response, OICHelper.MessageParameters.secPort)
            content_format = self.get_response_value(response, OICHelper.MessageParameters.contentFormat)
            if content_format is None:
				content_format = "UNSPECIFIED"
            response_result = response_result + "\n" + \
                content_format
        return response_result

    # Documentation for a function
    # @brief gets request result
    # @param request
    # @return payload_type(enum)
    def get_request_result(self, request):
        request_code = self.get_request_value(request, OICHelper.MessageParameters.requestCode)
        if request_code is not None:
            request_result = request_code
        else:
            request_result = ""
        request_payload = self.get_request_value(request, OICHelper.MessageParameters.payload)
        if request_payload is not None:
            request_result = request_result + "\n" + request_payload
        else:
            request_result = request_result + "\n" + ""
        request_ip = self.get_request_value(request, OICHelper.MessageParameters.destAddress)
        if request_ip is not None:
            request_result = request_result + "\n" + request_ip
        else:
            request_result = request_result + "\n" + ""
        request_port = self.get_request_value(request, OICHelper.MessageParameters.destPort)
        if request_port is not None:
            request_result = request_result + "\n" + request_port
        else:
            request_result = request_result + "\n" + ""
        request_mid = self.get_request_value(request, OICHelper.MessageParameters.mId)
        if request_mid is not None:
            request_result = request_result + "\n" + request_mid
        else:
            request_result = request_result + "\n" + ""
        request_token = self.get_request_value(request, OICHelper.MessageParameters.token)
        if request_token is not None:
            request_result = request_result + "\n" + request_token
        else:
            request_result = request_result + "\n" + ""
        request_obseve_flag = self.get_request_value(request, OICHelper.MessageParameters.observeFlag)
        if request_obseve_flag is not None:
            request_result = request_result + "\n" + request_obseve_flag
        else:
            request_result = request_result + "\n" + ""
        request_msg_type = self.get_request_value(request, OICHelper.MessageParameters.msgType)
        if request_msg_type is not None:
            request_result = request_result + "\n" + request_msg_type
        else:
            request_result = request_result + "\n" + ""
#        self.log_to_console(" request_result = " + request_result)
        request_href = self.get_request_value(request, OICHelper.MessageParameters.uri)
        if request_href is not None:
            request_result = request_result + "\n" + request_href
        else:
            request_result = request_result + "\n" + ""
        request_query = self.get_request_value(request, OICHelper.MessageParameters.query)
        if request_query is not None:
            request_result = request_result + "\n" + request_query
        else:
            request_result = request_result + "\n" + ""
        content_format = self.get_request_value(request, OICHelper.MessageParameters.contentFormat)
        if content_format is not None:
            request_result = request_result + "\n" + content_format
        else:
            request_result = request_result + "\n" + ""
        accept_format = self.get_request_value(request, OICHelper.MessageParameters.accept)
        if accept_format is not None:
            request_result = request_result + "\n" + accept_format
        else:
            request_result = request_result + "\n" + ""
        return request_result

    def create_json_representation(self):
        self.json_representation = HashMap()
        self.rep_map = JsonData()

    def add_into_json_representation(self, key, value, data_type=None):
        if data_type == "int" or data_type == "integer":
            value_to_insert = int(value)
        elif data_type == "float" or data_type== "number":
            value_to_insert = float(value)
        elif data_type == "bool" or data_type == "boolean":
            value_to_insert = Boolean.valueOf(value)
        elif data_type == "array":
            json_array = JsonData(value)
            value_to_insert = json_array.toArray()
        else:
            value_to_insert = value
        self.rep_map.put(key , value_to_insert )

    def get_json_string(self):
#        key = IotivityKey.REP.toString()
        intermediate_representation = HashMap()
#        intermediate_representation.put(key, self.json_representation)
        key = IotivityKey.ROOT.toString()
        final_representation = JsonData()
        final_representation.put(key, intermediate_representation)
        json_string = final_representation.toString()
        what = CREATED_PAYLOAD + json_string + '\n'
        return json_string

    def validate_unique_value_by_key(self, json, key):
        json_analyzer = JsonAnalyzer(json)
        valuelist = json_analyzer.getAllValueFromJson(key)
        if len(valuelist) != len(set(valuelist)):
            resource_rep_list = json_analyzer.getResourceJson()
            if len(resource_rep_list) != len(set(resource_rep_list)):
                return False
            else:
                return True
        else:
            return False

    def check_start_with_slash(self, json_string, key):
        json_analyzer = JsonAnalyzer(json_string)
        listofvalues = json_analyzer.getValue(key)
        for i in range(len(listofvalues)):
            if listofvalues[i][0] == "/":
                return True
            else:
                return False

   #This API needs test. 
   #Related TC is CT1_4_2
    def check_value_contains_pattern(self, json, key, pattern):
        json_analyzer = JsonAnalyzer(json)
        listofvalues = json_analyzer.getValue(key)
        for i in range(len(listofvalues)):
            if listofvalues[i].find(pattern) != 1:
                return listofvalues[i]

    def get_json_representation(self):
#        if self.rep_map.isEmpty():
#            self.prop_map.put("rt", self.resource_type)
#            self.prop_map.put("dis", self.resource_dis)
#            self.rep_map.put("sample_key", "sample_value")
#        if self.resource_interface_array.size() != 0:
#            self.prop_map.put("if", self.resource_interface_array)

#        final_representation = JsonData()

#        if self.resource_href != None:
#            final_representation.put("href", self.resource_href)
#        if not self.prop_map.isEmpty():
#            self.prop_map.put("rep", self.rep_map)
#            final_representation.put("prop", self.prop_map)
#        else:    
#            final_representation.put("rep", self.rep_map)
#        temp = ArrayList()
#        temp.add(final_representation)
#        json_string = final_representation.toString()
#        json_string = temp.toString()
        json_string = self.rep_map.toString()
        what = CREATED_PAYLOAD + json_string + '\n'
               
        return json_string

    def get_false(self):
        return False

    def get_true(self):
        return True
        
    def get_multicast_discovery_request_list(self, protocol, uri, query=None):
    	if query == None:
    		query = ''
        return self.provision_manager.getMulticastDiscoveryResponse(self.get_protocol(protocol), uri, query)

    def get_multicast_discovery_response_code(self, response):
        return self.provision_manager.getResponseCode(response)
        
    def get_secured_server_ip(self, response):
        return self.provision_manager.getSecuredServerIp(response)
    
    def get_secured_server_port(self, response):
        return self.provision_manager.getSecuredServerPort(response)
        
    def get_multicast_discovery_response_paylaod(self, response):
        return self.provision_manager.getResponsePayload(response)
    
    def get_response_payload(self, response):
        self.result = response.split("\n", 8)
        return self.result[1]
           
    def get_rep_value_from_response(self, key, response):
    	json_analyzer = JsonAnalyzer(response)
    	rep = json_analyzer.getValue("rep")
    	if not rep.isEmpty():
    		json_analyzer_1 = JsonAnalyzer(rep.get(0))
    		device_id = json_analyzer_1.getValue(key)
    		if not device_id.isEmpty():
    			return device_id.get(0)
    		else:
    			return ''
    	else:
    		return ''

    def get_device_id_from_response(self, response):
    	id = self.get_rep_value_from_response("deviceid", response)
    	return id
    		
    def get_oxm_list_from_response(self, response):
    	oxm_list = self.get_rep_value_from_response("oxm", response)
    	return oxm_list

    def check_oxm_list_contains_atleast_justworks_or_randompin_method(self, response):
        oxm_list = self.get_rep_value_from_response("oxm", response)
        justworks_method = '0'
        randompin_method = '1'
        
        if justworks_method in oxm_list or randompin_method in oxm_list:
            return True 
        else:
            return False

    def get_sm_property_from_response(self, response):
        sm = self.get_rep_value_from_response("sm", response)
        return sm

    def get_om_property_from_response(self, response):
        om = self.get_rep_value_from_response("om", response)
        return om

    def get_oxm_selection_from_response(self, response):
        oxm = self.get_rep_value_from_response("oxmsel", response)
        return oxm

    def get_owned_status_from_response(self, response):
        owned_status = self.get_rep_value_from_response("owned", response)
        return owned_status

    def get_device_owner_from_response(self, response):
        ownr = self.get_rep_value_from_response("ownr", response)
        return ownr

    def wait_for_user_confirmation(self, msg, option_list=None):
        "Waiting for User Input"
        sentence = ['\n<------------ ', msg]
        if (option_list != None):
            sentence += [' [', option_list, ']']
            option_list = option_list.split(" ")
        else:
            option_list = []
        sentence += [' and Press Enter',' ------------>\n']
        if self.is_cli_enabled():
            self.log_to_console("".join(sentence))
            return raw_input()
        else:	
            return self.ui.showInputDialog(msg, option_list)

    def print_message(self, msg):
        "Waiting for User Input"
        sentence = ['\n<------------ ', msg, ' ------------>\n']
        if self.is_cli_enabled():
            self.log_to_console("".join(sentence))
        else:
            self.ui.showMessageDialog(msg)

    def get_reset_token(self, href):
        return self.resource_map[href].getResetToken()

    def get_payload_from_file(self, payload_type, file_path):
        "Read file and get particular payload"
        payload_file = File(file_path)
        json_analyzer = JsonAnalyzer(payload_file)
        return json_analyzer.getJsonValueAsString(payload_type)

    def get_wireshark_file_name(self, tc_name):
        "Read testcase name and return wireshark file name"
        file_name = tc_name.replace(" - ","-")
        file_name = file_name.replace(".","_")
        file_name = file_name.replace(" ","_")
        time_stamp = datetime.datetime.now().strftime('_%Y-%m-%d_%H:%M:%S')
        file_name = file_name + time_stamp + ".pcapng"
        return file_name

    def is_crudn_operation_supported(self, href, key):
        self.dut_information = self.dut_information_manager.getDUTInformation(self.dut_id)
        self.dut_information.generateCRUDNState(href)
        is_supported = False
        if key == "COMPLETE_CREATE":
            is_supported = self.dut_information.getCRUDNState(href, CRUDNType.COMPLETE_CREATE)
        elif key == "COMPLETE_CREATE":
            is_supported = self.dut_information.getCRUDNState(href, CRUDNType.SUBORDINATE_CREATE)
        elif key == "RETRIEVE":
            is_supported = self.dut_information.getCRUDNState(href, CRUDNType.RETRIEVE)
        elif key == "COMPLETE_UPDATE":
            is_supported = self.dut_information.getCRUDNState(href, CRUDNType.COMPLETE_UPDATE)
        elif key == "PARTIAL_UPDATE":
            is_supported = self.dut_information.getCRUDNState(href, CRUDNType.PARTIAL_UPDATE)
        elif key == "DELETE":
            is_supported = self.dut_information.getCRUDNState(href, CRUDNType.DELETE)
        else:
            is_supported = False
        return is_supported

    def update_representation(self, href):
        "updates resource representation"
        self.resource_map[href].updateResourceRespresentation()

    def get_resource_interface(self, href):
        self.dut_information = self.dut_information_manager.getDUTInformation(self.dut_id)
        return  self.dut_information.getInterfaceList(href)

    def get_modified_json_string(self, json_string, key, value):
        json_analyzer = JsonAnalyzer(json_string)
        return  json_analyzer.getModifiedJsonString(key, value)

    def update_payload(self, payload, key, value):
        if value == 'none':
            return payload
        key_index = payload.find(key)
        first_index = payload.find(':', key_index+1)
        last_index = payload.find(',', first_index+1)
        if last_index == -1:
            last_index = payload.find('}', first_index+1)
        old_value = payload[first_index+1 : last_index]
        payload = payload[:first_index+1] + str(value) + payload[last_index:]
        return payload

    def check_data_model_version(self, value):
        error_msg = None
        if value == '':
            error_msg = "Data Model Version value is Empty."
            return error_msg
        parts = value.split('.')
        if len(parts) > 4:
            error_msg = "Data Model Version contains more than 4  dot separated values."
        elif len(parts) < 4:
            error_msg = "Data Model Version contains less than 4  dot separated values."
        return error_msg

    def check_spec_version(self, value):
        error_msg = None
        if value == '':
            error_msg = "Data Model Version value is Empty."
            return error_msg
        parts = value.split('.')
        if len(parts) > 4:
            error_msg = "Spec Version contains more than 4  dot separated values."
        elif len(parts) < 4:
            error_msg = "Spec Version contains less than 4  dot separated values."
        if parts[0] != "core":
            error_msg += " First part of dmv is not core."
        return error_msg

## End of Device Managment Keywords ##

## Security Relay Controller Keywords  ##
    def intialize_dtls_connection(self, relay_ip, relay_port, dut_ip, dut_port_secured, cipher_suite):
        return self.control_client.initDtls(relay_ip, int(relay_port), dut_ip, int(dut_port_secured), int(cipher_suite))

    def intialize_dtls_connection_with_random_pin(self, relay_ip, relay_port, dut_ip, dut_port_secured, cipher_suite, client_di, server_di, pin):
        return self.control_client.initDtls(relay_ip, int(relay_port), dut_ip, int(dut_port_secured), int(cipher_suite), client_di, server_di, pin)

## End of Security Relay Controller Keywords ##

## Provisioning Keywords ##
    def discover_owned_device(self, filter_ip):
        return self.provisioning_client.discoverOwnedDevice(filter_ip)

    def discover_unowned_device(self, filter_ip):
        return self.provisioning_client.discoverUnownedDevice(filter_ip)

    def find_private_data(self, device_id_encoded):
        return self.provisioning_client.findprivate_data(device_id_encoded)

    def store_private_data(self, device_id_encoded, private_data):
        self.provisioning_client.storeprivate_data(device_id_encoded, private_data)

    def dtls_connect_with_psk(self, private_data, client_device_id, server_device_id, relay_ip, relay_control_port, dut_ip, dut_secured_port):
        return self.provisioning_client.dtlsConnectWithPSK(private_data, client_device_id, server_device_id, relay_ip, relay_control_port, dut_ip, dut_secured_port)

    def configure_device_for_provisioning(self, otm, client_device_id, server_device_id):
        self.provisioning_client.configureDeviceForProvisioning(otm, client_device_id, server_device_id)

    def provision_with_just_works(self, client_device_id, server_device_id, relay_ip, relay_control_port, dut_ip, dut_secured_port):
        private_data = self.provisioning_client.provisionWithJustWorks(client_device_id, server_device_id, relay_ip, relay_control_port, dut_ip, dut_secured_port)
        if private_data is not None:
            store_private_data(server_device_id, private_data)
            return True
        else:
            return False

    def provision_with_random_pin(self, pin, client_device_id, server_device_id, relay_ip, relay_control_port, dut_ip, dut_secured_port):
        private_data = self.provisioning_client.provisionWithRandomPin(pin, client_device_id, server_device_id, relay_ip, relay_control_port, dut_ip, dut_secured_port)
        if private_data is not None:
            store_private_data(server_device_id, private_data)
            return True
        else:
            return False

    def get_current_system_ip_address(self, use_ip6):
        return self.provisioning_client.getCurrentSystemIpAddress(use_ip6)

    def get_secured_port(self):
        return self.provisioning_client.getSecuredPort()

    def get_device_id(self):
        return self.provisioning_client.getDeviceId()

    def transfer_ownership(self, dut_ip):
        self.log_to_console("----------Discovering UnOwned Resource----------")
        unowned_secured_device = self.provisioning_client.discoverUnownedDevice(dut_ip)
        server_device_id = unowned_secured_device.getDeviceId();
        private_data = self.provisioning_client.findPrivateData(server_device_id)
        self.log_to_console("----------Stablishing DTLS Handshake----------")
        is_dtls_connected = self.provisioning_client.dtlsConnectWithPSK(private_data, CLIENT_DEVICE_ID, server_device_id, get_current_system_ip_address(False), RELAY_CONTROL_PORT, dut_ip, get_secured_port())
        if is_dtls_connected is True:
            self.log_to_console("----------DTLS Connection Stablished----------")
        else:
            self.log_to_console("----------Failed to Stablish DTLS Connection----------")
            return None
        self.log_to_console("----------Sending OwnerShip Transfer Request----------")
        response = self.provisioning_client.transferOwnership(client_device_id, server_device_id, relay_ip, relay_data_port)
        if response is None:
            self.print_testcase_get(DUT, TE, 'No Response')
            return ""
        else:
            result = self.get_response_result(response)
            self.print_testcase_get(
                DUT,
                TE,
                'Response for Ownership Transfer',
                self.get_response_description(response))
        return result

    def terminate_dtls_connection(self):
        self.provisioning_client.terminateDtls()

    def get_provisioning_client_device_id(self):
        return CLIENT_DEVICE_ID

    def get_relay_data_port(self):
        return RELAY_DATA_PORT

    def get_relay_control_port(self):
        return RELAY_CONTROL_PORT

    def send_acl_request(self, client_id, owner_id):
        self.log_to_console("----------Sending ACL Entry Request----------")
        payload = ACL_PAYLOAD.replace("ClientDeviceId", client_id)
        payload_to_send = payload.replace("OwnerDeviceId", owner_id)
        relay_ip = self.provisioning_client.getCurrentSystemIpAddress(False)
        response = self.oic_client.sendRequest(Protocol.COAP, MessageType.NON, Method.GET, \
            OICHelper.getRandomMessageIdString(), OICHelper.createTokenString(), relay_ip, RELAY_DATA_PORT, CORE_ACL_URI,  payload_to_send)
        if response is None:
            self.print_testcase_get(DUT, TE, 'No Response')
            return ""
        else:
            result = self.get_response_result(response)
            self.print_testcase_get(
                DUT,
                TE,
                'Response for ACL Entry',
                self.get_response_description(response))
        return result

    def initialize_secured_device(self, ip, port):
        self.log_to_console("Initializing Device for secured data connection")
        use_ip6 = False
        private_data = None
        server_device_id = None
        pin = None
        relay_ip = self.provisioning_client.getCurrentSystemIpAddress(use_ip6)
        dut_ip = ip
        owned_secured_device = self.provisioning_client.discoverOwnedDevice(dut_ip)

        if owned_secured_device is not None:
            self.log_to_console("Owned device found!")
            server_device_id = ownedSecuredDevice.getDeviceId();
            private_data = self.provisioning_client.findPrivateData(server_device_id)

            if private_data is not None:
                self.provisioning_client.dtlsConnectWithPSK(private_data, CLIENT_DEVICE_ID, server_device_id, relay_ip, RELAY_CONTROL_PORT, dut_ip, port)
            else:
                self.log_to_console("Credential for the owned device not found")
        else:
            unowned_secured_device = self.provisioning_client.discoverUnownedDevice(dut_ip)

            if unowned_secured_device is not None:
                self.log_to_console("UnOwned device found!")
                server_device_id = unowned_secured_device.getDeviceId();
                selected_otm = unowned_secured_device.getOtm();
                self.provisioning_client.configureDeviceForProvisioning(selected_otm, CLIENT_DEVICE_ID, server_device_id)

                if selected_otm is OTM.RANDOM_PIN:
                    self.log_to_console("Enter pin: ")
                    pin = self.wait_for_user_confirmation("Please enter random pin: ")
                    private_data = self.provisioning_client.provisionWithRandomPin(pin, CLIENT_DEVICE_ID, server_device_id, relay_ip, RELAY_CONTROL_PORT, dut_ip, port)
                elif selected_otm is OTM.JUST_WORKS:
                    private_data = self.provisioning_client.provisionWithJustWorks(CLIENT_DEVICE_ID, server_device_id, relay_ip, RELAY_CONTROL_PORT, dut_ip, port)

                self.provisioning_client.storePrivateData(server_device_id, private_data)
                self.provisioning_client.transferOwnership(CLIENT_DEVICE_ID, server_device_id, relay_ip, RELAY_DATA_PORT)
                self.send_acl_request(CLIENT_DEVICE_ID, CLIENT_DEVICE_ID)
                self.provisioning_client.terminateDtls()

            self.provisioning_client.dtlsConnectWithPSK(private_data, CLIENT_DEVICE_ID, server_device_id, relay_ip, RELAY_CONTROL_PORT, dut_ip, port)

## End of Provisioning Keywords ##


###start new keywork by m.parves ###
    def get_resource_status(self, dut_id, server_ip, server_port, href=None):
        self.dic_get_response ={}
        _hrefs = []
        if href is None:
            _hrefs = self.get_dut_info_value(HREF, dut_id)
        else:
            _hrefs.append(href)
        for _href in _hrefs:
            msgid = self.generate_message_id()
            token = self.generate_token()
            _response = self.send_get_request(UNICAST, COAP, msgid, token, server_ip, server_port, _href, True)
            _response_json = self.get_response_json(_response)
            self.dic_get_response[_href] = str(_response_json)
            
    def get_parameter_query(self, dut_id, server_ip, server_port):
        list_of_href = self.get_dut_info_value(HREF, dut_id)  
        list_of_query = []          
        for href in list_of_href:
            self.log_to_console('Href: ' + href)
            msgid = self.generate_message_id()
            token = self.generate_token()
            response = self.send_get_request(UNICAST, COAP, msgid, token, server_ip, server_port, href, True)
            self.log_to_console('Response: ' + response)
            response_json = self.get_response_json(response)
            payload = str(response_json)
            self.log_to_console('payload: ' + payload)
            payload_json = JsonAnalyzer(payload)
            self.dut_information = self.dut_information_manager.getDUTInformation(dut_id)
            dut_resource = self.dut_information.getResourceWithQuery(str('href=' + href))
            list_of_key = []
            if dut_resource.size() > 0:
                list_of_rep = dut_resource.get(0).getDUTResourceValue("rep")
                for rep in list_of_rep:
                    rep = str(rep)
                    self.log_to_console('Rep: ' + rep)
                    rep_json = JsonAnalyzer(rep)
                    list_of_key.append(rep_json.getValue("key").get(0))      
            for key in list_of_key:
                self.log_to_console('key: ' + key)
                list_of_value = payload_json.getValue(key)
                if list_of_value.size() > 0:
                    self.log_to_console('value: ' + list_of_value.get(0))
                    query = href + '?' + key + '=' + list_of_value.get(0)
                    list_of_query.append(query)
                    
        return list_of_query

    def make_partial_update_payload(self, dut_id, access, interface=None):
        self.hrefs=[]
        self.options=[]
        _hrefs = self.get_dut_info_value(HREF, dut_id) 
        for _href in _hrefs:
            query = str('href=' + _href)
            _options = self.get_dut_resource_value2(dut_id, query, 'rep', access)
            get_response = None

            if _href in self.dic_get_response:
                get_response = self.dic_get_response[_href]

            for _option in _options:
                self.create_json_representation()
                testdata = _option[2]
                
                if interface is None:
                    if get_response is not None and get_response != "":
                        current_value = self.get_json_value(_option[0], get_response)[0]
                        if current_value == _option[2]:
                            testdata = _option[3]

                    self.add_into_json_representation( _option[0], testdata, _option[1])
                    self.options.append(str(self.get_json_representation()))
                    self.hrefs.append(str(_href))
                else:
                    _interfaces = self.get_dut_info_value(interface, dut_id, HREF+'='+_href)
                    current_value = _option[3]
                    if get_response is not None and get_response !="":
                        current_value = self.get_json_value(_option[0], get_response)[0]
                    for _interface in _interfaces:
                        if current_value == _option[2]:
                            testdata = _option[3]
                            current_value = testdata
                        else:
                            testdata = _option[2]
                            current_value = testdata

                        self.add_into_json_representation( _option[0], testdata, _option[1])
                        self.options.append(str(self.get_json_representation()))
                        self.hrefs.append(str(_href+'?'+interface+'='+_interface))

    def make_complete_update_payload(self, dut_id, access, interface=None):
        self.hrefs=[]
        self.options=[]
        _hrefs = self.get_dut_info_value(HREF, dut_id)
        for _href in _hrefs:
            query = str('href=' + _href)
            _options = self.get_dut_resource_value2(dut_id, query, 'rep', access)
            get_response = None

            if _href in self.dic_get_response:
                get_response = self.dic_get_response[_href]

            self.create_json_representation()
            if interface is None:
                for _option in _options:
                    testdata = _option[2]

                    if get_response is not None and get_response != "":
                        current_value = self.get_json_value(_option[0], get_response)[0]
                        if current_value == _option[2]:
                            testdata = _option[3]

                    self.add_into_json_representation( _option[0], testdata, _option[1])
                self.options.append(str(self.get_json_representation()))
                self.hrefs.append(str(_href))

            else:
                _interfaces = self.get_dut_info_value(interface, dut_id, HREF+'='+_href)
                for _interface in _interfaces:
                    for _option in _options:
                        current_value = _option[3]
                        if get_response is not None and get_response !="":
                            current_value = self.get_json_value(_option[0], get_response)[0]
                        if current_value == _option[2]:
                            testdata = _option[3]
                        else:
                            testdata = _option[2]

                        self.add_into_json_representation( _option[0], testdata, _option[1])
                    get_response = str(self.get_json_representation())
                    self.options.append(get_response)
                    self.hrefs.append(str(_href+'?'+interface+'='+_interface))

    def get_complete_update_hrefs(self):
        return self.hrefs

    def get_complete_update_payloads(self):
        return self.options

    def get_partial_update_hrefs(self):
        return self.hrefs
			
    def get_partial_update_payloads(self):
        return self.options

    def make_query(self, dut_id, key):
        queries = []
        hrefs = self.get_dut_info_value(HREF, dut_id) 
        for href in hrefs:
            query_str = HREF+'='+str(href)
            options = self.get_dut_info_value(key, dut_id, str(query_str)) 
            for option in options:
                query = href + '?'+key+'='+option
                queries.append(str(query))
        return queries
        
    def make_query_with_defined_uris(self, dut_id, key, *hrefs):
        queries = []
        for href in hrefs:
            query_str = HREF+'='+str(href)
            options = self.get_dut_info_value(key, dut_id, str(query_str)) 
            for option in options:
                query = href + '?'+key+'='+option
                queries.append(str(query))
        return queries

    def get_list_of_oic_core_resource(self, rt=None):
#        oic_resources = {RESOURCE_DISCOVERY_RT:RESOURCE_DISCOVERY_URI,DEVICE_DISCOVERY_RT:DEVICE_DISCOVERY_URI,PLATFORM_DISCOVERY_RT:PLATFORM_DISCOVERY_URI}
        oic_resources = {DEVICE_DISCOVERY_RT:DEVICE_DISCOVERY_URI,PLATFORM_DISCOVERY_RT:PLATFORM_DISCOVERY_URI}
#        oic_resources = {'oic.wk.d':'/oic/d','oic.wk.p':'/oic/p','oic.wk.res':'/oic/res'}
        if rt is None :
            return oic_resources.values()
        resources = []
        if oic_resources.get(rt) is not None :
            resources.append(oic_resources.get(rt))
        return resources
        
    def get_list_of_resource(self, response_payload, key):
        self.log_to_console(sys._getframe().f_code.co_name + " with key: " + key)
        self.log_to_console(response_payload)
        payload_json = JsonAnalyzer(response_payload)
        self.log_to_console(payload_json)
        links = payload_json.getValue("links")
        self.log_to_console("total link: " + str(links.size()))
        for link in links:
    		self.log_to_console(link)
        links_json = JsonAnalyzer(str(links))
        list_of_resource = links_json.getAllValueFromJson(key)
        self.log_to_console(list_of_resource)
        return list_of_resource

    def get_resource_values(self, response_payload, search_property_key, search_property_value, find_property_key):
        # Unimplemented
        self.log_to_console(sys._getframe().f_code.co_name + " in")
        self.log_to_console(response_payload)
        payload_json = JsonAnalyzer(response_payload)
        self.log_to_console(payload_json)
        links = payload_json.getValue("links")
        self.log_to_console("total link: " + str(links.size()))
        for link in links:
    		self.log_to_console(link)
    		link_json = JsonAnalyzer(link)
    		if links_json.getValue(search_property_key).get(0) == search_property_value:
    			return link_json.getValue(key)
        self.log_to_console(list_of_resource)
        return ''
        		
    def get_resource_uri_for_a_resource_type(self, response_payload, resource_type):        
        # Unimplemented
        if resource_type == 'oic.wk.p':
    		return '/oic/p'
        elif resource_type == 'oic.wk.d':
    		return ['/oic/d']	
        return ''
		
    def get_list_of_resource_uri_for_all_resource_type(self, response_payload, *list_of_resource_type):        
        # Unimplemented
        uris = ['/oic/p', '/oic/d']
        return uris

    def has_length_exceed(self, msg, maxLen):
        if len(msg) > maxLen:
    		return True
        return False		
           
    def are_uri_type_interface_defined(self, response, oic_resource):
        # Unimplemented
        
        return True
        
    def check_oic_prefix(self, prefix, response_type, response_uris):
        # Unimplemented
        if response_type.startswith(prefix) == False:
    		for response_uri in response_uris:
    			if response_uri.startswith("/" + prefix):
    				return False
    				
        return True  

    def get_all_value_for_a_key(self, json, key):
        # Unimplemented
        json_analyzer = JsonAnalyzer(json)
        values = json_analyzer.getAllValueFromJson(key)
        return values 
        
    def check_vendor_defined_oic_resource_type_format(self, resource_type):
        if resource_type.startswith('x.'):
    		cnt = 0
    		for ch in resource_type:
    			print(ch + "\n")
    			if ch == '.':
    				cnt = cnt + 1
    		if cnt < 2:
    			return False
        return True

    def verify_prefix_oic(self, response_payload):
        href_list = self.get_list_of_oic_core_resource(None)
        href_list.remove(DEVICE_DISCOVERY_URI)
        payload_json = JsonAnalyzer(response_payload)
        links = payload_json.getValue("links")
        payload_json = JsonAnalyzer(str(links))
        for href in href_list:
            href_uris = payload_json.getValueWithHref(href, "href")
            if href_uris.size() > 0 :
                href_uri = str(href_uris[0])
                if href_uri.startswith('/oic/') is False:
                    return False
        return True

    def verify_response_have_only_discoverable(self, response_payload):
        href_list = self.get_dut_info_value(IotivityKey.HREF.toString(), self.dut_id, IotivityKey.DIS.toString()+'=0')
        payload_json = JsonAnalyzer(response_payload)
        links = payload_json.getValue("links")
        payload_json = JsonAnalyzer(str(links))
        for href in href_list:
            response_hrefs = payload_json.getAllValueFromJson(IotivityKey.HREF.toString())
            if href in response_hrefs:
                return False
        return True

    def is_policy_value_correct(self, response_payload):
        href_list = self.get_dut_info_value(IotivityKey.HREF.toString(), self.dut_id, IotivityKey.DIS.toString()+'=1')
        payload_json = JsonAnalyzer(response_payload)
        links = payload_json.getValue("links")
        payload_json = JsonAnalyzer(str(links))
        for href in href_list:
            expected_p = 1
            if(payload_json.getValueWithHref(href, "bm").size() > 0):
                current_bm = payload_json.getValueWithHref(href, "bm")[0]
                obs_list = self.get_dut_info_value(IotivityKey.OBS.toString(), self.dut_id, IotivityKey.HREF.toString()+'='+href)
                expected_p = expected_p + (int(obs_list[0]) * 2)
                if (int(expected_p) == int(current_bm)) is False:
                    return False
        return True
        
    def get_list_of_value_for_a_data_type(self, json, data_type):
        # Unimplemented
        return []  

    def are_list_elements_unique(self, *listOfItem):
        if len(listOfItem) != len(set(listOfItem)):
    		return False
        return True
        
    def is_item_exist(self, item, *listOfItem):
        if item in listOfItem:
    		return True
        return False
        
    def get_item_count(self, item, *listOfItem):
        cnt = 0
        for element in listOfItem:
    		if element == item:
    			cnt = cnt + 1
        return cnt
        
    def get_contain_count(self, item, *listOfItem):
        cnt = 0
        for element in listOfItem:
    		self.log_to_console("element: " + element)
    		if element.find(item) >= 0:
    			cnt = cnt + 1
        return cnt

    def get_list_length(self, *listOfItem):
        return len(listOfItem)

    def get_prefix(self, msg, end_marker):
        index =  msg.find(end_marker)
        if index >= 0:
    		return msg[0:index]
        return msg
        
    def get_suffix(self, msg, start_marker):
        index =  msg.find(start_marker)
        if index >= 0:
    		return msg[index+1:]
        return msg
        
    def is_key_exist(self, key, payload):
        jsonAnalyzer = JsonAnalyzer(payload)
        values = jsonAnalyzer.getValue(key)
        if values.size() >= 1:
    		return True
        return False
        
    def get_list_of_value(self, msg, key):
        self.log_to_console(sys._getframe().f_code.co_name + " with key: " + key)
        self.log_to_console(msg)
        msg_json = JsonAnalyzer(msg)
        self.log_to_console(msg_json)
        value = msg_json.getValue(key)
        self.log_to_console(value)
        return value
    
    def verify_payload(self, payload, prev_payload):
        self.log_to_console(sys._getframe().f_code.co_name + " in")
        self.log_to_console("prev_payload: " + prev_payload)
        prev_payload = prev_payload[1:len(prev_payload)-1]
        self.log_to_console("prev_payload: " + prev_payload)
        parts = prev_payload.split(':')
        key = str(parts[0])
        length = len(key)-1
        self.log_to_console("length: " + str(length))
        key = key[1:length]
        prev_value = parts[1]
        self.log_to_console("key: " + key)
        self.log_to_console("value: " + prev_value)
        if prev_value[0] == '\"':
    		prev_value=prev_value[1:len(str(prev_value))-1]
        self.log_to_console(payload)
        payload_json = JsonAnalyzer(payload)
        self.log_to_console(payload_json)
        value = payload_json.getValue(key).get(0)
        self.log_to_console(value)
        if value == prev_value:
    		return True
        return False	                    
                      
###end new keywork by m.parves ###


    def compare_json(self, payload1, payload2):
		l1 = len(payload1)
		l2 = len(payload2)
		if l1 == 0 and l2 == 0:
			return True
		if l1 == 0 or l2 == 0:
			return False
		return OICHelper.compareJsonPayload(payload1, payload2)
        
