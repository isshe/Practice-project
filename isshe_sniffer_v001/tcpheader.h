#ifndef TCPHEADER_H
#define TCPHEADER_H

#include <QDialog>

namespace Ui {
class TcpHeader;
}

class TcpHeader : public QDialog
{
    Q_OBJECT

public:
    explicit TcpHeader(QWidget *parent = 0);
    ~TcpHeader();

private:
    Ui::TcpHeader *ui;
};

#endif // TCPHEADER_H
