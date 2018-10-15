#include "api.h"
#include "bpt.h"
#include <iostream>

int main() {
    DB my_db;
    std::cout << "llll";
    BPT_Table::key_t a = 1;
    my_db.insert(&a, "a");
}