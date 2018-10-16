#pragma once

#include <inttypes.h>

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

        virtual value_t _find  (const key_t key) = 0;
        virtual int     _insert(const key_t key, c_value_t value) = 0;
        virtual int     _delete(const key_t key) = 0;
    };

    // Disk manager class.
    class DiskMgr {
    public:
        DiskMgr(const char * filename);
        ~DiskMgr();

        // Push/Pop page at free page list.
        pageid_t alloc(void);
        void     free (pageid_t id);
        
        // Read/Write page on disk.
        void read (pageid_t id, page_t & dest);
        void write(pageid_t id, const page_t & src);
        
        inline pageid_t get_pageid(off_t off);
        int get_data_page(page_t & dest);
    
    private:
        int fd;
        #pragma pack(push, 1)   // No align
        struct {
            uint64_t free_off;
            uint64_t data_off;
            uint64_t num_pages;
        } header;
        #pragma pack(pop)
        int num_free_pages;
        off_t last_free_off;

        inline off_t get_offset(pageid_t id);
        void expand(int num_pages);
    };

    // Manage a table.
    class Table {
    public:
        Table(const char * filename, BASE_TYPE type = BPTree);
        ~Table();

        value_t _find  (const key_t key);
        int     _insert(const key_t key, c_value_t value);
        int     _delete(const key_t key);
        
    private:
        // TODO: Table info
        // e.g.) Schema

        BASE_TYPE type;

        IndexMgr * index_mgr;
        DiskMgr  * disk_mgr;
    };

}