#ifndef MAIN_H
#define MAIN_H

#include <pthread.h>
#include <pcap.h>
#include <QString>

//#include "mainwindow.h"

/*
//datelink layout
QString EN10MB_HEADER = ":/header/Ethernet_II.png";
QString PPP_HEADER = ":/header/ppp.png";

//network layout
QString IP_HEADER = ":/header/ip.png";
QString ARP_HEADER = ":/header/arp.png";
QString ICMP_REQ_REP = ":/header/icmp_req_reply.png";
QString ICMP_ERROR = ":/header/icmp_error.png";
QString ICMP_RE = ":/header/icmp_re.png";

//transport layout
QString TCP_HEADER = ":/header/tcp.png";
QString UDP_HEADER = ":/header/udp.png";
*/


//datelink layout
#define EN10MB_HEADER ":/header/Ethernet_II.png"
#define PPP_HEADER ":/header/ppp.png"

//network layout
#define IP_HEADER ":/header/ip.png"
#define ARP_HEADER ":/header/arp.png"
#define ICMP_REQ_REP ":/header/icmp_req_reply.png"
#define ICMP_ERROR ":/header/icmp_error.png"
#define ICMP_RE ":/header/icmp_re.png"

//transport layout
#define TCP_HEADER ":/header/tcp.png"
#define UDP_HEADER ":/header/udp.png"


//最大文件名
#define MAX_FILENAME_LEN    512


typedef struct PACKET_S_
{
    long long sequence;         //序号，也是每个packet的标识
    struct pcap_pkthdr *pkthdr;
    u_char *packet;
    PACKET_S_ *next;
}packet_s;

typedef struct GLOBAL_VARIABLE_S
{
    struct timeval start_time;              //开始抓包时间，用来计算时间
    packet_s *packets;                      //hash table
    pthread_mutex_t list_mutex;             //
    pcap_t* pd;
    pthread_t pcap_loop_tid;
    int dlt;                            //数据链路层
    unsigned short nwlayout_type;       //网络层
    unsigned char tslayout_type;        //传输层

    long long packet_sequence;

    //2017.02.15
    QString *pic_1;                     //button:对应第一层
    QString *pic_2;                     //对应第二层
    QString *pic_3;
    QString *pic_4;
    QString *pic_5;
    QString *pic_6;                     //
    QString *pic_7;                     //

} global_variable_s;


//2017 2 23
typedef struct OPTIONS_S
{
    bool start_pause;         //true: start, false: suspend
}options_s;



//2017.2.24
enum FLAG_E
{
    FLAG_RESUME = 1,
    FLAG_DETAIL = 2,
};


//function
void init_global_variable_s();

int open_interface(const char *interface);

void *packet_loop(void *arg);

void deal_with_packet(u_char *user_arg,
                      const struct pcap_pkthdr *pkthdr,
                      const u_char *packet);

QString get_time_str(const struct timeval *cur_time, struct timeval *start_time);


//2017.2.23
void init_options(options_s *options);


#endif // MAIN_H
