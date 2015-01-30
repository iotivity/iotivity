package org.iotivity.service.ssm;

public abstract class IQueryEngineEvent {
	public abstract void OnQueryEngineEvent(int cqid, DataReader result);
}