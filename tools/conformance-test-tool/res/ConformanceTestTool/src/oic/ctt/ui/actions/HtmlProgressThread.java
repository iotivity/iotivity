//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 
package oic.ctt.ui.actions;

import java.lang.reflect.InvocationTargetException;

import oic.ctt.ui.util.CTLogger;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.slf4j.Logger;

public class HtmlProgressThread implements IRunnableWithProgress {

    private static final String TASK_NAME = "Test Reporting...";
    private Logger              logger    = CTLogger.getInstance();

    @Override
    public void run(IProgressMonitor monitor) throws InvocationTargetException,
            InterruptedException {
        monitor.beginTask(TASK_NAME, IProgressMonitor.UNKNOWN);
        logger.info("reporting dialog opened");

        while (!monitor.isCanceled()) {
            Thread.sleep(100);
        }
        monitor.done();

    }

}
