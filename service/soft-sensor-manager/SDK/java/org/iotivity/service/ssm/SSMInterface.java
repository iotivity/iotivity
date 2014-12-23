package org.iotivity.service.ssm;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class SSMInterface {
	private class QueryEngineEventReceiver implements IQueryEngineEvent
	{
        private Lock								mMtxListener = new ReentrantLock();
		private Map<Integer, IQueryEngineEvent>   	mMapListener = new HashMap<Integer, IQueryEngineEvent>();
		
		public void OnQueryEngineEvent(int cqid, DataReader result)
		{
			mMtxListener.lock();

			mMapListener.get(cqid).OnQueryEngineEvent(cqid, result);
            
			mMtxListener.unlock();
		}
		void lockListener()
        {
			mMtxListener.lock();
        }

        void unlockListener()
        {
        	mMtxListener.unlock();
        }

        void addListener(int cqid, IQueryEngineEvent engineEvent)
        {
        	mMapListener.put(cqid, engineEvent);
        }

        void removeListener(int cqid)
        {
        	mMapListener.remove(cqid);
        }
	};
	
	private	CoreController				mSSMCore = null;
	private QueryEngine					mQueryEngine = null;
	private QueryEngineEventReceiver 	mQueryEngineEventListenerReceiver = new QueryEngineEventReceiver();
	private List<Integer>				mRunningCQLs = new ArrayList<Integer>();
	
	public SSMInterface()
	{		
	}
	
	public void startSSMCore(String initConfig) throws Exception
	{
		mSSMCore = CoreController.getInstance();
        mSSMCore.InitializeSSMCore(initConfig);
        mSSMCore.StartSSMCore();
        
        mQueryEngine = mSSMCore.CreateQueryEngine();
        
        if(mQueryEngine == null)
        	throw new Exception("Create Query Engine failed");
        
        mQueryEngine.RegisterQueryEvent(mQueryEngineEventListenerReceiver);
	}
	
	public void stopSSMCore()
	{
		mQueryEngine.RegisterQueryEvent(null);
		mSSMCore.ReleaseQueryEngine(mQueryEngine);
		mQueryEngineEventListenerReceiver = null;
		mQueryEngine = null;
		mSSMCore.StopSSMCore();
		mSSMCore.TerminateSSMCore();
	}
	
	public int registerQuery(String contextQuery, IQueryEngineEvent listener)
	{
		int cqid;
		
		mQueryEngineEventListenerReceiver.lockListener();
		cqid = mQueryEngine.ExecuteContextQuery(contextQuery);
		mQueryEngineEventListenerReceiver.addListener(cqid, listener);
		mRunningCQLs.add(cqid);
		mQueryEngineEventListenerReceiver.unlockListener();
		
		return cqid;
	}
	
	public void unregisterQuery(int cqid)
	{
		mQueryEngineEventListenerReceiver.lockListener();
		mQueryEngine.KillContextQuery(cqid);
		mQueryEngineEventListenerReceiver.removeListener(cqid);
		mRunningCQLs.remove(cqid);
		mQueryEngineEventListenerReceiver.unlockListener();
	}
}