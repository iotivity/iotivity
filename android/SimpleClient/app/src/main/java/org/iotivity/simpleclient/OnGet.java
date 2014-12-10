package org.iotivity.simpleclient;

import android.util.Log;

import org.iotivity.base.AbstractGetCallback;
import org.iotivity.base.OCHeaderOption;
import org.iotivity.base.OCRepresentation;
import org.iotivity.base.OCStackResult;

public class OnGet extends AbstractGetCallback {
    final private static String TAG = "OnGet";
    public void Callback(OCHeaderOption[] options, OCRepresentation rep, int eCode) {
        if(eCode == OCStackResult.OC_STACK_OK) {

            Log.i(TAG, "GET request was successful");
            Log.i(TAG, "Resource URI : " + rep.getUri());

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

            SimpleClient.putLightRepresentation(SimpleClient.curResource);
        }
        else {
           Log.e(TAG, "onGet Response error : " + eCode);
        }
    }

}
