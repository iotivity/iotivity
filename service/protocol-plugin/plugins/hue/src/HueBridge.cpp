#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "HueBridge.h"
using namespace std;

HueBridgeConnection::HueBridgeConnection()
{
    //TODO
}

HueBridgeConnection::~HueBridgeConnection()
{
    //TODO
}

string HueBridgeConnection::UpnpDiscovery()
{
    struct sockaddr_in   addrin, localaddr ;
    struct timeval       rtime      ;
    int                  newsock    ;
    int                  ret        ;
    char                 buf[1024]  ;
    int                  i = 0        ;
    int              yes = 1      ;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    rtime.tv_sec  = 2 ;
    rtime.tv_usec = 0 ;

    string host = "HOST: 239.255.255.250:1900\r\n";
    string st = "ST: upnp:rootdevice\r\n";
//    string st = "ST: ssdp:all\r\n";
    string mx = "MX: 3\r\n";
    string man = "MAN: ssdp:discover\r\n";
    string ssdp_msg = "M-SEARCH * HTTP/1.1\r\n";

    ssdp_msg.append(host).append(man).append(mx).append(st);

    bzero(&addrin, sizeof(addrin));
    addrin.sin_family = AF_INET;
    addrin.sin_addr.s_addr = inet_addr("239.255.255.250"); //htonl(INADDR_ANY)
//    addrin.sin_addr.s_addr = htonl(INADDR_ANY);
    addrin.sin_port = htons(1900);

    bzero(&localaddr, sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localaddr.sin_port = htons(1901);

    newsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if ( newsock < 0) {return "";}

    ret = setsockopt( newsock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&rtime, sizeof(struct timeval));
    if (ret == -1)
    {
        close(newsock);
        return "";
    }
    ret = setsockopt( newsock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ;
    if (ret == -1)
    {
        close(newsock);
        return "";
    }

    bind(newsock, (struct sockaddr *)&localaddr, addr_len);

    string hue_bridge;
    while (i < 10)
    {
        i++;
        yes = sizeof(struct sockaddr_in);
        memset(buf, 0, sizeof(buf));
        ssize_t ret_num;
        ret_num = sendto(newsock, ssdp_msg.c_str(), ssdp_msg.size(), 0, (struct sockaddr *)&addrin,
                         addr_len);
        if (ret_num == -1)
        {
            close(newsock);
            return "";
        }
        ret = recvfrom( newsock, buf, sizeof(buf), 0, (struct sockaddr *)&addrin, &addr_len);
        if ( ret < 0 ) {continue;}
        else
        {
            hue_bridge = inet_ntoa(addrin.sin_addr);
            break;
        }
    }
    close(newsock);
    return hue_bridge;
}



vector<string> GetBridges()
{
    vector<string> bridge_list;
    int i = 0;
    while (i < 5)
    {
        HueBridgeConnection *p = new HueBridgeConnection();
        string hue_bridge = p->UpnpDiscovery();
        vector<string>::iterator findit = find(bridge_list.begin(), bridge_list.end(), hue_bridge);
        if (findit == bridge_list.end())
            bridge_list.push_back(hue_bridge);
        delete p;
        i++;
    }
    return bridge_list;
}

