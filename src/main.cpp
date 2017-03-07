#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pcap.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <pthread.h>
#include <string>
#include "global.h"
#include "capture.h"
#include "init.h"
#include "util.h"
#include "report.h"
#include "main.h"

using namespace std;

#define DEV_NUM 20

int main(int , char** argv) {

    int err;
    pthread_t capture_tid[DEV_NUM];
    pthread_t report_tid;
    int dev_cnt;
    string dev_name[DEV_NUM];

    if(nice(-20) == -1) {
        printf("call nice error : %s\n", strerror(errno));
        //return 0;
    }
    
    // signal
    struct sigaction act;
    act.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &act, 0);
    signal(SIGUSR1, sig_handler);
    signal(SIGINT, sig_handler);
    signal(SIGSEGV, sig_handler);
    signal(SIGTERM, sig_handler);

    if (!init_config()) {
        ERR_EXIT("init config error");
    }

    dev_cnt = get_devname(dev_name);
    printf("%d device can be open live\n", dev_cnt);
    if (dev_cnt <= 0) {
        ERR_EXIT("device number <= 0");
    }

    // for every device create a thread process
    for (int i = 0; i < dev_cnt; i++) {
        printf("%d : %s\n", i, dev_name[i].c_str());
        err = pthread_create(capture_tid + i, NULL, capture_proc,  (void*)(dev_name[i].c_str()));
        if (err != 0) {
            ERR_EXIT("create capture thread error");
        }
    }

    err = pthread_create(&report_tid, NULL, report_proc, NULL);
    if (err != 0) {
        ERR_EXIT("create report thread error");
    }

    for (int i = 0; i < dev_cnt; i++) {
        err = pthread_join(capture_tid[i], NULL);
        if (err != 0) {
            ERR_EXIT("join capture thread error");
        }
    }

    err = pthread_join(report_tid, NULL);
    if (err != 0) {
        ERR_EXIT("join report thread error");
    }

    return 0;
}
