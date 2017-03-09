#include "protocolheader.h"
#include "ui_protocolheader.h"

ProtocolHeader::ProtocolHeader(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProtocolHeader)
{
    ui->setupUi(this);
}

ProtocolHeader::~ProtocolHeader()
{
    delete ui;
}
