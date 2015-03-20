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

/**
 * This file contains the exported symbol.
 */
#include <stdlib.h>
#include <map>
#include <string>
#include <sstream>
#include <iostream>

#include "BMISensor.h"
#include "SysTimer.h"

#ifdef __ANDROID__
#include "OCAndroid.h"
#endif

using namespace BMISensorName;

#define SENSOR_NAME "BMISensor"

#define WEIGHTSENSOR_NAME ((char *)"Thing_WeightSensor")
#define HEIGHTSENSOR_NAME ((char *)"Thing_HeightSensor")

#define WEIGHT_NAME ((char *)"weight")
#define HEIGHT_NAME ((char *)"height")




ICtxDelegate *g_pDelegate;

void InitializeContext(ICtxDelegate *pDelegate)
{
 
    std::cout << "[BMISensor] Sensor::" << __func__ << " is called."
              << std::endl;

   std::vector < ContextData > contextData;

    BMISensor *eventCls = new BMISensor();
    pDelegate->registerCallback(eventCls);
    g_pDelegate = pDelegate;

    std::cout << "BMISensor loaded" << std::endl;

    return;
}

BMISensor::BMISensor()
{
    m_result.m_usrname = "";       // each user.
    m_result.m_timestamp = "";     // measured time
    m_result.m_weight = 0;         // Height
    m_result.m_height = 0;         // Weight
    time(&(m_result.m_timepstampW));
    time(&(m_result.m_timepstampH));
    m_result.m_BMIresult = UNKOWNBMI;



}

void BMISensor::onCtxEvent(enum CTX_EVENT_TYPE eventType,
                                       std::vector< ContextData > contextDataList)
{


    switch (eventType)
    {
        case SPF_START:
            runLogic(contextDataList);
            break;
        default:
            std::cout << "Not support onCtxEvent" << std::endl;
            break;
    }
}

int BMISensor::runLogic(std::vector< ContextData > &contextDataList)
{

    DIResult result;

    if (getInput(contextDataList) == SUCCESS)
    {




        if ((result = makeBMI()) != SUCCESS)
        {
            std::cout << "Error : makeBMI() result = " << result << std::endl;
            return -1;
        }

        std::vector < ContextData > outList;
        ContextData out = setOutput(1);
        outList.push_back(out);
        g_pDelegate->addOutput(outList);

        return 0;
    }

    return -1;
}



DIResult BMISensor::getInput(std::vector< ContextData > &contextDataList)
{

    int contextSize = 0;
    double temp;

//    std::cout << "[BMISensor] BMISensor::" << "contextDataList.size() : "<< contextDataList.size() << std::endl;



    if ((contextSize = contextDataList.size()) == 0)    // number of input things
    {
          std::cout << "Physical Context data is not exist." << std::endl;
          return ERROR;
    }

    for (int i = 0; i < contextSize; i++)
    {

        std::vector < std::map< std::string, std::string > > lVector =
                            contextDataList[i].outputProperty;
        std::string name = lVector[0]["name"];


        if (contextDataList[i].rootName == WEIGHTSENSOR_NAME)
        {


            if (name.compare(WEIGHT_NAME) == 0){

                temp=std::stod( lVector[0]["value"]);

                if (temp > 0){
                    time(&(m_result.m_timepstampW));
                    m_result.m_weight=temp;
                }



            }


        }
        else if (contextDataList[i].rootName == HEIGHTSENSOR_NAME)
        {

            if (name.compare(HEIGHT_NAME) == 0){

                temp=std::stod( lVector[0]["value"]);
                if (temp > 0){
                    time(&(m_result.m_timepstampH));
                    m_result.m_height=temp;
                }

            }

        }

    }

    return SUCCESS;
}

/**
 * Calculation of BMI with Weight&Height
 */



DIResult BMISensor::makeBMI(void)
{


    //    std::cout << "[BMISensor] BMISensor::" << __func__ << " is called."
    //              << std::endl;

    double BMIvalue, timediffsecond;

    timediffsecond = abs( difftime( m_result.m_timepstampW,m_result.m_timepstampH));


    if(  timediffsecond >  DIFFTIME ){

           BMIvalue= 0;
           m_result.m_BMIresult=UNKOWNBMI;
           std::cout << "[BMISensor] :   OUTOFDATEBMI: "<<  m_result.m_BMIresult << std::endl;
    }
    else if( (m_result.m_height > 0)   &&  (m_result.m_weight > 0 )){
        BMIvalue=  m_result.m_weight / (m_result.m_height  *  m_result.m_height);


        std::cout << "[BMISensor] height : " << m_result.m_height << " weight : " << m_result.m_weight << " BMIvalue : "<< BMIvalue  <<" timediff : "<<timediffsecond  << std::endl;



            if (BMIvalue >= OVERWEIGHT_VAL)
            {
                m_result.m_BMIresult = (int) OBESE;
                std::cout << "[BMISensor] : BMIresult:" << m_result.m_BMIresult<< " OBESE "<< std::endl;
            }
            else if (BMIvalue >= NORMALRANGE_VAL )
            {
                m_result.m_BMIresult = (int) OVERWEIGHT;
                std::cout << "[BMISensor] : BMIresult:" << m_result.m_BMIresult<< " OVERWEIGHT "<< std::endl;

            }
            else if (BMIvalue >= UNDERWEIGHT_VAL )
            {
                m_result.m_BMIresult = (int) NORMALRANGE;
                std::cout << "[BMISensor] : BMIresult:" << m_result.m_BMIresult<< " NORMALRANGE "<< std::endl;

            }
            else
            {
                m_result.m_BMIresult = (int) UNDERWEIGHT;
                std::cout << "[BMISensor] : BMIresult:" << m_result.m_BMIresult<< " UNDERWEIGHT "<< std::endl;
            }

        }
        else {
            BMIvalue= -1;
            m_result.m_BMIresult=UNKOWNBMI;
            std::cout << "[BMISensor] :   UNKOWNBMI: "<<  m_result.m_BMIresult << std::endl;
        }



    return SUCCESS;


}


ContextData BMISensor::setOutput(int property_count)
{
    ContextData out;

    std::map < std::string, std::string > output_property;


//    std::cout << "[BMISensor] BMISensor::" << __func__ << " is called."
//              << std::endl;

    out.rootName = SENSOR_NAME;
    out.outputPropertyCount = property_count;


    std::ostringstream convert;
    convert << m_result.m_BMIresult;


    output_property.insert(std::make_pair("name", "BMIresult"));
    output_property.insert(std::make_pair("type", "int"));
    output_property.insert(std::make_pair("value",  convert.str() ));
    out.outputProperty.push_back(output_property);



    return out;
}

