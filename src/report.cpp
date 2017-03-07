#include <map>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <assert.h>
#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <ifaddrs.h>
#include <string.h>
#include <stdint.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <curl/curl.h>
#include <json/json.h>

#include "report.h"
#include "global.h"
#include "util.h"
#include "init.h"

using std::fstream;
using std::string;
using std::cout;
using std::endl;

static void do_report()
{
    /* Perform the request, res will get the return code */
    CURLcode res;
    res = curl_easy_perform(g_curl_rp);
    /* Check for errors */
    if(res != CURLE_OK)
        ERR_SYS("curl_easy_perform(rp) failed");
}



void* report_proc(void* arg) {

    Json::FastWriter writer;
    Json::Value  rp_content;
    string str_rp;
    if(init_curl() == false) {
        ERR_SYS("init curl error");
        return (void*)0;
    }

    while(1) {

        long long now = get_current_time();
        rp_content.clear();
        map<ip_link_t, net_info_t> :: iterator it;

        pthread_mutex_lock(&net_info_lock);
        for (it = net_map.begin(); it != net_map.end(); ) {
            net_info_t &net_info = it->second;
            ip_link_t ip = it->first;

            if (now - net_info.begin_time > 200000) {
                net_map.erase(ip);
                it++;
                continue;
            }else if (now - net_info.begin_time > 40000) {
                net_info.connected = false;
                it++;
                continue;
            } 

               struct in_addr inaddr;
               inaddr.s_addr = ip.src_ip;
               printf("src ip : %s   ", inet_ntoa(inaddr));
               inaddr.s_addr = ip.dst_ip;
               printf("dst ip : %s ", inet_ntoa(inaddr));

               printf("  %lf MB/s \n", net_info.trans_rate / (1<<20));
            
            Json::Value tmp;
  //          struct in_addr inaddr;
            inaddr.s_addr = ip.src_ip;
            tmp["src_ip"] = inet_ntoa(inaddr);
            tmp["src_addr"] = get_addr(ip.src_ip).c_str();
            inaddr.s_addr = ip.dst_ip;
            tmp["dst_ip"] = inet_ntoa(inaddr);
            tmp["dst_addr"] = get_addr(ip.dst_ip).c_str();
            tmp["rate"] = net_info.trans_rate;
            rp_content.append(tmp);

            it++;
        }

        pthread_mutex_unlock(&net_info_lock);
        str_rp = writer.write(rp_content).c_str();
        puts(str_rp.c_str());
        curl_easy_setopt(g_curl_rp, CURLOPT_POSTFIELDS, str_rp.c_str());
        //do_report();
        seconds_sleep(g_report_interval);
    }

    return (void*) 0;
}
