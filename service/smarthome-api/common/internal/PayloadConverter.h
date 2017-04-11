/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
#ifndef SMARTHOME_API_COMMON_PAYLOADCONVERTER_H_
#define SMARTHOME_API_COMMON_PAYLOADCONVERTER_H_

#include "octypes.h"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class PropertyBundle;
            namespace Converter
            {
                /**
                 * API to convert Property Bundle to OCPayload.
                 *
                 * @param[in] bundle Property Bundle.
                 * @return OCPayload.
                 */
                OCPayload* convertBundleToPayload(const PropertyBundle& bundle);

                /**
                 * API to convert OCPayload to Property Bundle.
                 *
                 * @param[in] payload OCPayload.
                 * @param[out] bundle Property Bundle.
                 * @return Returns ::SUCCESS if success
                 */
                bool convertPayloadToBundle(const OCPayload *payload, PropertyBundle& bundle);
            }
        }
    }
}

#endif /* SMARTHOME_API_COMMON_PAYLOADCONVERTER_H_ */
