#ifndef DATALINKLAYOUT_H
#define DATALINKLAYOUT_H

#include <pcap.h>

#include "main.h"

#define HW_STR_LEN 64


class DataLinkLayout
{
public:
    DataLinkLayout();

    void check_datalink_type(int type, const unsigned char *position, int flag);

    void display_ethernet_data(const unsigned char *position);
};

#endif // DATALINKLAYOUT_H
