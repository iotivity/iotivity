#include <jni.h>
/* Header for class org_iotivity_ca_caleinterface */

#ifndef _Included_org_iotivity_ca_caleinterface
#define _Included_org_iotivity_ca_caleinterface
#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CARegisterLeScanCallback
 * Signature: (Landroid/bluetooth/BluetoothAdapter/LeScanCallback;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CARegisterLeScanCallback
(JNIEnv *, jobject, jobject);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CARegisterLeGattCallback
 * Signature: (Landroid/bluetooth/BluetoothGattCallback;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CARegisterLeGattCallback
(JNIEnv *, jobject, jobject);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CARegisterLeGattServerCallback
 * Signature: (Landroid/bluetooth/BluetoothGattServerCallback;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CARegisterLeGattServerCallback
(JNIEnv *, jobject, jobject);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CARegisterBluetoothLeAdvertiseCallback
 * Signature: (Landroid/bluetooth/le/AdvertiseCallback;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CARegisterBluetoothLeAdvertiseCallback
(JNIEnv *, jobject, jobject);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeScanCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;I[B)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeScanCallback
(JNIEnv *, jobject, jobject, jint, jbyteArray);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattConnectionStateChangeCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;II)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattConnectionStateChangeCallback
(JNIEnv *, jobject, jobject, jint, jint);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattServicesDiscoveredCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServicesDiscoveredCallback
(JNIEnv *, jobject, jobject, jint);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattCharacteristicReadCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattCharacteristicReadCallback
(JNIEnv *, jobject, jobject, jobject, jbyteArray, jint);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattCharacteristicWritjclasseCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattCharacteristicWriteCallback
(JNIEnv *, jobject, jobject, jobject, jbyteArray, jint);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattCharacteristicChangedCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattCharacteristicChangedCallback
(JNIEnv *, jobject, jobject, jobject, jbyteArray);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattDescriptorReadCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattDescriptor;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattDescriptorReadCallback
(JNIEnv *, jobject, jobject, jobject, jint);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattDescriptorWriteCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattDescriptor;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattDescriptorWriteCallback
(JNIEnv *, jobject, jobject, jobject, jint);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattReliableWriteCompletedCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattReliableWriteCompletedCallback
(JNIEnv *, jobject, jobject, jint);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattReadRemoteRssiCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;II)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattReadRemoteRssiCallback
(JNIEnv *, jobject, jobject, jint, jint);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattServerConnectionStateChangeCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;II)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerConnectionStateChangeCallback
(JNIEnv *, jobject, jobject, jint, jint);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattServerServiceAddedCallback
 * Signature: (ILandroid/bluetooth/BluetoothGattService;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerServiceAddedCallback
(JNIEnv *, jobject, jint, jobject);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattServerCharacteristicReadRequestCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;IILandroid/
 * bluetooth/BluetoothGattCharacteristic;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerCharacteristicReadRequestCallback
(JNIEnv *, jobject, jobject, jint, jint, jobject, jbyteArray);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattServerCharacteristicWriteRequestCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;ILandroid/bluetooth/
 * BluetoothGattCharacteristic;ZZI[B)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerCharacteristicWriteRequestCallback
(JNIEnv *, jobject, jobject, jint, jobject, jbyteArray, jboolean, jboolean, jint, jbyteArray);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattServerDescriptorReadRequestCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;IILandroid/bluetooth/
 * BluetoothGattDescriptor;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerDescriptorReadRequestCallback
(JNIEnv *, jobject, jobject, jint, jint, jobject);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattServerDescriptorWriteRequestCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;ILandroid/bluetooth/
 * BluetoothGattDescriptor;ZZI[B)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerDescriptorWriteRequestCallback
(JNIEnv *, jobject, jobject, jint, jobject, jboolean, jboolean, jint, jbyteArray);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattServerExecuteWriteCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;IZ)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerExecuteWriteCallback
(JNIEnv *, jobject, jobject, jint, jboolean);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeGattServerNotificationSentCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeGattServerNotificationSentCallback
(JNIEnv *, jobject, jobject, jint);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeAdvertiseStartSuccessCallback
 * Signature: (Landroid/bluetooth/le/AdvertiseSettings;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeAdvertiseStartSuccessCallback
(JNIEnv *, jobject, jobject);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeAdvertiseStartFailureCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeAdvertiseStartFailureCallback
(JNIEnv *, jobject, jint);

/*
 * Class:     org_iotivity_ca_caleinterface
 * Method:    CALeStateChangedCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ca_caleinterface_CALeStateChangedCallback
(JNIEnv *, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif

