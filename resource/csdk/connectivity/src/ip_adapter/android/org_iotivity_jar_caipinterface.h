#include <jni.h>
/* Header for class org_iotivity_jar_caipinterface */

#ifndef _Included_org_iotivity_jar_caipinterface
#define _Included_org_iotivity_jar_caipinterface
#ifdef __cplusplus
extern "C"
{
#endif
/*
 * Class:     org_iotivity_jar_caipinterface
 * Method:    CAIPStateEnabled
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caipinterface_CAIPStateEnabled(JNIEnv *, jclass);

/*
 * Class:     org_iotivity_jar_caipinterface
 * Method:    CAIPStateDisabled
 * Signature: ()V
 */
JNIEXPORT
void JNICALL Java_org_iotivity_jar_caipinterface_CAIPStateDisabled
(JNIEnv *, jclass);

#ifdef __cplusplus
}
#endif
#endif
