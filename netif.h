#define TRUE 1
#define FALSE 0
#define eth_hdr_len 14
#define arp_hdr_len 28
#define ARP_PROTO 0x0806
#define ETH_HW_TYPE 1
#define IP4_PRO_TYPE 0x0800
#define ETH_HW_SIZE 6
#define IP4_PRO_SIZE 4
#define ARP_REQUEST 1
#define ARP_REPLY 2

/* global variable define part */
static size_t if_name_size;     // network interface

/* struct define part */
struct Eth_hdr
{
    unsigned char d_adr[6];
    unsigned char s_adr[6];
    unsigned char h_proto[2];
}__attribute__((packed));

struct Arp_hdr
{
    unsigned char hdr_type[2];
    unsigned char proto_type[2];
    unsigned char hdr_size;
    unsigned char proto_size;
    unsigned char opcode[2];
    unsigned char s_adr[6];
    unsigned char s_ip[4];
    unsigned char t_adr[6];
    unsigned char t_ip[4];
}__attribute__((packed));

/* function define part */
unsigned char *get_mac_adr(char *if_name);      // return array
void error_handler(char *msg);
char **get_if_name();                           // return pointer array address
char *get_ip_adr();                             // return array
