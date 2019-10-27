#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


int main(int argc, char *argv[])
{
    int a = 0x0806;
    
    printf("0x%x\n", a & 255);
    printf("0x%x\n", a  255);
    
    
    
    
    return 0;
}
