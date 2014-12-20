/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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
 ******************************************************************/

#ifndef WEBSERVICE_HTTP_HEADER_DEFINED_H_
#define WEBSERVICE_HTTP_HEADER_DEFINED_H_

#include <string>

namespace webservice
{

    const char *const   kServiceableTheLowestAPIVersion = "v1.0.0";

    typedef enum
    {
        kHttpContentTypeUnknown = -1,
        kHttpContentTypeTextPlain = 0,
        kHttpContentTypeTextCSV,
        kHttpContentTypeTextCSS,
        kHttpContentTypeTextHTML,
        kHttpContentTypeTextXML,
        kHttpContentTypeImageJPEG,
        kHttpContentTypeImageGIF,
        kHttpContentTypeImagePNG,
        kHttpContentTypeApplicationXML,
        kHttpContentTypeApplicationXHTML,
        kHttpContentTypeApplicationAtom,
        kHttpContentTypeApplicationAtomSvc,
        kHttpContentTypeApplicationAtomCat,
        kHttpContentTypeApplicationJavaScript,
        kHttpContentTypeApplicationJSON,
        kHttpContentTypeApplicationMSWord,
        kHttpContentTypeApplicationMSExcel,
        kHttPContentTypeApplicationMSPowerPoint,
        kHttpContentTypeApplicationPDF,
        kHttpContentTypeApplicationZIP,
        kHttpContentTypeApplicationFlash,
        kHttpContentTypeApplicationHTMLForm
    } HttpContentType;

    typedef struct __stContentType
    {
        HttpContentType content_type;
        std::string content_type_text;
    } stContentType;

    static stContentType content_type_tbl_[] =
    {
        {kHttpContentTypeApplicationJSON,         "application/json"},
        {kHttpContentTypeApplicationXML,          "application/xml"},
        {kHttpContentTypeApplicationXHTML,        "application/xhtml+xml"},
        {kHttpContentTypeApplicationAtom,         "application/atom+xml"},
        {kHttpContentTypeApplicationAtomSvc,      "application/atomsvc+xml"},
        {kHttpContentTypeApplicationAtomCat,      "application/atomcat+xml"},
        {kHttpContentTypeApplicationJavaScript,   "application/javascript"},
        {kHttpContentTypeApplicationMSWord,       "application/msword"},
        {kHttpContentTypeApplicationMSExcel,      "application/vnd.ms-excel"},
        {kHttPContentTypeApplicationMSPowerPoint, "application/vns.ms-powerpoint"},
        {kHttpContentTypeApplicationPDF,          "application/pdf"},
        {kHttpContentTypeApplicationZIP,          "application/zip"},
        {kHttpContentTypeApplicationFlash,        "application/x-shockwave-flash"},
        {kHttpContentTypeApplicationHTMLForm,     "application/x-www-form-urlencoded"},
        {kHttpContentTypeTextPlain,               "test/plain"},
        {kHttpContentTypeTextCSV,                 "test/csv"},
        {kHttpContentTypeTextCSS,                 "test/css"},
        {kHttpContentTypeTextHTML,                "test/html"},
        {kHttpContentTypeTextXML,                 "test/xml"},
        {kHttpContentTypeImageJPEG,               "image/jpeg"},
        {kHttpContentTypeImageGIF,                "image/gif"},
        {kHttpContentTypeImagePNG,                "image/png"},
        {kHttpContentTypeUnknown,                 ""},
    };

}

#endif /* WEBSERVICE_HTTP_HEADER_DEFINED_H_ */

