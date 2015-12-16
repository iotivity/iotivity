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
import datetime

sys.path.append("IoTConformanceTestLib.jar")
from oic.ctt.network.coap import *
from oic.ctt.network import *
from oic.ctt.provision import *
from oic.ctt.devicecontroller import *
from oic.ctt import *
from oic.ctt.formatter import *
from oic.ctt.network.control import *
import oic.ctt.DUTResource.CRUDNType as CRUDNType
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
        global HTTP
        global COAP
        global REP
        global SECURITY
        HTTP = 'HTTP'
        COAP = 'COAP'
        REP  = 'REP'
        SECURITY = 'SECURITY'
    

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
#        PAYLOAD = 'payload'
        SRCADDRESS = 'srcAddress'
        SRCPORT = 'srcPort'
        MSGID = 'msgID'
        TOKEN = 'token'

    def __init__(self):
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
        self.oic_client = OICClient()
        self.oic_server = OICCoapServer()
        self.provision_manager = ProvisionManager()
#        self.oic_resource_light = OICCoapResource("/device/test-tool", "core.light", "oic.if.baseline", True)
#        self.oic_resource_light.addResourceAttribute("power", "off")
#        self.oic_resource_light.addResourceAttribute("intensity", 100)
#        self.oic_resource_light.addResourceAttribute("manufacturer", "Korea")

#        self.oic_resource_light = OICCoapResource("/oic/r/door", "oic.r.door", "oic.if.a oic.if.baseline", True)
#        self.oic_resource_light.addResourceAttribute("openState","Open")
#        self.oic_resource_light.addResourceAttribute("openDuration","35")
#        self.oic_resource_light.addResourceAttribute("openAlarm",False)

#        self.oic_resource_light = OICCoapResource("/oic/r/brightlight", "oic.r.light.brightness", "oic.if.a oic.if.baseline", True)
#        self.oic_resource_light.addResourceAttribute("brightness",59)

#        self.oic_resource_light = OICCoapResource("/oic/r/sensor", "oic.r.sensor", "oic.if.s oic.if.baseline", True)
#        self.oic_resource_light.addResourceAttribute("value",True)

#        self.oic_resource_light = OICCoapResource("/oic/r/switch", "oic.r.switch.binary", "oic.if.a oic.if.baseline", True)
#        self.oic_resource_light.addResourceAttribute("value",True)

        self.oic_resource_light = OICCoapResource("/oic/r/temperature", "oic.r.temperature", "oic.if.a oic.if.s oic.if.baseline", True)
        self.oic_resource_light.addResourceAttribute("temperature", 35)
        self.oic_resource_light.addResourceAttribute("units", "C")
        self.oic_resource_light.addResourceAttribute("range", "-20, 50")

#        self.oic_resource_light = OICCoapResource("/oic/r/mode", "oic.r.mode", "oic.if.a oic.if.baseline", True)
#        self.oic_resource_light.addResourceAttribute("supportedModes", "A, B, C")
#        self.oic_resource_light.addResourceAttribute("modes", "B, C")

#        self.oic_resource_light = OICCoapResource("/oic/r/audio", "oic.r.audio", "oic.if.a oic.if.baseline", True)
#        self.oic_resource_light.addResourceAttribute("volume", 10)
#        self.oic_resource_light.addResourceAttribute("mute", true)

#        self.oic_resource_light = OICCoapResource("/oic/r/button", "oic.r.button", "oic.if.s oic.if.baseline", True)
#        self.oic_resource_light.addResourceAttribute("value", true)

#        self.oic_resource_light = OICCoapResource("/oic/r/illuminance", "oic.r.illuminance", "oic.if.s oic.if.baseline", True)
#        self.oic_resource_light.addResourceAttribute("illuminance", 500)

#        self.oic_resource_light = OICCoapResource("/oic/r/humidity", "oic.r.humidity", "oic.if.a oic.if.s oic.if.baseline", True)
#        self.oic_resource_light.addResourceAttribute("humidity", 80)
#        self.oic_resource_light.addResourceAttribute("desiredHumidity", 50)

        self.oic_resource_light_bwt = OICCoapResource("/device/test-tool-bwt", "core.light", "oic.if.baseline", True)
        self.oic_resource_light_bwt.addResourceAttribute("power", "off")
        self.oic_resource_light_bwt.addResourceAttribute("intensity", 100)
        self.oic_resource_light_bwt.addResourceAttribute("manufacturer", "Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_Block_Wise_Transfer_Data_End_of_BWT")
        self.request = OICRequestData()
        self.rep_key_list = ArrayList()
        self.ui = UserInterface()
        self.control_client = ControlClient()
        self.is_bwt_on = False

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

## Security Relay Controller Keywords  ##
    def intialize_dtls_connection(self, relay_ip, relay_port, dut_ip, dut_port_secured, cipher_suite):
		return self.control_client.initDtls(relay_ip, int(relay_port), dut_ip, int(dut_port_secured), int(cipher_suite))

    def intialize_dtls_connection_with_random_pin(self, relay_ip, relay_port, dut_ip, dut_port_secured, cipher_suite, client_di, server_di, pin):
        return self.control_client.initDtls(relay_ip, int(relay_port), dut_ip, int(dut_port_secured), int(cipher_suite), client_di, server_di, pin)


    def terminate_dtls_connection(self):
		return self.control_client.terminateDtls()

    def get_secured_port(self, dut_ip, dut_port):
        response_message = self.send_get_request("UNICAST", COAP, self.generate_message_id(), self.generate_token(), dut_ip, dut_port, "/oic/res", False)
        response_json = self.get_response_json(response_message)
        secured_port = OICHelper.parseSecuredPort(response_json)
        return secured_port

## End of Security Relay Controller Keywords ##


## End of Device Managment Keywords ##

    # Documentation for a function
    # @brief start server at Conformance test tool
    # @param is_confirmable(bool)
    # @return void
    def start_server(self, is_confirmable=True):
        "Run server for test"
        self.oic_server.addResource(self.oic_resource_light)
#        self.oic_server.addResource(self.oic_resource_light_bwt)
        self.oic_server.clearDiscoveryRequests()
        self.is_bwt_on = False
        self.oic_server.start()

    # Documentation for a function
    # @brief start server at Conformance test tool
    # @param is_confirmable(bool)
    # @return void
    def start_bwt_server(self, is_confirmable=True):
        self.oic_server.removeResource("/device/test-tool")
        "Run server for test"
#        self.oic_server.addResource(self.oic_resource_light)
        self.oic_server.addResource(self.oic_resource_light_bwt)
        self.oic_server.clearDiscoveryRequests()
        self.is_bwt_on = True
#        self.oic_server.start()

    # Documentation for a function
    # @brief get received request from server of Conformance test tool
    # @param none
    # @return request
    def get_request(self):
        "Return request when tool receive it"
        self.clear_request_list()
        self.oic_server.clearDiscoveryRequests()
        self.oic_resource_light.clearRequestList()
        self.print_testcase_get(DUT, TE, 'Waiting for Request from DUT')
        for sec in range(1, 60):
            BuiltIn().sleep(1)
            if self.is_bwt_on is False:
                request_list = self.oic_resource_light.getRequestList()
            else:
                request_list = self.oic_resource_light_bwt.getRequestList()
            if request_list.size() > 0:
                self.request = request_list.get(0)
        #        what = REQUEST_CODE + \
        #            self.get_request_code(self.request) + '\n   '
        #        what += REQUEST_TYPE + \
        #            self.get_request_type(self.request) + '\n   '
        #        what += PAYLOAD + self.get_payload(self.request) + '\n   '
        #        what += REQUEST_HEADER + \
        #            self.get_request_header(self.request) + '\n   '
        #        self.log_to_console(what)
                result = self.get_request_result(self.request)
                to_show = self.get_request_description(self.request)
                self.print_testcase_get(DUT, TE, ' Request Message from DUT', to_show)
                return result
        return None

    # Documentation for a function
    # @brief get request code
    # @param request
    # @return request code (string)
    #def get_request_code(self, request):
    #    "Return request code"
    #    return request.getRequestValue(OICHelper.MessageParameters.requestCode)

    # Documentation for a function
    # @brief get request type
    # @param request
    # @return request type (string)
    #def get_request_type(self, request):
    #    "Return request type"
    #    return request.getRequestValue(OICHelper.MessageParameters.msgType)

    # Documentation for a function
    # @brief get request header
    # @param request
    # @return request header (string)
    #def get_request_header(self, request):
    #    "Return request header"
    #    return request.getOptionSet()

    # Documentation for a function
    # @brief clear request list
    # @param none
    # @return void
    def clear_request_list(self):
        "Clear request list"
        self.oic_server.clearDiscoveryRequests()
        self.oic_resource_light.clearRequestList()

    # Documentation for a function
    # @brief get request code
    # @param none
    # @return reset flag
    def get_reset_state(self):
        "Return True if a RST have arrived, otherwise false"
        #self.oic_resource_light.updateLocalResourceResetFlag()
        return self.oic_resource_light.hasReset()

    # Documentation for a function
    # @brief clear reset state
    # @param none
    # @return void
    def clear_reset_state(self):
        "Clear rest flag"
        self.oic_resource_light.setResetReceived(False)

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
        return self.dut_information_manager.createDUTInformation(
            dut_id, dut_info_path)

    # Documentation for a function
    # @brief remove dut instance
    # @param dut_id
    # @return void
    def release_dut_information(self, dut_id):
        "Release dut instance"
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
                self.print_testcase_get(DUT, TE, 'Discovery Message from DUT', result)
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
#        self.log_to_console("Line 255")

        if responseList is None:
#            self.log_to_console("Line 258")
            self.print_testcase_get(DUT, TE, 'No Response')
#            self.log_to_console("Line 259")
            return ""
            
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
            is_confirmable):
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
        dut_list = self.get_dut_info_value(
            IotivityKey.HREF.toString(), self.dut_id, query)  # temp code
        if responseList is None:
            self.print_testcase_get(DUT, TE, 'No Response')
            return ""
        elif responseList.size() > 0:
            for response in responseList:
                result_temp = self.get_response_result(response)
                if(result_temp is ''):
                    continue
                result_json = self.get_response_json(result_temp)
                href = self.get_json_value(IotivityKey.HREF.toString(), result_json)
                di = self.get_json_value(IotivityKey.DI.toString(), result_json)
#                if(href.size() < 1):
#                    continue
                if (href.size() > 0 and href[0] in dut_list) or (di.size() > 0 ):
                    result = result_temp
                    self.print_testcase_get(
                        DUT,
                        TE,
                        'Response for Discover',
                        self.get_response_description(response))
#        response = responseList.get(0)
#        result = self.get_response_result(response)
#        self.print_testcase_get(DUT,TE,'Response for Discover',self.get_response_description(response))

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
            self.get_protocol(protocol_type),
            self.get_coap_type(is_confirmable),
            messageType,
            message_id,
            token,
            ip,
            int(port),
            default_uri,
            query)
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
        protocol = self.get_protocol(protocol_type)
        what = PROTOCOL_TYPE + protocol_type + '\n   '
        what += DESTINATION_URI + ip + ':' + str(port) + uri + '\n   '
        what += CONFIRMABLE_TYPE + str(is_confirmable) + '\n   '
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
        protocol = self.get_protocol(protocol_type)
        what = PROTOCOL_TYPE + protocol_type + '\n   '
        what += DESTINATION_URI + ip + ':' + str(port) + uri + '\n   '
        what += CONFIRMABLE_TYPE + str(is_confirmable) + '\n   '
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
        what = PROTOCOL_TYPE + protocol_type + '\n   '
        what += DESTINATION_URI + ip + ':' + str(port) + uri + '\n   '
        what += CONFIRMABLE_TYPE + str(is_confirmable) + '\n   '
        self.print_testcase_do(TE, DUT, 'Get Request', what)
        response = client.sendRequest(
            self.get_protocol(protocol_type),
            self.get_coap_type(is_confirmable),
            messageType,
            message_id,
            token,
            ip,
            int(port),
            uri)
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
        messageType = OICRequestData.Method.DELETE
        what = PROTOCOL_TYPE + protocol_type + '\n   '
        what += DESTINATION_URI + ip + ':' + str(port) + uri + '\n   '
        what += CONFIRMABLE_TYPE + str(is_confirmable) + '\n   '
        self.print_testcase_do(TE, DUT, 'Delete Request', what)
        
        response = client.sendRequest(
            self.get_protocol(protocol_type),
            self.get_coap_type(is_confirmable),
            messageType,
            message_id,
            token,
            ip,
            int(port),
            uri)
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
        self.result = input_response.split("\n", 7)
        return self.result[0]

    # Documentation for a function
    # @brief get response json
    # @param input_response
    # @return response json
    def get_response_json(self, input_response):
        self.result = input_response.split("\n", 7)
        return self.result[1]

    # Documentation for a function
    # @brief get response ip
    # @param input_response
    # @return response ip as result
    def get_response_ip(self, input_response):
        self.result = input_response.split("\n", 7)
        return self.result[2]

    # Documentation for a function
    # @brief get response port
    # @param input_response
    # @return response port as result
    def get_response_port(self, input_response):
        self.result = input_response.split("\n", 7)
        return self.result[3]

    # Documentation for a function
    # @brief get response token
    # @param input_response
    # @return response token as result
    def get_response_token(self, input_response):
        self.result = input_response.split("\n", 7)
        return self.result[5]

    # Documentation for a function
    # @brief get response msgid
    # @param input_response
    # @return msgid as result
    def get_response_msgid(self, input_response):
        self.result = input_response.split("\n", 7)
        return self.result[4]

    # Documentation for a function
    # @brief get response type
    # @param input_response
    # @return type of response as result
    def get_response_type(self, input_response):
        self.result = input_response.split("\n", 7)
        return self.result[6]

    # Documentation for a function
    # @brief get response code
    # @param input_response
    # @return type of response as result

    def get_request_code(self, input_request):
        self.result = input_request.split("\n", 8)
        return self.result[0]

    # Documentation for a function
    # @brief get request json
    # @param input_request
    # @return request json as result
    def get_request_json(self, input_request):
        self.result = input_request.split("\n", 8)
        return self.result[1]

    # Documentation for a function
    # @brief get request ip
    # @param input_request
    # @return request ip as result
    def get_request_ip(self, input_request):
        self.result = input_request.split("\n", 8)
        return self.result[2]

    # Documentation for a function
    # @brief get request port
    # @param input_request
    # @return request port as result
    def get_request_port(self, input_request):
        self.result = input_request.split("\n", 8)
        return self.result[3]

    # Documentation for a function
    # @brief get request token
    # @param input_request
    # @return request token as result
    def get_request_token(self, input_request):
        self.result = input_request.split("\n", 8)
        return self.result[5]

    # Documentation for a function
    # @brief get request msgid
    # @param input_request
    # @return request msgid as result
    def get_request_msgid(self, input_request):
        self.result = input_request.split("\n", 8)
        return self.result[4]

    # Documentation for a function
    # @brief get request observe
    # @param input_request
    # @return request observe as result
    def get_request_observe(self, input_request):
        self.result = input_request.split("\n", 8)
        return self.result[6]

    # Documentation for a function
    # @brief get request type
    # @param input_request
    # @return request type as result
    def get_request_type(self, input_request):
        self.result = input_request.split("\n", 8)
        return self.result[7]

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
    def get_dut_info_value(self, key, dut_id, query):
        if not dut_id:
            dut_id = self.dut_id
        self.dutInformation = self.dut_information_manager.getDUTInformation(
            dut_id)
        if key == IotivityKey.DI.toString():
            return self.dutInformation.getDi()
        self.result = self.dutInformation.getResourceValue(key, query)
        return self.result

    # Documentation for a function
    # @brief get dut resource value
    # @param dut_id, href_query, key, accessmode
    # @return DUTResourceValue
    def get_dut_resource_value(self, dut_id, href_query, key, accessmode):
        self.dutInformation = self.dut_information_manager.getDUTInformation(
            dut_id)
        dut_resource = self.dutInformation.getResourceWithQuery(href_query)
#        print "Tuhin"
#        print href_query
        if key == "rep":
            rep_list = dut_resource.get(0).getDUTResourceValue(key)
	    dict_list =[]
	    for rep in rep_list:
	        #print rep
	        rep = str(rep) 
                json_analyzer = JsonAnalyzer(rep)
                
                tmp_list = re.findall(r"[\w'.]+",rep)
	        key_value=[]
                if accessmode in json_analyzer.getValue("accessmode").get(0):
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

    def check_discovery_response(
            self,
            key,
            dut_id,
            response_string,
            response_format,
            query="",
            false=True):
        self.dutInformation = self.dut_information_manager.getDUTInformation(
            dut_id)
            
#        self.log_to_console(" key= " + str(key)+ " dui_id="+str(dut_id)+" response_string="+str(response_string)+" response_format="+str(response_format))
        dut_resources = self.dutInformation.getResourceWithQuery(query)
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
        json_analyzer = JsonAnalyzer(json_string)
        return json_analyzer.validate()

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
#        self.log_to_console(" response_result = " + response_result)
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
        request_ip = self.get_request_value(request, OICHelper.MessageParameters.srcAddress)
        if request_ip is not None:
            request_result = request_result + "\n" + request_ip
        else:
            request_result = request_result + "\n" + ""
        request_port = self.get_request_value(request, OICHelper.MessageParameters.srcPort)
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
        self.log_to_console(" request_result = " + request_result)
        return request_result

    def create_json_representation(self):
        self.json_representation = HashMap()
        self.rep_map = JsonData()
  
    def add_into_json_representation(self, key, value, data_type=None):
        if data_type == "int":
            value = int(value)
        elif data_type == "float":
            value = float(value)
        elif data_type == "bool":
            value = Boolean.valueOf(value)
        elif data_type == "array":
            json_array = JsonData(value)
            value = json_array.toArray()
        self.rep_map.put(key , value )

    def get_json_string(self):
        intermediate_representation = HashMap()
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
        self.result = response.split("\n", 6)
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
    	sm = self.get_rep_value_from_response("om", response)
    	return sm
    	
    def get_oxm_selection_from_response(self, response):
    	sm = self.get_rep_value_from_response("oxmsel", response)
    	return sm
    	
    def get_owned_status_from_response(self, response):
    	sm = self.get_rep_value_from_response("owned", response)
    	return sm
    	
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

    def get_reset_token(self):
        return self.oic_resource_light.getResetToken()

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
        time_stamp = datetime.datetime.now().strftime('_%Y%m%d_%H%M%S')
        file_name = file_name + time_stamp + ".pcapng"
        return file_name
    	
    def is_crudn_operation_supported(self, href, key):
        self.dutInformation = self.dut_information_manager.getDUTInformation(self.dut_id)
        self.dutInformation.generateCRUDNState(href)
        is_supported = False
        if key == "COMPLETE_CREATE":
            is_supported = self.dutInformation.getCRUDNState(href, CRUDNType.COMPLETE_CREATE)
        elif key == "COMPLETE_CREATE":
            is_supported = self.dutInformation.getCRUDNState(href, CRUDNType.SUBORDINATE_CREATE)
        elif key == "RETRIEVE":
            is_supported = self.dutInformation.getCRUDNState(href, CRUDNType.RETRIEVE)
        elif key == "COMPLETE_UPDATE":
            is_supported = self.dutInformation.getCRUDNState(href, CRUDNType.COMPLETE_UPDATE)
        elif key == "PARTIAL_UPDATE":
            is_supported = self.dutInformation.getCRUDNState(href, CRUDNType.PARTIAL_UPDATE)
        elif key == "DELETE":
            is_supported = self.dutInformation.getCRUDNState(href, CRUDNType.DELETE)
        else:
            is_supported = False
        return is_supported

    def update_representation(self):
        "updates resource representation"
        self.oic_resource_light.updateResourceRespresentation()

    def get_resource_interface(self, href):
        self.dutInformation = self.dut_information_manager.getDUTInformation(self.dut_id)
        return  self.dutInformation.getInterfaceList(href)
        
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
