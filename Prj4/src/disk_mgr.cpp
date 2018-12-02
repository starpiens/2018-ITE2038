#include "disk_mgr.h"
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <cassert>
#include <fcntl.h>

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

// Get page id by offset.
pageid_t get_pageid(tableid_t table_id, off_t offset) {
    assert( offset % PAGE_DATA_SZ == 0 );
    return offset / PAGE_DATA_SZ;
}

// Get offset by page id.
off_t get_offset(tableid_t table_id, pageid_t page_id) {
    return page_id * PAGE_DATA_SZ;
}

//====================================================================
// STRUCT PAGE DEFINITIONS

// Constructor. Get new page from the free page list.
page_t::page_t(const tableid_t table_id) {
    Table & target_table = g_tables[table_id];
    if (! target_table.free_offset) {
        // expand.
    }

    // Read first free page.
    FreePage & free_page = * new FreePage(table_id, FreePage::O_READ);
};

// Constructor. Get page at (table_id, page_id).
page_t::page_t(const tableid_t table_id, const pageid_t page_id) {

}

//====================================================================
// STRUCT TABLE

// 
struct Table {
    Table(const char * file_name, int file_descriptor);
    
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
// STRUCT FREEPAGE

// Free pages are linked and allocatoin is managed by the free page list.
struct FreePage : public page_t {
    // Constructor
    FreePage(const tableid_t table_id, int oflag);
    enum { O_READ, O_MAKE };

    off_t get_next_offset();
};

// Constructor. Get first free page.
FreePage::FreePage(const tableid_t table_id, int oflag) 
: page_t(table_id, get_pageid(table_id, )) {

}

// Get next free page offset.
off_t FreePage::get_next_offset() {
    
}