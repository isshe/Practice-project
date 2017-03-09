#include <QDebug>
#include <pcap.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string>

#include "select_interface.h"
#include "ui_select_interface.h"

SelectInterface::SelectInterface(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectInterface)
{

    ui->setupUi(this);
    get_all_interface();
    //connect(SelectInterface::on_treeWidget_doubleClicked(),)

}

SelectInterface::~SelectInterface()
{
    delete ui;
}

void SelectInterface::on_treeWidget_doubleClicked(const QModelIndex &index)
{
    qDebug() << index;
    qDebug() << ui->treeWidget->currentItem()->text(0);
    selected_if = ui->treeWidget->currentItem()->text(0);
    QDialog::accept();
}

int SelectInterface::get_all_interface(void)
{
    char errbuf[PCAP_ERRBUF_SIZE] = {0};
    char port_str[6] = {0};
    pcap_if_t   *it;
    int ret = 0;
    struct sockaddr_in sin;
    struct pcap_addr * pa;

    if ((ret = pcap_findalldevs(&it, errbuf)) == -1)
    {
        printf("error: %s\n", errbuf);
        exit(-1);
    }

    while(it != NULL)
    {
        printf("name = %s\n", it->name);
        printf("description = %s\n", it->description);
        printf("flags = %u\n", it->flags);
        pa = it->addresses;

        while(pa != NULL)
        {
            memcpy(&sin, pa->addr, sizeof(sin));
            printf("addr = %s\n", inet_ntoa(sin.sin_addr));
            printf("port = %d\n", sin.sin_port);
            if (pa->next != NULL && pa->next->next != NULL)
                pa = pa->next;
            else
                break;
        }

        memset(port_str, 0, sizeof(port_str));
        sprintf(port_str, "%d", sin.sin_port);
        QStringList str_list;
        str_list << it->name << inet_ntoa(sin.sin_addr) << port_str;

        QTreeWidgetItem *item = new QTreeWidgetItem(str_list);
        ui->treeWidget->addTopLevelItem(item);

        it = it->next;
    }
    return 0;
}

void SelectInterface::on_buttonBox_accepted()
{
    selected_if = ui->treeWidget->currentItem()->text(0);       //可能段错误
    qDebug() << selected_if;
}

void SelectInterface::on_buttonBox_rejected()
{
    selected_if = "";
    qDebug() << selected_if;
}
