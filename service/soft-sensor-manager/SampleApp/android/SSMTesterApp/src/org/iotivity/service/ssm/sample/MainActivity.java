package org.iotivity.service.ssm.sample;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.UUID;

import org.iotivity.service.ssm.*;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
//import android.view.Menu;
import android.view.View;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;

public class MainActivity extends Activity {
	
	private SSMInterface SoftSensorManager = null;
	private List<Integer>	mRunningQueries = new ArrayList<Integer>();
		
	private IQueryEngineEvent mQueryEngineEventListener = new IQueryEngineEvent()
	{
		@Override
		public void OnQueryEngineEvent(int cqid, DataReader result)
		{
			Log.i("[SSM]", "event received! cqid=" + cqid);
			PrintLog("Event from cqid " + cqid + " has received");
						
			List<String> models = result.GetAffectedModels();
			
			for(String modelName : models)
			{
				PrintLog("Model: " + modelName);
				int dataCount = result.GetModelDataCount(modelName);
				for(int i=0; i < dataCount; i++)
				{
					ModelData modelData = 
							result.GetModelData(modelName, i);
					for(int j=0; j < modelData.GetPropertyCount(); j++)
					{
						PrintLog("Name: " + modelData.GetPropertyName(j) + 
								" Value: " + modelData.GetPropertyValue(j));
					}
				}
			}
		}
	};
	
	void PrintLog(String log)
	{
		Message msg = new Message();
		Bundle data = new Bundle();
		data.putString("Log", log);
		msg.setData(data);
		logHandler.sendMessage(msg);
	}
	
	private Handler logHandler = new Handler()
	{
		@Override
		public void handleMessage(Message msg)
		{
			tvLog.append(msg.getData().getString("Log") + "\n");
			svLog.fullScroll(ScrollView.FOCUS_DOWN);
		}
	};
	
	private TextView 	tvLog = null;
	private ScrollView 	svLog = null;
	private EditText	edtQuery = null;
	private EditText 	edtUnregisterQuery = null;
    
    View.OnClickListener clickHandler = new View.OnClickListener() {
		
		public void onClick(View v)
		{
			switch(v.getId())
			{
			case R.id.btnRegisterQuery:
				int cqid = SoftSensorManager.registerQuery(edtQuery.getText().toString(), mQueryEngineEventListener);
				mRunningQueries.add(cqid);
				PrintLog(edtQuery.getText().toString() + " has executed, cqid=" + cqid);
				break;
				
			case R.id.btnUnregisterQuery:
				Iterator<Integer> it = mRunningQueries.iterator();
				while(it.hasNext())
				{
					if(it.next() == Integer.parseInt(edtUnregisterQuery.getText().toString()))
					{
						SoftSensorManager.unregisterQuery(Integer.parseInt(edtUnregisterQuery.getText().toString()));
						PrintLog("Unregister Query has executed, cqid=" + Integer.parseInt(edtUnregisterQuery.getText().toString()));
						it.remove();
					}
				}
				break;
				
			case R.id.btPlus:
				int queryNum = Integer.parseInt(edtUnregisterQuery.getText().toString()) + 1;
				edtUnregisterQuery.setText(queryNum + "");
				break;
				
			case R.id.btMinus:
				queryNum = Integer.parseInt(edtUnregisterQuery.getText().toString()) - 1;
				edtUnregisterQuery.setText(queryNum + "");
				break;
			}
		}
	};
	
	 View.OnClickListener textAddHandler = new View.OnClickListener() {
			
			public void onClick(View v)
			{
				switch(v.getId())
				{
				case R.id.btClear :
					edtQuery.setText("");
					break;
				
				case R.id.btLogClear :
					tvLog.setText("");
					break;
				
				case R.id.btFullDevice:
					edtQuery.setText("subscribe Device if Device.dataId != 0");
					break;
				
				case R.id.btDiscomfortIndex:
					edtQuery.setText("subscribe Device.DiscomfortIndexSensor if Device.DiscomfortIndexSensor.discomfortIndex > 0");
					break;
				}
			}
		};
		
    @Override
    public void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        tvLog = (TextView)findViewById(R.id.txtLog);
        svLog = (ScrollView)findViewById(R.id.sclLog);
        edtQuery = (EditText)findViewById(R.id.editQuery);
        edtUnregisterQuery = (EditText)findViewById(R.id.editUnregisterQuery);
        
        findViewById(R.id.btnRegisterQuery).setOnClickListener(clickHandler);     
        findViewById(R.id.btnUnregisterQuery).setOnClickListener(clickHandler);      
        findViewById(R.id.btFullDevice).setOnClickListener(textAddHandler);       
        findViewById(R.id.btDiscomfortIndex).setOnClickListener(textAddHandler);
        findViewById(R.id.btPlus).setOnClickListener(clickHandler);        
        findViewById(R.id.btMinus).setOnClickListener(clickHandler);
        findViewById(R.id.btClear).setOnClickListener(textAddHandler);
        findViewById(R.id.btLogClear).setOnClickListener(textAddHandler);
        
		copyFiles("lib");
        
        SoftSensorManager = new SSMInterface();
        
        String initConfig = "<SSMCore>" +
    			"<Device>" +
				"<UDN>"+getUUID()+"</UDN>" +
				"<Name>MyMobile</Name>" +
				"<Type>Mobile</Type>" +
				"</Device>" +
				"<Config>" +
				"<SoftSensorRepository>/data/data/" +  getPackageName() + "/files/</SoftSensorRepository>" +
				"<SoftSensorDescription>/data/data/" + getPackageName() + "/files/SoftSensorDescription.xml</SoftSensorDescription>" + 
				"</Config>" +
				"</SSMCore>";
        
        try 
        {
			SoftSensorManager.startSSMCore(initConfig);
		} 
        catch (Exception e) 
        {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
      
    @Override
    protected void onDestroy()
    {
    	SoftSensorManager.stopSSMCore();
    	
    	super.onDestroy();
    }
    
	private String getUUID() 
	{
		String 	PREF_UNIQUE_ID = "PREF_UNIQUE_ID";
		SharedPreferences sharedPrefs = getSharedPreferences(PREF_UNIQUE_ID, Context.MODE_PRIVATE);
		String uniqueID = sharedPrefs.getString(PREF_UNIQUE_ID, null);
	    
		if (uniqueID == null) 
		{
	       uniqueID = UUID.randomUUID().toString();
	       Editor editor = sharedPrefs.edit();
	       editor.putString(PREF_UNIQUE_ID, uniqueID);
	       editor.commit();
	    }
	    
		return uniqueID;
	}
		
	private void copyFiles(String path)
    {		 
		AssetManager assetManager = getAssets();
	    String assets[] = null;
	    
	    try 
	    {
	        assets = assetManager.list(path);
	        
	        if (assets.length == 0) 
	        {	        	
	            copyFile(path);
	        } 
	        else 
	        {
	            String fullPath = "/data/data/" + this.getClass().getPackage().toString() + "/" + path;
	            File dir = new File(fullPath);
	            
	            if (!dir.exists())
	                dir.mkdir();
	            for (int i = 0; i < assets.length; ++i) 
	            {
	            	copyFiles(path + "/" + assets[i]);
	            }
	        }
	    }
	    catch (IOException ex) 
	    {	
	        Log.e("tag", "I/O Exception", ex);
	    }
	}
	    
	private void copyFile(String filename) 
	{
	    AssetManager assetManager = getAssets();
	    InputStream in = null;
	    OutputStream out = null;
	    
	    try 
	    {
	        in = assetManager.open(filename);
	        out = openFileOutput(filename.split("/")[1], Context.MODE_PRIVATE);
	       
	        byte[] buffer = new byte[1024];
	        int read;

	        while ((read = in.read(buffer)) != -1) 
	        {
	            out.write(buffer, 0, read);
	        }
	        
	        in.close();
	        in = null;
	        out.flush();
	        out.close();
	        out = null;
	    }
	    catch (Exception e)
	    {
	        Log.e("tag", e.getMessage());
	    }
	}
}
