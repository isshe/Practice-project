#include <pcap.h>
#include <stdlib.h>
#include <net/ethernet.h>
#include <QDebug>
#include <arpa/inet.h>

#include "datalinklayout.h"
#include "mainwindow.h"
#include "main.h"
#include "networklayout.h"

#include "statisticsmw.h"

extern MainWindow *mw;
extern global_variable_s global_variable;
extern protocol_count_s protocol_count[MAX_PROTOCOL_LEN];

DataLinkLayout::DataLinkLayout()
{

}

//flag:
//1: 显示概要信息
//2：显示详细信息

void DataLinkLayout::check_datalink_type(int type, const unsigned char *start_position, int flag)
{
    mw->protocol_num = QString::number(type, 10);
    if(type == DLT_EN10MB) {

        if(flag == FLAG_RESUME)
        {
            //statistics模块的
            protocol_count[0].times++;      //第0个
            mw->protocol_str = "ETHERNET II";
        }
        else if (flag == FLAG_DETAIL)
        {
            //设置显示的图片
            mw->set_pic_name(&global_variable.pic_1, EN10MB_HEADER);

            display_ethernet_data(start_position);
        }
        //网络层
        struct ether_header *ethhdr = (struct ether_header *)start_position;
        NetworkLayout nwl;
        nwl.check_network_type(htons(ethhdr->ether_type), start_position + sizeof(struct ether_header), flag);

    }
#ifdef DLT_PFLOG
    else if (type == DLT_PFLOG) {
         protocol_count[1].times++;
        //packet_handler = handle_pflog_packet;
    }
#endif
    else if(type == DLT_RAW) {
         protocol_count[2].times++;
        //packet_handler = handle_raw_packet;
    }
    else if(type == DLT_NULL) {
        //packet_handler = handle_null_packet;
    }
#ifdef DLT_LOOP
    else if(type == DLT_LOOP) {
         protocol_count[3].times++;
        //packet_handler = handle_null_packet;
    }
#endif
#ifdef DLT_IEEE802_11_RADIO
    else if(type == DLT_IEEE802_11_RADIO) {
        //packet_handler = handle_radiotap_packet;
    }
#endif
    else if(type == DLT_IEEE802) {
        protocol_count[4].times++;
        //packet_handler = handle_tokenring_packet;
    }
    else if(type == DLT_PPP) {
        protocol_count[5].times++;
        //packet_handler = handle_ppp_packet;
    }
    /*
     * SLL support not available in older libpcaps
     */
#ifdef DLT_LINUX_SLL
    else if(type == DLT_LINUX_SLL) {
        //packet_handler = handle_cooked_packet;
    }
#endif
    else {                                                  //这部分考虑要不要，会直接退出！
        protocol_count[6].times++;
        fprintf(stderr, "Unsupported datalink type: %d\n"
                "Please email i.sshe@outlook.com, quoting the datalink type and what you were\n"
                "trying to do at the time\n.", type);
        exit(1);
    }

}


void DataLinkLayout::display_ethernet_data(const unsigned char *position)
{
    struct ether_header ethhdr;

    //添加root
    QTreeWidgetItem *datalink_root;
    QStringList str_list;
    str_list.clear();
    str_list << "Ethernet:";
    datalink_root = mw->add_top_item(str_list);

    datalink_root->setExpanded(true);           //这个是设置展开
    //datalink_root->setCheckState(0, Qt::Checked); //这个是打勾

    memcpy(&ethhdr, position, sizeof(struct ether_header));

    //添加child
    //src_hw_addr
    char str[HW_STR_LEN];
    memset(str, '\0', HW_STR_LEN);
    snprintf(str, HW_STR_LEN, "source hardware address: %x:%x:%x:%x:%x:%x",
             ethhdr.ether_shost[0],
             ethhdr.ether_shost[1],
             ethhdr.ether_shost[2],
             ethhdr.ether_shost[3],
             ethhdr.ether_shost[4],
             ethhdr.ether_shost[5]);
    QByteArray byte(str);

    QString qstr = QString(byte);
    str_list.clear();
    str_list << qstr;
    mw->add_child(datalink_root, str_list);

    //dst_hw_addr
    memset(str, '\0', HW_STR_LEN);
    snprintf(str, HW_STR_LEN, "destination hardware address: %x:%x:%x:%x:%x:%x",
             ethhdr.ether_dhost[0],
             ethhdr.ether_dhost[1],
             ethhdr.ether_dhost[2],
             ethhdr.ether_dhost[3],
             ethhdr.ether_dhost[4],
             ethhdr.ether_dhost[5]);
    byte = QByteArray(str);

    qstr = QString(byte);
    str_list.clear();
    str_list << qstr;
    mw->add_child(datalink_root, str_list);

    //protocol
    str_list.clear();
    global_variable.nwlayout_type = ntohs(ethhdr.ether_type);       //设置了类型!!!
    qstr = "protocol: (0x" + QString::number(global_variable.nwlayout_type, 16) + ")";
    str_list << qstr;
    mw->add_child(datalink_root, str_list);

    //qDebug() << "g mwlayout_type = " << global_variable.nwlayout_type;
}
