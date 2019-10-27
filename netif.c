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

#include "netif.h"

unsigned char *get_ip_adr(char *if_name)
{
    struct ifreq ifr;
    
    int sock;
    unsigned char *src_ip;
    
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
