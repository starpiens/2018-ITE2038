#include "table.h"
#include <iostream>

int main() {
    JiDB::Table my_table("test.txt", JiDB::BPTree);
    return 0;
}