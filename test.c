#include <stdio.h>
#include <stdlib.h>
#include <string.h>             // memcpy
#include <unistd.h>             // close()
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>             // struct ifreq, ifconf
#include <netinet/in.h>         // socket typedef
#include <ifaddrs.h>            // struct ifaddrs


static size_t if_name_size;

void error_handler(char *msg)
{
    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}


int main()
{
    struct ifreq ifr;
    
    int sock;
    unsigned char *mac_adr;
    
    mac_adr = (unsigned char*)malloc(sizeof(unsigned char)*6);
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock == -1)
        error_handler("socket making error in get_mac()");
    strcpy(ifr.ifr_name, "ens33");
    if(ioctl(sock, SIOCGIFHWADDR, &ifr) == -1)
        error_handler("ioctl() error in get_mac()");
    memcpy(mac_adr, ifr.ifr_hwaddr.sa_data, sizeof(unsigned char)*6);
    
    for(int i=0; i<6; i++)
        printf("%02X", mac_adr[i]);
//    for(int i=0; i<6; i++)
////        printf("%02x",(unsigned char)ifr.ifr_addr.sa_data[i]);
//        sprintf(&mac_adr[i*2], "%02X", (unsigned char)ifr.ifr_addr.sa_data[i]);
//
    close(sock);
//    printf("%d\n", sizeof(if_name_list));
}
//char **test2()
//{
////    int arr[3][4] = {
////            { 1, 2, 3, 4 },
////            { 5, 6, 7, 8 },
////            { 9, 10, 11, 12 }
////    };
//    char **arr;
//    arr = (char**)malloc(sizeof(char*)*4);
//    arr[0] = "aaaa";
//    arr[1] = "bbb";
//    arr[2] = "cccccc";
//    arr[3] = "dd";
//
////    for(int i=0; i<4; i++)
////        printf("%s\n", arr[i]);
////
//    return arr;
//}
//
//int main()
//{
//    char **arrr;
//
//    arrr = test2();
//
//    for(int i=0; i<4; i++)
//    {
//        printf("%s\n", arrr[i]);
//    }
//
//    return 0;
//}
//char *test()
//{
//    char *buf[3];
//
//    buf[0] = "aaaa";
//    buf[1] = "bbbbb";
//    buf[2] = "ccccc";
////    printf("%s", *(buf+1));
////    printf("%lud\n", sizeof(buf));
//
//    return *buf;
//}
//
//int main()
//{
//    char **buff;
//    buff = test();
//
////    buff = test();
//    printf("%s", buff);
////    printf("%s", buf);
////    for (int i=0; i<3; i++)
////    {
////        printf("%s\n", &buff[i]);
////    }
//    return 0;
//}
//
//
