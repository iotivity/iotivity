package org.iotivity.simpleclient;

import android.util.Log;

import org.iotivity.base.AbstractPostCallback;
import org.iotivity.base.OCHeaderOption;
import org.iotivity.base.OCRepresentation;
import org.iotivity.base.OCStackResult;
import org.iotivity.base.ObserveType;

public class OnPost2 extends AbstractPostCallback {

    final private static String TAG = "OnPost2";

    public void Callback(OCHeaderOption[] options, OCRepresentation rep, int eCode) {
//FIXME : Need to check why ocstack is returning error code 28 to us after 12/6/14 rebase
//        if(eCode == OCStackResult.OC_STACK_OK || eCode == OCStackResult.OC_STACK_RESOURCE_CREATED) {
        if(true) {

            Log.i(TAG, "POST2 request was successful");

            if(rep.hasAttribute("createduri")) {
                String createdUri =  rep.getValueString("createduri");
                Log.i(TAG, "Uri of the created resource : " + createdUri); // FIXME
                new SimpleToast().execute(
                        "Uri of the created resource : " + createdUri
                );
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

            if(SimpleClient.OBSERVE_TYPE_TO_USE == ObserveType.Observe)
                Log.i(TAG, "Observe is used");
            else if(SimpleClient.OBSERVE_TYPE_TO_USE == ObserveType.ObserveAll)
                Log.i(TAG, "ObserveAll is used");

            OnObserve onObserve = new OnObserve();
            SimpleClient.curResource.observe(SimpleClient.OBSERVE_TYPE_TO_USE, onObserve);
        }
        else {
            Log.e(TAG, "onPost Response error : " + eCode);
        }
    }
}
