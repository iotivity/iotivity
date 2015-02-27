package org.iotivity.service.ssm;

import java.util.List;

public class CoreController {
	static {
		try {
			String workingPath = System.getProperty("user.dir");

			// for android: not complete method
			if (System.getProperty("os.name").toLowerCase().equals("linux")) {
				// System.out.println("System load Android library");
				System.loadLibrary("oc_logger");
				System.loadLibrary("coap");
				System.loadLibrary("octbstack");
				System.loadLibrary("oc");
				System.loadLibrary("SSMCore");
			} else {
				// System.out.println("System load 32bit library");
				System.load(workingPath + "SSMCore_Windows.dll");
			}
		} catch (UnsatisfiedLinkError e) {
			System.out.println(e.getMessage());
		}
	}

	static private CoreController coreController;

	public static CoreController getInstance() {
		if (coreController == null)
			coreController = new CoreController();

		return coreController;
	}

	public native void InitializeSSMCore(String xmlDescription)
			throws Exception;

	public native void StartSSMCore() throws Exception;

	public native void StopSSMCore() throws Exception;

	public native void TerminateSSMCore() throws Exception;

	public native QueryEngine CreateQueryEngine() throws Exception;

	public native int ReleaseQueryEngine(QueryEngine queryEngine);

	// QueryEngine
	public native int ExecuteContextQuery(int pQueryEngineInstance,
			String contextQuery) throws Exception;

	public native void RegisterQueryEvent(int pQueryEngineInstance,
			IQueryEngineEvent queryEngineEvent);

	public native void KillContextQuery(int pQueryEngineInstance, int cqid)
			throws Exception;

	// public native void UnregiterQueryEvent(int pQueryEngineInstance,
	// IQueryEngineEvent queryEngineEvent);

	// IModelData
	public native int GetDataId(int pIModelDataInstance);

	public native int GetPropertyCount(int pIModelDataInstance);

	public native String GetPropertyName(int pIModelDataInstance,
			int propertyIndex);

	public native String GetPropertyValue(int pIModelDataInstance,
			int propertyIndex);

	// IDataReader
	public native List<String> GetAffectedModels(int pDataReaderInstance);

	public native int GetModelDataCount(int pDataReaderInstance,
			String modelName) throws Exception;

	public native ModelData GetModelData(int pDataReaderInstance,
			String modelName, int dataIndex) throws Exception;

	public native void RegisterReportReceiver(IReportReceiver reportReceiver);
}