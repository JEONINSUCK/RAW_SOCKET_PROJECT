#include <stdio.h>
#include <stdlib.h>
#include <string.h>             // memcpy(), memset()
#include <unistd.h>             // close()
#include <sys/socket.h>

#include <net/if.h>             // struct ifreq, ifconf
#include <netinet/in.h>         // socket typedef
#include <ifaddrs.h>            // struct ifaddrs
#include <arpa/inet.h>          // inet_ntoa(), inet_pton(), inet_addr()

#include "netif.h"

#define debugMode 1



int main(int argc, char *argv[])
{
    struct Eth_hdr eth_hdr;
    struct Arp_hdr arp_hdr;
    
    char **if_name_buf;             // my network interface name list
    unsigned char *src_mac_buf;     // my mac address buf
    char *src_ip_buf;               // my ip address buf
    unsigned char *trg_ip_buf = "127.0.0.1";               // target ip address buf
    unsigned char *trg_mac_buf;              // target mac address buf
    int sock;
    
    /* dynamic memory set */
    trg_mac_buf = (unsigned char*)malloc(sizeof(unsigned char) *6);
//    trg_ip_buf = (unsigned char*)malloc(sizeof(unsigned char) * 20);
    
    /* parameter check line */
    if(argc < 2)
        error_handler("Not enough perameter");
    
    memset(trg_mac_buf, 0xff, 6*sizeof(unsigned char));
    if(debugMode == 1)
    {
        printf("trg_mac_buf: ");
        for(int i=0; i<6; i++)
            printf("%02x:", trg_mac_buf[i]);
        printf("\n");
    }
    src_mac_buf = get_mac_adr(argv[1]);
    if (debugMode == 1)
    {
        printf("src_mac_buf: ");
        for(int i=0; i<6; i++)
            printf("%02x:", src_mac_buf[i]);
        printf("\n");
    }
//    strcpy(trg_ip_buf, argv[2]);
    if(debugMode == 1)
        printf("trg_ip_buf(char): %s\n", trg_ip_buf);
    src_ip_buf = get_ip_adr(argv[1]);
    if(debugMode == 1)
        printf("src_ip_buf(char): %s\n", src_ip_buf);

    /* ARP header */
    arp_hdr.hdr_type = htons(ETH_HW_TYPE);
    arp_hdr.proto_type = htons(IP4_PRO_TYPE);
    arp_hdr.hdr_size = ETH_HW_SIZE;
    arp_hdr.proto_size = IP4_PRO_SIZE;
    arp_hdr.opcode = htons(ARP_REQUEST);
    memcpy(&arp_hdr.s_adr, src_mac_buf, sizeof(unsigned char) * 6);
    
    

    return 0;
}
