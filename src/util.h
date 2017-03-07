#ifndef __UTIL_H
#define __UTIL_H

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
#include <signal.h>

using namespace std;

#define MAX_DEV_BUF 2048
#define MAX_IP_NUM 100

#define ERR_SYS(str) \
    do { \
        perror(str); \
    } while(0)

#define ERR_EXIT(str) \
    do { \
        perror(str); \
        exit(EXIT_FAILURE); \
    } while(0)

void process_cleanup(void);

void sig_handler(int signo);

int get_devname(string dev_name[]);

long long get_current_time();

void seconds_sleep(unsigned seconds);

string get_addr(uint32_t ip);

#endif
