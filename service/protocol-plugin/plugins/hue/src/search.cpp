#include <typeinfo>
#include "curl/curl.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include "HueBridge.h"

#include "HuePlugin.h"


int main()
{
    vector<string> bridge_list;
    bridge_list = GetBridges();
    if (bridge_list.empty())
    {
        cout << "No hue bridge found" << endl;
    }
    vector<string>::iterator it;
    cout << "the result: " << endl;
    for (it = bridge_list.begin(); it != bridge_list.end(); it++)
    {
        cout << *it << "," << endl;
        string hue_bridge_url;
        hue_bridge_url.append("http://").append(*it).append("/api/newdeveloper/lights");
        cout << "total url ---" << hue_bridge_url << endl;
    }
}
