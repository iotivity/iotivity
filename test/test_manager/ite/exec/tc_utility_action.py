import io
import os
import re
import time
import threading
import subprocess

from ite.multi_logger import print_runner_output, show_exeception, print_to_file

class TcUtilityAction:

    def __init__(self, dynamic_runner):
        self.dynamic_runner = dynamic_runner

    def common_tc_pre_action(self, binary_name, command, msg):
        print(command)
        status = False
        self.tc_execution_complete = False

        self.kill_background_process(binary_name)

        self.background_proc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        self.background_log_file_path = os.path.abspath(self.binary_name + '.log')
        self.background_log_fp = open(self.background_log_file_path, 'w')

        self.background_log_fp.write('Log from {}: \n'.format(self.binary_name))

        def start_execution():
            nonlocal status
            for line in io.TextIOWrapper(self.background_proc.stdout, encoding="ISO-8859-1"):
                line = line.strip()
                print(line)
                self.background_log_fp.write(line + '\n')
                self.background_log_fp.flush()
                if msg in line:
                    print('simulator started')
                    status = True
                if self.tc_execution_complete:
                    print('tc execution complete')
                    break

        self.binary_runner_thread = threading.Thread(target=start_execution)
        self.binary_runner_thread.start()

        cnt = 0
        timeout = 60
        while self.binary_runner_thread.is_alive() and cnt < timeout:
            print('Running es server attempt: {}'.format(cnt))
            self.binary_runner_thread.join(1)
            cnt += 1
            if status:
                break

        return status

    def kill_background_process(self, binary_name):
        os.system('killall {}'.format(binary_name))
        os.system('pkill {}'.format(binary_name))
        os.system('kill -9 $(pgrep {})'.format(binary_name))

    def common_tc_post_action(self, tc_log_file):

        self.background_log_fp.close()
        self.background_log_fp = open(self.background_log_file_path, 'r')

        for line in self.background_log_fp.readlines():
            tc_log_file.write('\n' + line)

        self.tc_execution_complete = True
        self.binary_runner_thread.join(2)
        self.background_proc.kill()

        self.kill_background_process(self.binary_name)


    def ca_linux_tc_pre_action(self):
        msg = 'Server Initilization Complete'
        self.binary_name = 'iotivity_es_server'
        command = 'stdbuf -oL ./{} 0'.format(self.binary_name)
        return self.common_tc_pre_action(self.binary_name, command, msg)


    def ca_linux_tc_post_action(self, tc_log_file):
        self.common_tc_post_action(tc_log_file)
