//******************************************************************
// Copyright 2018 Cable Television Laboratories, Inc.
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
// See the License for the specific lan guage governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef IOTVT_SRM_SPR_H
#define IOTVT_SRM_SPR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SP_SUPPORTED_PROFILES = 0,
    SP_CURRENT_PROFILE,
    SP_PROPERTY_COUNT
} SpProperty_t;


/**
 * Initialize sp resource by loading data from persistent storage.
 *
 * @return ::OC_STACK_OK, if initialization is successful, else ::OC_STACK_ERROR if
 * initialization fails.
 */
OCStackResult InitSpResource();

/**
 * Perform cleanup for sp resources.
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult DeInitSpResource();

/**
 * This function deallocates the memory for OicSecSp_t.
 *
 * @param sp is the pointer to @ref OicSecSp_t.
 */
void DeleteSpBinData(OicSecSp_t* sp);

/**
 * Converts SP into the cbor payload, including all Properties for a
 * full representation.
 *
 * @param sp       pointer to the initialized sp structure.
 * @param payload  pointer to sp cbor payload. OICFree(sp) must be called when done
 * @param size     size of the cbor payload converted.
 *                 0 in case of error, else a positive value if successful.
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult SpToCBORPayload(const OicSecSp_t *sp,
                              uint8_t **payload, size_t *size);

/**
 * Converts SP into the cbor payload, including only the
 * Properties marked "true" in the propertiesToInclude array.
 *
 * @param sp       pointer to the initialized sp structure.
 * @param payload  pointer to sp cbor payload. OICFree(sp) must be called when done
 * @param size     size of the cbor payload converted.
 *                 0 in case of error, else a positive value if successful.
 * @param propertiesToInclude Array of bools, size "SP_PROPERTY_COUNT",
 *                            where "true" indicates the corresponding property should
 *                            be included in the CBOR representation that is created.
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult SpToCBORPayloadPartial(const OicSecSp_t *sp,
                                     uint8_t **payload, size_t *size,
                                     const bool *propertiesToInclude);


/**
 * This method converts cbor into SP data.
 *
 * @param [i] cborPayload is the sp data in cbor format.
 * @param [i] size of the cborPayload. In case 0 is provided it assigns CBOR_SIZE (255) value.
 * @param [o] sp pointer to @ref OicSecSp_t to be populated with data
 * @param [o] decodedProperties Array of bools, size "SP_PROPERTY_COUNT",
 *                              where "true" indicates the corresponding property was
 *                              found in the CBOR and succesfully decoded and added to sp.
 *                              Can be NULL if you don't need these results
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
*/
OCStackResult CBORPayloadToSp(const uint8_t *cborPayload,
                              const size_t size,
                              OicSecSp_t **sp,
                              bool *decodedProperties);

/**
 * Return the index of a profile withing a list of supported profiles
 *
 * @param [i] supportedLen       number of entries in the supportedProfiles array
 * @param [i] supportedProfiles  array of supported profiles
 * @param [i] profileName        profile to find in supportedProfiles
 *
 * @return    index of the profileName withing supportedProfiles
 *            -1 if profileName not in supportedProfiles
 */
int ProfileIdx( size_t supportedLen, char **supportedProfiles, char *profileName);


/**
 * determine if required sp properties are present and valid
 *
 * @param [i] sp                sp structure against which to check props
 * @param [i] propertiesPresent array of bools, size "SP_PROPERTY_COUNT", where "true"
 *                              indicates that the property value has been set in sp
 *
 * @return ::true if required properties are present and valid, otherwise false
*/
bool RequiredSpPropsPresentAndValid(OicSecSp_t* sp, bool *propertiesPresent);

/**
 * Log the contents of an sp resource
 *
 * @param sp    sp structure to log
 * @param level logging level to use
 * @param tag   logging tag to use
 * @param msg   message to log prior to logging the SP values
 *              can be NULL for no message
 */
void LogSp(OicSecSp_t* sp, int level, const char* tag, const char* msg);

/**
 * Compare the contents of two sp property arrays for equality
 *
 * @param spProps1 first array with prop state (size "SP_PROPERTY_COUNT")
 * @param spProps2 first array with prop state (size "SP_PROPERTY_COUNT")
 *
 * @return true if contents of sp1 and sp2 are the same, otherwise false
 */
bool IsPropArraySame(bool* spProps1, bool* spProps2);

/**
 * Compare the contents of two sp structs for equality
 *
 * @param sp1 first sp for compare
 * @param sp2 second sp for compare
 * @param propertiesToCheck properties to check for equality (or NULL to check all properties)
 *                          size "SP_PROPERTY_COUNT", entries = true for props to compare
 *
 * @return true if the requested props of sp1 and sp2 are the same, otherwise false
 */
bool IsSpSame(OicSecSp_t* sp1, OicSecSp_t* sp2,  bool *propertiesToCheck);

/**
 * Set all of the values in an sp props array to either true or false
 *
 * @param spProps security profiles property array (size "SP_PROPERTY_COUNT")
 * @param setTo value to set all entries in the array to
 *
 */
void SetAllSpProps(bool* spProps, bool setTo);

/**
 * Make a copy of testSp, and install it as the SP resource.
 * Only for testing (does not update persitant storage)
 *
 * @param testSp sp resource to install
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
*/
OCStackResult InstallTestSp(OicSecSp_t* testSp);

#ifdef __cplusplus
}
#endif

#endif //IOTVT_SRM_SPR_H
