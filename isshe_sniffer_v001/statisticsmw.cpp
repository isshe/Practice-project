#include <QtWidgets>
#include <QDebug>

#include "pieview.h"

#include "statisticsmw.h"
#include "ui_statisticsmw.h"

//全局变量,MAX_PROTOCOL_LEN这里可以省略，但是后面遍历不大方便，所以加上了
protocol_count_s protocol_count[MAX_PROTOCOL_LEN] =
{
    //第一层
    {"EN10MB", 0, "#030303", 1},        //0
#ifdef DLT_PFLOG
    {"PFLOG", 0, "#EE7600", 1},
#endif
    {"RAW", 0, "#B8860B", 1},
#ifdef DLT_LOOP
    {"LOOP", 0, "#BC8F8F", 1},
#endif
/*
#ifdef DLT_IEEE802_11_RADIO
    {"IEEE802_11_RADIO", 0, 1},
#endif
*/
    {"IEEE802", 0, "#B3EE3A", 1},
    {"PPP", 0, "#B22222", 1},
    {"other", 0, "#AB82FF", 1},

    //第二层
    {"IP", 0, "#A0522D", 2},                //7
    {"PUP", 0, "#9AFF9A", 2},
    {"ARP", 0, "#9F79EE", 2},
    {"RARP", 0, "#8B5A00", 2},
    {"8021Q", 0, "#8B0A50", 2},
    {"IPV6", 0, "#6495ED", 2},
    {"other", 0, "#551A8B", 2},             //13

    //第三层
    {"TCP", 0, "#B23AEE", 3},               //14
    {"UDP", 0, "#4876FF", 3},
    {"other", 0, "#0000FF", 3},             //16

    {"ICMP", 0, "#ABCEDF", 2},
    {"OSPF", 0, "#FEDCBA", 2},

    //第四层
};

StatisticsMW::StatisticsMW(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StatisticsMW)
{
    setupModel();
    setupViews();

//    loadFile(":/icons/qtdata.cht");
    setpiedata(2);

    setWindowTitle(tr("Chart"));
    resize(870, 350);

//    ui->setupUi(this);
}

StatisticsMW::~StatisticsMW()
{
    delete ui;
}




void StatisticsMW::setupModel()
{
    model = new QStandardItemModel(8, 2, this);
    model->setHeaderData(0, Qt::Horizontal, tr("Protocol"));
    model->setHeaderData(1, Qt::Horizontal, tr("Quantity"));
}

void StatisticsMW::setupViews()
{
    QSplitter *splitter = new QSplitter;
    QTableView *table = new QTableView;
    pieChart = new PieView;
    splitter->addWidget(table);
    splitter->addWidget(pieChart);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    table->setModel(model);
    pieChart->setModel(model);

    QItemSelectionModel *selectionModel = new QItemSelectionModel(model);
    table->setSelectionModel(selectionModel);
    pieChart->setSelectionModel(selectionModel);

    QHeaderView *headerView = table->horizontalHeader();
    headerView->setStretchLastSection(true);

    setCentralWidget(splitter);
}

void StatisticsMW::openFile()
{
    const QString fileName =
        QFileDialog::getOpenFileName(this, tr("Choose a data file"), "", "*.cht");
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void StatisticsMW::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return;

    QTextStream stream(&file);
    QString line;



    int row = 0;
    do {
        line = stream.readLine();
        if (!line.isEmpty()) {
            QStringList pieces = line.split(',', QString::SkipEmptyParts);
            if (pieces.value(1) != "0")
            {
            model->insertRows(row, 1, QModelIndex());

            qDebug() << "pieces.value(0) = " << pieces.value(0);

            model->setData(model->index(row, 0, QModelIndex()),
                           pieces.value(0));
            model->setData(model->index(row, 1, QModelIndex()),
                           10);
            model->setData(model->index(row, 0, QModelIndex()),
                           "#ae4d66", Qt::DecorationRole);
            }
            row++;

        }
    } while (!line.isEmpty());

    file.close();
    //statusBar()->showMessage(tr("Loaded %1").arg(fileName), 2000);
}

void StatisticsMW::setpiedata(int layout)
{
    int total = 0;
    for (int i = 0; i < MAX_PROTOCOL_LEN; i++)
    {
        printf("%s: %d\n", protocol_count[i].protocol, protocol_count[i].times);
        if (protocol_count[i].layout == layout)
        {

            total += protocol_count[i].times;
        }
    }

    model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());

    int row = 0;

    for (int i = 0; i < MAX_PROTOCOL_LEN && total != 0; i++)
    {
        if (protocol_count[i].times != 0 && protocol_count[i].layout == layout)
        {
            model->insertRows(row, 1, QModelIndex());

            model->setData(model->index(row, 0, QModelIndex()),
                           protocol_count[i].protocol);
            model->setData(model->index(row, 1, QModelIndex()),
                           protocol_count[i].times / (double)total * 100);      //%
            model->setData(model->index(row, 0, QModelIndex()),
                           protocol_count[i].color, Qt::DecorationRole);
            row++;
        }

    }
}


void StatisticsMW::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save file as"), "", "*.cht");

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return;

    QTextStream stream(&file);
    for (int row = 0; row < model->rowCount(QModelIndex()); ++row) {

        QStringList pieces;

        pieces.append(model->data(model->index(row, 0, QModelIndex()),
                                  Qt::DisplayRole).toString());
        pieces.append(model->data(model->index(row, 1, QModelIndex()),
                                  Qt::DisplayRole).toString());
        pieces.append(model->data(model->index(row, 0, QModelIndex()),
                                  Qt::DecorationRole).toString());

        stream << pieces.join(',') << "\n";
    }

    file.close();
    statusBar()->showMessage(tr("Saved %1").arg(fileName), 2000);
}


//注意这个函数不在类中
void protocol_count_init()
{
    for (int i = 0; i < MAX_PROTOCOL_LEN; i++)
    {
        protocol_count[i].times = 0;
    }
}
