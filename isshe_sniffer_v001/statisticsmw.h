#ifndef STATISTICSMW_H
#define STATISTICSMW_H

#include <QMainWindow>

#define MAX_PROTOCOL_LEN    64      //

typedef struct PROTOCOL_COUNT_S
{
    char protocol[8];       //
    int times;
    char color[8];          //
    int layout;             //这个是为了可能实现按层统计，目前只实现按协议统计

}protocol_count_s;



QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QAbstractItemView;
class QItemSelectionModel;
QT_END_NAMESPACE
namespace Ui {
class StatisticsMW;
}

class StatisticsMW : public QMainWindow
{
    Q_OBJECT

public:
    explicit StatisticsMW(QWidget *parent = 0);
    ~StatisticsMW();

    void setpiedata(int layout);

private slots:
    void openFile();
    void saveFile();

private:
    Ui::StatisticsMW *ui;

    void setupModel();
    void setupViews();
    void loadFile(const QString &path);

    QAbstractItemModel *model;
    QAbstractItemView *pieChart;
    QItemSelectionModel *selectionModel;




};

//注意这个函数不在类中
void protocol_count_init();


#endif // STATISTICSMW_H
