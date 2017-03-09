#ifndef PROTOCOLHEADER_H
#define PROTOCOLHEADER_H

#include <QDialog>

namespace Ui {
class ProtocolHeader;
}

class ProtocolHeader : public QDialog
{
    Q_OBJECT

public:
    explicit ProtocolHeader(QWidget *parent = 0);
    ~ProtocolHeader();

private:
    Ui::ProtocolHeader *ui;
};

#endif // PROTOCOLHEADER_H
