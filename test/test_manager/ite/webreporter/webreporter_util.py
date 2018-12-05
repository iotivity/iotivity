'''
/******************************************************************
*
* Copyright 2018 Samsung Electronics All Rights Reserved.
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
******************************************************************/
'''
import time
from time import strftime

from xml.dom.minidom import Document
from ite.constants import *

WEB_REPORT = namedtuple('WEB_REPORT',
                      ['PLATFORM', 'TYPE', 'TARGET', 'MODULE', 'API_TC', 'INFO', 'VALUE', 'GENERATE_DATE', 'PASS', 'FAIL', 'PACKAGE', 'TC', 'KEY'])\
                      ('platform', 'type', 'target', 'module', "API TC", 'info', 'value', "generateDate", 'pass', 'fail', 'package', 'tc', 'key')

def create_xml_element(doc, parent, tagname):
    element = doc.createElement(tagname)
    parent.appendChild(element)

    return element

def create_summary_structure_node(doc, root):
    structure = create_xml_element(doc, root, WEB_MODEL_ELM.STRUCTURE)

    platform = create_xml_element(doc, structure, WEB_MODEL_ELM.HEADER)
    platform.setAttribute(WEB_MODEL_ATT.NAME, WEB_REPORT.PLATFORM)

    #tctype = create_xml_element(doc, platform, WEB_MODEL_ELM.HEADER)
    #tctype.setAttribute(WEB_MODEL_ATT.NAME, WEB_REPORT.TYPE)

    module = create_xml_element(doc, platform, WEB_MODEL_ELM.HEADER)
    module.setAttribute(WEB_MODEL_ATT.NAME, WEB_REPORT.MODULE)

def create_result_structure_node(doc, root):
    structure = create_xml_element(doc, root, WEB_MODEL_ELM.STRUCTURE)

    platform = create_xml_element(doc, structure, WEB_MODEL_ELM.HEADER)
    platform.setAttribute(WEB_MODEL_ATT.NAME, WEB_REPORT.PLATFORM)

    target = create_xml_element(doc, platform, WEB_MODEL_ELM.HEADER)
    target.setAttribute(WEB_MODEL_ATT.NAME, WEB_REPORT.TARGET)

    tctype = create_xml_element(doc, target, WEB_MODEL_ELM.HEADER)
    tctype.setAttribute(WEB_MODEL_ATT.NAME, WEB_REPORT.TYPE)

    module = create_xml_element(doc, tctype, WEB_MODEL_ELM.HEADER)
    module.setAttribute(WEB_MODEL_ATT.NAME, WEB_REPORT.MODULE)

def create_info_node(doc, root):
    info_node = create_xml_element(doc, root, WEB_REPORT.INFO)

    value_node = create_xml_element(doc, info_node, WEB_REPORT.VALUE)
    value_node.setAttribute(WEB_MODEL_ATT.NAME, WEB_REPORT.GENERATE_DATE)

    timestring = strftime("%Y-%m-%d", time.localtime(time.time()))
    value_node.setAttribute(WEB_REPORT.VALUE, timestring)

    return info_node
