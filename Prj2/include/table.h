#pragma once

namespace JiDB
{
    typedef uint64_t pageid_t;

    // Base 
    enum BASE_TYPE { 
        BPTree, RTree, GiST
    };
    
    // Index manager class.
    class IndexMgr {
    public:
        IndexMgr();
        ~IndexMgr();

        virtual int    _insert(const void * key, const void * value) = 0;
        virtual void * _find  (const void * key) = 0;
        virtual int    _delete(const void * key) = 0;
    };

    // Disk manager class.
    class DiskMgr {
    public:
        DiskMgr(char * filename, int page_size);
        ~DiskMgr();
    
    private:
        int page_size;
        int fd;
    };

    // 파일 하나에 대한 관리. 하나의 파일은 하나의 자료구조를 갖는다.
    class Table {
    public:
        Table(const char * filename, BASE_TYPE type = BPTree);
        ~Table();

        void * _find  (const void * key);
        int    _insert(const void * key, const void * value);
        int    _delete(const void * key);
        
    private:
        // TODO: Table info
        // e.g.) Scheme
        // 하지만 이번 버전에서는 scheme가 고정되어 있다고 가정한다.
        typedef int64_t      key_t;
        typedef char *       value_t;
        typedef const char * c_value_t;

        BASE_TYPE type;

        IndexMgr * index_mgr;
        DiskMgr  * disk_mgr;
    };

}