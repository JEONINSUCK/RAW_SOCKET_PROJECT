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
#include <net/ethernet.h>

#include "netif.h"

#define debugMode 1

int arp_init(struct Arp_hdr *arp_p, char *if_name, char *trg_ip);
int eth_init(struct Eth_hdr *eth_p, char *if_name);
void error_handler(char *msg);

int main(int argc, char *argv[])
{
    struct Eth_hdr eth_hdr;
    struct Arp_hdr arp_hdr;
    struct sockaddr_ll device;
    
    
    unsigned char *full_packet;
    int sock;
    int success;
    int ifindex;
    
    char *test;
    
    /* parameter check line */
    if(argc < 3)
        error_handler("Not enough perameter");
    
    /* dynamic memory set */
    full_packet = (unsigned char*)malloc(sizeof(eth_hdr)+sizeof(arp_hdr));
    
    /* Arp header set */
    arp_init(&arp_hdr, argv[1], argv[2]);
    if(debugMode == 1)
    {
        printf("************************* ARP HEADER *************************\n");
        printf("trg_mac_buf: ");
        for(int i=0; i<6; i++)
            printf("%02x:", arp_hdr.t_adr[i]);
        printf("\n");
        printf("src_mac_buf: ");
        for(int i=0; i<6; i++)
            printf("%02x:", arp_hdr.s_adr[i]);
        printf("\n");
        printf("trg_ip_buf(char): %d.%d.%d.%d\n", arp_hdr.t_ip[0],arp_hdr.t_ip[1],arp_hdr.t_ip[2],arp_hdr.t_ip[3]);
        printf("src_ip_buf(char): %d,%d,%d,%d\n", arp_hdr.s_ip[0],arp_hdr.s_ip[1],arp_hdr.s_ip[2],arp_hdr.s_ip[3]);
    }
    
    /* Ethernet header set*/
    eth_init(&eth_hdr, argv[1]);
    if(debugMode == 1)
    {
        printf("\n");
        printf("************************* EHTERNET HEADER *************************\n");
        printf("trg_mac_buf: ");
        for(int i=0; i<6; i++)
            printf("%02x:", eth_hdr.t_adr[i]);
        printf("\n");
        printf("src_mac_buf: ");
        for(int i=0; i<6; i++)
            printf("%02x:", eth_hdr.s_adr[i]);
        printf("\n");
        printf("hw proto type: 0x%02x%02x\n",eth_hdr.h_proto[0],eth_hdr.h_proto[1]);
    }
    
    /* ethernet header & arp header combination */
    memcpy(full_packet, &eth_hdr, sizeof(unsigned char) * ETH_HDRLEN);
    memcpy(full_packet+sizeof(eth_hdr), &arp_hdr, sizeof(unsigned char) * ARP_HDRLEN);
    
//    for(int i=0; i<42; i++)
//        printf("%02x\n", full_packet[i]);

    /* sockaddr_ll set */
    memset (&device, 0, sizeof (device));
    ifindex = get_if_index(argv[1]);
    if(ifindex == -1)
        error_handler("ifindex() error in main() method");
    if(debugMode == 1)
        printf("\nifindex: %d\n", ifindex);
    device.sll_ifindex = ifindex;
    device.sll_family = AF_PACKET;
    memcpy (device.sll_addr, arp_hdr.s_adr, 6 * sizeof (uint8_t));
    device.sll_halen = 6;
    
    /* socket send part */
    if((sock=socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
        error_handler("socket() error in main method");
    if((success = sendto(sock, full_packet, ETH_HDRLEN+ARP_HDRLEN,0, (struct sockaddr *)&device, sizeof(device))) < 0)
        error_handler("sento() error in main method");
    
    close(sock);
    return 0;
}


int arp_init(struct Arp_hdr *arp_p, char *if_name, char *trg_ip)
{
    char **if_name_buf;             // my network interface name list
    unsigned char *src_mac_buf;     // my mac address buf
    unsigned char *src_ip_buf;               // my ip address buf
    unsigned char *trg_ip_buf;               // target ip address buf
    unsigned char *trg_mac_buf;              // target mac address buf
    unsigned int conv_ip;
    
    /* dynamic memory set */
    trg_mac_buf = (unsigned char*)malloc(sizeof(unsigned char) * 6);
    
    memset(trg_mac_buf, 0xff, 6*sizeof(unsigned char));
    src_mac_buf = get_mac_adr(if_name);
    trg_ip_buf = trg_ip;
    src_ip_buf = get_ip_adr(if_name);
    
    /* ARP header */
    arp_p->hdr_type = htons(ETH_HW_TYPE);              // 1
    arp_p->proto_type = htons(IP4_PRO_TYPE);           // 0x0800
    arp_p->hdr_size = ETH_HW_SIZE;                     // 6
    arp_p->proto_size = IP4_PRO_SIZE;                  // 4
    arp_p->opcode = htons(ARP_REQUEST);                // 1
    
    /* source mac & ip set */
    memcpy(&arp_p->s_adr, src_mac_buf, sizeof(unsigned char) * 6);
    if((conv_ip = inet_addr(src_ip_buf)) < 0)
    {
        printf("inet_addr() error in arp_init() method");
        return -1;
    }
    for(int i=0; i<4; i++)
        arp_p->s_ip[i] = (conv_ip >> 8*i) & 255;
    
    /* target mac & ip set */
    memcpy(&arp_p->t_adr, trg_mac_buf, sizeof(unsigned char) * 6);
    if((conv_ip = inet_addr(trg_ip_buf)) < 0)
    {
        printf("inet_addr() error in arp_init() method");
        return -1;
    }
    for(int i=0; i<4; i++)
        arp_p->t_ip[i] = (conv_ip >> 8*i) & 255;
    
    free(trg_mac_buf);
    return 0;
}

int eth_init(struct Eth_hdr *eth_p, char *if_name)
{
    unsigned char *src_mac_buf;     // my mac address buf
    unsigned char *trg_mac_buf;              // target mac address buf
    
    trg_mac_buf = (unsigned char*)malloc(sizeof(unsigned char) * 6);
    
    src_mac_buf = get_mac_adr(if_name);
    if(*src_mac_buf == -1)
        return -1;
    memset(trg_mac_buf, 0xff, 6*sizeof(unsigned char));
    
    /* ETHERNET header*/
    memcpy(&eth_p->s_adr, src_mac_buf, sizeof(unsigned char) * 6);
    memcpy(&eth_p->t_adr, trg_mac_buf, sizeof(unsigned char) * 6);
    eth_p->h_proto[1] = ARP_PROTO & 255;
    eth_p->h_proto[0] = (ARP_PROTO >> 8) & 255;
    
    free(trg_mac_buf);
    return 0;
}

void error_handler(char *msg)
{
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}
