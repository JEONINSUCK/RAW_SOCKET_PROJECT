#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h> // struct ip
#include <netinet/ip_icmp.h>    // sturct icmp
#include <netinet/in.h> // IPPROTO_ICMP
#include <arpa/inet.h>

#include "netif.h"

#define DEBUGMODE 1

int ip_init(struct ip *ip_p, char *if_xname, char *dst_ip);
int icmp_init(struct icmp *icmp_p);
void error_handler(char *msg);
uint16_t check_sum(uint16_t *iphdr, int len);
// uint16_t checksum(uint16_t *addr, int len);

int main(int argc, char *argv[])
{
    struct ip ip_hdr;
    struct icmp icmp_hdr;
    struct sockaddr_in sin;

    int sock;
    int on = 1;
    uint8_t *packet;

    if(argc < 3)
    {
        printf("icmp_raw <INTERFACE> <DST_IP>\n");
        exit(EXIT_FAILURE);
    }

    /* ip header set */
    ip_init(&ip_hdr, argv[1], argv[2]);
    icmp_init(&icmp_hdr);
    
    memcpy(packet, &ip_hdr, IP_HDRLEN);
    memcpy((packet + IP_HDRLEN), &icmp_hdr, ICMP_HDRLEN);

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ip_hdr.ip_dst.s_addr;

    if((sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0 )
        error_handler("socket making error");

    if(setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
        error_handler("setsockopt() error in main method");

    if(sendto(sock, packet, IP_HDRLEN + ICMP_HDRLEN, 0, (struct sockaddr *) &sin, sizeof(struct sockaddr)) < 0)
        error_handler("sendto() error in main method");

    return 0;
}

int ip_init(struct ip *ip_p, char *if_name, char *dst_ip)
{
    if (DEBUGMODE == 1)
        printf("ip_init() start\n");

    char *src_ip_buf;
    char *dst_ip_buf;
    int if_index;
    int checksum;

    src_ip_buf = (char *)get_ip_adr(if_name);

    /* IP header */
    ip_p->ip_v = 4;
    ip_p->ip_hl = IP_HDRLEN / 4; // ip header length device 32bit(20>>2=5)
    ip_p->ip_tos = 0;
    ip_p->ip_len = htons(IP_HDRLEN + ICMP_HDRLEN);
    ip_p->ip_id = htons(0); 
    ip_p->ip_off = htons(0);
    ip_p->ip_ttl = 255;
    ip_p->ip_p = IPPROTO_ICMP;

    if (dns_to_ip(dst_ip, &dst_ip_buf) == -1)
        return -1;
    if ((inet_pton(AF_INET, src_ip_buf, &(ip_p->ip_src))) != 1)
        return -1;
    if ((inet_pton(AF_INET, dst_ip_buf, &(ip_p->ip_dst))) != 1)
        return -1;

    ip_p->ip_sum = 0;
    ip_p->ip_sum = check_sum((uint16_t *) ip_p, IP_HDRLEN);

    return 0;    
}

void error_handler(char *msg)
{
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}

uint16_t check_sum(uint16_t *iphdr, int len)
{
    int count = len;
    uint32_t sum = 0;
    uint16_t answer = 0;

    while (count > 1)
    {
        sum += *(iphdr++);
        count -= 2;
    }
    
    if( count > 1)
        sum += *(uint8_t *)iphdr;
    
    while(sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);
    
    answer = ~sum;

    printf("%x   %x\n", sum, answer);

    return answer;
}

int icmp_init(struct icmp *icmp_p)
{
    if(DEBUGMODE == 1)
        printf("icmp_init() start\n");

    icmp_p->icmp_type = ICMP_ECHO;
    icmp_p->icmp_code = 0;
    icmp_p->icmp_id = htons(1000);
    icmp_p->icmp_seq = htons(0);
    icmp_p->icmp_cksum = 0;
    icmp_p->icmp_cksum = check_sum((uint16_t *)icmp_p, ICMP_HDRLEN);

    return 0;
}