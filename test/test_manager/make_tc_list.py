#!/usr/bin/python3
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
import os

def make_tc_list(directory, file_name):
    file_path=os.path.join(directory, file_name)
    f = open(file_path,'r')
    contents = f.read()
    f.close()

    mylist=[]
    prefix_list = [' -f ', ' -u ', ' -c ', ' -g ']
    param_list = ['binary_name', 'suite_name', 'tc_name', 'package_name']
    contents = contents.strip()
    for row in contents.split('\n'):
        for info in row.split(','):
            ##print(info.split(':')[0].strip())
            for i in range(0, len(param_list)):
                if info.split(':')[0].strip() == param_list[i] and len(info.split(':')[1].strip()) != 0:
                    mylist.append(prefix_list[i] + info.split(':')[1].strip())
                    break
    return mylist

a=make_tc_list('./', 'tc_list.txt')
print(a)
'''
if info[]
    sun = (row.split(',')[1]).split(':')[1]
    tcn = (row.split(',')[2]).split(':')[1]
    pcn = (row.split(',')[3]).split(':')[1]
    #print('-u '+sun+'-c '+tcn+'-g '+pcn)

    if "time" in row:
        #print(row.split("time")[1])
'''
