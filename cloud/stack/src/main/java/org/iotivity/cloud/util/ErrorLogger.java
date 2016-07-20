package org.iotivity.cloud.util;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.text.SimpleDateFormat;
import java.util.Calendar;

import io.netty.channel.Channel;

public class ErrorLogger extends Logger {

    private static FileOutputStream fos = null;
    private static PrintStream      ps  = null;

    static public void Init() throws FileNotFoundException {
        File dir = new File("..//errLog//");
        if (!dir.isDirectory()) {
            System.out.println("Is Not Folder");
            dir.mkdirs();
        }
        fos = new FileOutputStream("../errLog/[" + getDate() + "] error.log",
                true);
        ps = new PrintStream(fos);
    }

    static public void write(Channel channel, Throwable t) {
        String log = channel.id().asLongText().substring(26) + " "
                + t.getMessage();
        ps.println(getTime() + " " + log);
        t.printStackTrace(ps);
        Logger.v(log);
    }

    protected static String getDate() {
        Calendar calendar = Calendar.getInstance();
        SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd");
        return dateFormat.format(calendar.getTime());
    }
}
