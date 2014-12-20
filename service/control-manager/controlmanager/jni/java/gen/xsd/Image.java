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

package gen.xsd;

import OC.Cm.Serialization.IDeserializer;
import OC.Cm.Serialization.ISerializer;

/**
 * This class defines methods that serialize and/or deserialize contents of XSD
 * complex/list type Image.
 * 
 * @since 1.0.0
 */
public class Image extends OC.Cm.Serialization.ISerializable {
    /**
     * [Optional] Indicate the format type of image.
     */
    public gen.xsd.ImageFormatType ImageFormat;

    /**
     * [Optional] Indicate the image size.
     */
    public Long                    imageSize;

    /**
     * [Optional] This attribute contains binary stream of captured image.
     */
    public String                  capturedImage;

    /**
     * [Optional] This attribute contains a string of the target URL or ID to
     * save the captured video. (e.g., â€œhttp://........â€�,
     * â€œIMEI42f00aaaaabb06fb90â€�)
     */
    public String                  saveLocation;

    /**
     * Default constructor of Image class.
     * 
     * @since 1.0.0
     */
    public Image() {
    }

    /**
     * Destroy method for Image class.
     * 
     * @since 1.0.0
     */
    @Override
    public void destroy() {
        super.destroy();
    }

    /**
     * Returns name of the serializable.
     * 
     * @since 1.0.0
     */
    @Override
    public String getElementName() {
        return Elements.EN_IMAGE;
    }

    /**
     * Serializes the contents of Image object.
     * 
     * @param serializer
     *            Serializer object that provides methods to serialize.
     * 
     * @return True if serialization is successful / False if serialization
     *         failed
     * @since 1.0.0
     */
    @Override
    public boolean serializeContents(ISerializer serializer) {
        boolean retValue = true;
        try {
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != ImageFormat) {
                    if (ImageFormat.value.length() > 0) {
                        if (true == ImageFormat.validateContent()) {
                            bRet = serializer.setStringPropertyValue(
                                    "imageFormat", ImageFormat.value);
                        } else
                            bRet = false;
                    }
                    if (false == bRet)
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " imageFormat");
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != imageSize) {
                    bRet = serializer.setUnsignedInt("imageSize", imageSize);
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " imageSize");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != capturedImage) {
                    if (capturedImage.length() > 0) {
                        bRet = serializer.setStringPropertyValue(
                                "capturedImage", capturedImage);
                    }
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " capturedImage");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            if (serializer.validateMandatory() == true) {
                boolean bRet = true;
                if (null != saveLocation) {
                    if (saveLocation.length() > 0) {
                        bRet = serializer.setStringPropertyValue(
                                "saveLocation", saveLocation);
                    }
                    if (false == bRet) {
                        serializer
                                .setErrorMessage("Failed To Serialize parameter (Invalid content) : "
                                        + " saveLocation");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }

    /**
     * Deserializes data and prepares the Image object contents.
     * 
     * @param deserializer
     *            Deserializer object that provides methods to deserialize.
     * 
     * @return True if deserialization is successful / False if deserialization
     *         failed
     * @since 1.0.0
     */
    @Override
    public boolean deSerializeContents(IDeserializer deserializer) {
        boolean retValue = true;
        try {
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("imageFormat")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        ImageFormat = new gen.xsd.ImageFormatType();
                        ImageFormat.value = deserializer
                                .getStringPropertyValue("imageFormat");
                        if (false == ImageFormat.validateContent()) {
                            deserializer
                                    .setErrorMessage("Parameter Contains Invalid Value : "
                                            + " imageFormat");
                            bRet = false;
                        } else
                            bRet = true;
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("imageSize")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        imageSize = deserializer.getUnsignedInt("imageSize");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("capturedImage")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        capturedImage = deserializer
                                .getStringPropertyValue("capturedImage");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
            {
                boolean bRet = true;
                if (true == deserializer.hasProperty("saveLocation")) {
                    if (deserializer.validateMandatory() == false)
                        bRet = false;
                    if (bRet == true) {
                        saveLocation = deserializer
                                .getStringPropertyValue("saveLocation");
                    }
                }
                if (retValue == true)
                    retValue = bRet;
            }
        } catch (Exception e) {
            return false;
        }
        return retValue;

    }
}
