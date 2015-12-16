/*
   Copyright 2011 Rolf Kulemann, Pascal Bockhorn

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

package oic.ctt.logger;

import org.apache.log4j.ConsoleAppender;
import org.apache.log4j.Layout;
import org.apache.log4j.Level;
import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;
import org.apache.log4j.PatternLayout;
import org.apache.log4j.RollingFileAppender;
import org.apache.log4j.helpers.LogLog;

import java.io.FileReader;
import java.io.IOException;
import java.util.Properties;

public class LogConfiguratorJava {
    private Level   mRootLevel          = Level.DEBUG;
    private String  mFilePattern        = "%d - [%p::%c::%C] - %m%n";
    private String  mConsolePattern     = "%m%n";
    private String  mFileName           = "oictest.log";
    private int     mMaxBackupSize      = 5;
    private long    mMaxFileSize        = 512 * 1024;
    private boolean mImmediateFlush     = true;
    private boolean mUseConsoleAppender = true;
    private boolean mUseFileAppender    = true;
    private boolean mResetConfiguration = true;
    private boolean mInternalDebugging  = false;

    public LogConfiguratorJava() {
    }

    public LogConfiguratorJava(final String fileName) {
        setFileName(fileName);
    }

    public LogConfiguratorJava(final String fileName, final Level rootLevel) {
        this(fileName);
        setRootLevel(rootLevel);
    }

    public LogConfiguratorJava(final String fileName, final Level rootLevel,
            final String filePattern) {
        this(fileName);
        setRootLevel(rootLevel);
        setFilePattern(filePattern);
    }

    public LogConfiguratorJava(final String fileName, final int maxBackupSize,
            final long maxFileSize, final String filePattern,
            final Level rootLevel) {
        this(fileName, rootLevel, filePattern);
        setMaxBackupSize(maxBackupSize);
        setMaxFileSize(maxFileSize);
    }

    public void readConfigurationFromFile(String fileName) throws IOException {

        try {

            FileReader fileReader = new FileReader(fileName);

            Properties properties = new Properties();
            properties.load(fileReader);

            String logLevelSetting = properties.getProperty("logLevel").trim();
            String fileOutputSetting = properties.getProperty("fileOutput")
                    .trim();
            String fileNameSetting = properties.getProperty("fileName").trim();
            String fileLogMsgFormatSetting = properties
                    .getProperty("fileMsgPattern");
            String consoleOutputSetting = properties
                    .getProperty("consoleOutput").trim();
            String consoleLogMsgFormatSetting = properties
                    .getProperty("consoleMsgPattern");

            Level logLevel = null;
            switch (logLevelSetting.toUpperCase()) {
                case "TRACE":
                    logLevel = Level.TRACE;
                    break;

                case "INFO":
                    logLevel = Level.INFO;
                    break;

                case "DEBUG":
                    logLevel = Level.DEBUG;
                    break;

                case "WARN":
                    logLevel = Level.WARN;
                    break;

                case "ERROR":
                    logLevel = Level.ERROR;
                    break;

                default:
                    logLevel = Level.INFO;
            }
            setRootLevel(logLevel);

            if (fileOutputSetting.toLowerCase().equals("true"))
                setUseFileAppender(true);
            else
                setUseFileAppender(false);

            if (fileNameSetting.length() > 0)
                setFileName(fileNameSetting);
            else
                setFileName("ConformanceTool.log");

            if (fileLogMsgFormatSetting.length() > 0)
                setFilePattern(fileLogMsgFormatSetting);
            else
                setFilePattern(
                        "[%c] [%d{dd MMM yyyy} %d{HH:mm:ss.SSS}] %l [%-5p] - %m%n");

            if (consoleOutputSetting.toLowerCase().equals("false"))
                setUseConsoleAppender(false);
            else
                setUseConsoleAppender(true);

            if (consoleLogMsgFormatSetting.length() > 0)
                setConsolePattern(consoleLogMsgFormatSetting);
            else
                setConsolePattern(
                        "[%c] [%d{dd MMM yyyy} %d{HH:mm:ss.SSS}] %l [%-5p] - %m%n");
        } catch (IOException e) {
            e.printStackTrace();

        }
    }

    public void setDefaultConfiguration() {
        setRootLevel(Level.DEBUG);

        setUseFileAppender(false);
        setFileName("ConformanceTool.log");
        setFilePattern(
                "[%c] [%d{dd MMM yyyy} %d{HH:mm:ss.SSS}] %l [%-5p] - %m%n");

        setUseConsoleAppender(true);
        setConsolePattern(
                "[%c] [%d{dd MMM yyyy} %d{HH:mm:ss.SSS}] %l [%-5p] - %m%n");
    }

    public void configure() {
        final Logger root = Logger.getRootLogger();

        if (isResetConfiguration()) {
            LogManager.getLoggerRepository().resetConfiguration();
        }

        LogLog.setInternalDebugging(isInternalDebugging());

        if (isUseFileAppender()) {
            configureFileAppender();
        }

        if (isUseConsoleAppender()) {
            configureConsoleAppender();
        }

        root.setLevel(getRootLevel());
    }

    public void setLevel(final String loggerName, final Level level) {
        Logger logger = Logger.getLogger(loggerName);

        if (logger != null)
            logger.setLevel(level);
    }

    private void configureFileAppender() {
        final Logger root = Logger.getRootLogger();
        final RollingFileAppender rollingFileAppender;
        final Layout fileLayout = new PatternLayout(getFilePattern());

        try {
            rollingFileAppender = new RollingFileAppender(fileLayout,
                    getFileName());
        } catch (final IOException e) {
            throw new RuntimeException("Exception configuring log system", e);
        }

        rollingFileAppender.setMaxBackupIndex(getMaxBackupSize());
        rollingFileAppender.setMaximumFileSize(getMaxFileSize());
        rollingFileAppender.setImmediateFlush(isImmediateFlush());

        root.addAppender(rollingFileAppender);
    }

    private void configureConsoleAppender() {
        final Logger root = Logger.getRootLogger();
        final Layout consoleLayout = new PatternLayout(getConsolePattern());
        final ConsoleAppender consoleAppender = new ConsoleAppender(
                consoleLayout);

        root.addAppender(consoleAppender);
    }

    public Level getRootLevel() {
        return mRootLevel;
    }

    public void setRootLevel(final Level level) {
        this.mRootLevel = level;
    }

    public String getFilePattern() {
        return mFilePattern;
    }

    public void setFilePattern(final String filePattern) {
        this.mFilePattern = filePattern;
    }

    public String getConsolePattern() {
        return mConsolePattern;
    }

    public void setConsolePattern(final String consolePattern) {
        this.mConsolePattern = consolePattern;
    }

    public String getFileName() {
        return mFileName;
    }

    public void setFileName(final String fileName) {
        this.mFileName = fileName;
    }

    public int getMaxBackupSize() {
        return mMaxBackupSize;
    }

    public void setMaxBackupSize(final int maxBackupSize) {
        this.mMaxBackupSize = maxBackupSize;
    }

    public long getMaxFileSize() {
        return mMaxFileSize;
    }

    public void setMaxFileSize(final long maxFileSize) {
        this.mMaxFileSize = maxFileSize;
    }

    public boolean isImmediateFlush() {
        return mImmediateFlush;
    }

    public void setImmediateFlush(final boolean immediateFlush) {
        this.mImmediateFlush = immediateFlush;
    }

    public boolean isUseFileAppender() {
        return mUseFileAppender;
    }

    public void setUseFileAppender(final boolean useFileAppender) {
        this.mUseFileAppender = useFileAppender;
    }

    public boolean isUseConsoleAppender() {
        return mUseConsoleAppender;
    }

    public void setUseConsoleAppender(final boolean useConsoleAppender) {
        this.mUseConsoleAppender = useConsoleAppender;
    }

    public void setResetConfiguration(boolean resetConfiguration) {
        this.mResetConfiguration = resetConfiguration;
    }

    public boolean isResetConfiguration() {
        return mResetConfiguration;
    }

    public void setInternalDebugging(boolean internalDebugging) {
        this.mInternalDebugging = internalDebugging;
    }

    public boolean isInternalDebugging() {
        return mInternalDebugging;
    }
}