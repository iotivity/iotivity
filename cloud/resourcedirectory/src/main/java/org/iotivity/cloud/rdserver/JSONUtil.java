package org.iotivity.cloud.rdserver;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;

public class JSONUtil {

    private static ObjectMapper mapper = new ObjectMapper();

    public ArrayList<String> parseJSON(byte[] payload, String key) {

        if (payload == null)
            return null;

        ArrayList<String> value = null;

        try {
            @SuppressWarnings("unchecked")
            Map<String, ArrayList<String>> jsonMap = mapper.readValue(payload,
                    Map.class);
            value = jsonMap.get(key);
        } catch (IOException ioe) {
            ioe.printStackTrace();
        }

        return value;
    }

    public byte[] writeJSON(HashMap<Object, Object> data) throws Exception {
        if (data == null)
            return null;

        byte[] json = null;
        try {
            json = mapper.writeValueAsBytes(data);
        } catch (JsonProcessingException e) {
            e.printStackTrace();
        }

        if (json == null)
            json = "{}".getBytes(StandardCharsets.UTF_8);

        return json;
    }
}
