#!/usr/bin/python3

import sys
import os
import time
import shutil
from time import strftime
import DynamicReportGen
from configuration import *

print ('## Start Annlyzing memcheck XMLs ##')

run_dir = MEMCHECK_RESULT_RUN_DIR

folder_prefix =""

if len(sys.argv) > 1:
    folder_prefix = sys.argv[1]


timestring = strftime("%Y%m%d_%H%M%S", time.localtime(time.time()))

if os.path.exists(run_dir):
  result_dir = os.path.join(MEMCHECK_RESULT_DIR, folder_prefix + "TestResult_" + timestring)
  shutil.move(run_dir, result_dir)
else:
   print("### Can't find Memcheck Result Source Dir: " + run_dir)
   raise SystemExit(1)

memcheck_report_name =result_dir+os.sep+"Memcheck_Result"+timestring+".xlsx"
reporter = DynamicReportGen.DynamicReportGen()
reporter.collectErrors(result_dir,memcheck_report_name)
print ("report generation complete")



