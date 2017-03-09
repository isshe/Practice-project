#ifndef OSPF_H
#define OSPF_H

#include <sys/types.h>

#include <netinet/in.h>

struct ospfhdr
{
    unsigned char o_version;
    unsigned char o_mess_type;
    unsigned short o_pkt_len;
    struct in_addr o_src_router;
    struct in_addr o_area_id;
    unsigned short o_checksum;
    unsigned short o_auth_type;
    unsigned long long o_auth_data;      //这里应该用64bit
};


struct ospf_hello_pkt
{
    struct in_addr o_net_mask;
    unsigned short o_interval;
    unsigned char o_options;
    unsigned char o_priority;
    unsigned int o_router_dead_interval;
    struct in_addr o_dst_router;
    struct in_addr o_bk_dst_router;
};

#endif // OSPF_H
