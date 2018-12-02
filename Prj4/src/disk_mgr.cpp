#include "disk_mgr.h"
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <fcntl.h>
#include <cassert>
#include <stdexcept>

//====================================================================
// DECLARATIONS
struct Table;
struct FreePage;


//====================================================================
// GLOBALS

// A set to check if file is already opened
static std::unordered_set< const char * > g_filenames;
// A map for store tables.
static std::unordered_map< tableid_t, Table & > g_tables;


//====================================================================
// API FUNCTION DEFINITIONS

// Open existing data file using 'pathname' or create one if not existed.
// If success, return table_id. Otherwise, return -1.
int open_table(const char * pathname, int num_columns) {
    Table * new_table;
    try {
        new_table = new Table(pathname, num_columns);
        g_tables.insert({ new_table->table_id, *new_table });
    } catch (std::exception & e) {
        fprintf(stderr, "Failed to open file: %s\n", pathname);
        fprintf(stderr, "> %s\n", e.what());
        return -1;
    }
    return 0;
}

// Write the pages relating to this table to disk and close the table.
// If success, return 0. Otherwise, return -1.
int close_table(tableid_t table_id) {
    
    g_tables.erase(table_id);
}

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
    if (! target_table.free_pageid) {
        target_table.expand();
    }
    
    this->table_id = table_id;
    this->page_id  = target_table.free_pageid;

    FreePage first_free_page(table_id, 0);
    off_t next_free_page = first_free_page.get_next_offset();
    g_tables[table_id].free_pageid = get_pageid(table_id, next_free_page);
}

// Constructor. Get page at (table_id, page_id).
page_t::page_t(const tableid_t table_id, const pageid_t page_id) {

}

// Destructor.
page_t::~page_t() {

}

//====================================================================
// STRUCT TABLE

// 
struct Table {
    // Constructor
    Table(const char * file_name, int num_columns);
    ~Table();
    
    void expand(void);
    void increment_num_pages(void);
    void advance_free_pageid(void);

    void read_header(void);
    void write_header(void);

    const char * file_name;
    int          file_descriptor;
    tableid_t    table_id;
    
    // Data from header page
    char     data[PAGE_DATA_SZ];
    pageid_t free_pageid;   // First free page
    pageid_t data_pageid;   // First data page (root).
    uint64_t num_pages;     // Number of pages
    uint64_t num_cols;      // Number of columns
};

// Constructor. 
// Open a new file and load data from file.
Table::Table(const char * file_name, int num_columns) :
    file_name(file_name),
    file_descriptor(open(file_name, O_RDWR | O_CREAT, 644)),
    table_id(file_descriptor),
    num_cols(num_columns) {

    if (file_descriptor < 0) {
        throw std::runtime_error("DB File not opened");
    }
    if (g_filenames.find(file_name) != g_filenames.end()) {
        throw std::runtime_error("DB File is already opened");
    }

    read_header();
    g_filenames.insert(file_name);
}

// Destructor.
Table::~Table() {
    write_header();
    close(file_descriptor);
    g_filenames.erase(file_name);
}

// Increment the number of pages.
inline void Table::increment_num_pages() {
    num_pages++;
}

// Read header info.
void Table::read_header() {
    lseek(file_descriptor, 0, SEEK_SET);
    read(file_descriptor, data, PAGE_DATA_SZ);

    off_t free_page_offset = *reinterpret_cast< uint64_t * >(data + 0);
    off_t data_page_offset = *reinterpret_cast< uint64_t * >(data + 8);
    
    free_pageid = get_pageid(table_id, free_page_offset);
    data_pageid = get_pageid(table_id, data_page_offset);
    num_pages   = *reinterpret_cast< uint64_t * >(data + 16);
    num_cols    = *reinterpret_cast< uint64_t * >(data + 24);
}

// Write header info.
void Table::write_header() {
    off_t free_page_offset = get_offset(table_id, free_pageid);
    off_t data_page_offset = get_offset(table_id, data_pageid);
    
    *reinterpret_cast< uint64_t * >(data + 0)  = free_page_offset;
    *reinterpret_cast< uint64_t * >(data + 8)  = data_page_offset;
    *reinterpret_cast< uint64_t * >(data + 16) = num_pages;
    *reinterpret_cast< uint64_t * >(data + 24) = num_cols;

    lseek(file_descriptor, 0, SEEK_SET);
    write(file_descriptor, data, PAGE_DATA_SZ);
    fsync(file_descriptor);
}


//====================================================================
// STRUCT FREEPAGE

// Free pages are linked and allocatoin is managed by the free page list.
struct FreePage : private page_t {
    // Constructor
    FreePage(const tableid_t table_id, const pageid_t page_id);

    off_t get_next_offset();
};

// Constructor.
FreePage::FreePage(const tableid_t table_id, const pageid_t page_id) :
    page_t(table_id, page_id) {
    
}

// Get next free page offset.
off_t FreePage::get_next_offset() {
    return *reinterpret_cast< uint64_t * >(data);
}