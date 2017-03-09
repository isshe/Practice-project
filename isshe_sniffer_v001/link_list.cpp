#include <pcap.h>
#include <stdlib.h>
#include <string.h>
//#include <QDebug>

#include "link_list.h"

packet_list_s *list_insert(packet_list_s **head, const u_char *packet, u_int packet_len, long long seq)
{
    packet_list_s *new_node = NULL;

    new_node->packet = (u_char *)malloc(packet_len);
    memcpy(new_node->packet, packet, packet_len);
    new_node->sequence = seq;

    //qDebug() << seq;
    if (*head == NULL)        //原来是空表
    {
        new_node->next = NULL;
        *head = new_node;
    }
    else
    {
        new_node->next = (*head)->next;
        *head = new_node;
    }

    return new_node;
}

int list_delete(packet_list_s **head, u_char *packet)
{
    //

    return 0;
}

void list_print(packet_list_s *head)
{
    packet_list_s *cur_node = head;

    while(cur_node != NULL)
    {
        //qDebug << cur_node->sequence;
//        printf("%lld", cur_node->sequence);
        cur_node = cur_node->next;
    }
}
