#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/in.h>         //ip protocol的值
#include <arpa/inet.h>
#include <net/if_arp.h>

#include <QDebug>

#include "networklayout.h"
#include "mainwindow.h"
#include "transportlayout.h"
#include "vlan.h"
#include "ospf.h"
#include "statisticsmw.h"

extern MainWindow *mw;
extern global_variable_s global_variable;
extern protocol_count_s protocol_count[MAX_PROTOCOL_LEN];

NetworkLayout::NetworkLayout()
{

}

void NetworkLayout::check_network_type(unsigned short type,
                                       const unsigned char *start_position, int flag)
{
    mw->protocol_num = QString::number(type);

    unsigned char *cur_position = (unsigned char *)start_position;

    if (type == ETHERTYPE_IP)
    {
        mw->protocol_str = "IP";
        qDebug() << "IP 协议";
        struct ip *iphdr = (struct ip *)cur_position;

        if (flag == FLAG_RESUME)
        {
            protocol_count[7].times++;
            mw->src_addr = inet_ntoa(iphdr->ip_src);
            mw->dst_addr = inet_ntoa(iphdr->ip_dst);
            mw->protocol_num = QString::number(iphdr->ip_p, 10);
//            return;         //到这里就不用继续了，如果只是显示概要信息
        }
        else if (flag == FLAG_DETAIL)
        {
            display_ip_data(iphdr);
            //2017.02.16
            mw->set_pic_name(&global_variable.pic_2, IP_HEADER);
        }

        //传输层
        //这个放到case里面会出错
        TransportLayout tsl;
        tsl.check_transport_type(iphdr->ip_p, start_position + sizeof(struct iphdr), flag);
    }
    else if (type == ETHERTYPE_PUP)
    {
        if (flag == FLAG_RESUME)
        {
            mw->protocol_str = "PUP";
            protocol_count[8].times++;
        }
    }
    else if (type == ETHERTYPE_ARP)
    {
        if (flag == FLAG_RESUME)
        {
            mw->protocol_str = "ARP";
            protocol_count[9].times++;
        }
        else if (flag == FLAG_DETAIL)
        {

            display_arp_data((struct arphdr *)cur_position);
            mw->set_pic_name(&global_variable.pic_2, ARP_HEADER);
        }
    }
    else if (type == ETHERTYPE_REVARP)
    {
        if (flag == FLAG_RESUME)
        {
            mw->protocol_str = "REARP";
            protocol_count[10].times++;
        }
    }
    else if (type == ETHERTYPE_VLAN)
    {
        if (flag == FLAG_RESUME)
        {
            mw->protocol_str = "802.1Q";
            protocol_count[11].times++;
        }
        else if (flag == FLAG_DETAIL)
        {
    //            struct vlanhdr vlan_header;
    //            memset(&vlan_header, '\0', sizeof(struct vlanhdr));
    //            memcpy(&vlan_header, cur_position, sizeof(struct vlanhdr));
            display_vlan_data((struct vlanhdr *)cur_position);
        }

        struct vlanhdr *vhdr = (struct vlanhdr *)cur_position;
        check_network_type(htons(vhdr->v_type), start_position+sizeof(struct vlanhdr), flag);
    }
    else if (type == ETHERTYPE_IPV6)
    {
        if (flag == FLAG_RESUME)
        {
            mw->protocol_str = "IPv6";
            protocol_count[12].times++;
        }
    }
    else
    {
        if (flag == FLAG_RESUME)
        {
            //mw->protocol_str = "UNKNOW";
            protocol_count[13].times++;             //other
        }

        //printf("type = %x...\n", type);
    }

}



void NetworkLayout::display_ip_data(struct ip *iphdr)
{
    //添加root
    QTreeWidgetItem *network_root;
    QStringList str_list;
    QString qstr;

    str_list.clear();
    str_list << "IP:";
    network_root = mw->add_top_item(str_list);
    network_root->setExpanded(true);           //这个是设置展开

#ifdef _IP_VHL
    mw->add_child_to_mw(network_root, "version << 4 | header length >> 2: ", iphdr->ip_vhl);
#else
    mw->add_child_to_mw(network_root, "header length: ", iphdr->ip_hl);
    mw->add_child_to_mw(network_root, "version: ", iphdr->ip_v);
#endif /* not _IP_VHL */

    mw->add_child_to_mw(network_root, "type of service: ", iphdr->ip_tos);
    mw->add_child_to_mw(network_root, "total length: ", iphdr->ip_len);
    mw->add_child_to_mw(network_root, "identification: ", iphdr->ip_id);
    mw->add_child_to_mw(network_root, "fragment offset field: ", iphdr->ip_off);
    mw->add_child_to_mw(network_root, "time to live: ", iphdr->ip_ttl);
    mw->add_child_to_mw(network_root, "protocol: ", iphdr->ip_p);
    mw->add_child_to_mw(network_root, "checksum: ", iphdr->ip_sum);
    mw->add_child_to_mw(network_root, "source address: ", inet_ntoa(iphdr->ip_src));
    mw->add_child_to_mw(network_root, "destination address: ", inet_ntoa(iphdr->ip_dst));

    //下一层协议
    global_variable.tslayout_type = iphdr->ip_p;
}

void NetworkLayout::display_arp_data(struct arphdr *arp_header)
{
    //添加root
    QTreeWidgetItem *root;
    QStringList str_list;

    str_list.clear();
    str_list << "ARP:";
    root = mw->add_top_item(str_list);
    root->setExpanded(true);           //这个是设置展开

    mw->add_child_to_mw(root, "Format of hardware address: ", ntohs(arp_header->ar_hrd));
    mw->add_child_to_mw(root, "Format of protocol address: ", ntohs(arp_header->ar_pro));
    mw->add_child_to_mw(root, "Length of hardware address: ", arp_header->ar_hln);
    mw->add_child_to_mw(root, "Length of protocol address: ", arp_header->ar_pln);
    mw->add_child_to_mw(root, "ARP opcode (command): ", ntohs(arp_header->ar_op));
}

void NetworkLayout::display_vlan_data(struct vlanhdr *vlan_header)
{
    //添加root
    QTreeWidgetItem *root;
    QStringList str_list;

    str_list.clear();
    str_list << "802.1Q(VLAN):";
    root = mw->add_top_item(str_list);
    root->setExpanded(true);           //这个是设置展开

    mw->add_child_to_mw(root, "Priority: ", vlan_header->v_priority);
    mw->add_child_to_mw(root, "CFI: ", vlan_header->v_cfi);
    mw->add_child_to_mw(root, "ID: ", vlan_header->v_id);
    mw->add_child_to_mw(root, "type: ", vlan_header->v_type);
}

void NetworkLayout::display_ospf_data(struct ospfhdr *ospf_header)
{
    //添加root
    QTreeWidgetItem *root;
    QStringList str_list;

    str_list.clear();
    str_list << "OSPF Header:";
    root = mw->add_top_item(str_list);
    root->setExpanded(true);           //这个是设置展开

    mw->add_child_to_mw(root, "Version: ", ospf_header->o_version);
    mw->add_child_to_mw(root, "Message Type: ", ospf_header->o_mess_type);
    mw->add_child_to_mw(root, "Packet Length: ", ntohs(ospf_header->o_pkt_len));
    mw->add_child_to_mw(root, "Source OSPF Router: ", inet_ntoa(ospf_header->o_src_router));
    mw->add_child_to_mw(root, "Area ID: ", inet_ntoa(ospf_header->o_area_id));
    mw->add_child_to_mw(root, "Checksum: ", ntohs(ospf_header->o_checksum));
    mw->add_child_to_mw(root, "Auth Type: ", ntohs(ospf_header->o_auth_type));
    mw->add_child_to_mw(root, "Auth Data: ", ospf_header->o_auth_data);
}
