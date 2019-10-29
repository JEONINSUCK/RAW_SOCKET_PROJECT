#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "netif.h"


int main(int argc, char *argv[])
{
    char *test;
    test = ip_conv("127.0.0.1");
    
    printf("%02x %02x %02x %02x\n", test[0],test[1],test[2],test[3]);
    
    
    return 0;
}
