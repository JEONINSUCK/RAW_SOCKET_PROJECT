#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>                     // struct ip
#include <netinet/in.h>                     // IPPROTO_ICMP

#include "netif.h"

int ip_init(struct ip *ip_p, char *if_name);
void error_handler(char *msg);

int main(int argc, char *argv[])
{
    struct ip ip_hdr;
    
    char *ip;
    int status;

    if(dns_to_ip("google.com", &ip) == -1)
        error_handler("dns_to_ip() error in main method");

    printf("%s\n", ip);

    // printf("%s\n", &ip);
    /* ip header set */
    // ip_init(&ip_hdr,"ens33");


    return 0;
}

int ip_init(struct ip *ip_p, char *if_name)
{
    unsigned char *src_ip_buf;
    
    int if_index;
    int checksum;
    
    src_ip_buf = get_ip_adr(if_name);
    
    /* IP header */
    ip_p->ip_v = 4;
    ip_p->ip_hl = IP_HDRLEN / 4;            // ip header length device 32bit(20>>2=5)
    ip_p->ip_tos = 0;
    ip_p->ip_len = htons(IP_HDRLEN + ICMP_HDRLEN);
    ip_p->ip_id = htons(0);
    ip_p->ip_off = htons(0);
    ip_p->ip_ttl = 255;
    ip_p->ip_p = IPPROTO_ICMP;
}

void error_handler(char *msg)
{
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}