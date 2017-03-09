#include <netinet/in.h>         //ip protocol的值
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <net/ethernet.h>
#include <netinet/icmp6.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "main.h"
#include "mainwindow.h"
#include "transportlayout.h"
#include "statisticsmw.h"

extern MainWindow *mw;
extern global_variable_s global_variable;
extern protocol_count_s protocol_count[MAX_PROTOCOL_LEN];

TransportLayout::TransportLayout()
{

}


void TransportLayout::check_transport_type(unsigned char type, const unsigned char *start_position, int flag)
{

//    unsigned char *cur_position = start_position;

    mw->protocol_num = QString::number(type);

    switch (type)
    {
    case IPPROTO_TCP:           //6
        if (flag == FLAG_RESUME)
        {
            protocol_count[14].times++;         //
            mw->protocol_str = "TCP";

        }
        else if (flag == FLAG_DETAIL)
        {
            display_tcp_header((struct tcphdr *)start_position);

            //2017.02.16
            mw->set_pic_name(&global_variable.pic_3, TCP_HEADER);
        }

        break;

    case IPPROTO_UDP:           //17
        if (flag == FLAG_RESUME)
        {
            protocol_count[15].times++;
            mw->protocol_str = "UDP";
        }
        else if (flag == FLAG_DETAIL)
        {
            display_udp_header((struct udphdr *)start_position);
            mw->set_pic_name(&global_variable.pic_3, UDP_HEADER);
        }
        break;

    case IPPROTO_ICMP:
        if (flag == FLAG_RESUME)
        {
            protocol_count[17].times++;
            mw->protocol_str = "ICMP";
        }
        else if (flag == FLAG_DETAIL)
        {
            display_icmp_header((struct icmp6_hdr *)start_position);
            mw->set_pic_name(&global_variable.pic_3, ICMP_REQ_REP);             //这里更好的方法是根据类型再分以下。
        }
        break;

    case IPPROTO_IGMP:
        break;
        //...还有好多

    case 89:        //ospf
        if (flag == FLAG_RESUME)
        {
            protocol_count[18].times++;
            mw->protocol_str = "OSPF";
        }
        else if (flag == FLAG_DETAIL)
        {
            display_ospf_header((struct ospfhdr *)start_position);
        }
        break;
    default:
        if (flag == FLAG_RESUME)
        {
            protocol_count[16].times++;
        }

        break;
    }
}

void TransportLayout::display_tcp_header(struct tcphdr *header)
{
    //添加root
    QTreeWidgetItem *tcp_root;
    QStringList str_list;
    QString qstr;

    str_list.clear();
    str_list << "TCP:";
    tcp_root = mw->add_top_item(str_list);
    tcp_root->setExpanded(true);           //这个是设置展开

    mw->add_child_to_mw(tcp_root, "源端口: ", header->th_sport);
    mw->add_child_to_mw(tcp_root, "目的端口: ", header->th_dport);
    mw->add_child_to_mw(tcp_root, "序号: ", header->th_seq);
    mw->add_child_to_mw(tcp_root, "确认序号: ", header->th_ack);
#if __DARWIN_BYTE_ORDER == __DARWIN_LITTLE_ENDIAN
    mw->add_child_to_mw(tcp_root, "保留: ", header->th_x2);
    mw->add_child_to_mw(tcp_root, "数据偏移: ", header->th_off);
#endif
#if __DARWIN_BYTE_ORDER == __DARWIN_BIG_ENDIAN
    mw->add_child_to_mw(tcp_root, "数据偏移: ", header->th_off);
    mw->add_child_to_mw(tcp_root, "保留: ", header->th_x2);
#endif
    mw->add_child_to_mw(tcp_root, "标志: ", header->th_flags);
    mw->add_child_to_mw(tcp_root, "窗口: ", header->th_win);
    mw->add_child_to_mw(tcp_root, "校验和: ", header->th_sum);
    mw->add_child_to_mw(tcp_root, "紧急指针: ", header->th_urp);
    /*这个flag显示还可以再完善
    #define	TH_FIN	0x01
    #define	TH_SYN	0x02
    #define	TH_RST	0x04
    #define	TH_PUSH	0x08
    #define	TH_ACK	0x10
    #define	TH_URG	0x20
    #define	TH_ECE	0x40
    #define	TH_CWR	0x80
    #define	TH_FLAGS	(TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
    */

}

void TransportLayout::display_udp_header(struct udphdr *header)
{
    //添加root
    QTreeWidgetItem *root;
    QStringList str_list;

    str_list.clear();
    str_list << "UDP:";
    root = mw->add_top_item(str_list);
    root->setExpanded(true);           //这个是设置展开

    mw->add_child_to_mw(root, "源端口: ", header->uh_sport);
    mw->add_child_to_mw(root, "目的端口: ", header->uh_dport);
    mw->add_child_to_mw(root, "长度: ", header->uh_ulen);
    mw->add_child_to_mw(root, "校验和: ", header->uh_sum);
}

void TransportLayout::display_icmp_header(struct icmp6_hdr *header)
{
    //添加root
    QTreeWidgetItem *root;
    QStringList str_list;

    str_list.clear();
    str_list << "ICMP:";
    root = mw->add_top_item(str_list);
    root->setExpanded(true);           //这个是设置展开

    mw->add_child_to_mw(root, "报文类型: ", header->icmp6_type);
    mw->add_child_to_mw(root, "代码: ", header->icmp6_code);
    mw->add_child_to_mw(root, "校验和: ", header->icmp6_type);

    unsigned int icmp_dataun = header->icmp6_dataun.icmp6_un_data32[0];
    mw->add_child_to_mw(root, "type-specific field: ", icmp_dataun);

}

void TransportLayout::display_ospf_header(struct ospfhdr *header)
{
    //添加root
    QTreeWidgetItem *root;
    QStringList str_list;

    str_list.clear();
    str_list << "OSPF:";
    root = mw->add_top_item(str_list);
    root->setExpanded(true);           //这个是设置展开

    mw->add_child_to_mw(root, "版本号: ", header->o_version);
    mw->add_child_to_mw(root, "报文类型: ", header->o_mess_type);
    mw->add_child_to_mw(root, "报文总长度: ", header->o_pkt_len);
    mw->add_child_to_mw(root, "路由器ID: ", inet_ntoa(header->o_src_router));
    mw->add_child_to_mw(root, "路由器所有区域ID: ", inet_ntoa(header->o_area_id));
    mw->add_child_to_mw(root, "校验和: ", header->o_checksum);
    mw->add_child_to_mw(root, "认证类型: ", header->o_auth_type);
    mw->add_child_to_mw(root, "认证内容: ", header->o_auth_data);
}

