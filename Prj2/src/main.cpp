#include "table.h"
#include <iostream>

int main() {
    JiDB::Table my_table("test.txt");
    JiDB::Table::key_t a = 1;
    my_db.insert(&a, "a");
    return 0;
}