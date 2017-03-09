#ifndef TRANSPORTLAYOUT_H
#define TRANSPORTLAYOUT_H

#include "main.h"

#include <netinet/tcp.h>
#include <netinet/udp.h>

#include "ospf.h"


class TransportLayout
{
public:
    TransportLayout();

    void check_transport_type(unsigned char type, const unsigned char *start_position, int flag);

    void display_tcp_header(struct tcphdr *header);

    void display_udp_header(struct udphdr *header);

    void display_icmp_header(struct icmp6_hdr *header);

    void display_ospf_header(struct ospfhdr *header);
};

#endif // TRANSPORTLAYOUT_H
