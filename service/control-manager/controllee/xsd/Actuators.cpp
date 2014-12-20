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

#include "Actuators.h"


#include "Elements.h"

using namespace std;

Actuators::Actuators()
{

    mpDeviceIDs = NULL;
    mpGroupIDs = NULL;
    mpOperation = NULL;
    mpMode = NULL;
    mpMovement = NULL;
    mpTemperatures = NULL;
    mpTemperature = NULL;
    mpHumidity = NULL;

    mpDoors = NULL;
    mpDoor = NULL;
    mpLight = NULL;

#if 0 // OIC IGNORE

    mpWind = NULL;
    mpWasher = NULL;
    mpFridge = NULL;
    mpOven = NULL;
    mpAudio = NULL;
    mpAVSources = NULL;
    mpAVSource = NULL;
    mpCamera = NULL;
    mpCaptures = NULL;
    mpCapture = NULL;

#endif

    mpLevel = NULL;
    mpMessages = NULL;
    mpMessage = NULL;
    mpDetection = NULL;

}

Actuators::~Actuators()
{
    if ( NULL != mpDeviceIDs) { delete mpDeviceIDs; mpDeviceIDs = NULL; }
    if ( NULL != mpGroupIDs) { delete mpGroupIDs; mpGroupIDs = NULL; }
    if ( NULL != mpOperation) { delete mpOperation; mpOperation = NULL; }
    if ( NULL != mpMode) { delete mpMode; mpMode = NULL; }
    if ( NULL != mpMovement) { delete mpMovement; mpMovement = NULL; }
    if ( NULL != mpTemperatures) { delete mpTemperatures; mpTemperatures = NULL; }
    if ( NULL != mpTemperature) { delete mpTemperature; mpTemperature = NULL; }
    if ( NULL != mpHumidity) { delete mpHumidity; mpHumidity = NULL; }

    if ( NULL != mpDoors) { delete mpDoors; mpDoors = NULL; }
    if ( NULL != mpDoor) { delete mpDoor; mpDoor = NULL; }
    if ( NULL != mpLight) { delete mpLight; mpLight = NULL; }

#if 0 // OIC IGNORE

    if ( NULL != mpWind) { delete mpWind; mpWind = NULL; }
    if ( NULL != mpWasher) { delete mpWasher; mpWasher = NULL; }
    if ( NULL != mpFridge) { delete mpFridge; mpFridge = NULL; }
    if ( NULL != mpOven) { delete mpOven; mpOven = NULL; }
    if ( NULL != mpAudio) { delete mpAudio; mpAudio = NULL; }
    if ( NULL != mpAVSources) { delete mpAVSources; mpAVSources = NULL; }
    if ( NULL != mpAVSource) { delete mpAVSource; mpAVSource = NULL; }
    if ( NULL != mpCamera) { delete mpCamera; mpCamera = NULL; }
    if ( NULL != mpCaptures) { delete mpCaptures; mpCaptures = NULL; }
    if ( NULL != mpCapture) { delete mpCapture; mpCapture = NULL; }

#endif

    if ( NULL != mpLevel) { delete mpLevel; mpLevel = NULL; }
    if ( NULL != mpMessages) { delete mpMessages; mpMessages = NULL; }
    if ( NULL != mpMessage) { delete mpMessage; mpMessage = NULL; }
    if ( NULL != mpDetection) { delete mpDetection; mpDetection = NULL; }

}

bool
Actuators::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    {
        bool bRet = true;

        if ( NULL != mpDeviceIDs )
        {
            mpDeviceIDs->setInstanceName("deviceIDs");
            bRet = mpDeviceIDs->serializeContents(serializer) ;
        }
        if (false == bRet) { if (serializer.isInFilteringMode("deviceIDs") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : deviceIDs"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpGroupIDs )
        {
            mpGroupIDs->setInstanceName("groupIDs");
            bRet = mpGroupIDs->serializeContents(serializer) ;
        }
        if (false == bRet) { if (serializer.isInFilteringMode("groupIDs") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : groupIDs"); return false; }}
    }
    if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpOperation )
            {
                bRet = mpOperation->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Operation") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Operation"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpMode )
            {
                bRet = mpMode->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Mode") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Mode"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpMovement )
            {
                bRet = mpMovement->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Movement") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Movement"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpTemperatures )
            {
                bRet = mpTemperatures->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Temperatures") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Temperatures"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpTemperature )
            {
                bRet = mpTemperature->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Temperature") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Temperature"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpHumidity )
            {
                bRet = mpHumidity->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Humidity") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Humidity"); return false; }}
        }

    }       if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpDoors )
            {
                bRet = mpDoors->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Doors") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Doors"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpDoor )
            {
                bRet = mpDoor->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Door") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Door"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpLight )
            {
                bRet = mpLight->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Light") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Light"); return false; }}
        }

    }

#if 0 // OIC IGNORE

    if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpWind )
            {
                bRet = mpWind->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Wind") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Wind"); return false; }}
        }

    }


    if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpWasher )
            {
                bRet = mpWasher->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Washer") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Washer"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpFridge )
            {
                bRet = mpFridge->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Fridge") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Fridge"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpOven )
            {
                bRet = mpOven->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Oven") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Oven"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpAudio )
            {
                bRet = mpAudio->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Audio") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Audio"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpAVSources )
            {
                bRet = mpAVSources->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("AVSources") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : AVSources"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpAVSource )
            {
                bRet = mpAVSource->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("AVSource") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : AVSource"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpCamera )
            {
                bRet = mpCamera->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Camera") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Camera"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpCaptures )
            {
                bRet = mpCaptures->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Captures") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Captures"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpCapture )
            {
                bRet = mpCapture->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Capture") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Capture"); return false; }}
        }

    }

#endif

    if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpLevel )
            {
                bRet = mpLevel->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Level") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Level"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpMessages )
            {
                bRet = mpMessages->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Messages") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Messages"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpMessage )
            {
                bRet = mpMessage->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Message") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Message"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpDetection )
            {
                bRet = mpDetection->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Detection") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Detection"); return false; }}
        }

    }

    return true ;
}

bool
Actuators::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    {
        if (true == deserializer.hasProperty("deviceIDs"))
        {
            mpDeviceIDs = new ::ArrayOfUuid();
            if (mpDeviceIDs == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  deviceIDs"); return false;}
            mpDeviceIDs->setInstanceName("deviceIDs");
            if (false == mpDeviceIDs->deSerializeContents(deserializer)) {  deserializer.setErrorMessage("Parameter Contains Invalid Value : deviceIDs"); return false ;}
        }
    }
    {
        if (true == deserializer.hasProperty("groupIDs"))
        {
            mpGroupIDs = new ::ArrayOfString();
            if (mpGroupIDs == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  groupIDs"); return false;}
            mpGroupIDs->setInstanceName("groupIDs");
            if (false == mpGroupIDs->deSerializeContents(deserializer)) {  deserializer.setErrorMessage("Parameter Contains Invalid Value : groupIDs"); return false ;}
        }
    }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Operation"))
        {
            mpOperation = new ::Operation();
            if (mpOperation == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Operation"); return false;}
            if (false == mpOperation->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Operation")) {deserializer.setErrorMessage("Additional data to deserialize: Operation"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Mode"))
        {
            mpMode = new ::Mode();
            if (mpMode == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Mode"); return false;}
            if (false == mpMode->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Mode")) {deserializer.setErrorMessage("Additional data to deserialize: Mode"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Movement"))
        {
            mpMovement = new ::Movement();
            if (mpMovement == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Movement"); return false;}
            if (false == mpMovement->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Movement")) {deserializer.setErrorMessage("Additional data to deserialize: Movement"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Temperatures"))
        {
            mpTemperatures = new ::Temperatures();
            if (mpTemperatures == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Temperatures"); return false;}
            if (false == mpTemperatures->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Temperatures")) {deserializer.setErrorMessage("Additional data to deserialize: Temperatures"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Temperature"))
        {
            mpTemperature = new ::Temperature();
            if (mpTemperature == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Temperature"); return false;}
            if (false == mpTemperature->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Temperature")) {deserializer.setErrorMessage("Additional data to deserialize: Temperature"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Humidity"))
        {
            mpHumidity = new ::Humidity();
            if (mpHumidity == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Humidity"); return false;}
            if (false == mpHumidity->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Humidity")) {deserializer.setErrorMessage("Additional data to deserialize: Humidity"); return false; } }

#if 0 // OIC IGNORE

    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Wind"))
        {
            mpWind = new ::Wind();
            if (mpWind == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Wind"); return false;}
            if (false == mpWind->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Wind")) {deserializer.setErrorMessage("Additional data to deserialize: Wind"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Doors"))
        {
            mpDoors = new ::Doors();
            if (mpDoors == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Doors"); return false;}
            if (false == mpDoors->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Doors")) {deserializer.setErrorMessage("Additional data to deserialize: Doors"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Door"))
        {
            mpDoor = new ::Door();
            if (mpDoor == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Door"); return false;}
            if (false == mpDoor->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Door")) {deserializer.setErrorMessage("Additional data to deserialize: Door"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Light"))
        {
            mpLight = new ::Light();
            if (mpLight == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Light"); return false;}
            if (false == mpLight->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Light")) {deserializer.setErrorMessage("Additional data to deserialize: Light"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Washer"))
        {
            mpWasher = new ::Washer();
            if (mpWasher == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Washer"); return false;}
            if (false == mpWasher->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Washer")) {deserializer.setErrorMessage("Additional data to deserialize: Washer"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Fridge"))
        {
            mpFridge = new ::Fridge();
            if (mpFridge == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Fridge"); return false;}
            if (false == mpFridge->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Fridge")) {deserializer.setErrorMessage("Additional data to deserialize: Fridge"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Oven"))
        {
            mpOven = new ::Oven();
            if (mpOven == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Oven"); return false;}
            if (false == mpOven->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Oven")) {deserializer.setErrorMessage("Additional data to deserialize: Oven"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Audio"))
        {
            mpAudio = new ::Audio();
            if (mpAudio == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Audio"); return false;}
            if (false == mpAudio->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Audio")) {deserializer.setErrorMessage("Additional data to deserialize: Audio"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("AVSources"))
        {
            mpAVSources = new ::AVSources();
            if (mpAVSources == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  AVSources"); return false;}
            if (false == mpAVSources->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("AVSources")) {deserializer.setErrorMessage("Additional data to deserialize: AVSources"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("AVSource"))
        {
            mpAVSource = new ::AVSource();
            if (mpAVSource == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  AVSource"); return false;}
            if (false == mpAVSource->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("AVSource")) {deserializer.setErrorMessage("Additional data to deserialize: AVSource"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Camera"))
        {
            mpCamera = new ::Camera();
            if (mpCamera == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Camera"); return false;}
            if (false == mpCamera->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Camera")) {deserializer.setErrorMessage("Additional data to deserialize: Camera"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Captures"))
        {
            mpCaptures = new ::Captures();
            if (mpCaptures == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Captures"); return false;}
            if (false == mpCaptures->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Captures")) {deserializer.setErrorMessage("Additional data to deserialize: Captures"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Capture"))
        {
            mpCapture = new ::Capture();
            if (mpCapture == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Capture"); return false;}
            if (false == mpCapture->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Capture")) {deserializer.setErrorMessage("Additional data to deserialize: Capture"); return false; } }

#endif

    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Level"))
        {
            mpLevel = new ::Level();
            if (mpLevel == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Level"); return false;}
            if (false == mpLevel->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Level")) {deserializer.setErrorMessage("Additional data to deserialize: Level"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Messages"))
        {
            mpMessages = new ::Messages();
            if (mpMessages == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Messages"); return false;}
            if (false == mpMessages->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Messages")) {deserializer.setErrorMessage("Additional data to deserialize: Messages"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Message"))
        {
            mpMessage = new ::Message();
            if (mpMessage == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Message"); return false;}
            if (false == mpMessage->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Message")) {deserializer.setErrorMessage("Additional data to deserialize: Message"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Detection"))
        {
            mpDetection = new ::Detection();
            if (mpDetection == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Detection"); return false;}
            if (false == mpDetection->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Detection")) {deserializer.setErrorMessage("Additional data to deserialize: Detection"); return false; } }


    return true ;
}

std::string
Actuators::getElementName()
{
    return EN_ACTUATORS;
}

bool
Actuators::clone(::Actuators *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    {
        if ( NULL != mpDeviceIDs) { delete mpDeviceIDs; mpDeviceIDs = NULL; }

        if ( NULL != pSrcObject->mpDeviceIDs )
        {
            mpDeviceIDs = new ::ArrayOfUuid();
            if (mpDeviceIDs == NULL ) { return false;}
            mpDeviceIDs->members = pSrcObject->mpDeviceIDs->members;
        }
    }
    {
        if ( NULL != mpGroupIDs) { delete mpGroupIDs; mpGroupIDs = NULL; }

        if ( NULL != pSrcObject->mpGroupIDs )
        {
            mpGroupIDs = new ::ArrayOfString();
            if (mpGroupIDs == NULL ) { return false;}
            mpGroupIDs->members = pSrcObject->mpGroupIDs->members;
        }
    }
    {
        if ( NULL != mpOperation) { delete mpOperation; mpOperation = NULL; }

        if ( NULL != pSrcObject->mpOperation )
        {
            mpOperation = new ::Operation();
            if (mpOperation == NULL ) { return false;}
            if (false == mpOperation->clone(pSrcObject->mpOperation)) {return false;} ;
        }
    }
    {
        if ( NULL != mpMode) { delete mpMode; mpMode = NULL; }

        if ( NULL != pSrcObject->mpMode )
        {
            mpMode = new ::Mode();
            if (mpMode == NULL ) { return false;}
            if (false == mpMode->clone(pSrcObject->mpMode)) {return false;} ;
        }
    }
    {
        if ( NULL != mpMovement) { delete mpMovement; mpMovement = NULL; }

        if ( NULL != pSrcObject->mpMovement )
        {
            mpMovement = new ::Movement();
            if (mpMovement == NULL ) { return false;}
            if (false == mpMovement->clone(pSrcObject->mpMovement)) {return false;} ;
        }
    }
    {
        if ( NULL != mpTemperatures) { delete mpTemperatures; mpTemperatures = NULL; }

        if ( NULL != pSrcObject->mpTemperatures )
        {
            mpTemperatures = new ::Temperatures();
            if (mpTemperatures == NULL ) { return false;}
            if (false == mpTemperatures->clone(pSrcObject->mpTemperatures)) {return false;} ;
        }
    }
    {
        if ( NULL != mpTemperature) { delete mpTemperature; mpTemperature = NULL; }

        if ( NULL != pSrcObject->mpTemperature )
        {
            mpTemperature = new ::Temperature();
            if (mpTemperature == NULL ) { return false;}
            if (false == mpTemperature->clone(pSrcObject->mpTemperature)) {return false;} ;
        }
    }
    {
        if ( NULL != mpHumidity) { delete mpHumidity; mpHumidity = NULL; }

        if ( NULL != pSrcObject->mpHumidity )
        {
            mpHumidity = new ::Humidity();
            if (mpHumidity == NULL ) { return false;}
            if (false == mpHumidity->clone(pSrcObject->mpHumidity)) {return false;} ;
        }
    }

    {
        if ( NULL != mpDoors) { delete mpDoors; mpDoors = NULL; }

        if ( NULL != pSrcObject->mpDoors )
        {
            mpDoors = new ::Doors();
            if (mpDoors == NULL ) { return false;}
            if (false == mpDoors->clone(pSrcObject->mpDoors)) {return false;} ;
        }
    }
    {
        if ( NULL != mpDoor) { delete mpDoor; mpDoor = NULL; }

        if ( NULL != pSrcObject->mpDoor )
        {
            mpDoor = new ::Door();
            if (mpDoor == NULL ) { return false;}
            if (false == mpDoor->clone(pSrcObject->mpDoor)) {return false;} ;
        }
    }
    {
        if ( NULL != mpLight) { delete mpLight; mpLight = NULL; }

        if ( NULL != pSrcObject->mpLight )
        {
            mpLight = new ::Light();
            if (mpLight == NULL ) { return false;}
            if (false == mpLight->clone(pSrcObject->mpLight)) {return false;} ;
        }
    }

#if 0 // OIC IGNORE

    {
        if ( NULL != mpWind) { delete mpWind; mpWind = NULL; }

        if ( NULL != pSrcObject->mpWind )
        {
            mpWind = new ::Wind();
            if (mpWind == NULL ) { return false;}
            if (false == mpWind->clone(pSrcObject->mpWind)) {return false;} ;
        }
    }
    {
        if ( NULL != mpWasher) { delete mpWasher; mpWasher = NULL; }

        if ( NULL != pSrcObject->mpWasher )
        {
            mpWasher = new ::Washer();
            if (mpWasher == NULL ) { return false;}
            if (false == mpWasher->clone(pSrcObject->mpWasher)) {return false;} ;
        }
    }
    {
        if ( NULL != mpFridge) { delete mpFridge; mpFridge = NULL; }

        if ( NULL != pSrcObject->mpFridge )
        {
            mpFridge = new ::Fridge();
            if (mpFridge == NULL ) { return false;}
            if (false == mpFridge->clone(pSrcObject->mpFridge)) {return false;} ;
        }
    }
    {
        if ( NULL != mpOven) { delete mpOven; mpOven = NULL; }

        if ( NULL != pSrcObject->mpOven )
        {
            mpOven = new ::Oven();
            if (mpOven == NULL ) { return false;}
            if (false == mpOven->clone(pSrcObject->mpOven)) {return false;} ;
        }
    }
    {
        if ( NULL != mpAudio) { delete mpAudio; mpAudio = NULL; }

        if ( NULL != pSrcObject->mpAudio )
        {
            mpAudio = new ::Audio();
            if (mpAudio == NULL ) { return false;}
            if (false == mpAudio->clone(pSrcObject->mpAudio)) {return false;} ;
        }
    }
    {
        if ( NULL != mpAVSources) { delete mpAVSources; mpAVSources = NULL; }

        if ( NULL != pSrcObject->mpAVSources )
        {
            mpAVSources = new ::AVSources();
            if (mpAVSources == NULL ) { return false;}
            if (false == mpAVSources->clone(pSrcObject->mpAVSources)) {return false;} ;
        }
    }
    {
        if ( NULL != mpAVSource) { delete mpAVSource; mpAVSource = NULL; }

        if ( NULL != pSrcObject->mpAVSource )
        {
            mpAVSource = new ::AVSource();
            if (mpAVSource == NULL ) { return false;}
            if (false == mpAVSource->clone(pSrcObject->mpAVSource)) {return false;} ;
        }
    }
    {
        if ( NULL != mpCamera) { delete mpCamera; mpCamera = NULL; }

        if ( NULL != pSrcObject->mpCamera )
        {
            mpCamera = new ::Camera();
            if (mpCamera == NULL ) { return false;}
            if (false == mpCamera->clone(pSrcObject->mpCamera)) {return false;} ;
        }
    }
    {
        if ( NULL != mpCaptures) { delete mpCaptures; mpCaptures = NULL; }

        if ( NULL != pSrcObject->mpCaptures )
        {
            mpCaptures = new ::Captures();
            if (mpCaptures == NULL ) { return false;}
            if (false == mpCaptures->clone(pSrcObject->mpCaptures)) {return false;} ;
        }
    }
    {
        if ( NULL != mpCapture) { delete mpCapture; mpCapture = NULL; }

        if ( NULL != pSrcObject->mpCapture )
        {
            mpCapture = new ::Capture();
            if (mpCapture == NULL ) { return false;}
            if (false == mpCapture->clone(pSrcObject->mpCapture)) {return false;} ;
        }
    }

#endif // OIC IGNORE

    {
        if ( NULL != mpLevel) { delete mpLevel; mpLevel = NULL; }

        if ( NULL != pSrcObject->mpLevel )
        {
            mpLevel = new ::Level();
            if (mpLevel == NULL ) { return false;}
            if (false == mpLevel->clone(pSrcObject->mpLevel)) {return false;} ;
        }
    }
    {
        if ( NULL != mpMessages) { delete mpMessages; mpMessages = NULL; }

        if ( NULL != pSrcObject->mpMessages )
        {
            mpMessages = new ::Messages();
            if (mpMessages == NULL ) { return false;}
            if (false == mpMessages->clone(pSrcObject->mpMessages)) {return false;} ;
        }
    }
    {
        if ( NULL != mpMessage) { delete mpMessage; mpMessage = NULL; }

        if ( NULL != pSrcObject->mpMessage )
        {
            mpMessage = new ::Message();
            if (mpMessage == NULL ) { return false;}
            if (false == mpMessage->clone(pSrcObject->mpMessage)) {return false;} ;
        }
    }
    {
        if ( NULL != mpDetection) { delete mpDetection; mpDetection = NULL; }

        if ( NULL != pSrcObject->mpDetection )
        {
            mpDetection = new ::Detection();
            if (mpDetection == NULL ) { return false;}
            if (false == mpDetection->clone(pSrcObject->mpDetection)) {return false;} ;
        }
    }


    return true ;
}
