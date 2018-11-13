#pragma once

#include <inttypes.h>


using key_t     = int64_t;
using val_t     = int64_t;
using tableid_t = int;


int     init_db    (int buf_num);
int     shutdown_db(void);

int     open_table (const char * pathname, int num_column);
int     close_table(tableid_t table_id);

int     insert     (int table_id, key_t key, val_t * values);
int     remove     (int table_id, key_t key);
key_t * find       (int table_id, key_t key);

key_t   join       (const char * query);