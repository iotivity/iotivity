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
    RVI,
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
		{RVI, 			0, "rvi", "Vehicle", "core.rvi", "core.rvi", "/a/rvi",
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
