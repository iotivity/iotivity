package org.iotivity.simpleclient;

import android.os.AsyncTask;
import android.widget.Toast;

public class SimpleToast extends AsyncTask<String, String, String> {
    String toastMessage;

    @Override
    protected String doInBackground(String... params) {
        toastMessage = params[0];
        return toastMessage;
    }

    protected void OnProgressUpdate(String... values) {
        super.onProgressUpdate(values);
    }
   // This is executed in the context of the main GUI thread
    protected void onPostExecute(String result){
           Toast toast = Toast.makeText(SimpleClient.mActivity, result, Toast.LENGTH_SHORT);
           toast.show();
    }

}
