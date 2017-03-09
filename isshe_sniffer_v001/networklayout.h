#ifndef NETWORKLAYOUT_H
#define NETWORKLAYOUT_H



#include "main.h"
#include "vlan.h"

#define STR_LEN 64

class NetworkLayout
{
public:
    NetworkLayout();

    //第二个参数考虑改为：char *
    void check_network_type(unsigned short type, const unsigned char *start_position, int flag);

    void display_ip_data(struct ip *iphdr);

    void display_arp_data(struct arphdr *arp_header);

    void display_vlan_data(struct vlanhdr *vlanhdr);

    void display_ospf_data(struct ospfhdr *ospf_header);
};

#endif // NETWORKLAYOUT_H
