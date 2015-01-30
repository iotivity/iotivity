package org.iotivity.service.ssm;

public abstract class IReportReceiver {
	public abstract void OnMessageReceived(String tag, String msg);
}
