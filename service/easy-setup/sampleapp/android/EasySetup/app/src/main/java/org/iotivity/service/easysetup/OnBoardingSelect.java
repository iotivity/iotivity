package org.iotivity.service.easysetup;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.Toast;

public class OnBoardingSelect extends Activity {

    RadioButton radio_wifi, radio_ble;
    Button button_continue;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_on_boarding_select);

        radio_wifi = (RadioButton) findViewById(R.id.radioWifi);
        radio_ble = (RadioButton) findViewById(R.id.radioBle);
        button_continue = (Button) findViewById(R.id.button_continue);

        button_continue.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent;
                if (radio_wifi.isChecked())
                    intent = new Intent(OnBoardingSelect.this, MainActivity.class);
                else if (radio_ble.isChecked())
                    intent = new Intent(OnBoardingSelect.this, BLEActivity.class);
                else {

                    Toast.makeText(OnBoardingSelect.this, "Please Select OnBoarding method " +
                            "before Continuing", Toast.LENGTH_SHORT).show();
                    return;
                }
                startActivity(intent);
            }
        });
    }


}
