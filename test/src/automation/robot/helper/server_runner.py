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
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/
'''

import socket
import subprocess
import errno
import os
import sys
import time
import optparse
from optparse import *
from socket import error as socket_error

sys.path.append('.')
sys.path.append('/usr/local/lib/python2.7/dist-packages')

import DeviceControlKeyword
from robotremoteserver import RobotRemoteServer

if __name__ == '__main__':

    ip = [(s.connect(('8.8.8.8', 80)), s.getsockname()[0], s.close()) for s in [socket.socket(socket.AF_INET, socket.SOCK_DGRAM)]][0][1]
    port = 8270

    parser = optparse.OptionParser()
    parser.add_option("-f", "--force", dest="kill", help="Kill Previous jython process", default=False)
    parser.add_option("-p", "--port", dest="port", help="Provides the Robot Server's port", default=port)
    parser.add_option("-i", "--ip", dest="ip", help="Provides the Robot Server's port", default=ip)
    opts, args = parser.parse_args()

    try:
        RobotRemoteServer(DeviceControlKeyword.DeviceControlKeyword(), host=opts.ip, port=opts.port)
    except socket_error as serr:
        print ('Unable to create Rebot Remote Server. Address already in use')

        confirm = 'No'

        if opts.kill == False:
            confirm = raw_input("Do you want kill it? Please Enter yes/no: ")

        if opts.kill or confirm == 'yes':
            print ('Tring to kill process.........')
            ret = subprocess.check_output("fuser -k -n tcp "+str(port), shell=True)
            time.sleep(3)
            RobotRemoteServer(DeviceControlKeyword.DeviceControlKeyword(), host=opts.ip, port=opts.port)
