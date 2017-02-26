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


