#include <stdio.h>
#include <stdlib.h>
#include <string.h>             // memcpy
#include <unistd.h>             // close()
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>             // struct ifreq, ifconf
#include <netinet/in.h>         // socket typedef
#include <ifaddrs.h>            // struct ifaddrs
#include <arpa/inet.h>           // inet_ntoa()

#define TRUE 1
#define FALSE 0
#define eth_hdr_len 14
#define arp_hdr_len 28

static size_t if_name_size;     // network interface

unsigned char *get_mac_adr(char *if_name);
void error_handler(char *msg);
char **get_if_name();
char *get_ip_adr();

struct eth_hdr
{
    char d_adr[6];
    char s_adr[6];
    char h_proto[2];
}__attribute__((packed));

struct arp_hdr
{
    char hdr_type[2];
    char proto_type[2];
    char hdr_size;
    char proto_size;
    char opcode[2];
    char s_adr[6];
    char s_ip[4];
    char t_adr[6];
    char t_ip[4];
}__attribute__((packed));


int main(int argc, char *argv[])
{
    if(argc < 2)
        error_handler("Not enough perameter");
    /* get_if_name method test line */
//    char **if_name_buf;
//    if_name_buf = get_if_name();
//    for(int i=0; i<if_name_size; i++)
//        printf("%s\n", if_name_buf[i]);
    
    /* get_mac_adr method test line */
//    unsigned char *src_mac_buf;
//    src_mac_buf = get_mac_adr(argv[1]);
//    printf("Network interface %s's mac: ", argv[1]);
//    for(int i=0; i<6; i++)
//        printf("%02x:", src_mac_buf[i]);
//    printf("\n");
    
    /* get_ip_adr method test line */
//    char *ip_adr_buf;
//    ip_adr_buf = get_ip_adr(argv[1]);
//    printf("%s\n", ip_adr_buf);

    return 0;
}

char *get_ip_adr(char *if_name)
{
    struct ifreq ifr;
    
    int sock;
    char *src_ip;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == -1)
        error_handler("socket making error in get_ip_adr()");
    ifr.ifr_addr.sa_family = AF_INET;
    strcpy(ifr.ifr_name, if_name);
    if(ioctl(sock, SIOCGIFADDR, &ifr) == -1)
        error_handler("ioctl() error in get_ip_adr()");
    src_ip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    
    close(sock);
    return src_ip;
}

char **get_if_name()
{
    struct ifaddrs *addrs;
    struct ifaddrs *tmp;
    
    int i=0;
    char **if_name_list;              // memories maximum 10 interface
    
    if_name_list = (char**)malloc(sizeof(char*)*10);
    
    getifaddrs(&addrs);
    tmp = addrs;
    while(tmp)
    {
        if(tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET)
        {
            if_name_list[i] = tmp->ifa_name;
//            printf("%s\n", if_name_list[i]);
        }
        tmp = tmp->ifa_next;
        i++;
    }
    if_name_size = strlen(*if_name_list);
    freeifaddrs(addrs);
    return if_name_list;
}

unsigned char *get_mac_adr(char *if_name)
{
   struct ifreq ifr;
    
    int sock;
    unsigned char *mac_adr;
    
    mac_adr = (unsigned char*)malloc(sizeof(unsigned char)*6);
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == -1)
        error_handler("socket making error in get_mac()");
    strcpy(ifr.ifr_name, if_name);
    if(ioctl(sock, SIOCGIFHWADDR, &ifr) == -1)
        error_handler("ioctl() error in get_mac()");
    memcpy(mac_adr, ifr.ifr_hwaddr.sa_data, sizeof(unsigned char)*6);
//    for(int i=0; i<6; i++)
//        printf("%02X", mac_adr[i]);
    close(sock);
    return mac_adr;
}

void error_handler(char *msg)
{
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}
