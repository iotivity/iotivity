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
package oic.ctt.formatter;

import java.util.ArrayList;
import java.util.Arrays;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.*;



public class JsonData {
    private String     json;
    private JSONParser parser;
    public JSONObject  mJsonObject;

    public JsonData() {
        mJsonObject = new JSONObject();
        parser = new JSONParser();
    }

    public JsonData(String jsonString) throws ParseException {
        json = jsonString;
        parser = new JSONParser();
    }

    public void put(Object key, Object value) {
        mJsonObject.put(key, value);
    }

    @Override
    public String toString() {
        return mJsonObject.toJSONString();
    }

    public ArrayList<Object> toArray() throws ParseException {
        JSONArray array = (JSONArray) parser.parse(json);
        return new ArrayList<Object>(Arrays.asList(array.toArray()));
    }
}
