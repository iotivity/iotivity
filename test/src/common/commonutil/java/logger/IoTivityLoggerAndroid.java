/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

package oic.iotivity.logger;

import java.io.File;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.apache.log4j.Level;
import android.os.Environment;
import de.mindpipe.android.logging.log4j.LogConfigurator;

public class IoTivityLoggerAndroid {
	static LogConfigurator logConfigurator = new LogConfigurator();
	
	public static Logger getInstance(){
		final Logger logger = LoggerFactory.getLogger("IoTivityLogger");
		configureLog();
		return logger;
	}
	
    public static void configureLog()
    {    	
    	logConfigurator.setRootLevel(Level.DEBUG);

    	logConfigurator.setUseFileAppender(true);
    	logConfigurator.setFileName(Environment.getExternalStorageDirectory() + File.separator + "oictest.log");
    	logConfigurator.setFilePattern("[%c] [%d{dd MMM yyyy} %d{HH:mm:ss.SSS}] %l [%-5p] - %m%n");
    	
    	logConfigurator.setUseLogCatAppender(true);
    	logConfigurator.setLogCatPattern("[%c] [%d{dd MMM yyyy} %d{HH:mm:ss.SSS}] %l [%-5p] - %m%n");
    	
    	logConfigurator.configure();
    }

}
