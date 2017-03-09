
#include <QtWidgets>

#include "statistics.h"
#include "ui_statistics.h"
#include "pieview.h"

Statistics::Statistics(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Statistics)
{
//    ui->setupUi(this);

    setupModel();
    setupViews();
//    statusBar();

    loadFile(":/icons/qtdata.cht");
    setWindowTitle(tr("Chart"));
    resize(870, 550);


}

Statistics::~Statistics()
{
    delete ui;
}


void Statistics::setupModel()
{
    model = new QStandardItemModel(8, 2, this);
    model->setHeaderData(0, Qt::Horizontal, tr("Label"));
    model->setHeaderData(1, Qt::Horizontal, tr("Quantity"));
}

void Statistics::setupViews()
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

    //setCentralWidget(splitter);
}

void Statistics::openFile()
{
    const QString fileName =
        QFileDialog::getOpenFileName(this, tr("Choose a data file"), "", "*.cht");
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void Statistics::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return;

    QTextStream stream(&file);
    QString line;

    model->removeRows(0, model->rowCount(QModelIndex()), QModelIndex());

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
                           "10");
            model->setData(model->index(row, 0, QModelIndex()),
                           "#ae4d66", Qt::DecorationRole);
            }
            row++;

        }
    } while (!line.isEmpty());

    file.close();
    //statusBar()->showMessage(tr("Loaded %1").arg(fileName), 2000);
}

void Statistics::saveFile()
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
//    statusBar()->showMessage(tr("Saved %1").arg(fileName), 2000);
}
