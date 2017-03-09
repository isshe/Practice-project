#include <QDebug>

#include "hashtable.h"

packet_s *create_hash()
{
    packet_s *hash = (packet_s *)malloc(sizeof(packet_s) * HASH_TABLE_LEN);
    if (hash == NULL)
    {
        return hash;
    }

    //初始化
    memset(hash, '\0', sizeof(packet_s) * HASH_TABLE_LEN);

    return hash;
}

void free_hash(packet_s *hash)
{
    if (hash == NULL)
    {
        return;
    }
    packet_s *cur_node = NULL;
    packet_s *tmp_node = NULL;

    for (int i = 0; i < HASH_TABLE_LEN; i++)
    {
        cur_node = hash[i].next;
        hash[i].next = NULL;            //
        while(cur_node != NULL)
        {
            tmp_node = cur_node->next;
            if (cur_node->pkthdr != NULL)
            {
                free(cur_node->pkthdr);
            }
            if (cur_node->packet != NULL)
            {
                free(cur_node->packet);
            }
            free(cur_node);
            cur_node = tmp_node;
        }

    }

//    free(hash);           //这个先不free。
}


packet_s *find_hash(packet_s *hash, long long seq)
{
    qDebug() << "find_hash";
    if (hash == NULL)
    {
        return NULL;
    }
    int key = seq % HASH_TABLE_LEN;
    packet_s *res = NULL;

    packet_s *cur_node = hash[key].next;
    while(cur_node != NULL)
    {
        if (cur_node->sequence == seq)
        {
            res = cur_node;
            break;
        }

        cur_node = cur_node->next;
    }

    return res;
}

int insert_hash(packet_s *hash, packet_s *node)
{
    if (hash == NULL || node == NULL)
    {
        return -1;
    }

    struct pcap_pkthdr *pkthdr = (struct pcap_pkthdr *)malloc(sizeof(struct pcap_pkthdr));                             
    if (pkthdr == NULL)
    {
        return -1;
    }

    u_char *packet = (u_char *)malloc(node->pkthdr->caplen);//注意这里会不会段错误
    if (packet == NULL)
    {
        return -1;
    }

    packet_s *new_node = (packet_s *)malloc(sizeof(packet_s));
    if (new_node == NULL)
    {
        return -1;
    }

    memcpy(pkthdr, node->pkthdr, sizeof(struct pcap_pkthdr));
    memcpy(packet, node->packet, node->pkthdr->caplen);

    new_node->sequence = node->sequence;
    new_node->pkthdr = pkthdr;
    new_node->packet = packet;

    //照理说应该不需要查找再插入, 保留
    /*
    if(!find_hash(hash, new_node->sequence))
    */

    int key = new_node->sequence % HASH_TABLE_LEN;

    new_node->next = hash[key].next;
    hash[key].next = new_node;

    return 0;
}
