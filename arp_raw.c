#include <stdio.h>
#include <stdlib.h>
#include <string.h>             // memcpy(), memset()
#include <unistd.h>             // close()
#include <sys/socket.h>

#include <net/if.h>             // struct ifreq, ifconf
#include <netinet/in.h>         // socket typedef
#include <ifaddrs.h>            // struct ifaddrs
#include <arpa/inet.h>          // inet_ntoa(), inet_pton(), inet_addr()
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)

#include "netif.h"

#define debugMode 1



int main(int argc, char *argv[])
{
    struct Eth_hdr eth_hdr;
    struct Arp_hdr arp_hdr;
    struct sockaddr_in device;
    
    char **if_name_buf;             // my network interface name list
    unsigned char *src_mac_buf;     // my mac address buf
    unsigned char *src_ip_buf;               // my ip address buf
    unsigned char *trg_ip_buf = "172.16.255.1";               // target ip address buf
    unsigned char *trg_mac_buf;              // target mac address buf
    unsigned int conv_ip;
    unsigned char *full_packet;
    int sock = 0;
    int success = 0;
    
    
    
    
    /* dynamic memory set */
    full_packet = (unsigned char*)malloc(sizeof(eth_hdr)+sizeof(arp_hdr));
    trg_mac_buf = (unsigned char*)malloc(sizeof(unsigned char) * 6);
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
    
    /* source mac & ip set */
    memcpy(&arp_hdr.s_adr, src_mac_buf, sizeof(unsigned char) * 6);
    conv_ip = inet_addr(src_ip_buf);
    for(int i=0; i<4; i++)
        arp_hdr.s_ip[i] = (conv_ip >> 8*i) & 255;
    
    /* target mac & ip set */
    memcpy(&arp_hdr.t_adr, trg_mac_buf, sizeof(unsigned char) * 6);
    conv_ip = inet_addr(trg_ip_buf);
    for(int i=0; i<4; i++)
        arp_hdr.t_ip[i] = (conv_ip >> 8*i) & 255;
    
    /* ETHERNET header*/
    memcpy(&eth_hdr.s_adr, src_mac_buf, sizeof(unsigned char) * 6);
    memcpy(&eth_hdr.t_adr, trg_mac_buf, sizeof(unsigned char) * 6);
    eth_hdr.h_proto[1] = ARP_PROTO & 255;
    eth_hdr.h_proto[0] = (ARP_PROTO >> 8) & 255;
    
    memcpy(full_packet, &eth_hdr, sizeof(unsigned char) * ETH_HDRLEN);
    memcpy(full_packet+sizeof(eth_hdr), &arp_hdr, sizeof(unsigned char) * ARP_HDRLEN);
    
//    for(int i=0; i<42; i++)
//        printf("%02x\n", full_packet[i]);

    memset (&device, 0, sizeof (device));
//    if ((device.sll_ifindex = if_nametoindex (interface)) == 0) {
//      perror ("if_nametoindex() failed to obtain interface index ");
//      exit (EXIT_FAILURE);
//    }
    bzero(&device, sizeof(device));
    device.sin_family = AF_INET;
    device.sin_addr.s_addr = inet_addr(trg_ip_buf);
    device.sin_port = htons(67);
    
    
    if((sock=socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        error_handler("socket() error in main method");
    if((success = sendto(sock, full_packet, ETH_HDRLEN+ARP_HDRLEN,0, (struct sockaddr *) &device, sizeof(device))) <=0)
        error_handler("sento() error in main method");
    
    
    return 0;
}
