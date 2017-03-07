#include <global.h>


map<ip_link_t, net_info_t> net_map;
pthread_mutex_t net_info_lock;

string generic_filter_ruler;
vector<string> local_ips;
vector<uint32_t> u_local_ips;

string g_local_addr;

Json::Value g_conf;
Json::Value g_lbase;

struct curl_slist *g_list;
CURL *g_curl_rp;

uint32_t g_report_interval;
string g_report_url;

void get_local_ip() {

    struct in_addr addr;
    struct ifaddrs *ifap = NULL;
    struct ifaddrs *ifa = NULL;
    __be32 ip127_0_0_1 = inet_addr("127.0.0.1");

    int err = getifaddrs(&ifap);
    if (err) {
        fprintf(stderr, "get eth* ipv4 failed");
        exit(100);
    }
    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_INET) continue;
        addr = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
        if (addr.s_addr != ip127_0_0_1) {
            local_ips.push_back(inet_ntoa(addr));
            u_local_ips.push_back(addr.s_addr);
            printf("%s\n", inet_ntoa(addr));
        }
    }
    freeifaddrs(ifap);
}
