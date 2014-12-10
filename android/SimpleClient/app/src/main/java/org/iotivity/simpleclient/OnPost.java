package org.iotivity.simpleclient;

import android.util.Log;

import org.iotivity.base.AbstractPostCallback;
import org.iotivity.base.OCHeaderOption;
import org.iotivity.base.OCRepresentation;

public class OnPost extends AbstractPostCallback {

    final private static String TAG = "OnPost";

    public void Callback(OCHeaderOption[] options, OCRepresentation rep, int eCode) {
//FIXME : Need to check why ocstack is returning error code 28 to us after 12/6/14 rebase
//        if(eCode == OCStackResult.OC_STACK_OK || eCode == OCStackResult.OC_STACK_RESOURCE_CREATED) {
        if(true) {
                Log.i(TAG, "POST request was successful");

            if(rep.hasAttribute("createduri")) {
                Log.i(TAG, "Uri of the created resource : " + rep.getValueString("createduri"));
            }
            else {
                SimpleClient.mylight.m_state = rep.getValueBool("state");
                Log.i(TAG, "state : " + SimpleClient.mylight.m_state);
                SimpleClient.mylight.m_power = rep.getValueInt("power");
                Log.i(TAG, "power : " + SimpleClient.mylight.m_power);
                SimpleClient.mylight.m_name = rep.getValueString("name");
                Log.i(TAG, "name  : " + SimpleClient.mylight.m_name);

                new SimpleToast().execute(
                        "state : " + SimpleClient.mylight.m_state +
                                "\npower : " + SimpleClient.mylight.m_power +
                                "\nname  : " + SimpleClient.mylight.m_name
                );
            }

            OCRepresentation rep2 = new OCRepresentation();

            Log.e(TAG, "Posting Light Representation ...");

            SimpleClient.mylight.m_state = true;
            SimpleClient.mylight.m_power = 55;

            Log.i(TAG, "setValueBool : " + SimpleClient.mylight.m_state);
            Log.i(TAG, "setValueInt  : " + SimpleClient.mylight.m_power);
            rep2.setValueBool("state", SimpleClient.mylight.m_state);
            rep2.setValueInt("power", SimpleClient.mylight.m_power);

            OnPost2 onPost2 = new OnPost2();
            SimpleClient.curResource.post(rep2, onPost2);
        }
        else {
            Log.e(TAG, "onPost Response error : " + eCode);
        }

    }
}
