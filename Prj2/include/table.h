#pragma once

namespace JiDB
{

    enum BASE_TYPE { 
        BPTree, RTree, GiST
    };

    // 파일 하나에 대한 관리. 하나의 파일은 하나의 자료구조를 갖는다.
    class Table {
    public:
        Table(const char * filename, BASE_TYPE type = BPTree);
        ~Table();

        int insert(void * key, const void * value);
        void * find(void * key);
        
    private:
        // Table info
        BASE_TYPE type;
        
        int fd;

    };

}