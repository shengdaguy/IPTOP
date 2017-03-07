#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <linux/types.h>
#include<map>
#include <vector>
#include <string>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <linux/types.h>
#include <ifaddrs.h>
#include "json/json.h"
#include <curl/curl.h>

using namespace std;

#define SIZE_1K (1 << 10)
#define SIZE_10K (10 << 10)
#define SIZE_64K (64 << 10)
#define SIZE_65K (65 << 10)

#pragma pack(1)

typedef struct {

    //the time of the Iplink(ip <--> ip) begin count time
    long long begin_time;
    //the time of the Iplink(ip <--> ip)last active time
    long long last_time;
    //the ip link is connected
    bool connected;
    //the transfe byte from the last time to now
    long long trans_total;
    //the transfe rate in ip <--> ip
    double trans_rate;

}net_info_t;

typedef struct ip_link{
    uint32_t src_ip;
    uint32_t dst_ip;

    bool operator < (const ip_link &B) const {
        if (src_ip < B.src_ip) return true;
        if (src_ip == B.src_ip && dst_ip < B.dst_ip) return true;

        return false;
    }

}ip_link_t;

#pragma pack()

extern map<ip_link_t, net_info_t> net_map;
extern pthread_mutex_t net_info_lock;

/**/
extern string generic_filter_ruler;
extern vector<string> local_ips;
extern vector<uint32_t> u_local_ips;

extern string g_local_addr;

extern Json::Value g_conf;
extern Json::Value g_lbase;

extern struct curl_slist *g_list;
extern CURL *g_curl_rp;

extern uint32_t g_report_interval;
extern string g_report_url;

void get_local_ip();

#endif
