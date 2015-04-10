#include <jni.h>
/* Header for class com_iotivity_jar_cawifiinterface */

#ifndef _Included_com_iotivity_jar_cawifiinterface
#define _Included_com_iotivity_jar_cawifiinterface
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_iotivity_jar_cawifiinterface
 * Method:    CAWiFiStateEnabled
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_iotivity_jar_cawifiinterface_CAWiFiStateEnabled
(JNIEnv *, jclass);

/*
 * Class:     com_iotivity_jar_cawifiinterface
 * Method:    CAWiFiStateDisabled
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_iotivity_jar_cawifiinterface_CAWiFiStateDisabled
(JNIEnv *, jclass);

#ifdef __cplusplus
}
#endif
#endif

