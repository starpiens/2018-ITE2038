#pragma once

#include <inttypes.h>
#include <unistd.h>
#include <cstring>

namespace JiDB
{
    const int PAGE_SZ = 0x1000;
    
    using pageid_t = uint64_t;
    using page_t   = struct {
        void free(void);

        char     raw[PAGE_SZ];
        pageid_t id;
    };

    using key_t   = int64_t;
    using value_t = struct _Value {
        _Value() = default;
        _Value(const char * val) {
            memcpy(value, val, sizeof(value));
        }
    private:
        char value[120]; 
    };

    // Base data structure type.
    enum BASE_TYPE { 
        BPTree, RTree, GiST
    };

    // Disk manager class.
    class DiskMgr {
    public:
        DiskMgr(const char * filename);
        virtual ~DiskMgr();

        // Push/Pop page at free page list.
        page_t & alloc(void);
        void     free (pageid_t id);
        
        // Read/Write page on disk.
        void read (pageid_t id, page_t & dest);
        void write(pageid_t id, const page_t & src) const;
        
        pageid_t get_pageid(off_t off) const;
        off_t    get_offset(pageid_t id) const;
        
        int get_data_page(page_t & dest);
    
    private:
        int fd;

        #pragma pack(push, 1)
        // Header page
        struct HeaderPage {
            uint64_t free_off;
            uint64_t data_off;
            uint64_t num_pages;
        } header;

        // Free
        struct FreePage {
            uint64_t next_off;
        };
        #pragma pack(pop)

        // Helper data.
        int num_free_pages;
        off_t last_free_off;

        inline int read_off (off_t off, page_t &dest);
        inline int write_off(off_t off, const page_t &src);
        inline void read_header (void);
        inline void write_header(void);
        inline void setup_helper_data(void);

        inline int expand(int num_pages);
    };

    // Index manager class.
    class IndexMgr {
    public:
        IndexMgr(const char * filename) { disk_mgr = new DiskMgr(filename); }
        virtual ~IndexMgr() { delete disk_mgr; }

        virtual value_t * _find  (const key_t key) = 0;
        virtual int       _insert(const key_t key, const value_t value) = 0;
        virtual int       _delete(const key_t key) = 0;
    
    protected:
        DiskMgr * disk_mgr;
    };

    // Manage a table.
    class Table {
    public:
        Table(const char * filename, BASE_TYPE type = BPTree);
        virtual ~Table();

        value_t * _find  (const key_t key);
        int       _insert(const key_t key, const value_t value);
        int       _delete(const key_t key);
        
    private:
        // TODO: Table info
        // e.g.) Schema

        BASE_TYPE type;

        IndexMgr * index_mgr;
    };

}