#pragma once

namespace JiDB
{

    enum BASE_TYPE { 
        BPTree, RTree, GiST
    };
    
    // Index manager class.
    class IndexMgr {
    public:
        IndexMgr();
        ~IndexMgr();
        virtual int    _insert(void * key, const void * value) = 0;
        virtual void * _find  (void * key) = 0;
        virtual int    _delete(void * key) = 0;
    };

    // Disk manager class.
    class DiskMgr {
    public:
        const int PAGE_SZ;

        int fd;
        
        
    };

    // 파일 하나에 대한 관리. 하나의 파일은 하나의 자료구조를 갖는다.
    class Table {
    public:
        Table(const char * filename, BASE_TYPE type = BPTree);
        ~Table();

        int _insert(void * key, const void * value);
        void * _find(void * key);
        int _delete(void * key);
        
    private:
        // TODO: Table info
        // e.g.) Scheme

        BASE_TYPE type;
        int fd;

        IndexMgr * index_mgr;
        DiskMgr  * disk_mgr;
    };

}