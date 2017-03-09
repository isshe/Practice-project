#include <QDebug>
#include <QStringList>
#include <QAction>
#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "select_interface.h"
#include "capturethread.h"
#include "main.h"
#include "datalinklayout.h"
#include "networklayout.h"
#include "transportlayout.h"
#include "protocolheader.h"

#include "hashtable.h"

#include "statistics.h"
#include "statisticsmw.h"

extern global_variable_s global_variable;
extern options_s options;
extern protocol_count_s protocol_count[MAX_PROTOCOL_LEN];

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(tr("isshe filter"));

    //    QMenu *menu = menuBar()->addMenu(tr("Select IF"));        //添加到菜单栏
    //    menu->addAction(select_if_action);

    select_if_action = new QAction(QIcon(":/icons/select.png"), tr("Select interface"), this);
    select_if_action->setStatusTip(tr("Select Network Interface"));
    connect(select_if_action, &QAction::triggered, this, &MainWindow::select_interface);

    start_action = new QAction(QIcon(":/icons/start.png"), tr("Start Capture"), this);
    start_action->setStatusTip(tr("Start packet Capture"));
    connect(start_action, &QAction::triggered, this, &MainWindow::start);

    pause_action = new QAction(QIcon(":/icons/pause.png"), tr("Pause Capture"), this);
    pause_action->setStatusTip(tr("Pause packet Capture"));
    connect(pause_action, &QAction::triggered, this, &MainWindow::pause);

    restart_action = new QAction(QIcon(":/icons/restart.png"), tr("restart Capture"), this);
    restart_action->setStatusTip(tr("restart packet Capture"));
    connect(restart_action, &QAction::triggered, this, &MainWindow::restart);

    saveas_action = new QAction(QIcon(":/icons/saveas.png"), tr("Save file"), this);
    saveas_action->setStatusTip(tr("Save file"));
    connect(saveas_action, &QAction::triggered, this, &MainWindow::save_file);

    openfile_action = new QAction(QIcon(":/icons/openfile.png"), tr("Open file"), this);
    openfile_action->setStatusTip(tr("Open file"));
    connect(openfile_action, &QAction::triggered, this, &MainWindow::open_file);

    statistics_action = new QAction(QIcon(":/icons/statistics.png"), tr("Open file"), this);
    statistics_action->setStatusTip(tr("Open file"));
    connect(statistics_action, &QAction::triggered, this, &MainWindow::statistics);

    //QToolBar *tool_bar = addToolBar(tr("ToolBar"));
    ui->mainToolBar->addAction(select_if_action);
    ui->mainToolBar->addAction(start_action);
    ui->mainToolBar->addAction(pause_action);
    ui->mainToolBar->addAction(restart_action);
    ui->mainToolBar->addAction(saveas_action);
    ui->mainToolBar->addAction(openfile_action);
    ui->mainToolBar->addAction(statistics_action);


    statusBar();

    select_interface();
}

MainWindow::~MainWindow()
{
    delete ui;
}


/**
 * 更新的方向：
 * 1. 做一个相对更集成的初始化
 * 2.
 */
void MainWindow::select_interface()
{
    SelectInterface si;
    int ret = si.exec();

    if (ret == QDialog::Accepted)        //确定
    {
        //先关线程
        if (global_variable.pcap_loop_tid != 0)
        {
            pthread_cancel(global_variable.pcap_loop_tid);
            global_variable.pcap_loop_tid = 0;  //
        }

        selected_if = si.selected_if;
        qDebug() << "selected if = " << selected_if;
        char *str;
        QByteArray ba = si.selected_if.toLatin1();
        str = ba.data();
        global_variable.dlt = open_interface(str);

        ui->treeWidget->clear();
        ui->treeWidget_2->clear();

        pthread_mutex_lock(&global_variable.list_mutex);
        protocol_count_init();                             //这里是statistics模块的
        if (global_variable.packets != NULL)
        {
            qDebug() << "free_hash";
            free_hash(global_variable.packets);             //
//            global_variable.packets = NULL;
        }
//        global_variable.packets = create_hash();

        global_variable.packet_sequence = 0;
        global_variable.start_time.tv_sec = 0;
        global_variable.start_time.tv_usec = 0;
//        gettimeofday(&(global_variable.start_time), NULL);

        pthread_mutex_unlock(&global_variable.list_mutex);

        pthread_create(&(global_variable.pcap_loop_tid), NULL, packet_loop, NULL);
        qDebug() << "behind pthread create";
    }
    else
    {
        qDebug() << ret;
        selected_if = "";
        qDebug() << "selected if = " << selected_if;
        //取消
    }
}


void MainWindow::init_resume_info()
{
    sequence = "";
    src_addr = "";
    dst_addr = "";
    protocol_num = "";
    protocol_str = "";
    pkt_len = "";
    cap_time = "";
}

//点击的时候，从链表中获取相应的包解析，显示到
void MainWindow::on_treeWidget_clicked(const QModelIndex &index)
{
    qDebug() << index;

    //QTreeWidgetItem *root = new QTreeWidgetItem(ui->treeWidget, str_list);
    QString seq_str = ui->treeWidget->currentItem()->text(0);     //
    bool ok;
    long long seq = seq_str.toLongLong(&ok, 10);

    packet_s *cur_node = find_hash(global_variable.packets, seq);
    if( cur_node != NULL)
    {
        qDebug() << "cur_node != NULL";
        ui->treeWidget_2->clear();

        //数据链路层
        DataLinkLayout dll;
        dll.check_datalink_type(global_variable.dlt, cur_node->packet, FLAG_DETAIL);
        //这里这样写有问题，应该把上层嵌入到下层中
        //网络层
        //NetworkLayout nwl;
        //qDebug() << "nwlayout_type =" << global_variable.nwlayout_type;
        //nwl.check_network_type(global_variable.nwlayout_type, cur_node);

        //传输层
        //TransportLayout tsl;
        //tsl.check_transport_type(global_variable.tslayout_type, cur_node);

        //应用层
    }

}

void MainWindow::set_selected_if(QString interface)
{
    selected_if = interface;
}


void MainWindow::on_lineEdit_editingFinished()
{
    filter_rule = ui->lineEdit->text();
    qDebug() << filter_rule;
}


int MainWindow::add_treewidget_line()
{
    QStringList str_list;
    QString temp_p = protocol_str + "(" + protocol_num + ")";
    str_list << sequence << cap_time << src_addr << dst_addr << temp_p << pkt_len;

    QTreeWidgetItem *item = new QTreeWidgetItem(str_list);
    ui->treeWidget->addTopLevelItem(item);

    return 0;
}

QTreeWidgetItem * MainWindow::add_top_item(QStringList str_list)
{
    QTreeWidgetItem * item=new QTreeWidgetItem(str_list);
    ui->treeWidget_2->addTopLevelItem(item);
    return item;
}

QTreeWidgetItem * MainWindow::add_child(QTreeWidgetItem *parent, QStringList &str_list)
{
    QTreeWidgetItem * item=new QTreeWidgetItem(str_list);
    parent->addChild(item);
    return item;

}

void MainWindow::add_child_to_mw(QTreeWidgetItem *root, QString name , unsigned char value)
{
    QString qstr;
    QStringList str_list;
    char str[STR_LEN] = {0};

    memset(str, '\0', STR_LEN);
    snprintf(str, STR_LEN, "%u(0x%x)", value, value);

    qstr = name + str;


    str_list << qstr;
    add_child(root, str_list);
}

void MainWindow::add_child_to_mw(QTreeWidgetItem *root, QString name, unsigned short value)
{
    QString qstr;
    QStringList str_list;
    char str[STR_LEN] = {0};

    memset(str, '\0', STR_LEN);
    snprintf(str, STR_LEN, "%u(0x%x)", value, value);

    qstr = name + str;

    str_list << qstr;
    add_child(root, str_list);
}

void MainWindow::add_child_to_mw(QTreeWidgetItem *root, QString name, unsigned int value)
{
    QString qstr;
    QStringList str_list;
    char str[STR_LEN] = {0};

    memset(str, '\0', STR_LEN);
    snprintf(str, STR_LEN, "%u(0x%x)", value, value);

    qstr = name + str;

    str_list << qstr;
    add_child(root, str_list);
}

void MainWindow::add_child_to_mw(QTreeWidgetItem *root, QString name, unsigned long value)
{
    QString qstr;
    QStringList str_list;
    char str[STR_LEN] = {0};

    memset(str, '\0', STR_LEN);
    snprintf(str, STR_LEN, "%lu(0x%lx)", value, value);

    qstr = name + str;

    str_list << qstr;
    add_child(root, str_list);
}

void MainWindow::add_child_to_mw(QTreeWidgetItem *root, QString name, unsigned long long value)
{
    QString qstr;
    QStringList str_list;
    char str[STR_LEN] = {0};

    memset(str, '\0', STR_LEN);
    snprintf(str, STR_LEN, "%llu(0x%llx)", value, value);

    qstr = name + str;

    str_list << qstr;
    add_child(root, str_list);
}

void MainWindow::add_child_to_mw(QTreeWidgetItem *root, QString name, char *value)
{
    QString qstr;
    QStringList str_list;
    qstr = name + value;

    str_list << qstr;
    add_child(root, str_list);
}

void MainWindow::show_protocol(QString *filename)
{
    if (filename == NULL)       //这里应该可以改成别的更好的写法
    {
        return;
    }
    ProtocolHeader *ph = new ProtocolHeader;
    ph->setWindowTitle(*filename);

//    QString *filename = global_variable.pic_1;//":/2_layout/ip_header.png";
    QPixmap pixmap(*filename);
    QLabel *label = new QLabel(ph);
    label->setScaledContents(true);
    label->setPixmap(pixmap);

    //tree widget
    QTreeWidget *treeWidget = new QTreeWidget(ph);
    //treeWidget->setFrameStyle(QFrame::NoFrame);             //不要边框
    treeWidget->setHeaderHidden(true);
    treeWidget->resize(1000, 1000);
    show_pro_info_treewidget(treeWidget, filename);         //

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addWidget(treeWidget);


    ph->setLayout(layout);

    ph->show();
}


void MainWindow::on_pushButton_clicked()
{
    show_protocol(global_variable.pic_1);
}

void MainWindow::on_pushButton_2_clicked()
{
    show_protocol(global_variable.pic_2);
}

void MainWindow::on_pushButton_3_clicked()
{
    show_protocol(global_variable.pic_3);
}

void MainWindow::on_pushButton_4_clicked()
{
    show_protocol(global_variable.pic_4);
}

void MainWindow::on_pushButton_5_clicked()
{
    show_protocol(global_variable.pic_5);
}

void MainWindow::on_pushButton_6_clicked()
{
    show_protocol(global_variable.pic_6);
}

void MainWindow::on_pushButton_7_clicked()
{
    show_protocol(global_variable.pic_7);
}


void MainWindow::set_pic_name(QString **pic, QString filename)
{
    if (*pic != NULL)
    {
        delete(*pic);
        *pic = NULL;
    }
    *pic = new QString(filename);
}


//2017.02.23
//开始或暂停捕获, 这里如果有需要，要加锁
void MainWindow::start()
{
    options.start_pause = true;
}


void MainWindow::pause()
{
    options.start_pause = false;
}

void MainWindow::restart()
{
    pthread_mutex_lock(&global_variable.list_mutex);
    ui->treeWidget->clear();
    ui->treeWidget_2->clear();
    global_variable.packet_sequence = 0;
    global_variable.start_time.tv_sec = 0;
    global_variable.start_time.tv_usec = 0;
    protocol_count_init();                              //
//    gettimeofday(&(global_variable.start_time), NULL);
    if (global_variable.packets != NULL)
    {
        qDebug() << "free_hash";
        free_hash(global_variable.packets);             //
    }
    pthread_mutex_unlock(&global_variable.list_mutex);
}

/**
 * @brief MainWindow::save_file
 * @param hash:hash table
 * 方法一：
 * 1. 遍历hash表去写，缺点：顺序乱了。
 *
 * 方法二：（使用）
 * 1. 按照序号，一个一个查找后再写。有序。缺点：速度慢。
 */

void MainWindow::write_file(packet_s *hash, const char *filename)
{

    long long seq = global_variable.packet_sequence;
    pcap_t *handler = global_variable.pd;
    packet_s *cur_pkt = NULL;


    //打开文件
    pcap_dumper_t *dumpfp = pcap_dump_open(handler, filename);

    for (long long i = 1; i <= seq; i++)
    {
        if ((cur_pkt = find_hash(hash, i)) != NULL)
        {
            pcap_dump((u_char *)dumpfp, cur_pkt->pkthdr, cur_pkt->packet);
        }
    }


    pcap_dump_close(dumpfp);
}

void MainWindow::save_file()
{
    QString filename = QFileDialog::getSaveFileName(this,
        tr("Save File"),
        "",
        tr("*.pcap")); //选择路径

    if (filename == "")
    {
        return ;
    }
//    filename = filename + ".pcap";      //

    QByteArray ba = filename.toLatin1();
    char *str_filename = ba.data();

    write_file(global_variable.packets, str_filename);

}

void MainWindow::open_file()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open File"),
        "",
        tr("*.pcap")); //选择路径

    if (filename == "")
    {
        return ;
    }

    QByteArray ba = filename.toLatin1();
    char *str_filename = ba.data();

    char error[MAX_FILENAME_LEN];
    pcap_t* pd = global_variable.pd;

    qDebug() << "open file name = " << str_filename;
    pause();            //暂停、清空信息
    restart();          //
    start();


    if (global_variable.pcap_loop_tid != 0)
    {
        pthread_cancel(global_variable.pcap_loop_tid);
        global_variable.pcap_loop_tid = 0;  //
    }

    if((global_variable.pd = pcap_open_offline(str_filename,error)) == NULL)  //打开文件
    {
        printf("%s\n",error);
        global_variable.pd = pd;            //
    }

    pthread_create(&(global_variable.pcap_loop_tid), NULL, packet_loop, NULL);

//    packet_loop(NULL);
}

void MainWindow::statistics()
{
    StatisticsMW *st = new StatisticsMW();
    //Statistics *st = new Statistics();
    //st->exec();
    st->show();
}

void MainWindow::show_en10mb_header(QTreeWidget *treeWidget)
{
    QStringList str_list;
    str_list << "Ethernet II:";
    QTreeWidgetItem * item=new QTreeWidgetItem(str_list);
    treeWidget->addTopLevelItem(item);
    item->setExpanded(true);

    str_list.clear();
    str_list << "destination ether addr（目的硬件地址）：";
    add_child(item, str_list);

    str_list.clear();
    str_list << "source ether addr（源硬件地址）：";
    add_child(item, str_list);

    str_list.clear();
    str_list << "protocol（协议）：";
    QTreeWidgetItem * protocol = add_child(item, str_list);
    protocol->setExpanded(true);

    str_list.clear();
    str_list << "ETHERTYPE_PUP		0x0200: Xerox PUP";
    add_child(protocol, str_list);

    str_list.clear();
    str_list << "ETHERTYPE_SPRITE		0x0500: Sprite";
    add_child(protocol, str_list);

    str_list.clear();
    str_list << "ETHERTYPE_IP		0x0800: IP";
    add_child(protocol, str_list);

    str_list.clear();
    str_list << "ETHERTYPE_ARP		0x0806: Address resolution";
    add_child(protocol, str_list);

    str_list.clear();
    str_list << "ETHERTYPE_REVARP		0x8035: Reverse ARP";
    add_child(protocol, str_list);

    str_list.clear();
    str_list << "ETHERTYPE_AT		0x809B: AppleTalk protocol";
    add_child(protocol, str_list);

    str_list.clear();
    str_list << "ETHERTYPE_AARP		0x80F3: AppleTalk ARP";
    add_child(protocol, str_list);

    str_list.clear();
    str_list << "ETHERTYPE_VLAN		0x8100: IEEE 802.1Q VLAN tagging";
    add_child(protocol, str_list);

    str_list.clear();
    str_list << "ETHERTYPE_IPX		0x8137: IPX";
    add_child(protocol, str_list);

    str_list.clear();
    str_list << "ETHERTYPE_IPV6		0x86dd: IP protocol version 6";
    add_child(protocol, str_list);

    str_list.clear();
    str_list << "ETHERTYPE_LOOPBACK		0x9000: used to test interfaces";
    add_child(protocol, str_list);
    //delete(item);
}

void MainWindow::show_ppp_header(QTreeWidget *treeWidget)
{

}

void MainWindow::show_ip_header(QTreeWidget *treeWidget)
{

}

void MainWindow::show_arp_header(QTreeWidget *treeWidget)
{

}

void MainWindow::show_icmp(QTreeWidget *treeWidget)
{

}

void MainWindow::show_tcp_header(QTreeWidget *treeWidget)
{

}

void MainWindow::show_udp_header(QTreeWidget *treeWidget)
{

}

void MainWindow::show_pro_info_treewidget(QTreeWidget *treeWidget, QString *filename)
{
    if (filename == QString(QLatin1String(EN10MB_HEADER)))
    {
        show_en10mb_header(treeWidget);
    }
    else if (filename == QString(QLatin1String(PPP_HEADER)))
    {
        show_ppp_header(treeWidget);
    }
    else if (filename == QString(QLatin1String(IP_HEADER)))
    {
        show_ip_header(treeWidget);
    }
    else if (filename == QString(QLatin1String(ARP_HEADER)))
    {
        show_arp_header(treeWidget);
    }
    else if (filename == QString(QLatin1String(ICMP_REQ_REP)))
    {
        show_icmp(treeWidget);
    }
    else if (filename == QString(QLatin1String(TCP_HEADER)))
    {
        show_tcp_header(treeWidget);
    }
    else if (filename == QString(QLatin1String(UDP_HEADER)))
    {
        show_udp_header(treeWidget);
    }
}
