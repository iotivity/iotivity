package org.iotivity.service.ssm;

public class ModelData {
	
	private int pIModelDataInstance;
	
	public ModelData(int iModelDataInstance)
	{
		pIModelDataInstance = iModelDataInstance;
	}
	
	public int GetDataId()
	{
		return CoreController.getInstance().GetDataId(pIModelDataInstance);
	}
	
	public int GetPropertyCount()
	{
		return CoreController.getInstance().GetPropertyCount(pIModelDataInstance);
	}
	
	public String GetPropertyName(int propertyIndex)
	{
		return CoreController.getInstance().GetPropertyName(pIModelDataInstance, propertyIndex);
	}
	
	public String GetPropertyValue(int propertyIndex)
	{
		return CoreController.getInstance().GetPropertyValue(pIModelDataInstance, propertyIndex);
	}
}