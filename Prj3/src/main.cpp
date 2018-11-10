#include "table.h"
#include <iostream>

using namespace JiDB;

int main() {
    Table * my_table = new Table("test.txt");
    for (int i = 0; i < 1000; i++) {
        char val[120];
        sprintf(val, "%d%d%d", i, i, i);
        my_table->_insert(i, val);
    }
    return 0;
}