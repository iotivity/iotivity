#ifndef __TCT_LIBOIC_CORE_H__
#define __TCT_LIBOIC_CORE_H__

#include "testcase.h"

extern void utc_liboic_startup(void);
extern void utc_liboic_cleanup(void);

extern int wifi_tc_001_initializeWifi_p(void);
extern int wifi_tc_002_initializeWifi_n(void);
extern int wifi_tc_003_getWIFIInterfaceInformation_p(void);
extern int wifi_tc_004_getWIFIInterfaceInformation_n(void);
extern int wifi_tc_005_sendWifiUnicastData_p(void);
extern int wifi_tc_006_sendWifiUnicastData_n(void);
extern int wifi_tc_007_sendWifiMulticastData_p(void);
extern int wifi_tc_008_sendWifiMulticastData_n(void);
extern int wifi_tc_009_startWifiServers_p(void);
extern int wifi_tc_010_startWifiServers_n(void);
extern int wifi_tc_011_stopWifiServers_p(void);

testcase tc_array[] = {
    {"wifi_tc_001_initializeWifi_p", wifi_tc_001_initializeWifi_p, utc_liboic_startup, utc_liboic_cleanup},
    {"wifi_tc_002_initializeWifi_n", wifi_tc_002_initializeWifi_n, utc_liboic_startup, utc_liboic_cleanup},
    {"wifi_tc_003_getWIFIInterfaceInformation_p", wifi_tc_003_getWIFIInterfaceInformation_p, utc_liboic_startup, utc_liboic_cleanup},
    {"wifi_tc_004_getWIFIInterfaceInformation_n", wifi_tc_004_getWIFIInterfaceInformation_n, utc_liboic_startup, utc_liboic_cleanup},
    {"wifi_tc_005_sendWifiUnicastData_p", wifi_tc_005_sendWifiUnicastData_p, utc_liboic_startup, utc_liboic_cleanup},
    {"wifi_tc_006_sendWifiUnicastData_n", wifi_tc_006_sendWifiUnicastData_n, utc_liboic_startup, utc_liboic_cleanup},
    {"wifi_tc_007_sendWifiMulticastData_p", wifi_tc_007_sendWifiMulticastData_p, utc_liboic_startup, utc_liboic_cleanup},
    {"wifi_tc_008_sendWifiMulticastData_n", wifi_tc_008_sendWifiMulticastData_n, utc_liboic_startup, utc_liboic_cleanup},
    {"wifi_tc_009_startWifiServers_p", wifi_tc_009_startWifiServers_p, utc_liboic_startup, utc_liboic_cleanup},
    {"wifi_tc_010_startWifiServers_n", wifi_tc_010_startWifiServers_n, utc_liboic_startup, utc_liboic_cleanup},
    {"wifi_tc_011_stopWifiServers_p", wifi_tc_011_stopWifiServers_p, utc_liboic_startup, utc_liboic_cleanup},
    {NULL, NULL}
};

#endif // __TCT_LIBOIC_CORE_H__
