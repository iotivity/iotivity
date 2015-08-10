package oic.simulator.logger;

import java.util.Calendar;
import java.util.Date;

import oic.simulator.serviceprovider.Activator;
import oic.simulator.serviceprovider.utils.Constants;

import org.oic.simulator.ILogger;

public class LoggerCallback implements ILogger {

    @Override
    public void write(String time, int level, String message) {
        if (null == time || level < 0 || null == message) {
            return;
        }
        // Parse the time
        Date date = parseTime(time);
        if (null == date) {
            return;
        }
        Activator activator = Activator.getDefault();
        if (null == activator) {
            return;
        }
        activator.getLogManager().log(level, date, message);
    }

    private Date parseTime(String time) {
        Date date;
        String[] token = time.split("\\.");
        int h, m, s;
        try {
            if (token.length == Constants.PROPER_LOG_TIME_TOKEN_LENGTH) {
                h = Integer.parseInt(token[0]);
                m = Integer.parseInt(token[1]);
                s = Integer.parseInt(token[2]);

                Calendar calendar;
                calendar = Calendar.getInstance();
                calendar.set(Calendar.HOUR, h);
                calendar.set(Calendar.MINUTE, m);
                calendar.set(Calendar.SECOND, s);

                date = calendar.getTime();
            } else {
                date = null;
            }
        } catch (NumberFormatException nfe) {
            date = null;
        }
        return date;
    }
}