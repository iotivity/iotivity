/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

/*
 * BMISensor.h
 */

#ifndef BMISENSOR_H_
#define BMISENSOR_H_

/**
 * This header file is included to define _EXPORT_.
 */
#include "SSMModelDefinition.h"

#ifdef __cplusplus
extern "C"
{
#endif
INTERFACE_DECLSPEC void InitializeContext(ICtxDelegate *pDelegate);
#ifdef __cplusplus
}
;
#endif


#if 0
Who BMI
weight / height*hegith

< 18.5  :underweight
18.5 <= < 25 : Normal Range
<= 25  < 30 : Overweight
<= 30 : Obese
#endif

#define UNKOWNBMI   -1
#define OUTOFDATEBMI   0
#define UNDERWEIGHT     1
#define NORMALRANGE     2
#define OVERWEIGHT      3
#define OBESE           4


#define UNDERWEIGHT_VAL     18.5
#define NORMALRANGE_VAL     25.9
#define OVERWEIGHT_VAL      30


#define DIFFTIME      4     // valid time difference. (seconds)





namespace BMISensorName
{
//#define PHYSICAL_EA 2


    typedef struct _physicalInput_
    {
        char *m_thingName;
        int m_inputNum;
        void *m_pInputStruct;
    } physicalInput;

    typedef enum
    {
        SUCCESS = 0, ERROR
    } DIResult;

    class BMISensor: public ICtxEvent
    {
        private:

 //           static physicalInput s_PHYSICAL_SOFTSENSORs[PHYSICAL_EA];

            class InValue
            {
                public:
                    std::string m_usrname;          // each user. ; for future
                    std::string m_timestamp;
                    double  m_height; // Height.
                    double  m_weight; // Weight
                    time_t  m_timepstampH;
                    time_t  m_timepstampW;
                    int m_BMIresult;

            };

            //InValue m_DI[PHYSICAL_EA];
            InValue m_result;

            int runLogic(std::vector< ContextData > &contextDataList);

        public:
            BMISensor();

            void onCtxEvent(enum CTX_EVENT_TYPE eventType, std::vector< ContextData > contextDataList);

            DIResult getInput(std::vector< ContextData > &contextDataList);
            DIResult makeBMI(void);
            ContextData setOutput(int property_count);

            friend void INTERFACE_DECLSPEC initializeContext(ICtxDelegate *pDelegate);
    };
};

#endif /* BMISENSOR_H_ */
