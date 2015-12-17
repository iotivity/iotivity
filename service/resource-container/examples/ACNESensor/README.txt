== Advanced Computer Network - IoTivity Project ==

Acne Sensor

Proposed by team4
Hyunwoo Lee
Duckju Kim

=== Description ===

This will display simple status of the user's skin (especially face) and advice of makeup to maintain your skin health.  
This Sensor has two parts: Local sensor & Global (Outside) sensor.

Global (Outside) part
This leverages two sensors, outside temperature and humidity.
It will get and store temperature and humidity every hour.
When a client (local part) requests with GET message, it will reply with the resent data.

Local part
This leverages three local sensors, skin temperature, moisture, and oil.
It will get values from these sensors when the user triggers.
Also, it get two values, outside temperature and humidity from the global part.
Using those five values, it will analyze the skin of the user and make some advice of makeup.

=== Working Parts ===

Hyunwoo Lee: (skin) TempSensor, MoistSensor, OilSensor

Duckju Kim: GlobalSensor, Environment(SConscript)

Both: ACNESensor and its logic
