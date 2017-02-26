'''
/***************************************************************************************************
 * Copyright (c) 2010 Samsung Electronics Co., Ltd. All right reserved.
 *
 * This software is the confidential and proprietary information of Samsung Electronics, Inc.
 * ("Confidential Information"). You shall not disclose such Confidential Information and shall
 * use it only in accordance with the terms of the license agreement you entered into with
 * Samsung Electronics.
 ***************************************************************************************************/
'''

def open_read_file(path):
    try :
        file = open(path, encoding='utf-8', errors='ignore')
        return file
    except IOError:
        print("There is a IO Error on " + path)
        return None

def read_file(path, startpos = -1, endpos = -1):
    file = open_read_file(path)
    data = ''

    if file == None:
        return data

    if startpos >= 0 and endpos >= 0:
        file.seek(startpos)
        data = file.read(endpos - startpos)
    else:
        data = file.read()

    file.close()

    return data

def open_write_file(path):
    try:
        txt = open(path, 'w', encoding='utf-8', errors='ignore')
        return txt
    except IOError:
        print("Can not open file: " + path)
        return False

def open_append_file(path):
    try:
        txt = open(path, 'a', encoding='utf-8', errors='ignore')
        return txt
    except IOError:
        print("Can not open file: " + path)
        return False
