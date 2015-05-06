#include <jni.h>
/* Header for class org_iotivity_jar_caedrinterface */

#ifndef Included_org_iotivity_jar_caedrinterface
#define Included_org_iotivity_jar_caedrinterface
#ifdef __cplusplus
extern "C"
{
#endif
/*
 * Class:     org_iotivity_jar_caedrinterface
 * Method:    CAEDRStateChangedCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caedrinterface_CAEDRStateChangedCallback
(JNIEnv *, jobject, jint);

/*
 * Class:     org_iotivity_jar_caedrinterface
 * Method:    CAEDRStateChangedCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caedrinterface_CAEDRBondStateChangedCallback
(JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
