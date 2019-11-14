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

#define debugMode 0
#define BUFSIZE 1024

int arp_init(struct Arp_hdr *arp_p, char *if_name, char *trg_ip);
int eth_init(struct Eth_hdr *eth_p, char *if_name);
int arp_bind(int if_index, int *sock);
int arp_read(int fd);
int arp_send(char *ifname, char *ip);
void error_handler(char *msg);

int main(int argc, char *argv[])
{
    int sock;
    int ifindex;
    
    if (argc < 3)
        error_handler("arp_raw <INTERFACE_NAME> <IP>");
    
    ifindex = get_if_index("ens33");
    
    if(arp_bind(ifindex, &sock) == -1)
        error_handler("bind error");
    
    if(arp_send(argv[1], argv[2]) == -1)
    error_handler("arp_send() error");
    
    while(1)
    {
        int r = arp_read(sock);
        if(r == 0)
        {
            break;
        }
    }
    close(sock);
    return 0;
}


int arp_init(struct Arp_hdr *arp_p, char *if_name, char *trg_ip)
{
    if(debugMode == 1)
        printf("arp_init() start\n");
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
    if(debugMode == 1)
        printf("arp_init() success\n");
    return 0;
}

int eth_init(struct Eth_hdr *eth_p, char *if_name)
{
    if(debugMode == 1)
        printf("eth_init() start\n");
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
    if(debugMode == 1)
        printf("eth_init() success\n");
    return 0;
}

int arp_send(char *ifname, char *ip)
{
    if(debugMode == 1)
        printf("arp_send() start\n");
    struct Eth_hdr eth_hdr;
    struct Arp_hdr arp_hdr;
    struct sockaddr_ll device;
    
    
    unsigned char *full_packet;
    int sock;
    int success;
    int ifindex;
    
    char *test;
    
    /* dynamic memory set */
    full_packet = (unsigned char*)malloc(sizeof(eth_hdr)+sizeof(arp_hdr));
    
    /* Arp header set */
    if(arp_init(&arp_hdr, ifname, ip) == -1)
    {
        if(debugMode == 1)
            printf("arp_init() error in arp_send() method\n");
        return -1;
    }
    
    /* Ethernet header set*/
    if(eth_init(&eth_hdr, ifname) == -1)
    {
        if(debugMode == 1)
            printf("eth_init() error in arp_send() method\n");
        return -1;
    }
    
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
    
    /* ethernet header & arp header combination */
    memcpy(full_packet, &eth_hdr, sizeof(unsigned char) * ETH_HDRLEN);
    memcpy(full_packet+sizeof(eth_hdr), &arp_hdr, sizeof(unsigned char) * ARP_HDRLEN);
    
//    for(int i=0; i<42; i++)
//        printf("%02x\n", full_packet[i]);

    /* sockaddr_ll set */
    memset (&device, 0, sizeof (device));
    ifindex = get_if_index(ifname);
    if(ifindex == -1)
    {
        if(debugMode == 1)
            printf("get_if_index() error in arp_send() method\n");
        return -1;
    }
    if(debugMode == 1)
        printf("\nifindex: %d\n", ifindex);
    device.sll_ifindex = ifindex;
    device.sll_family = AF_PACKET;
    memcpy (device.sll_addr, arp_hdr.s_adr, 6 * sizeof (uint8_t));
    device.sll_halen = 6;
    
    /* socket send part */
    if((sock=socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
        return -1;
    if((success = sendto(sock, full_packet, ETH_HDRLEN+ARP_HDRLEN,0, (struct sockaddr *)&device, sizeof(device))) < 0)
        return -1;
    
    close(sock);
    return 0;
}

int arp_bind(int if_index, int *sock)
{
    if(debugMode == 1)
        printf("arp_bind() start\n");
    struct sockaddr_ll sll;
    
    memset(&sll, 0, sizeof(struct sockaddr_ll));
    
    *sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if(sock < 0)
        return -1;
    
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_index;
    if(bind(*sock, (struct sockaddr*) &sll, sizeof(struct sockaddr_ll)) < 0)
        return -1;
    if(debugMode == 1)
        printf("arp_bind() success\n");
    return 0;
}

int arp_read(int fd)
{
    if(debugMode == 1)
        printf("arp_read() start\n");
    struct ethhdr *recv_eth_hdr;
    struct Arp_hdr *recv_arp_hdr;
    struct in_addr sender_a;
    
    char buf[BUFSIZE];
    int length;
    
    length = recvfrom(fd, buf, BUFSIZE, 0, NULL, NULL);
    if(length == -1)
        return -1;
    
    recv_eth_hdr = (struct ethhdr *)buf;
    recv_arp_hdr = (struct Arp_hdr *)(buf + ETH_HDRLEN);
    
    if (ntohs(recv_eth_hdr->h_proto) != 0x0806) {
        printf("Not an ARP packet\n");
        return -1;
    }
    if (ntohs(recv_arp_hdr->opcode) != 0x02) {
        printf("Not an ARP reply\n");
        return -1;
    }
    
    memset(&sender_a, 0, sizeof(struct in_addr));
    memcpy(&sender_a.s_addr, recv_arp_hdr->s_ip, sizeof(uint32_t));

    printf("IP Address: %d.%d.%d.%d\n",
           recv_arp_hdr->s_ip[0],
           recv_arp_hdr->s_ip[1],
           recv_arp_hdr->s_ip[2],
           recv_arp_hdr->s_ip[3]);
    
    printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
    recv_arp_hdr->s_adr[0],
    recv_arp_hdr->s_adr[1],
    recv_arp_hdr->s_adr[2],
    recv_arp_hdr->s_adr[3],
    recv_arp_hdr->s_adr[4],
    recv_arp_hdr->s_adr[5]);

    if(debugMode == 1)
        printf("arp_read() success\n");
    return 0;
}


void error_handler(char *msg)
{
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}
