#pragma once

#include "api.h"
#include "common.h"
#include <unistd.h>
#include <inttypes.h>

//==============================================================
// CONSTANTS
const int PAGE_DATA_SZ = 0x1000;

//==============================================================
// TYPES
using pageid_t = uint64_t;
struct page_t {
    // Constructor
    page_t(const tableid_t table_id);
    page_t(const tableid_t table_id, const pageid_t page_id);
    ~page_t();
    
    void write(void);    // Write this page on disk.
    void free(void);     // Add this page to free page list.

    tableid_t table_id;
    pageid_t  page_id;
    bool      is_dirty = 0;
    char      data[PAGE_DATA_SZ];     // For synchronization
};

pageid_t get_pageid(tableid_t table_id, off_t offset);
off_t    get_offset(tableid_t table_id, pageid_t page_id);