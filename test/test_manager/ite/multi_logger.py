#!/usr/bin/python3

import os
import sys
import datetime
import traceback
import logging

global logger

logger = logging.getLogger('scope.name')
logger.setLevel('INFO')

global log_file_name
log_file_name = ''

def set_file_handler(module, platform_type, verdict_path):
    tokens = verdict_path.split(os.sep)
    global log_file_name
    log_file_name = 'test_manager_{}_{}.log'.format(module, platform_type)
    if len(tokens) > 1:
        log_file_name = os.sep.join(tokens[0:-1]) + os.sep + log_file_name

    file_log_handler = logging.FileHandler(log_file_name)
    logger.addHandler(file_log_handler)

def print_to_file(*msgs):
    global log_file_name

    if logger:
        for msg in msgs:
            timestring = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            logger.info(timestring + ' ' + str(msg))

def print_runner_output(*msgs):
    global log_file_name

    for msg in msgs:
        print (msg)

    print_to_file(msgs)

def show_exeception():
    #print_runner_output("Unexpected error:", sys.exc_info()[0])
    traceback.print_exc(file=sys.stdout)
    if logger:
        logger.info(traceback.print_exc())
