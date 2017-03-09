#ifndef SELECT_INTERFACE_H
#define SELECT_INTERFACE_H

#include <QDialog>

namespace Ui {
class SelectInterface;
}

class SelectInterface : public QDialog
{
    Q_OBJECT

public:
    explicit SelectInterface(QWidget *parent = 0);
    ~SelectInterface();

private slots:
    void on_treeWidget_doubleClicked(const QModelIndex &index);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::SelectInterface *ui;

public:
    int get_all_interface(void);

    //available
    QString selected_if;
};

#endif // SELECT_INTERFACE_H
