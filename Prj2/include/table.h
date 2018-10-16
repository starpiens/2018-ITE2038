#pragma once

namespace JiDB
{
    const int PAGE_SZ = 0x1000;
    
    typedef uint64_t pageid_t;
    typedef struct {
        char raw[PAGE_SZ];
    } page_t;

    typedef int64_t      key_t;
    typedef char *       value_t;
    typedef const char * c_value_t;

    // Base data structure type.
    enum BASE_TYPE { 
        BPTree, RTree, GiST
    };
    
    // Index manager class.
    class IndexMgr {
    public:
        IndexMgr();
        ~IndexMgr();

        value_t _find  (const key_t key);
        int     _insert(const key_t key, c_value_t value);
        int     _delete(const key_t key);
    };

    // Disk manager class.
    class DiskMgr {
    public:
        DiskMgr(char * filename, int page_size);
        ~DiskMgr();

        virtual pageid_t alloc(void);
        virtual void     free (pageid_t id);
        
        virtual void     read (pageid_t id, page_t & dest);
        virtual void     write(pageid_t id);
    
    private:
        int fd;
    };

    // 파일 하나에 대한 관리. 
    class Table {
    public:
        Table(const char * filename, BASE_TYPE type = BPTree);
        ~Table();

        value_t _find  (const key_t key);
        int     _insert(const key_t key, c_value_t value);
        int     _delete(const key_t key);
        
    private:
        // TODO: Table info
        // e.g.) Scheme

        BASE_TYPE type;

        IndexMgr * index_mgr;
        DiskMgr  * disk_mgr;
    };

}