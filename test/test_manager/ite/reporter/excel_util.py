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

import sys

from xlsxwriter.workbook import Workbook
from xlsxwriter.utility import *

from ite.config import *
from ite.constants import *

class ExcelFormat:


    def __init__(self, workbook):
        for xlsx_format in list(XLSX_FORMAT):
            self.__dict__[xlsx_format] = workbook.add_format(XLSX_FORMAT[xlsx_format])

        self.cell_wrap.set_text_wrap()

        self.result_title = {TC_RESULT.PASS : self.pass_title,
                             TC_RESULT.FAIL : self.fail_title,
                             'Total'        : self.total,
                             'Pass Rate'    : self.total,}


def merge_cell(sheet, first_row, first_col, last_row, last_col, body, formatter):
    if (first_row > last_row or first_col > last_col):
        return
    elif (first_row == last_row and first_col == last_col):
        sheet.write(first_row, first_col, body, formatter)
    else:
        sheet.merge_range(first_row, first_col, last_row, last_col, body, formatter)

def get_cell_name(col, row):
    colname = xl_col_to_name(col)

    return '%s%d' % (colname,  row + 1)


