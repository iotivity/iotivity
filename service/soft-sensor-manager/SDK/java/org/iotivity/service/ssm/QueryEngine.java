package org.iotivity.service.ssm;

public class QueryEngine
{
	private int pQueryEngineInstance;
	
	public QueryEngine(int queryEngineInstance)
	{
		pQueryEngineInstance = queryEngineInstance;
	}
	
	public int GetQueryEngineInstance()
	{
		return pQueryEngineInstance;
	}
	
	public int ExecuteContextQuery(String contextQuery)
	{
		return CoreController.getInstance().ExecuteContextQuery(pQueryEngineInstance, contextQuery);
	}
	
	public void RegisterQueryEvent(IQueryEngineEvent queryEngineEvent)
	{
		CoreController.getInstance().RegisterQueryEvent(pQueryEngineInstance, queryEngineEvent);
	}
	
	public void KillContextQuery(int cqid)
	{
		CoreController.getInstance().KillContextQuery(pQueryEngineInstance, cqid);
	}
}