#include <jni.h>
/* Header for class org_iotivity_ca_CaWiFiInterface */

#ifndef _Included_org_iotivity_ca_CaWiFiInterface
#define _Included_org_iotivity_ca_CaWiFiInterface
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_iotivity_ca_CaWiFiInterface
 * Method:    CAWiFiStateEnabled
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaWiFiInterface_CAWiFiStateEnabled
(JNIEnv *, jclass);

/*
 * Class:     org_iotivity_ca_CaWiFiInterface
 * Method:    CAWiFiStateDisabled
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaWiFiInterface_CAWiFiStateDisabled
(JNIEnv *, jclass);

#ifdef __cplusplus
}
#endif
#endif

