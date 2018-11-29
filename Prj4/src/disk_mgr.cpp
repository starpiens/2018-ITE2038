#include "disk_mgr.h"
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

namespace DiskMgr {

    //====================================================================
    // DECLARATIONS
    struct Table;
    struct FreePage;


    //====================================================================
    // GLOBALS

    // A set to check if file is already opened
    static std::unordered_set< const char * > g_filenames;
    // A map for store tables.
    static std::unordered_map< tableid_t, Table > g_tables;


    //====================================================================
    // API FUNCTION DEFINITIONS

    // Allocate new page from free page list.
    page_t & read_page(tableid_t table_id) {
        Table & target_table = g_tables[table_id];
        if (! target_table.free_offset) {
            // expand.
        }
        
        // Read the first free page.
        pageid_t free_pageid = get_pageid(table_id, target_table.free_offset);
        FreePage & free_page = * dynamic_cast< FreePage* >(
            & read_page(table_id, free_pageid)
        );

    }

    page_t & read_page(tableid_t table_id, pageid_t page_id) {

    }

    pageid_t get_pageid(tableid_t table_id, off_t offset) {

    }

    off_t get_offset(tableid_t table_id, pageid_t page_id) {

    }

    //====================================================================
    // STRUCT TABLE
    
    // 
    struct Table {
        Table(const char * file_name, int file_descriptor, tableid_t table_id);
        
        void increment_num_pages(void);

        const char    * file_name;
        const int       fd;
        const tableid_t table_id;
        
        // Data from header page
        void     read_header(void);
        void     write_header(void);
        off_t    free_offset;   // First free page offset
        off_t    data_offset;   // First data page offset
        uint64_t num_pages;     // Number of pages
        uint64_t num_cols;      // Number of columns
    };


    //====================================================================
    // Free pages are linked and allocation is managed by the free page list.
    struct FreePage : public page_t {
        void get_next_offset();
    };

    //====================================================================
    
}