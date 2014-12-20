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

#include "Image.h"


#include "Elements.h"

using namespace std;

Image::Image()
{

    mpImageFormat = NULL;
    mpImageSize = NULL;
    mpCapturedImage = NULL;
    mpSaveLocation = NULL;

}

Image::~Image()
{
    if ( NULL != mpImageFormat) { delete mpImageFormat; mpImageFormat = NULL; }
    if ( NULL != mpImageSize) { delete mpImageSize; mpImageSize = NULL; }
    if ( NULL != mpCapturedImage) { delete mpCapturedImage; mpCapturedImage = NULL; }
    if ( NULL != mpSaveLocation) { delete mpSaveLocation; mpSaveLocation = NULL; }

}

bool
Image::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpImageFormat )
        {
            if (mpImageFormat->value.length() > 0)
            {
                if (true == mpImageFormat->validateContent())
                {
                    bRet = serializer.setPropertyValue("imageFormat", mpImageFormat->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("imageFormat") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : imageFormat"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpImageSize )
        {
            if (true == mpImageSize->validateContent())
            {
                bRet = serializer.setPropertyValue("imageSize", mpImageSize->value);
            }
            else { bRet = false ;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("imageSize") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : imageSize"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpCapturedImage )
        {
            if (mpCapturedImage->length() > 0)
            {
                bRet = serializer.setPropertyValue("capturedImage", *mpCapturedImage);
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("capturedImage") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : capturedImage"); return false; }}
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
Image::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (true == deserializer.hasProperty("imageFormat"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  imageFormat"); return false;}
        mpImageFormat = new ::ImageFormatType();
        if (mpImageFormat == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  imageFormat"); return false;}
        if (false == deserializer.getPropertyValue("imageFormat", &(mpImageFormat->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  imageFormat"); return false ;}
        if (false == mpImageFormat->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : imageFormat"); return false ;}
    }
    if (true == deserializer.hasProperty("imageSize"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  imageSize"); return false;}
        mpImageSize = new ::UInt64();
        if (mpImageSize == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  imageSize"); return false;}
        if (false == deserializer.getPropertyValue("imageSize", &(mpImageSize->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  imageSize"); return false ;}
        if (false == mpImageSize->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : imageSize"); return false ;}
    }
    if (true == deserializer.hasProperty("capturedImage"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  capturedImage"); return false;}
        mpCapturedImage = new std::string();
        if (mpCapturedImage == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  capturedImage"); return false;}
        if (false == deserializer.getPropertyValue("capturedImage", mpCapturedImage)) { deserializer.setErrorMessage("Failed to deserialize parameter :  capturedImage"); return false ;}
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
Image::getElementName()
{
    return EN_IMAGE;
}

bool
Image::clone(::Image *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    {
        if ( NULL != mpImageFormat) { delete mpImageFormat; mpImageFormat = NULL; }

        if ( NULL != pSrcObject->mpImageFormat )
        {
            mpImageFormat = new ::ImageFormatType();
            if (mpImageFormat == NULL ) {return false;}
            mpImageFormat->value = (pSrcObject->mpImageFormat->value );
        }
    }
    {
        if ( NULL != mpImageSize) { delete mpImageSize; mpImageSize = NULL; }

        if ( NULL != pSrcObject->mpImageSize )
        {
            mpImageSize = new ::UInt64();
            if (mpImageSize == NULL ) {return false;}
            mpImageSize->value = (pSrcObject->mpImageSize->value );
        }
    }
    {
        if ( NULL != mpCapturedImage) { delete mpCapturedImage; mpCapturedImage = NULL; }

        if ( NULL != pSrcObject->mpCapturedImage )
        {
            mpCapturedImage = new std::string();
            if (mpCapturedImage == NULL ) {return false;}
            *mpCapturedImage = *(pSrcObject->mpCapturedImage );
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
