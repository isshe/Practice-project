#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QAction>
#include <pthread.h>
#include <pcap.h>
#include <QString>


#include "main.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTreeWidget *tree;

private slots:
    void on_treeWidget_clicked(const QModelIndex &index);

    void on_lineEdit_editingFinished();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::MainWindow *ui;

public:

    /*available*/
    //action
    QAction *select_if_action;
    QAction *start_action;
    QAction *pause_action;
    QAction *restart_action;
    QAction *saveas_action;
    QAction *openfile_action;
    QAction *statistics_action;

    //保存已经保存的接口
    QString selected_if;

    //过滤规则
    QString filter_rule;

    //抓包线程
    pcap_handler packet_handler;

    //display-左边
    QString sequence;
    QString src_addr;
    QString dst_addr;
    QString protocol_num;
    QString protocol_str;
    QString pkt_len;
    QString cap_time;

    /*function*/
    //选择接口
    void select_interface();
    //设置已经选择的接口
    void set_selected_if(QString interface);

    //
    int add_treewidget_line(void);
    //display-右边
    //数据链路层
    QString display_datalink_layout();

    //网络层
    QString display_network_layout();

    //传输层
    QString display_transport_layout();

    //应用层
    QString display_applicatio_layout();

    //数据

    QTreeWidgetItem * add_top_item(QStringList str_list);
    QTreeWidgetItem * add_child(QTreeWidgetItem *parent, QStringList &str_list);

    void add_child_to_mw(QTreeWidgetItem *root, QString name, unsigned char value);

    void add_child_to_mw(QTreeWidgetItem *root, QString name, unsigned short value);

    void add_child_to_mw(QTreeWidgetItem *root, QString name, unsigned int value);

    void add_child_to_mw(QTreeWidgetItem *root, QString name, unsigned long value);

    void add_child_to_mw(QTreeWidgetItem *root, QString name, unsigned long long value);

    void add_child_to_mw(QTreeWidgetItem *root, QString name, char *value);

    void show_protocol(QString *filename);

    void set_pic_name(QString **pic, QString filename);

    //20170223
    void start();
    void pause();
    void restart();

    void write_file(packet_s *hash, const char *filename);

    void save_file();

    void open_file();

    void statistics();

    void init_resume_info();

    void show_pro_info_treewidget(QTreeWidget *treeWidget, QString *filename);

    void show_en10mb_header(QTreeWidget *treeWidget);
    void show_ppp_header(QTreeWidget *treeWidget);
    void show_ip_header(QTreeWidget *treeWidget);
    void show_arp_header(QTreeWidget *treeWidget);
    void show_icmp(QTreeWidget *treeWidget);
    void show_tcp_header(QTreeWidget *treeWidget);
    void show_udp_header(QTreeWidget *treeWidget);
};

#endif // MAINWINDOW_H
