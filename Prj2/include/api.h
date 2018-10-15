#pragma once

#include "bpt.h"
#include <vector>
#include <iostream>

class DB {
public:
    DB() { open_db("asdf"); }
    ~DB() {}
    
    int open_db(char * path) {
        v_table.push_back(new BPT_Table(path));
    }
    void insert(void * key, const void * value) {
        std::cout << "asdf";
        v_table[0]->insert(key, value);
    }

private:
    std::vector<class Table *> v_table;
};