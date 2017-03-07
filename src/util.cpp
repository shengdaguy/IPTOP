#include "util.h"
#include <errno.h>
#include "global.h"

void process_cleanup(void) {

    //cleanup the curl
    curl_slist_free_all(g_list);
    curl_easy_cleanup(g_curl_rp);
    curl_global_cleanup();

    printf("program exit.\n");
}

void sig_handler(int signo) {
    if (signo == SIGUSR1) {
        process_cleanup();
        exit(0x11);
    } else if (signo == SIGINT) {
        process_cleanup();
        exit(0x12);
    } else if (signo == SIGSEGV) {
        process_cleanup();
        exit(0x13);
    } else if (signo == SIGTERM) {
        process_cleanup();
        exit(0x14);
    }
}


int get_devname(string dev_name[]) {

    struct ifreq ifr;
    struct ifconf ifc;
    char buf[MAX_DEV_BUF];

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1) {
        printf("socket error\n");
        return -1;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
        printf("ioctl error\n");
        return -1;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
        printf("ioctl error\n");
        return -1;
    }

    struct ifreq* it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));
    char szMac[64];
    int count = 0;
    for (; it != end; ++it) {
        strcpy(ifr.ifr_name, it->ifr_name);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
            if (! (ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
                if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
                    unsigned char * ptr ;
                    ptr = (unsigned char  *)&ifr.ifr_ifru.ifru_hwaddr.sa_data[0];
                    snprintf(szMac,64,"%02X:%02X:%02X:%02X:%02X:%02X",*ptr,*(ptr+1),*(ptr+2),*(ptr+3),*(ptr+4),*(ptr+5));
                    printf("%d,Interface name : %s , Mac address : %s \n",count,ifr.ifr_name,szMac);
                    dev_name[count] = ifr.ifr_name;
                    count ++ ;
                }
            }
        }else{
            printf("get mac info error\n");
            return -1;
        }
    }
    return count;
}


long long get_current_time()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	long long ms = tv.tv_sec;
	return ms * 1000 + tv.tv_usec / 1000;
}

void seconds_sleep(unsigned seconds){
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    int err;
    do{
        err = select(0, NULL, NULL, NULL, &tv);
    }while(err<0 && errno == EINTR);
}

string get_addr(uint32_t ip) {

    int size = u_local_ips.size();
    
    for (int i = 0; i < size; i++) {
        if (ip == u_local_ips[i]) return g_local_addr;
    }

    return "";
}
