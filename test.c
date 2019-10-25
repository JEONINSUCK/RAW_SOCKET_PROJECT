#include <stdio.h>

#include "netif.h"

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
    char *ip_adr_buf;
    ip_adr_buf = get_ip_adr(argv[1]);
    printf("%s\n", ip_adr_buf);

    return 0;
}
