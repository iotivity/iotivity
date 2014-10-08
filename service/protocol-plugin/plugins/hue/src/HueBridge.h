#ifndef __HUE_BRIDGE_H__
#define __HUE_BRIDGE_H__
#include<string>
#include<vector>
using namespace std;

class HueBridgeConnection
{
    public:
        HueBridgeConnection();
        virtual ~HueBridgeConnection();
        string UpnpDiscovery();
};

extern "C" {
    vector<string> GetBridges();
}
#endif //__HUE_BRIDGE_H__
