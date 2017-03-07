#ifndef __CAPTURE_H_
#define __CAPTURE_H_


#include <global.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pcap.h>
#include <time.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/if_ether.h>
#include<netinet/ip.h>
#include<netinet/udp.h>
#include<netinet/tcp.h>
#include<netinet/ip_icmp.h>


using namespace std;


void getPacket(u_char *arg, const struct pcap_pkthdr *pkthdr, const u_char *packet);
void* capture_proc(void* arg);


#endif
