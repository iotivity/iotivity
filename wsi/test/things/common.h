#ifndef _COMMON_H_
#define _COMMON_H_

#include <string>
#include <map>
#include <cstdlib>
#include <functional>
#include <mutex>
#include <condition_variable>
#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
using namespace std;

typedef enum {
    BULB,
    THERMOSTAT,
    WASHER,
    TV,
    AIRCON,
    DOOR,
    RVI,
} things_e;

int chosenThing = BULB;


struct sockdata{
	int state;
	int param;
};

struct thingbox
{
	things_e t;
	sockdata data;
	string name;
	string desc;
	string res;
	string intf;
	string uri;
	map<string, double> props;
};

thingbox things[] = {
		{BULB, 		{0, 0}, "bulb", "OCF Light", "core.light", "core.brightlight", "/a/light",
				{
					{"state", 0},
					{"param", 0},
					{"color", 0}
				}
		},
		{THERMOSTAT,	{0, 0}, "thermostat", "OCF Thermostat", "core.thermostat", "core.thermostat", "/a/thermostat",
				{
					{"state", 0},
					{"param", 0},
					{"temp", 0}
				}
		},
		{WASHER,	{0, 0}, "washer", "OCF Washer", "core.washer", "core.washer", "/a/washer",
				{
					{"state", 0},
					{"param", 0},
					{"time", 0}
				}
		},
		{TV,	{0, 0}, "tv", "OCF Television", "core.tv", "core.tv", "/a/tv",
				{
					{"state", 0},
					{"param", 0},
					{"source", 0}
				}
		},
		{AIRCON,	{0, 0}, "aircon", "OCF AirCon", "core.aircon", "core.aircon", "/a/aircon",
				{
					{"state", 0},
					{"param", 0},
					{"temp", 0},
					{"fanspeed", 0}
				}
		},
		{DOOR,	{0, 0}, "door", "OCF Door", "core.door", "core.door", "/a/door",
				{
					{"state", 0},
					{"param", 0},
					{"doorbell", 0}
				}
		},
		{RVI,	{0, 0}, "rvi", "Vehicle", "core.rvi", "core.rvi", "/a/rvi",
				{
					{"leftTemperature", 0},
					{"rightTemperature", 0},
					{"leftSeatHeat", 0},
					{"rightSeatHeat", 0},
					{"fanSpeed", 0},
					{"fanDown", 0},
					{"fanRight", 0},
					{"fanUp", 0},
					{"fanAC", 0},
					{"fanAuto", 0},
					{"fanRecirc", 0},
					{"defrostMax", 0},
					{"defrostFront", 0},
					{"defrostRear", 0},
					{"lat", 0},
					{"lon", 0},
					{"bearing", 0}
				}
		}
};


#endif
