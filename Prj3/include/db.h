#pragma once

#include "bpt.h"
#include <vector>
#include <string>

namespace JiDB 
{

    class DB {
    public:
        DB(std::string name);
        ~DB();
        
        int open_table(char * path, BASE_TYPE type);
        int insert(void * key, const void * value);
        
    private:
        std::vector<class Table *> v_table;
    };

}