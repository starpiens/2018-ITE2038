#pragma once

#include <inttypes.h>

int open_table(char * pathname, int num_column);
explicit int insert(int table_id, int64_t key, char * value);
int remove(int table_id, int64_t key);
int64_t * find(int table_id, int64_t key);

