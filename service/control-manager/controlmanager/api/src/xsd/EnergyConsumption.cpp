//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=



#include "EnergyConsumption.h"


#include "Elements.h"

using namespace std;

EnergyConsumption::EnergyConsumption()
{

    mpBatteryCharge = NULL;
    mpInstantaneousPower = NULL;
    mpCumulativeConsumption = NULL;
    mpStartTime = NULL;
    mpHourlyConsumption = NULL;
    mpDailyConsumption = NULL;
    mpWeeklyConsumption = NULL;
    mpMonthlyConsumption = NULL;
    mpReportInterval = NULL;
    mpUsageThreshold = NULL;
    mpSaveLocation = NULL;

}

EnergyConsumption::~EnergyConsumption()
{
    if ( NULL != mpBatteryCharge) { delete mpBatteryCharge; mpBatteryCharge = NULL; }
    if ( NULL != mpInstantaneousPower) { delete mpInstantaneousPower; mpInstantaneousPower = NULL; }
    if ( NULL != mpCumulativeConsumption) { delete mpCumulativeConsumption; mpCumulativeConsumption = NULL; }
    if ( NULL != mpStartTime) { delete mpStartTime; mpStartTime = NULL; }
    if ( NULL != mpHourlyConsumption) { delete mpHourlyConsumption; mpHourlyConsumption = NULL; }
    if ( NULL != mpDailyConsumption) { delete mpDailyConsumption; mpDailyConsumption = NULL; }
    if ( NULL != mpWeeklyConsumption) { delete mpWeeklyConsumption; mpWeeklyConsumption = NULL; }
    if ( NULL != mpMonthlyConsumption) { delete mpMonthlyConsumption; mpMonthlyConsumption = NULL; }
    if ( NULL != mpReportInterval) { delete mpReportInterval; mpReportInterval = NULL; }
    if ( NULL != mpUsageThreshold) { delete mpUsageThreshold; mpUsageThreshold = NULL; }
    if ( NULL != mpSaveLocation) { delete mpSaveLocation; mpSaveLocation = NULL; }

}

bool
EnergyConsumption::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpBatteryCharge )
        {
            bRet = serializer.setPropertyValue("batteryCharge", *mpBatteryCharge);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("batteryCharge") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : batteryCharge"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpInstantaneousPower )
        {
            if (true == mpInstantaneousPower->validateContent())
            {
                bRet = serializer.setPropertyValue("instantaneousPower", mpInstantaneousPower->value);
            }
            else { bRet = false ;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("instantaneousPower") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : instantaneousPower"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpCumulativeConsumption )
        {
            if (true == mpCumulativeConsumption->validateContent())
            {
                bRet = serializer.setPropertyValue("cumulativeConsumption", mpCumulativeConsumption->value);
            }
            else { bRet = false ;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("cumulativeConsumption") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : cumulativeConsumption"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpStartTime )
        {
            if (mpStartTime->value.length() > 0)
            {
                if (true == mpStartTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("startTime", mpStartTime->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("startTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : startTime"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpHourlyConsumption )
        {
            if (true == mpHourlyConsumption->validateContent())
            {
                bRet = serializer.setPropertyValue("hourlyConsumption", mpHourlyConsumption->value);
            }
            else { bRet = false ;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("hourlyConsumption") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : hourlyConsumption"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpDailyConsumption )
        {
            if (true == mpDailyConsumption->validateContent())
            {
                bRet = serializer.setPropertyValue("dailyConsumption", mpDailyConsumption->value);
            }
            else { bRet = false ;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("dailyConsumption") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : dailyConsumption"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpWeeklyConsumption )
        {
            if (true == mpWeeklyConsumption->validateContent())
            {
                bRet = serializer.setPropertyValue("weeklyConsumption", mpWeeklyConsumption->value);
            }
            else { bRet = false ;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("weeklyConsumption") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : weeklyConsumption"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpMonthlyConsumption )
        {
            if (true == mpMonthlyConsumption->validateContent())
            {
                bRet = serializer.setPropertyValue("monthlyConsumption", mpMonthlyConsumption->value);
            }
            else { bRet = false ;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("monthlyConsumption") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : monthlyConsumption"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpReportInterval )
        {
            bRet = serializer.setPropertyValue("reportInterval", *mpReportInterval);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("reportInterval") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : reportInterval"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpUsageThreshold )
        {
            if (true == mpUsageThreshold->validateContent())
            {
                bRet = serializer.setPropertyValue("usageThreshold", mpUsageThreshold->value);
            }
            else { bRet = false ;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("usageThreshold") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : usageThreshold"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpSaveLocation )
        {
            if (mpSaveLocation->length() > 0)
            {
                bRet = serializer.setPropertyValue("saveLocation", *mpSaveLocation);
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("saveLocation") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : saveLocation"); return false; }}
    }


    return true ;
}

bool
EnergyConsumption::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("batteryCharge"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  batteryCharge"); return false;}
        mpBatteryCharge = new uint8_t();
        if (mpBatteryCharge == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  batteryCharge"); return false;}
        if (false == deserializer.getPropertyValue("batteryCharge", mpBatteryCharge)) { deserializer.setErrorMessage("Failed to deserialize parameter :  batteryCharge"); return false ;}
    }
    if (true == deserializer.hasProperty("instantaneousPower"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  instantaneousPower"); return false;}
        mpInstantaneousPower = new ::Float32();
        if (mpInstantaneousPower == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  instantaneousPower"); return false;}
        if (false == deserializer.getPropertyValue("instantaneousPower",
                &(mpInstantaneousPower->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  instantaneousPower"); return false ;}
        if (false == mpInstantaneousPower->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : instantaneousPower"); return false ;}
    }
    if (true == deserializer.hasProperty("cumulativeConsumption"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  cumulativeConsumption"); return false;}
        mpCumulativeConsumption = new ::Float32();
        if (mpCumulativeConsumption == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  cumulativeConsumption"); return false;}
        if (false == deserializer.getPropertyValue("cumulativeConsumption",
                &(mpCumulativeConsumption->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  cumulativeConsumption"); return false ;}
        if (false == mpCumulativeConsumption->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : cumulativeConsumption"); return false ;}
    }
    if (true == deserializer.hasProperty("startTime"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  startTime"); return false;}
        mpStartTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpStartTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  startTime"); return false;}
        if (false == deserializer.getPropertyValue("startTime",
                &(mpStartTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  startTime"); return false ;}
        if (false == mpStartTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : startTime"); return false ;}
    }
    if (true == deserializer.hasProperty("hourlyConsumption"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  hourlyConsumption"); return false;}
        mpHourlyConsumption = new ::Float32();
        if (mpHourlyConsumption == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  hourlyConsumption"); return false;}
        if (false == deserializer.getPropertyValue("hourlyConsumption",
                &(mpHourlyConsumption->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  hourlyConsumption"); return false ;}
        if (false == mpHourlyConsumption->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : hourlyConsumption"); return false ;}
    }
    if (true == deserializer.hasProperty("dailyConsumption"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  dailyConsumption"); return false;}
        mpDailyConsumption = new ::Float32();
        if (mpDailyConsumption == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  dailyConsumption"); return false;}
        if (false == deserializer.getPropertyValue("dailyConsumption",
                &(mpDailyConsumption->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  dailyConsumption"); return false ;}
        if (false == mpDailyConsumption->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : dailyConsumption"); return false ;}
    }
    if (true == deserializer.hasProperty("weeklyConsumption"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  weeklyConsumption"); return false;}
        mpWeeklyConsumption = new ::Float32();
        if (mpWeeklyConsumption == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  weeklyConsumption"); return false;}
        if (false == deserializer.getPropertyValue("weeklyConsumption",
                &(mpWeeklyConsumption->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  weeklyConsumption"); return false ;}
        if (false == mpWeeklyConsumption->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : weeklyConsumption"); return false ;}
    }
    if (true == deserializer.hasProperty("monthlyConsumption"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  monthlyConsumption"); return false;}
        mpMonthlyConsumption = new ::Float32();
        if (mpMonthlyConsumption == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  monthlyConsumption"); return false;}
        if (false == deserializer.getPropertyValue("monthlyConsumption",
                &(mpMonthlyConsumption->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  monthlyConsumption"); return false ;}
        if (false == mpMonthlyConsumption->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : monthlyConsumption"); return false ;}
    }
    if (true == deserializer.hasProperty("reportInterval"))
    {
        mpReportInterval = new uint32_t();
        if (mpReportInterval == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  reportInterval"); return false;}
        if (false == deserializer.getPropertyValue("reportInterval", mpReportInterval)) { deserializer.setErrorMessage("Failed to deserialize parameter :  reportInterval"); return false ;}
    }
    if (true == deserializer.hasProperty("usageThreshold"))
    {
        mpUsageThreshold = new ::Float32();
        if (mpUsageThreshold == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  usageThreshold"); return false;}
        if (false == deserializer.getPropertyValue("usageThreshold",
                &(mpUsageThreshold->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  usageThreshold"); return false ;}
        if (false == mpUsageThreshold->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : usageThreshold"); return false ;}
    }
    if (true == deserializer.hasProperty("saveLocation"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  saveLocation"); return false;}
        mpSaveLocation = new std::string();
        if (mpSaveLocation == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  saveLocation"); return false;}
        if (false == deserializer.getPropertyValue("saveLocation", mpSaveLocation)) { deserializer.setErrorMessage("Failed to deserialize parameter :  saveLocation"); return false ;}
    }


    return true ;
}

std::string
EnergyConsumption::getElementName()
{
    return EN_ENERGYCONSUMPTION;
}

bool
EnergyConsumption::clone(::EnergyConsumption *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpBatteryCharge) { delete mpBatteryCharge; mpBatteryCharge = NULL; }

        if ( NULL != pSrcObject->mpBatteryCharge )
        {
            mpBatteryCharge = new uint8_t();
            if (mpBatteryCharge == NULL ) {return false;}
            *mpBatteryCharge = *(pSrcObject->mpBatteryCharge );
        }
    }
    {
        if ( NULL != mpInstantaneousPower) { delete mpInstantaneousPower; mpInstantaneousPower = NULL; }

        if ( NULL != pSrcObject->mpInstantaneousPower )
        {
            mpInstantaneousPower = new ::Float32();
            if (mpInstantaneousPower == NULL ) {return false;}
            mpInstantaneousPower->value = (pSrcObject->mpInstantaneousPower->value );
        }
    }
    {
        if ( NULL != mpCumulativeConsumption) { delete mpCumulativeConsumption; mpCumulativeConsumption = NULL; }

        if ( NULL != pSrcObject->mpCumulativeConsumption )
        {
            mpCumulativeConsumption = new ::Float32();
            if (mpCumulativeConsumption == NULL ) {return false;}
            mpCumulativeConsumption->value = (pSrcObject->mpCumulativeConsumption->value );
        }
    }
    {
        if ( NULL != mpStartTime) { delete mpStartTime; mpStartTime = NULL; }

        if ( NULL != pSrcObject->mpStartTime )
        {
            mpStartTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (mpStartTime == NULL ) {return false;}
            mpStartTime->value = (pSrcObject->mpStartTime->value );
        }
    }
    {
        if ( NULL != mpHourlyConsumption) { delete mpHourlyConsumption; mpHourlyConsumption = NULL; }

        if ( NULL != pSrcObject->mpHourlyConsumption )
        {
            mpHourlyConsumption = new ::Float32();
            if (mpHourlyConsumption == NULL ) {return false;}
            mpHourlyConsumption->value = (pSrcObject->mpHourlyConsumption->value );
        }
    }
    {
        if ( NULL != mpDailyConsumption) { delete mpDailyConsumption; mpDailyConsumption = NULL; }

        if ( NULL != pSrcObject->mpDailyConsumption )
        {
            mpDailyConsumption = new ::Float32();
            if (mpDailyConsumption == NULL ) {return false;}
            mpDailyConsumption->value = (pSrcObject->mpDailyConsumption->value );
        }
    }
    {
        if ( NULL != mpWeeklyConsumption) { delete mpWeeklyConsumption; mpWeeklyConsumption = NULL; }

        if ( NULL != pSrcObject->mpWeeklyConsumption )
        {
            mpWeeklyConsumption = new ::Float32();
            if (mpWeeklyConsumption == NULL ) {return false;}
            mpWeeklyConsumption->value = (pSrcObject->mpWeeklyConsumption->value );
        }
    }
    {
        if ( NULL != mpMonthlyConsumption) { delete mpMonthlyConsumption; mpMonthlyConsumption = NULL; }

        if ( NULL != pSrcObject->mpMonthlyConsumption )
        {
            mpMonthlyConsumption = new ::Float32();
            if (mpMonthlyConsumption == NULL ) {return false;}
            mpMonthlyConsumption->value = (pSrcObject->mpMonthlyConsumption->value );
        }
    }
    {
        if ( NULL != mpReportInterval) { delete mpReportInterval; mpReportInterval = NULL; }

        if ( NULL != pSrcObject->mpReportInterval )
        {
            mpReportInterval = new uint32_t();
            if (mpReportInterval == NULL ) {return false;}
            *mpReportInterval = *(pSrcObject->mpReportInterval );
        }
    }
    {
        if ( NULL != mpUsageThreshold) { delete mpUsageThreshold; mpUsageThreshold = NULL; }

        if ( NULL != pSrcObject->mpUsageThreshold )
        {
            mpUsageThreshold = new ::Float32();
            if (mpUsageThreshold == NULL ) {return false;}
            mpUsageThreshold->value = (pSrcObject->mpUsageThreshold->value );
        }
    }
    {
        if ( NULL != mpSaveLocation) { delete mpSaveLocation; mpSaveLocation = NULL; }

        if ( NULL != pSrcObject->mpSaveLocation )
        {
            mpSaveLocation = new std::string();
            if (mpSaveLocation == NULL ) {return false;}
            *mpSaveLocation = *(pSrcObject->mpSaveLocation );
        }
    }


    return true ;
}
