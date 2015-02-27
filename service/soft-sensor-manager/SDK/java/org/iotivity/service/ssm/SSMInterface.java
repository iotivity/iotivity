package org.iotivity.service.ssm;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class SSMInterface {
	private class QueryEngineEventReceiver extends IQueryEngineEvent {
		private Lock mMtxListener = new ReentrantLock();
		private Map<Integer, IQueryEngineEvent> mMapListener = new HashMap<Integer, IQueryEngineEvent>();

		public void OnQueryEngineEvent(int cqid, DataReader result) {
			mMtxListener.lock();

			mMapListener.get(cqid).OnQueryEngineEvent(cqid, result);

			mMtxListener.unlock();
		}

		void lockListener() {
			mMtxListener.lock();
		}

		void unlockListener() {
			mMtxListener.unlock();
		}

		void addListener(int cqid, IQueryEngineEvent engineEvent) {
			mMapListener.put(cqid, engineEvent);
		}

		void removeListener(int cqid) {
			mMapListener.remove(cqid);
		}
	};

	private CoreController mSSMCore = null;
	private QueryEngine mQueryEngine = null;
	private QueryEngineEventReceiver mQueryEngineEventListenerReceiver = new QueryEngineEventReceiver();
	private List<Integer> mRunningCQLs = new ArrayList<Integer>();

	public SSMInterface() {
	}

	public void startSSMCore(String initConfig) throws Exception {
		mSSMCore = CoreController.getInstance();
		mSSMCore.InitializeSSMCore(initConfig);
		mSSMCore.StartSSMCore();

		mQueryEngine = mSSMCore.CreateQueryEngine();

		mQueryEngine.RegisterQueryEvent(mQueryEngineEventListenerReceiver);
	}

	public void stopSSMCore() throws Exception {
		mQueryEngine.RegisterQueryEvent(null);
		mSSMCore.ReleaseQueryEngine(mQueryEngine);
		mQueryEngineEventListenerReceiver = null;
		mQueryEngine = null;
		mSSMCore.StopSSMCore();
		mSSMCore.TerminateSSMCore();
	}

	public int registerQuery(String contextQuery, IQueryEngineEvent listener)
			throws Exception {
		int cqid = 0;

		try {
			mQueryEngineEventListenerReceiver.lockListener();
			cqid = mQueryEngine.ExecuteContextQuery(contextQuery);
			mQueryEngineEventListenerReceiver.addListener(cqid, listener);
			mRunningCQLs.add(cqid);
		} catch (Exception e) {
			throw e;
		} finally {
			mQueryEngineEventListenerReceiver.unlockListener();
		}

		return cqid;
	}

	public void unregisterQuery(int cqid) throws Exception {
		try {
			mQueryEngineEventListenerReceiver.lockListener();
			mQueryEngine.KillContextQuery(cqid);
			mQueryEngineEventListenerReceiver.removeListener(cqid);
			mRunningCQLs.remove((Object) cqid);
		} catch (Exception e) {
			throw e;
		} finally {
			mQueryEngineEventListenerReceiver.unlockListener();
		}
	}
}