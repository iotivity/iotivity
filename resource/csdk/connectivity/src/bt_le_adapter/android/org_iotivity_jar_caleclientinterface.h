#include <jni.h>
/* Header for class org_iotivity_jar_caleclientinterface */

#ifndef CA_Included_org_iotivity_jar_caleclientinterface_H_
#define CA_Included_org_iotivity_jar_caleclientinterface_H_
#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Class:     org_iotivity_jar_caleclientinterface
 * Method:    CARegisterLeScanCallback
 * Signature: (Landroid/bluetooth/BluetoothAdapter/LeScanCallback;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleclientinterface_CARegisterLeScanCallback
(JNIEnv *, jobject, jobject);

/*
 * Class:     org_iotivity_jar_caleclientinterface
 * Method:    CARegisterLeGattCallback
 * Signature: (Landroid/bluetooth/BluetoothGattCallback;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleclientinterface_CARegisterLeGattCallback
(JNIEnv *, jobject, jobject);

/*
 * Class:     org_iotivity_jar_caleclientinterface
 * Method:    CALeScanCallback
 * Signature: (Landroid/bluetooth/BluetoothDevice;I[B)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleclientinterface_CALeScanCallback
(JNIEnv *, jobject, jobject, jint, jbyteArray);

/*
 * Class:     org_iotivity_jar_caleclientinterface
 * Method:    CALeGattConnectionStateChangeCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;II)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleclientinterface_CALeGattConnectionStateChangeCallback
(JNIEnv *, jobject, jobject, jint, jint);

/*
 * Class:     org_iotivity_jar_caleclientinterface
 * Method:    CALeGattServicesDiscoveredCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleclientinterface_CALeGattServicesDiscoveredCallback
(JNIEnv *, jobject, jobject, jint);

/*
 * Class:     org_iotivity_jar_caleclientinterface
 * Method:    CALeGattCharacteristicReadCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleclientinterface_CALeGattCharacteristicReadCallback
(JNIEnv *, jobject, jobject, jobject, jbyteArray, jint);

/*
 * Class:     org_iotivity_jar_caleclientinterface
 * Method:    CALeGattCharacteristicWritjclasseCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleclientinterface_CALeGattCharacteristicWriteCallback
(JNIEnv *, jobject, jobject, jobject, jbyteArray, jint);

/*
 * Class:     org_iotivity_jar_caleclientinterface
 * Method:    CALeGattCharacteristicChangedCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattCharacteristic;)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleclientinterface_CALeGattCharacteristicChangedCallback
(JNIEnv *, jobject, jobject, jobject, jbyteArray);

/*
 * Class:     org_iotivity_jar_caleclientinterface
 * Method:    CALeGattDescriptorReadCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattDescriptor;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleclientinterface_CALeGattDescriptorReadCallback
(JNIEnv *, jobject, jobject, jobject, jint);

/*
 * Class:     org_iotivity_jar_caleclientinterface
 * Method:    CALeGattDescriptorWriteCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;Landroid/bluetooth/BluetoothGattDescriptor;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleclientinterface_CALeGattDescriptorWriteCallback
(JNIEnv *, jobject, jobject, jobject, jint);

/*
 * Class:     org_iotivity_jar_caleclientinterface
 * Method:    CALeGattReliableWriteCompletedCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleclientinterface_CALeGattReliableWriteCompletedCallback
(JNIEnv *, jobject, jobject, jint);

/*
 * Class:     org_iotivity_jar_caleclientinterface
 * Method:    CALeGattReadRemoteRssiCallback
 * Signature: (Landroid/bluetooth/BluetoothGatt;II)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleclientinterface_CALeGattReadRemoteRssiCallback
(JNIEnv *, jobject, jobject, jint, jint);

/*
 * Class:     org_iotivity_jar_caleclientinterface
 * Method:    CALeStateChangedCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleclientinterface_CALeStateChangedCallback
(JNIEnv *, jobject, jint);

/*
 * Class:     org_iotivity_jar_caleclientinterface
 * Method:    CALeBondStateChangedCallback
 * Signature: (I)V
 */
JNIEXPORT void JNICALL
Java_org_iotivity_jar_caleclientinterface_CALeBondStateChangedCallback
(JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif

