#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "main.h"

#define HASH_TABLE_LEN 2017 //997

packet_s *create_hash();

packet_s *find_hash(packet_s *hash, long long seq);


void free_hash(packet_s *hash);

int insert_hash(packet_s *hash, packet_s *node);



#endif // HASHTABLE_H
