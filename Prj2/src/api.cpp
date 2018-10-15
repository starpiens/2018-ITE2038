#include "api.h"

namespace JiDB
{

    DB::DB(std::string name) {
        
    }

    DB::~DB() {
        //
    }

    // 파일이 존재하면, 헤더 파일을 불러온다.
    // 존재하지 않으면 헤더 파일을 만든다. 
    int DB::open_table(char * path, BASE_TYPE type = BPTree) {
        
        v_table.push_back(new Table(path, BPTree));
    }

    void DB::insert(void * key, const void * value) {
        v_table[0]->insert(key, value);
    }


}