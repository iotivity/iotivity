package org.iotivity.service.ssm;

import java.util.List;

import android.util.Log;

public class CoreController 
{
	static
	{
		try
		{
			String workingPath = System.getProperty( "user.dir" );
			
			Log.i("SSMTester", "loading ssm lib");
			
			// for android: not complete method
			if(System.getProperty("os.name").toLowerCase().equals("linux"))
			{
				//System.out.println("System load Android library");
				System.loadLibrary("SSMCore_Android");
			}
			else
			{
				//System.out.println("System load 32bit library");
				workingPath += "/../Outputs/";
				System.load( workingPath + "SSMCore_Windows.dll");
			}
			
			Log.i("SSMTester", "loading done");
		}
		catch(UnsatisfiedLinkError e)
		{
			System.out.println(e.getMessage());
		}
	}
	
	static private CoreController coreController;
	
	public static CoreController getInstance()
	{
		if(coreController == null)
			coreController = new CoreController();
		
		return coreController;
	}
	
	public native void InitializeSSMCore(String xmlDescription);
	public native void StartSSMCore();
	public native void StopSSMCore();
	public native void TerminateSSMCore();
	public native QueryEngine CreateQueryEngine();
	public native int ReleaseQueryEngine(QueryEngine queryEngine);
	
	// QueryEngine
	public native int ExecuteContextQuery(int pQueryEngineInstance, String contextQuery);
	public native void RegisterQueryEvent(int pQueryEngineInstance, IQueryEngineEvent queryEngineEvent);
	public native void KillContextQuery(int pQueryEngineInstance, int cqid);	
		
	//public native void UnregiterQueryEvent(int pQueryEngineInstance, IQueryEngineEvent queryEngineEvent);

	//IModelData
	public native int GetDataId(int pIModelDataInstance);	
	public native int GetPropertyCount(int pIModelDataInstance);
	public native String GetPropertyName(int pIModelDataInstance, int propertyIndex);
	public native String GetPropertyValue(int pIModelDataInstance, int propertyIndex);
		
	//IDataReader
	public native List<String> GetAffectedModels(int pDataReaderInstance);
	public native int GetModelDataCount(int pDataReaderInstance, String modelName);
	public native ModelData GetModelData(int pDataReaderInstance, String modelName, int dataIndex);
	
	public native void RegisterReportReceiver(IReportReceiver reportReceiver);
}