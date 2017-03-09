//百度看如何做扇形图/条形图

#include <QApplication>
#include <stdio.h>
#include <pcap.h>
#include <pthread.h>
#include <QDebug>
#include <net/ethernet.h>
#include <netinet/tcp.h>
#include <string.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <QString>
#include <net/if_arp.h>

#include "main.h"
#include "capturethread.h"
#include "link_list.h"
#include "mainwindow.h"
#include "vlan.h"
#include "hashtable.h"
#include "datalinklayout.h"

global_variable_s global_variable;
MainWindow *mw;
options_s options;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    init_global_variable_s();       //初始化
    init_options(&options);

    MainWindow w;
    //w.show();
    mw = &w;
    mw->show();

    qDebug() << w.selected_if;

    if (w.selected_if != "")
    {

    }
    else
    {
        //
        exit(-1);
    }

    return a.exec();
}

int open_interface(const char *interface)
{
    char errbuf[PCAP_ERRBUF_SIZE];

    qDebug() << "ch: interface = " << interface;
    global_variable.pd = pcap_open_live(interface, 65535, 1, 0, errbuf);
    // DEBUG: pd = pcap_open_offline("tcpdump.out", errbuf);
    if(global_variable.pd == NULL) {
        qDebug() << "open interface error:" << errbuf;
        exit(1);
    }

    int dlt = pcap_datalink(global_variable.pd);

    return dlt;
}

//
void *packet_loop(void *arg)
{
    qDebug() << "packet_loop";
    pcap_loop(global_variable.pd, 0, deal_with_packet, NULL);
    return NULL;
}


//
void deal_with_packet(u_char *user_arg,
                      const struct pcap_pkthdr *pkthdr,
                      const u_char *packet)
{

    //2017.02.23
    if(options.start_pause == false)
    {
        return;
    }

    packet_s node;

    pthread_mutex_lock(&global_variable.list_mutex);

    global_variable.packet_sequence++;
    node.sequence = global_variable.packet_sequence;
    node.pkthdr = (struct pcap_pkthdr *)pkthdr;
    node.packet = (u_char*)packet;
    insert_hash(global_variable.packets, &node);

    pthread_mutex_unlock(&(global_variable.list_mutex));

    mw->init_resume_info();
    mw->sequence = QString::number(global_variable.packet_sequence, 10);

    mw->pkt_len = QString::number(pkthdr->caplen, 10);
    mw->cap_time = get_time_str(&pkthdr->ts, &global_variable.start_time);
    DataLinkLayout dll;
    dll.check_datalink_type(global_variable.dlt, packet, FLAG_RESUME);
//    set_base_str(pkthdr, packet);
    mw->add_treewidget_line();

    //sleep(1);
}

void init_global_variable_s()
{

    global_variable.packets = create_hash();                //在这里新建hash table
    global_variable.pcap_loop_tid = 0;
    global_variable.pd = NULL;
    global_variable.dlt = -1;
    global_variable.nwlayout_type = -1;
    global_variable.tslayout_type = -1;
    pthread_mutex_init(&global_variable.list_mutex, NULL);   //初始化锁
    global_variable.packet_sequence = 0;

    global_variable.pic_1 = NULL;
    global_variable.pic_2 = NULL;
    global_variable.pic_3 = NULL;
    global_variable.pic_4 = NULL;
    global_variable.pic_5 = NULL;
    global_variable.pic_6 = NULL;
    global_variable.pic_7 = NULL;
}

QString get_time_str(const struct timeval *cur_time, struct timeval *start_time)
{
    char time_str[32];
    struct timeval res_tv;
    struct timeval *temp_cur = (struct timeval *)cur_time;

//    qDebug() << "cur_time.tv_sec = " << cur_time->tv_sec;
//    qDebug() << "cur_time.tv_sec = " << cur_time->tv_usec;

    if (start_time->tv_sec == 0 && start_time->tv_usec == 0)          //把第一个包作为时间基准
    {
        start_time->tv_sec = temp_cur->tv_sec;
        start_time->tv_usec = temp_cur->tv_usec;
    }

    if (temp_cur->tv_usec - start_time->tv_usec < 0)
    {
        temp_cur->tv_sec -= 1;
        res_tv.tv_usec = temp_cur->tv_usec - start_time->tv_usec + 1000000;
    }
    else
    {
        res_tv.tv_usec = temp_cur->tv_usec - start_time->tv_usec;
    }

    res_tv.tv_sec = temp_cur->tv_sec - start_time->tv_sec;
    memset(time_str, '\0', sizeof(time_str));
    snprintf(time_str, sizeof(time_str), "%ld.%ld", res_tv.tv_sec, res_tv.tv_usec);

    QString qstr = QString(time_str);

    return qstr;
}


//初始化选项
void init_options(options_s *options)
{
    options->start_pause = true;          //def:start
}
