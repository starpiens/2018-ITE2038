#pragma once

#include "table.h"
#include <inttypes.h>
#include <iostream>

// class BPT_Table : public Table {
// public:
//     BPT_Table(const char * filename) : Table(filename) {}
//     ~BPT_Table() {}

//     typedef int64_t key_t;

//     int insert(void * key, const void * value) {
//         if (!key) return -1;
//         key_t to_insert = *(key_t *)key;
//         std::cout << to_insert << std::endl;
//     }

// };