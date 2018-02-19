/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

package org.iotivity.test.rfw.common.devicecontroller.interfaces;

import java.io.IOException;

import org.iotivity.test.rfw.common.devicecontroller.datamodel.Coordinate;
import org.iotivity.test.rfw.common.devicecontroller.datamodel.KeyEventType;

public interface IScreenController {
    boolean touch(Coordinate point) throws IOException;

    boolean touchDown(Coordinate point) throws IOException;

    boolean touchUp(Coordinate point) throws IOException;

    boolean touchMove(Coordinate point) throws IOException;

    boolean pressKey(KeyEventType key) throws IOException;

    boolean sendString(String text) throws IOException;

    boolean tap(Coordinate point) throws IOException;

    boolean drag(Coordinate from, Coordinate to)
            throws IOException, InterruptedException;

    // void openControlSocket(Socket socket) throws IOException;

    void closeControlSocket() throws IOException;
}
