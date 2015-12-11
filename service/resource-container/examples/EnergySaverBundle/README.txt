== EnergySaver: An example of softsensor implementation ==

=== Description ===

EnergySaver is a softsensor that saves energy consumption in a room. The main goal of EnergySaver is to find a room with lights on but no one inside. To achieve this, EnergySaver combins the outputs from two physical sensors as follows:
  1) Lightness level: this level is used to check whether the lights are turned on or off.
  2) # of people in the room: this value is used to check if the room is empty or occupied.

The status of the room is determined by the following rule:
  * WASTING_ENERGY: (lightness level) > 80 and (the room is empty)
  * ATTENTION_REQUIRED: (lightness level) > 50 and (the room is empty)
  * NORMAL: otherwise (i.e. (lightness level) <= 50 or (the room is occupied))

=== Input Sensors ===

1) LightSensor 
  * goal: check the lightness level of the room
  * output: light
  * type: double 
  * range: 0 (no light) - 100 (maximum lightness level)

2) EntranceCheckSensor
  * goal: count the number of people inside the room
  * output: peopleCount
  * type: int
