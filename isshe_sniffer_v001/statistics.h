#ifndef STATISTICS_H
#define STATISTICS_H

#include <QDialog>



QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QAbstractItemView;
class QItemSelectionModel;
QT_END_NAMESPACE

namespace Ui {
class Statistics;
}

class Statistics : public QDialog
{
    Q_OBJECT

public:
    explicit Statistics(QWidget *parent = 0);
    ~Statistics();

private slots:
    void openFile();
    void saveFile();

private:
    Ui::Statistics *ui;

    void setupModel();
    void setupViews();
    void loadFile(const QString &path);

    QAbstractItemModel *model;
    QAbstractItemView *pieChart;
    QItemSelectionModel *selectionModel;
};

#endif // STATISTICS_H
