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
    HVAC,
    CARLOCATION
} things_e;

int chosenThing = BULB;

struct thingbox
{
	things_e t;
	int state;
	string name;
	string desc;
	string res;
	string intf;
	string uri;
	map<string, double> props;
};

thingbox things[] = {
		{BULB, 		0, "bulb", "OCF Light", "core.light", "core.brightlight", "/a/light",
				{
					{"power", 35},
					{"state", 0}
				}
		},
		{THERMOSTAT,	0, "thermostat", "OCF Thermostat", "core.thermostat", "core.thermostat", "/a/thermostat",
				{
					{"temp", 36},
					{"state", 0}
				}
		},
		{HVAC, 			0, "hvac", "Vehicle HVAC", "core.rvihvac", "core.rvihvac", "/rvi/hvac",
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
					{"defrostRear", 0}
				}
		},
		{CARLOCATION,	0, "carloc", "Vehicle Location", "core.rvilocation", "core.rvilocation", "/rvi/location",
				{
					{"lat", 0},
					{"lon", 0},
					{"bearing", 0}
				}
		}
};


#endif
