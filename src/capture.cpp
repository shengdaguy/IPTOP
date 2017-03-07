#include <capture.h>
#include "util.h"

#define MAX_PACKET_SIZE SIZE_65K

/*
 * 监控所有网卡 拦截所有的IP数据包
 * 维护global数据
 */
void* capture_proc(void* arg) {
    int err;
    char err_buf[PCAP_ERRBUF_SIZE];

    pcap_t *device;
    char dev_name[1024];
    bpf_u_int32 netaddr, mask;
    bpf_program filter;

    strncpy(dev_name, (char*)arg, sizeof(dev_name));

    if ((device = pcap_open_live(dev_name, MAX_PACKET_SIZE, 1, 0, err_buf)) == NULL) {
        puts("pcap_open_live fail");
        puts(err_buf);
        return (void*)0;
    }


    /*对每一个device lookupnet && pcap_compile && setfiter*/

    err = pcap_lookupnet(dev_name, &netaddr, &mask, err_buf);
    if (err < 0) {
        printf("%s error : ", dev_name);
        puts(err_buf);    
        return (void*)0;
    }
/*
    struct in_addr inaddr;

    inaddr.s_addr = netaddr;
    printf("ip : %s\n", inet_ntoa(inaddr));
    inaddr.s_addr = mask;
    printf("mask : %s\n", inet_ntoa(inaddr));
*/
    if(pcap_compile(device, &filter, generic_filter_ruler.c_str(), 1, mask) < 0) {
        printf("%s error : ", dev_name);
        puts(err_buf);    
        return (void*)0;
    }
//    pcap_compile(device, &filter, "port 22", 1, mask);
    if(pcap_setfilter(device, &filter) < 0) {
        printf("%s error : ", dev_name);
        puts(err_buf);    
        return (void*)0;
    }

    puts("set filter success");

    int id = 0;
    /*dont should -1 maybe should by the time to control*/
    pcap_loop(device, -1, getPacket, (u_char*)&id);

    pcap_close(device);


    return (void*)0;
}



void getPacket(u_char *arg, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{

    struct ether_header *ethernet;
    struct iphdr *ip;
    //  struct icmphdr *icmp;


    /*
     * TODO
     * depend on the libpcap document, we need the do
     * the  check of the packet
     */

    if (pkthdr->len != pkthdr->caplen) {
        puts("capture the packet error len != caplen");
        return;
    }
    /*
       printf("Packet length: %d\n", pkthdr->len);
       printf("Number of bytes: %d\n", pkthdr->caplen);
       printf("Recieved time: %s", ctime((const time_t *)&pkthdr->ts.tv_sec)); 

*/

    ethernet = (struct ether_header *)packet;

    if (ntohs(ethernet->ether_type) == ETHERTYPE_IP) {

        //printf("it's a Ip packet\n");
        //printf("id: %d\n", ++(*id));


        ip = (struct iphdr*)(packet + sizeof(struct ether_header));

        if (ntohs(ip->tot_len) != pkthdr->caplen - 14) {
          //  printf("ip packet not right!!!! %d %d\n", ntohs(ip->tot_len), pkthdr->caplen);
            return;
        }

        ip_link_t ip_link;

        ip_link.src_ip = ip->saddr;
        ip_link.dst_ip = ip->daddr;

        pthread_mutex_lock(&net_info_lock);
        net_info_t net_info;


        //printf("%u %u  ", ip_link.src_ip, ip_link.dst_ip);
        //printf("count = %d\n", net_map.count(ip_link));
        long long now = get_current_time();
        if (net_map.count(ip_link) == 0) {

            net_info.connected = true;
            net_info.begin_time = now;
            net_info.trans_total = pkthdr->caplen;
            net_info.trans_rate = pkthdr->caplen;//Byte/s assume the base time is 1s
        }
        else {
            net_info = net_map[ip_link];
            if ((now - net_info.begin_time) > 2000) {
                net_info.begin_time = now;
                net_info.trans_total = pkthdr->caplen;
                net_info.trans_rate = pkthdr->caplen;//Byte/s
            }
            else {
                net_info.trans_total += pkthdr->caplen;
                long long time_interval = (now - net_info.begin_time);
                time_interval = (time_interval + 999) / 1000;
                time_interval = (time_interval == 0 ? 1 : time_interval);

                net_info.trans_rate = (net_info.trans_total * 1.0) / time_interval;
            }

            /*
            struct in_addr inaddr;
            inaddr.s_addr = ip->saddr;
            printf("src ip : %s   ", inet_ntoa(inaddr));
            inaddr.s_addr = ip->daddr;
            printf("dst ip : %s ", inet_ntoa(inaddr));

            printf("  %lf MB/s \n", net_info.trans_rate / (1<<20));
            */
        }

        net_info.last_time = now;
        net_map[ip_link] = net_info; 
        pthread_mutex_unlock(&net_info_lock);
    }
}
