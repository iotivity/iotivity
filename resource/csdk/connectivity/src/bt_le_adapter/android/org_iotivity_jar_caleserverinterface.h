#include <jni.h>
/* Header for class org_iotivity_jar_caleserverinterface */

#ifndef CA_Included_org_iotivity_jar_caleserverinterface_H_
#define CA_Included_org_iotivity_jar_caleserverinterface_H_
#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Class:     org_iotivity_jar_caleserverinterface
 * Method:    CARegisterLeGattServerCallback
 * Signature: (Landroid/bluetooth/BluetoothGattServerCallback;)V
 */
JNIEXPORT void JNICALL

Java_org_iotivity_jar_caleserverinterface_CARegisterLeGattServerCallback
(JNIEnv *, jobject, jobject);

/*
 * Class:     org_iotivity_jar_caleserverinterface
 * Method:    CARegisterBluetoothLeAdvertiseCallback
 * Signature: (Landroid/bluetooth/le/AdvertiseCallback;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleserverinterface_CARegisterBluetoothLeAdvertiseCallback
(JNIEnv *, jobject, jobject);

/*
 * Class:     org_iotivity_jar_caleserverinterface
 * Method:    CALeGattServerConnectionStateChangeCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;II)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleserverinterface_CALeGattServerConnectionStateChangeCallback
(JNIEnv *, jobject, jobject, jint, jint);

/*
 * Class:     org_iotivity_jar_caleserverinterface
 * Method:    CALeGattServerServiceAddedCallback
 * Signature: (ILandroid/bluetooth/BluetoothGattService;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleserverinterface_CALeGattServerServiceAddedCallback
(JNIEnv *, jobject, jint, jobject);

/*
 * Class:     org_iotivity_jar_caleserverinterface
 * Method:    CALeGattServerCharacteristicReadRequestCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;IILandroid/
 * bluetooth/BluetoothGattCharacteristic;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleserverinterface_CALeGattServerCharacteristicReadRequestCallback
(JNIEnv *, jobject, jobject, jint, jint, jobject, jbyteArray);

/*
 * Class:     org_iotivity_jar_caleserverinterface
 * Method:    CALeGattServerCharacteristicWriteRequestCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;ILandroid/bluetooth/
 * BluetoothGattCharacteristic;ZZI[B)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleserverinterface_CALeGattServerCharacteristicWriteRequestCallback
(JNIEnv *, jobject, jobject, jint, jobject, jbyteArray, jboolean, jboolean, jint, jbyteArray);

/*
 * Class:     org_iotivity_jar_caleserverinterface
 * Method:    CALeGattServerDescriptorReadRequestCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;IILandroid/bluetooth/
 * BluetoothGattDescriptor;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleserverinterface_CALeGattServerDescriptorReadRequestCallback
(JNIEnv *, jobject, jobject, jint, jint, jobject);

/*
 * Class:     org_iotivity_jar_caleserverinterface
 * Method:    CALeGattServerDescriptorWriteRequestCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;ILandroid/bluetooth/
 * BluetoothGattDescriptor;ZZI[B)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleserverinterface_CALeGattServerDescriptorWriteRequestCallback
(JNIEnv *, jobject, jobject, jint, jobject, jboolean, jboolean, jint, jbyteArray);

/*
 * Class:     org_iotivity_jar_caleserverinterface
 * Method:    CALeGattServerExecuteWriteCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;IZ)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleserverinterface_CALeGattServerExecuteWriteCallback
(JNIEnv *, jobject, jobject, jint, jboolean);

/*
 * Class:     org_iotivity_jar_caleserverinterface
 * Method:    CALeGattServerNotificationSentCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleserverinterface_CALeGattServerNotificationSentCallback
(JNIEnv *, jobject, jobject, jint);

/*
 * Class:     org_iotivity_jar_caleserverinterface
 * Method:    CALeAdvertiseStartSuccessCallback
 * Signature: (Landroid/bluetooth/le/AdvertiseSettings;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleserverinterface_CALeAdvertiseStartSuccessCallback
(JNIEnv *, jobject, jobject);

/*
 * Class:     org_iotivity_jar_caleserverinterface
 * Method:    CALeAdvertiseStartFailureCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleserverinterface_CALeAdvertiseStartFailureCallback
(JNIEnv *, jobject, jint);


#ifdef __cplusplus
}
#endif
#endif

